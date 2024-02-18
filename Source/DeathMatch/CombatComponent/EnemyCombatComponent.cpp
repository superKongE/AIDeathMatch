#include "DeathMatch/CombatComponent/EnemyCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PrimitiveComponent.h"

#include "DeathMatch/AIController/EnemyController.h"
#include "DeathMatch/Enemy/Enemy.h"
#include "DeathMatch/Weapon/Revenant_Bomb.h"

UEnemyCombatComponent::UEnemyCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UEnemyCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
}
void UEnemyCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Jump();
}
void UEnemyCombatComponent::TraceUnderCrosshair()
{
	if (GEngine && OwnerCharacter)
	{
		FVector Start = OwnerCharacter->GetMesh()->GetSocketLocation(FName("Eyes_Position"));
		FVector direction = FRotationMatrix(OwnerCharacter->GetMesh()->GetSocketRotation(FName("Eyes_Position"))).GetUnitAxis(EAxis::X);
		FVector End = Start + direction * 8000.f;

		TArray<AActor*> IgnoreActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> ETQ;
		ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		UKismetSystemLibrary::LineTraceMultiForObjects(this, Start, End, ETQ, false, IgnoreActors, EDrawDebugTrace::None, HitResults, true);
		if (HitResults.IsEmpty())
		{
			FHitResult Hit;
			Hit.ImpactPoint = End;
			HitResults.Add(Hit);
			//OwnerCharacter->SetHitTargetLocation(Hit.ImpactPoint);
		}
		else
		{
			//OwnerCharacter->SetHitTargetLocation(HitResults[0].ImpactPoint);
		}
	}
}

void UEnemyCombatComponent::Jump()
{
	if (OwnerCharacter == nullptr) return;

	FVector FowardVector = OwnerCharacter->GetVelocity();
	FowardVector.Normalize();

	const FVector StartLocation = OwnerCharacter->GetActorLocation() + FowardVector * 200.f;
	const FVector EndLocation = StartLocation - FVector(0.f, 0.f, 500.f);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
	if (!HitResult.bBlockingHit)
	{
		OwnerCharacter->Jump();
	}
}


void UEnemyCombatComponent::Ready()
{
	CurrentAmmo = MaxAmmo;
	CurrentCombatState = EAICombatState::ECS_Idle;
}



void UEnemyCombatComponent::SpendAmmo(int32 Ammo)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - Ammo, 0, MaxAmmo);
}
bool UEnemyCombatComponent::CanAttack()
{
	if (CurrentCombatState == EAICombatState::ECS_Idle && OwnerCharacter && OwnerCharacter->GetbViewTarget()) return true;

	return false;
}
void UEnemyCombatComponent::SpawnDefaultAttackEffect(const FVector& SpawnLocation)
{
	if (OwnerCharacter == nullptr || DefaultAttackParticle == nullptr || DefaultAttackHitParticle == nullptr) return;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DefaultAttackParticle, OwnerCharacter->GetMesh()->GetSocketLocation(FName("MuzzlePoint")));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DefaultAttackHitParticle, SpawnLocation);
}
void UEnemyCombatComponent::SpawnStrongAttackEffect(const FVector& SpawnLocation)
{
	if (OwnerCharacter == nullptr || StrongAttackParticle == nullptr || StrongAttackHitParticle == nullptr) return;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, StrongAttackParticle, OwnerCharacter->GetMesh()->GetSocketLocation(FName("MuzzlePoint")));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, StrongAttackHitParticle, SpawnLocation);
}
void UEnemyCombatComponent::Attack()
{
	// idle 상태인지
	if (!CanAttack()) return;

	ACharacter* TargetActor = OwnerCharacter->GetTargetActor();
	if (TargetActor == nullptr) return;

	FHitResult HitResult;
	const FName& TargetBoneName = OwnerCharacter->GetTargetBoneName();


	const int32 RandNumber = FMath::FRandRange(1, 101);
	if (RandNumber <= DefaultAttackPercentage && CurrentAmmo >= 1)
	{
		CurrentCombatState = EAICombatState::ECS_Attack;
		OwnerCharacter->PlayAnimMonatage(EEnemyState::EES_DefaultAttack);

		AttackLineTrace(HitResult, TargetActor, TargetBoneName);

		SpendAmmo(DefaultAttackSpendAmmo);
		SpawnDefaultAttackEffect(HitResult.ImpactPoint);

		ARootCharacter* HitCharacter = Cast<ARootCharacter>(HitResult.GetActor());
		if(HitCharacter)
			UGameplayStatics::ApplyDamage(HitCharacter, DefaultDamage, OwnerCharacter->GetEnemyController(), OwnerCharacter, UDamageType::StaticClass());

		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UEnemyCombatComponent::AttackTimerEnd, DefaultAttackTimer, false);
	}
	else if(DefaultAttackPercentage < RandNumber && RandNumber <= StrongAttackPercentage && CurrentAmmo >= 3)
	{
		CurrentCombatState = EAICombatState::ECS_Attack;
		OwnerCharacter->PlayAnimMonatage(EEnemyState::EES_StrongAttack);

		AttackLineTrace(HitResult, TargetActor, TargetBoneName);

		SpendAmmo(StrongAttackSpendAmmo);
		SpawnStrongAttackEffect(HitResult.ImpactPoint);

		ARootCharacter* HitCharacter = Cast<ARootCharacter>(HitResult.GetActor());
		if (HitCharacter)
			UGameplayStatics::ApplyDamage(HitCharacter, StrongDamage, OwnerCharacter->GetEnemyController(), OwnerCharacter, UDamageType::StaticClass());

		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UEnemyCombatComponent::AttackTimerEnd, StrongAttackTimer, false);
	}
	else if(StrongAttackPercentage < RandNumber  && RandNumber <= StrongAttackPercentage)
	{
		CurrentCombatState = EAICombatState::ECS_Attack;
		SetIsOffUpperarmTransformModify(true);
		OwnerCharacter->PlayAnimMonatage(EEnemyState::EES_FirstSkill);
		FirstSkillCoolTimeEnd = false;

		if (BombClass)
		{
			FActorSpawnParameters FSP;
			FSP.Owner = OwnerCharacter;
			FSP.Instigator = OwnerCharacter;
			FVector Location = OwnerCharacter->GetBombSpawnPoint();
			AEnemyController* OwnerPlayerController = OwnerCharacter->GetAIController();

			FVector LaunchVector;
			UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVector, Location, TargetActor->GetActorLocation(), 0.f, 0.9f);


			FCollisionQueryParams FQP;
			FQP.AddIgnoredActor(OwnerCharacter);

			// 폭탄을 소환하기전에 캐릭터와 폭탄이 소환될 위치 사이에 LineTrace를 하여 충돌감지
			GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), Location, ECollisionChannel::ECC_Visibility, FQP);
			if (HitResult.bBlockingHit)
			{
				ARevenant_Bomb* Bomb = GetWorld()->SpawnActor<ARevenant_Bomb>(BombClass, Location, Location.Rotation(), FSP);
				Bomb->Bomb(HitResult.ImpactPoint);
			}
			else
			{
				ARevenant_Bomb* Bomb = GetWorld()->SpawnActor<ARevenant_Bomb>(BombClass, Location, LaunchVector.Rotation(), FSP);
				Bomb->GetMesh()->AddImpulse(LaunchVector, NAME_None, true);
			}
		}

		GetWorld()->GetTimerManager().SetTimer(FirstSkillTimerHandle, this, &UEnemyCombatComponent::FirstSKillTimerEnd, FirstSkillTimer, false);
	}


	if (CurrentAmmo == 0)
	{
		OwnerCharacter->SetSpendAllAmmo(true);
	}
}
void UEnemyCombatComponent::AttackLineTrace(FHitResult& HitResult, const ACharacter* TargetActor, const FName& TargetBoneName)
{
	const USkeletalMeshSocket* MuzzleSocket = OwnerCharacter->GetMesh()->GetSocketByName("MuzzlePoint");
	if (MuzzleSocket == nullptr) return;


	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(OwnerCharacter->GetMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector TargetBoneLocation = TargetActor->GetMesh()->GetSocketLocation(TargetBoneName);
	const FVector ToTargetNormalized = (TargetBoneLocation - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * FMath::Abs((TargetActor->GetActorLocation() - OwnerCharacter->GetActorLocation()).Size());

	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, AttackSphereRadius);

	const FVector EndLoc = SphereCenter + RandVec; // 원 내부의 어느 위치
	FVector ToEndLoc = (EndLoc - TraceStart); // 원 내부의 어느 위치의 방향 벡터
	ToEndLoc = TraceStart + ToEndLoc * 8000.f / ToEndLoc.Size();

	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, ToEndLoc, ECollisionChannel::ECC_Visibility, FQP);
	//DrawDebugLine(GetWorld(), TraceStart, ToEndLoc, FColor::Red, true);
}


bool UEnemyCombatComponent::Teleport(const FVector& Location)
{
	if (!CanTeleport()) return false;

	TeleportLocation = Location;
	if (GetWorld()->FindTeleportSpot(OwnerCharacter, TeleportLocation, OwnerCharacter->GetActorRotation()))
	{
		CurrentCombatState = EAICombatState::ECS_Attack;
		OwnerCharacter->SetCanTeleport(false);
		OwnerCharacter->PlayAnimMonatage(EEnemyState::EES_SecondSkill);
		SecondSkillCoolTimeEnd = false;
		GetWorld()->GetTimerManager().SetTimer(SecondSkillTimerHandle, this, &UEnemyCombatComponent::SecondSkillTimerEnd, SecondSkillTimer, false);

		return true;
	}
	
	return false;
}
bool UEnemyCombatComponent::CanTeleport()
{
	if (CurrentCombatState == EAICombatState::ECS_Idle && SecondSkillCoolTimeEnd) return true;

	return false;
}
void UEnemyCombatComponent::SecondSkillEnd()
{
	if (OwnerCharacter == nullptr)return;

	OwnerCharacter->SetActorLocation(TeleportLocation);
	OwnerCharacter->TeleportTo(TeleportLocation + FVector(0.f,0.f,100.f), OwnerCharacter->GetActorRotation());
	CurrentCombatState = EAICombatState::ECS_Idle;
}
void UEnemyCombatComponent::SecondSkillTimerEnd()
{
	OwnerCharacter->SetCanTeleport(true);
	SecondSkillCoolTimeEnd = true;
}


void UEnemyCombatComponent::AttackTimerEnd()
{
	CurrentCombatState = EAICombatState::ECS_Idle;
}
void UEnemyCombatComponent::FirstSKillTimerEnd()
{
	FirstSkillCoolTimeEnd = true;
}
void UEnemyCombatComponent::FirstSkillEnd()
{
	CurrentCombatState = EAICombatState::ECS_Idle;
}


void UEnemyCombatComponent::ThirdSkillEnd()
{
	CurrentCombatState = EAICombatState::ECS_Idle;
}


void UEnemyCombatComponent::AttackEnd()
{
	//CurrentCombatState = EAICombatState::ECS_Idle;
}



bool UEnemyCombatComponent::CanReload()
{
	if (OwnerCharacter && CurrentCombatState == EAICombatState::ECS_Idle)
		return true;

	return false;
}
void UEnemyCombatComponent::Reload()
{
	if (!CanReload()) return;

	IsOffUpperarmTransformModify = true;
	CurrentCombatState = EAICombatState::ECS_Reload;

	OwnerCharacter->PlayAnimMonatage(EEnemyState::EES_Reload);
}
void UEnemyCombatComponent::ReloadEnd()
{
	IsOffUpperarmTransformModify = false;
	CurrentAmmo = MaxAmmo;
	OwnerCharacter->SetSpendAllAmmo(false);
	CurrentCombatState = EAICombatState::ECS_Idle;
}
