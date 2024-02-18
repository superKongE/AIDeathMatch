#include "DeathMatch/Enemy/Enemy.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/ChildActorComponent.h"
#include "Components/CapsuleComponent.h"

#include "DeathMatch/Character/RootCharacter.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "DeathMatch//AIController/EnemyController.h"
#include "DeathMatch/CombatComponent/EnemyCombatComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));

	BombPoint = CreateDefaultSubobject<UChildActorComponent>(TEXT("BombSpawnPoint"));
	BombPoint->SetupAttachment(RootComponent);

	ViewTargets.Emplace(FName("head"));
	ViewTargets.Emplace(FName("spine_01"));
	ViewTargets.Emplace(FName("lowerarm_l"));
	ViewTargets.Emplace(FName("hand_l"));
	ViewTargets.Emplace(FName("lowerarm_r"));
	ViewTargets.Emplace(FName("hand_r"));
	ViewTargets.Emplace(FName("calf_l"));
	ViewTargets.Emplace(FName("foot_l"));
	ViewTargets.Emplace(FName("calf_r"));
	ViewTargets.Emplace(FName("foot_r"));
}
void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->SetOwnerCharacter(this);
	}
}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	CurrentHP = MaxHP;
	IsSpawned = true;
	SpawnLocation = GetActorLocation();

	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("StartPoint"), GetActorLocation());
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("TeleportLocation"), FVector::ZeroVector);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("bViewTarget"), false);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDeath"), false);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanTeleport"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsFindPlayerTimeEnd"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("bSpendAllAmmo"), false);
	}
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Detect(DeltaTime);
	AISightPerception();
}
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ARootCharacter>(OtherActor))
	{
		EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
		if (EnemyController)
		{
			bOverlap = true;
			OverlappedActor = OtherActor;
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsOverlapped"), true);
		}
	}
}
void AEnemy::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (EnemyController)
	{
		bOverlap = false;
		OverlappedActor = nullptr;
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsOverlapped"), false);
		//EnemyController->RunBehaviorTree(BehaviorTree);
	}
}


float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDeath) return DamageAmount;

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0, MaxHP);

	if (CurrentHP <= 0.f)
	{
		Death();

		if (FVector::DotProduct(GetActorForwardVector(), DamageCauser->GetActorLocation()) >= 0.f)
			IsFowardDeath = true;
		else
			IsFowardDeath = false;
	}

	return DamageAmount;
}
void AEnemy::Death()
{
	IsDeath = true;
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (EnemyController)
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDeath"), true);

	GetWorldTimerManager().SetTimer(DeathEndTimerHandle, this, &AEnemy::DeathEnd, DeathEndTimer, false);
	GetWorldTimerManager().SetTimer(DeathSpawnTimerHandle, this, &AEnemy::Spawn, DeathSpawnTimer, false);
}
void AEnemy::DeathEnd()
{
	GetMesh()->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AEnemy::Spawn()
{
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (EnemyController)
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDeath"), false);

	IsSpawned = true;
	IsDeath = false;
	CurrentHP = MaxHP;
	if (CombatComponent)
		CombatComponent->Ready();

	SetActorLocation(SpawnLocation);
	GetMesh()->SetVisibility(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


bool AEnemy::Teleport(const FVector& TeleportLocation)
{
	if (CombatComponent)
	{
		if (CombatComponent->Teleport(TeleportLocation)) 
			return true;
		else 
			return false;
	}

	return false;
}
void AEnemy::SetCanTeleport(bool bCan)
{
	bCanTeleport = bCan;
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (EnemyController)
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanTeleport"), bCanTeleport);
}


// AI Controller의 OnPerception에 의해서 호출됨
void AEnemy::CanSeeEnemy(ACharacter* ViewTargetActor)
{
	if (ViewTargetActor)
	{
		CurrentDetectEnemyTime = 0.f;
		TargetActor = ViewTargetActor;
		IsFindPlayerTimeEnd = true;
		CurrentDetectEnemyTime = 0.f;
		bViewTarget = true;
		ViewTargetActorLocation = ViewTargetActor->GetActorLocation();
	}
	else
	{
		if (TargetActor)
		{
			CurrentDetectEnemyTime = 0.f;
			IsFindPlayerTimeEnd = false;
			ViewTargetActorLocation = TargetActor->GetActorLocation();
		}

		bViewTarget = false;
	}

	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), TargetActor);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("bViewTarget"), bViewTarget);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsFindPlayerTimeEnd"), IsFindPlayerTimeEnd);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("ViewTargetLocation"), ViewTargetActorLocation);
	}
}
// AI가 보고있는 적의 신체 부분
void AEnemy::SetTargetBone(FName& BoneName) const
{
	TargetBoneName = BoneName;
}



void AEnemy::AISightPerception()
{
	// AI가 적을 감지시
	// AI의 눈에서 적의 신체부분(머리, 팔, 다리 등)까지의 각도 구하기
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (TargetActor && EnemyController)
	{
		FVector Start = GetMesh()->GetSocketLocation(FName("Eyes_Position"));
		HitTargetLocation = TargetActor->GetMesh()->GetSocketLocation(TargetBoneName);
		Pitch = UKismetMathLibrary::FindLookAtRotation(Start, HitTargetLocation).Pitch;
	}
	else if (TargetActor == nullptr)
	{
		FVector Start = GetMesh()->GetSocketLocation(FName("Eyes_Position"));
		FVector direction = FRotationMatrix(GetMesh()->GetSocketRotation(FName("Eyes_Position"))).GetUnitAxis(EAxis::X);
		FVector End = Start + direction * 8000.f;

		FHitResult HitResult;
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, FQP);
		if (HitResult.bBlockingHit)
			HitTargetLocation = HitResult.ImpactPoint;
		else
			HitTargetLocation = End;
	}
}
void AEnemy::Detect(float DeltaTime)
{
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;

	if (!IsFindPlayerTimeEnd && EnemyController)
	{
		CurrentDetectEnemyTime += DeltaTime;
		if (CurrentDetectEnemyTime >= MaxDetectEnemyTime)
		{
			TargetActor = nullptr;
			IsFindPlayerTimeEnd = true;

			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsFindPlayerTimeEnd"), IsFindPlayerTimeEnd);
		}

		if (TargetActor)
		{
			ARootCharacter* Chr = Cast<ARootCharacter>(TargetActor);
			if (Chr && Chr->GetISDeath())
			{
				Pitch = 0.f;
				TargetActor = nullptr;
				IsFindPlayerTimeEnd = true;

				EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("bViewTarget"), false);
				EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsFindPlayerTimeEnd"), IsFindPlayerTimeEnd);

				return;
			}
		}
	}
}



void AEnemy::SetSpendAllAmmo(bool bSpendAll)
{
	EnemyController = EnemyController == nullptr ? Cast<AEnemyController>(GetController()) : EnemyController;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("bSpendAllAmmo"), bSpendAll);
	}
}



void AEnemy::Attack()
{
	if (CombatComponent)
	{
		CombatComponent->Attack();
	}
}
void AEnemy::PlayAnimMonatage(EEnemyState EnemyState)
{
	if (AnimMontage == nullptr) return;

	UAnimInstance* tempAnimInstance = GetMesh()->GetAnimInstance();
	if (tempAnimInstance == nullptr) return;

	tempAnimInstance->Montage_Play(AnimMontage);

	FName SectionName;
	switch (EnemyState)
	{
	case EEnemyState::EES_DefaultAttack:
		SectionName = FName("DefaultAttack");
		break;

	case EEnemyState::EES_StrongAttack:
		SectionName = FName("StrongAttack");
		break;

	case EEnemyState::EES_FirstSkill:
		SectionName = FName("FirstSkill");
		break;

	case EEnemyState::EES_SecondSkill:
		SectionName = FName("SecondSkill");
		break;

	case EEnemyState::EES_ThirdSkill:
		SectionName = FName("ThirdSkill");

	case EEnemyState::EES_Reload:
		SectionName = FName("Reload");
		break;
	}

	tempAnimInstance->Montage_JumpToSection(SectionName);
}
void AEnemy::AttackEnd()
{
	if (CombatComponent)
	{
		CombatComponent->AttackEnd();
	}
}
void AEnemy::FirstSkillEnd()
{
	if (CombatComponent)
	{
		CombatComponent->FirstSkillEnd();
	}
}
void AEnemy::SecondSkillEnd()
{
	if (CombatComponent)
	{
		CombatComponent->SecondSkillEnd();
	}
}
void AEnemy::ThirdSkillEnd()
{
	if (CombatComponent)
	{
		CombatComponent->ThirdSkillEnd();
	}
}
void AEnemy::Reload()
{
	if (CombatComponent)
	{
		CombatComponent->Reload();
	}
}
void AEnemy::ReloadEnd()
{
	if (CombatComponent)
	{
		CombatComponent->ReloadEnd();
	}
}



bool AEnemy::FootStepNotify(const FName& BoneName)
{
	const USkeletalMeshSocket* foot_l = GetMesh()->GetSocketByName(BoneName);
	const FVector Start = foot_l->GetSocketLocation(GetMesh()) + FVector(0.f, 0.f, 50.f);
	const FVector End = Start + FVector(0.f, 0.f, -100.f);

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.bReturnPhysicalMaterial = true;
	FQP.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, FQP);

	if (HitResult.PhysMaterial.Get() == nullptr) return false;

	auto HitSurface = HitResult.PhysMaterial->SurfaceType;
	if (HitSurface == EPhysicalSurface::SurfaceType_Default)
	{
		if (GroundQue && GroundParticle)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, GroundQue, HitResult.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GroundParticle, HitResult.ImpactPoint);
			return true;
		}
	}
	else if (HitSurface == EPhysicalSurface::SurfaceType1)
	{
		if (WoodQue && WoodParticle)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, WoodQue, HitResult.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, WoodParticle, HitResult.ImpactPoint);
			return true;
		}
	}
	else if (HitSurface == EPhysicalSurface::SurfaceType2)
	{
		if (RockQue && RockParticle)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, RockQue, HitResult.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, RockParticle, HitResult.ImpactPoint);
			return true;
		}
	}

	return false;
}


FVector AEnemy::GetBombSpawnPoint()
{
	if (BombPoint)
		return BombPoint->GetComponentLocation();

	return FVector::ZeroVector;
}
