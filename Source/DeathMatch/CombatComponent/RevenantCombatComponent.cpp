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
	// �����̵� �߿��� ���ݴ�� �����̵� ���
	if (bTeleportButtonPressed)
	{
		CancelTeleport();
		return;
	}
	if (!CanDefaultAttack()) return;

	// ���¸� ���ݻ��·� ����, ((���� �ִϸ��̼� ���)), ������ ����, ���� �� ����Ʈ �߻�
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
			// �Ѿ��� actor�� �ƴѰ��� ����ɶ����� ������ ����
			CurrentDefaultAttackDamage -= 10.f;
		}
	}
	CurrentDefaultAttackDamage = DefaultAttackDamage;
}
void URevenantCombatComponent::StrongAttack(const TArray<FHitResult>& HitResultsArr)
{
	// �����̵��߿��� ���ݴ�� �����̵�
	if (bTeleportButtonPressed)
	{
		Teleport();
		return;
	}
	if (!CanStrongAttack()) return;

	// ���¸� ���ݻ��·� ����, ((���� �ִϸ��̼� ���)), ������ ����
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

		// ��ź�� ��ȯ�ϱ����� ĳ���Ϳ� ��ź�� ��ȯ�� ��ġ ���̿� LineTrace�� �Ͽ� �浹����
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
	// ��Ÿ���� ������ �ʾҰų�, Idle ���°� �ƴ϶��
	if (!CanFirstSkill()) return;

	// ���� ���� �� ��Ÿ�� ����
	Super::FirstSkillPressed();

	//SetIsOffUpperarmTransformModify(true);

	PlayAttackMontage(ERevenantSkill::ERS_FirstSkill);

	// ��ź ������
	ThrowBomb();
}
void URevenantCombatComponent::FirstSkillReleased()
{
	
}
// �̺�Ʈ �׷��������� ȣ���
void URevenantCombatComponent::FirstSkillEnd()
{
	//SetIsOffUpperarmTransformModify(false);
	bFirstSkillPressed = false;
	Super::FirstSkillEnd();
}


void URevenantCombatComponent::SecondSkillPressed()
{
	if (OwnerCharacter->GetPlayerController() == nullptr) return;

	// �����̵��� ������ �����̰�, ��Ÿ���� �����ٸ�
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
// �̺�Ʈ �׷��������� ȣ���
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
	// Ÿ�̸� ����, ���¼���
	Super::ThirdSkillPressed();

	bThirdSkillPressed = true;
	IsFullBodySkill = true;
	bUseFootIK = false;
	OwnerCharacter->ChangeInputExceptMouse(false);

	// Ư�� �ð��� ������ �ڵ����� ��ų�� ����
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
// �̺�Ʈ �׷��������� ȣ���
void URevenantCombatComponent::ThirdSkillEnd()
{
	// idle ���·� ����
	Super::ThirdSkillEnd();

	OwnerCharacter->ChangeInputExceptMouse(true);
	IsFullBodySkill = false;
	bUseFootIK = true;
}
// �̺�Ʈ �׷��������� ȣ���
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

		// ��Ÿ�� ����
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
	// Idle �����̰�, ź���� �ִ� �������� ����� �Ѵ�
	if (CurrentCombatState == ECombatState::ECS_Idle && CurrentAmmo < MaxAmmo) return true;

	return false;
}


bool URevenantCombatComponent::CanDefaultAttack()
{
	// Super : Idle �����̰� ��Ÿ���� ������
	if(Super::CanDefaultAttack() &&
		CurrentAmmo - DefaultAttackSpendAmmo >= 0) return true;

	return false;
}
bool URevenantCombatComponent::CanStrongAttack()
{
	// Super : Idle �����̰� ��Ÿ���� ������
	if (Super::CanDefaultAttack() &&
		CurrentAmmo - StrongAttackSpendAmmo >= 0) return true;

	return false;
}
bool URevenantCombatComponent::CanFirstSkill()
{
	// Super : ��ų ��Ÿ�� üũ, Idle��������
	if (Super::CanFirstSkill() && RevenantCharacter) return true;

	return false;
}
bool URevenantCombatComponent::CanSecondSkill()
{
	// Super : ��ų ��Ÿ�� üũ, Idle��������
	if (Super::CanSecondSkill() && RevenantCharacter) return true;

	return false;
}
bool URevenantCombatComponent::CanThirdSkill()
{
	// Super : ��ų ��Ÿ�� üũ, Idle��������
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
	// ���� ���� �ʱ�ȭ
	Super::Death();

	// ThirdSkill�� ����ϴ� ���� ���
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

	// SecondSkill�� ����ϴ� ���� ���
	if (ReaperTeleport)
	{
		if (ReaperTeleport->Destroy())
			ReaperTeleport = nullptr;
	}

	bTeleportButtonPressed = false;
	CurrentAmmo = MaxAmmo;
	//SetAmmoHUD(CurrentAmmo);
}