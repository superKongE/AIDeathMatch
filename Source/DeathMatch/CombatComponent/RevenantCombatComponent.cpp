// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatch/CombatComponent/RevenantCombatComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"

#include "DeathMatch/Weapon/Revenant_Bomb.h"
#include "DeathMatch/Character/RevenantCharacter.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/Character/ReaperTeleport.h"
#include "DeathMatch/HUD/RevenantHUD.h"

URevenantCombatComponent::URevenantCombatComponent()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage>AnimMontageAsset(TEXT("AnimMontage'/Game/Blueprint/Animation/Revenant_Animation/AttackMontage/BP_AttackMontage.BP_AttackMontage'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>DefaultAttackParticleAsset(TEXT("NiagaraSystem'/Game/sA_Megapack_v1/sA_ShootingVfxPack/FX/NiagaraSystems/NS_AR_Muzzleflash_1_ONCE.NS_AR_Muzzleflash_1_ONCE'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>StrongAttackParticleAsset(TEXT("NiagaraSystem'/Game/sA_Megapack_v1/sA_ShootingVfxPack/FX/NiagaraSystems/NS_AR_Muzzleflash_1_ONCE.NS_AR_Muzzleflash_1_ONCE'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>DefaultAttackHitParticleAsset(TEXT("NiagaraSystem'/Game/sA_Megapack_v1/sA_StylizedAttacksPack/FX/NiagaraSystems/NS_BasicHit_2.NS_BasicHit_2'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>StrongAttackHitParticleAsset(TEXT("NiagaraSystem'/Game/sA_Megapack_v1/sA_StylizedAttacksPack/FX/NiagaraSystems/NS_BasicHit.NS_BasicHit'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>ThirdSkillParticleAsset(TEXT("NiagaraSystem'/Game/sA_Megapack_v1/sA_Rayvfx/Fx/NiagaraSystems/NS_Ray_1.NS_Ray_1'"));

	static ConstructorHelpers::FClassFinder<AActor>BombClassAsset(TEXT("Blueprint'/Game/Blueprint/Weapon/BP_RevenantBomb.BP_RevenantBomb_C'"));
	static ConstructorHelpers::FClassFinder<AActor>ReaperTeleportClassAsset(TEXT("Blueprint'/Game/Blueprint/Character/ReaperTeleport/BP_ReaperTeleport.BP_ReaperTeleport_C'"));

	if (AnimMontageAsset.Succeeded())
		CombatMontage = AnimMontageAsset.Object;
	if (DefaultAttackParticleAsset.Succeeded())
		DefaultAttackParticle = DefaultAttackParticleAsset.Object;
	if (StrongAttackParticleAsset.Succeeded())
		StrongAttackParticle = StrongAttackParticleAsset.Object;
	if (DefaultAttackHitParticleAsset.Succeeded())
		DefaultAttackHitParticle = DefaultAttackHitParticleAsset.Object;
	if (StrongAttackHitParticleAsset.Succeeded())
		StrongAttackHitParticle = StrongAttackHitParticleAsset.Object;
	if (ThirdSkillParticleAsset.Succeeded())
		ThirdSkillParticle = ThirdSkillParticleAsset.Object;

	if (BombClassAsset.Succeeded())
		BombClass = BombClassAsset.Class;
	if (ReaperTeleportClassAsset.Succeeded())
		ReaperTeleportClass = ReaperTeleportClassAsset.Class;
}

void URevenantCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
}
void URevenantCombatComponent::SetCharacter(ARootCharacter* Character)
{
	Super::SetCharacter(Character);

	RevenantCharacter = Cast<ARevenantCharacter>(Character);
}

void URevenantCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceUnderCrosshair();
	SetCrossHair(DeltaTime);
	SpawnTeleport();

	ThirdSkillAttack();
}
void URevenantCombatComponent::PlayAttackMontage(ERevenantSkill State)
{
	UAnimInstance* AnimInstance = RevenantCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);

		FName SectionName;
		switch (State)
		{
		case ERevenantSkill::ERS_DefaultAttack:
			SectionName = FName("DefaultAttack");
			break;

		case ERevenantSkill::ERS_StrongAttack:
			SectionName = FName("StrongAttack");
			break;

		case ERevenantSkill::ERS_FirstSkill:
			SectionName = FName("FirstSkill");
			break;

		case ERevenantSkill::ERS_SecondSkill:
			SectionName = FName("SecondSkill");
			break;

		case ERevenantSkill::ERS_ThirdSkill:
			SectionName = FName("ThirdSkill");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, CombatMontage);
	}
}

void URevenantCombatComponent::DefaultAttack(const TArray<FHitResult>& HitResultsArr)
{
	// 순간이동 중에는 공격대신 순간이동 취소
	if (bTeleportButtonPressed)
	{
		CancelTeleport();
		return;
	}
	if (!CanDefaultAttack()) return;

	// 상태를 공격상태로 변경, ((공격 애니메이션 재생)), 딜레이 설정, 맞은 곳 이펙트 발생
	PlayAttackMontage(ERevenantSkill::ERS_DefaultAttack);
	Super::DefaultAttack(HitResultsArr);
	SpendAmmo(DefaultAttackSpendAmmo);

	RevenantCharacter = RevenantCharacter == nullptr ? Cast<ARevenantCharacter>(OwnerCharacter) : RevenantCharacter;
	if (RevenantCharacter == nullptr || RevenantCharacter->GetPlayerController() == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = RevenantCharacter->GetMesh()->GetSocketByName(FName("AttackPoint"));
	if (GetDefaultAttackParticle())
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GetDefaultAttackParticle(), MuzzleSocket->GetSocketLocation(RevenantCharacter->GetMesh()));

	for (const FHitResult& HitResult : HitResultsArr)
	{
		if (Cast<ACharacter>(HitResult.GetActor()))
		{
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), CurrentDefaultAttackDamage, RevenantCharacter->GetPlayerController(), RevenantCharacter, UDamageType::StaticClass());
		}
		else
		{
			// 총알이 actor가 아닌곳에 관통될때마다 데미지 감소
			CurrentDefaultAttackDamage -= 10.f;
		}
	}
	CurrentDefaultAttackDamage = DefaultAttackDamage;
}
void URevenantCombatComponent::StrongAttack(const TArray<FHitResult>& HitResultsArr)
{
	// 순간이동중에는 공격대신 순간이동
	if (bTeleportButtonPressed)
	{
		Teleport();
		return;
	}
	if (!CanStrongAttack()) return;

	// 상태를 공격상태로 변경, ((공격 애니메이션 재생)), 딜레이 설정
	PlayAttackMontage(ERevenantSkill::ERS_StrongAttack);
	Super::StrongAttack(HitResultsArr);
	SpendAmmo(StrongAttackSpendAmmo);

	RevenantCharacter = RevenantCharacter == nullptr ? Cast<ARevenantCharacter>(GetOwnerCharacter()) : RevenantCharacter;
	if (RevenantCharacter == nullptr || RevenantCharacter->GetPlayerController() == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = RevenantCharacter->GetMesh()->GetSocketByName(FName("AttackPoint"));
	FRotator MuzzleRotator = MuzzleSocket->GetSocketLocation(RevenantCharacter->GetMesh()).Rotation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GetStrongAttackParticle(), MuzzleSocket->GetSocketLocation(RevenantCharacter->GetMesh()), MuzzleRotator);

	for (const FHitResult& HitResult : HitResultsArr)
	{
		if (Cast<ACharacter>(HitResult.GetActor()))
		{
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), CurrentStrongAttackDamage, RevenantCharacter->GetPlayerController(), RevenantCharacter, UDamageType::StaticClass());
		}
		else
		{
			CurrentStrongAttackDamage -= 10.f;
		}
	}
	CurrentStrongAttackDamage = StrongAttackDamage;
}



void URevenantCombatComponent::ThrowBomb()
{
	if (BombClass)
	{
		FActorSpawnParameters FSP;
		FSP.Owner = RevenantCharacter;
		FSP.Instigator = RevenantCharacter;
		FVector Location = RevenantCharacter->GetBombSpawnPoint()->GetComponentLocation();
		ARootPlayerController* OwnerPlayerController = RevenantCharacter->GetPlayerController();
		FRotator Rotation = OwnerPlayerController->PlayerCameraManager->GetCameraRotation();

		FHitResult HitResult;
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActor(OwnerCharacter);

		// 폭탄을 소환하기전에 캐릭터와 폭탄이 소환될 위치 사이에 LineTrace를 하여 충돌감지
		GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), Location, ECollisionChannel::ECC_Visibility, FQP);
		if (HitResult.bBlockingHit)
		{
			ARevenant_Bomb* Bomb = GetWorld()->SpawnActor<ARevenant_Bomb>(BombClass, Location, Rotation, FSP);
			Bomb->Bomb(HitResult.ImpactPoint);
		}
		else
		{
			GetWorld()->SpawnActor<ARevenant_Bomb>(BombClass, Location, Rotation, FSP);
		}
	}
}
void URevenantCombatComponent::FirstSkillPressed()
{
	// 쿨타임이 끝나지 않았거나, Idle 상태가 아니라면
	if (!CanFirstSkill()) return;

	// 공격 상태 및 쿨타임 설정
	Super::FirstSkillPressed();

	//SetIsOffUpperarmTransformModify(true);

	PlayAttackMontage(ERevenantSkill::ERS_FirstSkill);

	// 폭탄 날리기
	ThrowBomb();
}
void URevenantCombatComponent::FirstSkillReleased()
{
	
}
// 이벤트 그래프에의해 호출됨
void URevenantCombatComponent::FirstSkillEnd()
{
	//SetIsOffUpperarmTransformModify(false);
	bFirstSkillPressed = false;
	Super::FirstSkillEnd();
}


void URevenantCombatComponent::SecondSkillPressed()
{
	if (OwnerCharacter->GetPlayerController() == nullptr) return;

	// 순간이동이 가능한 상태이고, 쿨타임이 끝났다면
	if (CanSecondSkill())
	{
		bTeleportButtonPressed = !bTeleportButtonPressed;
		bSecondSkillPressed = true;
		if (!bTeleportButtonPressed)
		{
			RevenantHUD = RevenantHUD == nullptr ? Cast<ARevenantHUD>(OwnerCharacter->GetPlayerController()->GetHUD()) : RevenantHUD;
			if (RevenantHUD)
			{
				RevenantHUD->ShowSecondSkillHUD(false);
			}

			if (ReaperTeleport != nullptr)
			{
				if (ReaperTeleport->Destroy())
				{
					ReaperTeleport = nullptr;
				}
			}
		}
		else
		{
			RevenantHUD = RevenantHUD == nullptr ? Cast<ARevenantHUD>(OwnerCharacter->GetPlayerController()->GetHUD()) : RevenantHUD;
			if (RevenantHUD)
			{
				RevenantHUD->ShowSecondSkillHUD(true);
			}
		}
	}
}
void URevenantCombatComponent::SecondSkillReleased()
{
	if (!bSecondSkillPressed) return;

	bSecondSkillPressed = false;
}
// 이벤트 그래프에의해 호출됨
void URevenantCombatComponent::SecondSkillEnd()
{
	Super::SecondSkillEnd();

	ActiveKeyboardInput(true);
	ActiveMouseInput(true);
}


void URevenantCombatComponent::ThirdSkillAttack()
{
	if (bThirdSkillPressed)
	{
		const FVector HandSocketLocation = RevenantCharacter->GetMesh()->GetSocketLocation(FName("hand_r_ability_socket"));
		const FVector FowardVecotr = RevenantCharacter->GetMesh()->GetRightVector();
		const FRotator SocketRotation = RevenantCharacter->GetMesh()->GetSocketRotation(FName("hand_r_ability_socket"));
		const FVector Start = HandSocketLocation + FowardVecotr * 50.f;
		FVector Direction = HitResults[0].ImpactPoint - HandSocketLocation;
		Direction.Normalize();

		float Distance = (HitResults[0].ImpactPoint - HandSocketLocation).Size();
		if (Distance > ThirdSkillLength)
			Distance = ThirdSkillLength;

		if (ThirdSkillEffect == nullptr)
		{
			ThirdSkillEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ThirdSkillParticle,
				HandSocketLocation + FowardVecotr * 50.f, (HitResults[0].ImpactPoint - HandSocketLocation).Rotation());

			ThirdSkillEffect->SetWorldScale3D(FVector(1.f, 2.f, 1.f));
		}
		else
		{
			ThirdSkillEffect->SetWorldLocation(HandSocketLocation + FowardVecotr * 50.f);
			ThirdSkillEffect->SetWorldRotation((HitResults[0].ImpactPoint - HandSocketLocation).Rotation());
			ThirdSkillEffect->SetNiagaraVariableFloat(FString("Length"), Distance / 1700.f);
		}

		ETraceTypeQuery ETQ = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
		TArray<AActor*> ActorToIgnore;
		ActorToIgnore.Add(OwnerCharacter);
		TArray<FHitResult> ThirdSkillHitResults;
		UKismetSystemLibrary::CapsuleTraceMulti(this, Start, Start + Direction * Distance, 50.f, 50.f, ETQ, false, ActorToIgnore, EDrawDebugTrace::None, ThirdSkillHitResults, true);
		for (FHitResult Hit : HitResults)
		{
			if (Cast<ACharacter>(Hit.GetActor()))
			{
				UGameplayStatics::ApplyDamage(Hit.GetActor(), ThirdSkillDamage, OwnerCharacter->GetPlayerController(), OwnerCharacter, UDamageType::StaticClass());
			}
		}
	}
}
void URevenantCombatComponent::ThirdSkillPressed()
{
	if (!CanThirdSkill()) return;

	PlayAttackMontage(ERevenantSkill::ERS_ThirdSkill);
	// 타이머 설정, 상태설정
	Super::ThirdSkillPressed();

	bThirdSkillPressed = true;
	IsFullBodySkill = true;
	bUseFootIK = false;
	OwnerCharacter->ChangeInputExceptMouse(false);

	// 특정 시간이 지나면 자동으로 스킬이 끝남
	GetWorld()->GetTimerManager().SetTimer(ThirdSkillReleasedHandle, this, &URevenantCombatComponent::ThirdSkillReleased, 15.f, false);
}
void URevenantCombatComponent::ThirdSkillReleased()
{
	if (!bThirdSkillPressed) return;

	bThirdSkillPressed = false;
	ThirdSkillJumpToEnd();
	
	GetWorld()->GetTimerManager().ClearTimer(ThirdSkillReleasedHandle);

	if (ThirdSkillEffect != nullptr)
	{
		ThirdSkillEffect->DestroyComponent();
		ThirdSkillEffect = nullptr;
	}
	
	ThirdSkillJumpToEnd();
}
// 이벤트 그래프에의해 호출됨
void URevenantCombatComponent::ThirdSkillEnd()
{
	// idle 상태로 설정
	Super::ThirdSkillEnd();

	OwnerCharacter->ChangeInputExceptMouse(true);
	IsFullBodySkill = false;
	bUseFootIK = true;
}
// 이벤트 그래프에의해 호출됨
void URevenantCombatComponent::ThirdSkillJumpToLoop()
{
	UAnimInstance* AnimInstance = RevenantCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);

		FName SectionName = FName("ThirdSkillLoop");

		AnimInstance->Montage_JumpToSection(SectionName, CombatMontage);
	}
}
void URevenantCombatComponent::ThirdSkillJumpToEnd()
{
	UAnimInstance* AnimInstance = GetOwnerCharacter()->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage);

		FName SectionName = FName("ThirdSkillEnd");

		AnimInstance->Montage_JumpToSection(SectionName, CombatMontage);
	}
}



bool URevenantCombatComponent::Teleport()
{
	if (GetOwnerCharacter() && ReaperTeleport)
	{
		if (!GetOwnerCharacter()->TeleportTo(ReaperTeleport->GetActorLocation() + FVector(0.f, 0.f, 100.f), ReaperTeleport->GetActorRotation()))
			return false;

		PlayAttackMontage(ERevenantSkill::ERS_SecondSkill);

		// 쿨타임 설정
		Super::SecondSkillPressed();
		bTeleportButtonPressed = false;

		if (ReaperTeleport->Destroy())
			ReaperTeleport = nullptr;

		RevenantHUD = RevenantHUD == nullptr ? Cast<ARevenantHUD>(RevenantCharacter->GetPlayerController()->GetHUD()) : RevenantHUD;
		if (RevenantHUD)
			RevenantHUD->ShowSecondSkillHUD(false);

		ActiveKeyboardInput(false);
		ActiveMouseInput(false);

		return true;
	}

	return false;
}
void URevenantCombatComponent::CancelTeleport()
{
	RevenantHUD = RevenantHUD == nullptr ? Cast<ARevenantHUD>(RevenantCharacter->GetPlayerController()->GetHUD()) : RevenantHUD;
	if (RevenantHUD)
		RevenantHUD->ShowSecondSkillHUD(false);

	if (ReaperTeleport)
	{
		if (ReaperTeleport->Destroy())
			ReaperTeleport = nullptr;
	}
	bTeleportButtonPressed = false;
}
bool URevenantCombatComponent::CanTeleport()
{
	if (CurrentCombatState == ECombatState::ECS_Idle) return true;

	return false;
}
void URevenantCombatComponent::SpawnTeleport()
{
	RevenantCharacter = RevenantCharacter == nullptr ? Cast<ARevenantCharacter>(GetOwnerCharacter()) : RevenantCharacter;
	if (RevenantCharacter && bTeleportButtonPressed && ReaperTeleportClass)
	{
		if (ReaperTeleport == nullptr)
		{
			FActorSpawnParameters sp;
			sp.Owner = OwnerCharacter;
			sp.Instigator = OwnerCharacter;

			if (ReaperTeleportClass)
				ReaperTeleport = GetWorld()->SpawnActor<AReaperTeleport>(ReaperTeleportClass, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorRotation(), sp);
		}

		ARootPlayerController* OwnerPlayerController = RevenantCharacter->GetPlayerController();

		FVector CameraLocation = OwnerPlayerController->PlayerCameraManager->GetCameraLocation();
		FVector CameraForwardVector = OwnerPlayerController->PlayerCameraManager->GetActorForwardVector();
		FVector CameraUpVector = OwnerPlayerController->PlayerCameraManager->GetActorUpVector();
		FVector Start = CameraLocation + (CameraForwardVector * 100.f);
		FVector End = Start + CameraForwardVector * TeleportDistance;

		FHitResult HitResult;
		TArray<AActor*> IngoreActors;
		IngoreActors.Add(RevenantCharacter);
		UKismetSystemLibrary::CapsuleTraceSingle(this, Start, End, 34.f, 88.f, ETraceTypeQuery::TraceTypeQuery1, false, IngoreActors, EDrawDebugTrace::None, HitResult, true);

		if (HitResult.bBlockingHit)
		{
			Start = HitResult.ImpactPoint + FVector(0.f, 0.f, 88.f);
			End = Start + FVector(0.f, 0.f, -176.f);

			GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
			if (HitResult.bBlockingHit && (Start - HitResult.ImpactPoint).Z > 0.f)
			{
				FVector ToTarget = RevenantCharacter->GetActorLocation() - HitResult.ImpactPoint;
				FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
				ReaperTeleport->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, 40.f));
				ReaperTeleport->SetActorRotation(LookAtRotation);
			}
		}
	}
}


void URevenantCombatComponent::Reload()
{
	if (!CanReload()) return;

	SetIsOffUpperarmTransformModify(true);

	SetCurrentCombatState(ECombatState::ECS_Reload);
	PlayReloadMontage();
}
void URevenantCombatComponent::PlayReloadMontage()
{
	UAnimInstance* AnimInstance = RevenantCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);
		AnimInstance->Montage_JumpToSection(FName("Reload"), CombatMontage);
	}
}
void URevenantCombatComponent::ReloadEnd()
{
	SetCurrentCombatState(ECombatState::ECS_Idle);
	SetIsOffUpperarmTransformModify(false);
	CurrentAmmo = MaxAmmo;
	SetAmmoHUD(CurrentAmmo);
}
void URevenantCombatComponent::SpendAmmo(const int32 Ammo)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - Ammo, 0, MaxAmmo);

	SetAmmoHUD(CurrentAmmo);
}
void URevenantCombatComponent::SetAmmoHUD(const int32 Ammo)
{
	if (OwnerCharacter)
		OwnerCharacter->SetHUDAmmo(Ammo, MaxAmmo);
}
bool URevenantCombatComponent::CanReload()
{
	// Idle 상태이고, 탄약이 최대 개수보다 적어야 한다
	if (CurrentCombatState == ECombatState::ECS_Idle && CurrentAmmo < MaxAmmo) return true;

	return false;
}


bool URevenantCombatComponent::CanDefaultAttack()
{
	// Super : Idle 상태이고 쿨타임이 끝난지
	if(Super::CanDefaultAttack() &&
		CurrentAmmo - DefaultAttackSpendAmmo >= 0) return true;

	return false;
}
bool URevenantCombatComponent::CanStrongAttack()
{
	// Super : Idle 상태이고 쿨타임이 끝난지
	if (Super::CanDefaultAttack() &&
		CurrentAmmo - StrongAttackSpendAmmo >= 0) return true;

	return false;
}
bool URevenantCombatComponent::CanFirstSkill()
{
	// Super : 스킬 쿨타임 체크, Idle상태인지
	if (Super::CanFirstSkill() && RevenantCharacter) return true;

	return false;
}
bool URevenantCombatComponent::CanSecondSkill()
{
	// Super : 스킬 쿨타임 체크, Idle상태인지
	if (Super::CanSecondSkill() && RevenantCharacter) return true;

	return false;
}
bool URevenantCombatComponent::CanThirdSkill()
{
	// Super : 스킬 쿨타임 체크, Idle상태인지
	if (Super::CanThirdSkill() && RevenantCharacter) return true;

	return false;
}






void URevenantCombatComponent::Ready()
{
	//Super::Ready();
	SetAmmoHUD(CurrentAmmo);
}
void URevenantCombatComponent::Death()
{
	// 각종 상태 초기화
	Super::Death();

	// ThirdSkill을 사용하다 죽을 경우
	if (ThirdSkillReleasedHandle.IsValid())
		ThirdSkillReleased();
	if (ThirdSkillEffect != nullptr)
	{
		ThirdSkillEffect->DestroyComponent();
		ThirdSkillEffect = nullptr;
	}

	RevenantHUD = RevenantHUD == nullptr ? Cast<ARevenantHUD>(OwnerCharacter->GetPlayerController()->GetHUD()) : RevenantHUD;
	if (RevenantHUD)
		RevenantHUD->ShowSecondSkillHUD(false);

	// SecondSkill을 사용하다 죽을 경우
	if (ReaperTeleport)
	{
		if (ReaperTeleport->Destroy())
			ReaperTeleport = nullptr;
	}

	bTeleportButtonPressed = false;
	CurrentAmmo = MaxAmmo;
	//SetAmmoHUD(CurrentAmmo);
}