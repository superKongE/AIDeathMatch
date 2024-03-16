#include "DeathMatch/CombatComponent/GideonCombatComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DecalActor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"

#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/Character/GideonCharacter.h"
#include "DeathMatch/Weapon/GideonProjectile.h"
#include "DeathMatch/Skill/GideonSecondSkill.h"
#include "DeathMatch/Skill/GideonThirdSkill.h"

UGideonCombatComponent::UGideonCombatComponent()
{	
	IsFly = false;
	bFirstSkillInterp = false;
	CurrentTime = 0.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage>AnimMontageAsset(TEXT("AnimMontage'/Game/Blueprint/Animation/Gideon_Animation/Gideon_CombatMontage.Gideon_CombatMontage'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>SecondSkillDecalInterfaceAsset(TEXT("Material'/Game/Blueprint/Decal/Gideon_Decal.Gideon_Decal'"));

	static ConstructorHelpers::FClassFinder<AActor>GideonProjectileOneAsset(TEXT("Blueprint'/Game/Blueprint/Weapon/BP_Gideon_Default_Projectile_1.BP_Gideon_Default_Projectile_1_C'"));
	static ConstructorHelpers::FClassFinder<AActor>GideonProjectileTwoAsset(TEXT("Blueprint'/Game/Blueprint/Weapon/BP_Gideon_Default_Projectile_2.BP_Gideon_Default_Projectile_2_C'"));
	static ConstructorHelpers::FClassFinder<AActor>GideonProjectileThreeAsset(TEXT("Blueprint'/Game/Blueprint/Weapon/BP_Gideon_Default_Projectile_3.BP_Gideon_Default_Projectile_3_C'"));
	static ConstructorHelpers::FClassFinder<AActor>GideonStrongProjectileAsset(TEXT("Blueprint'/Game/Blueprint/Weapon/BP_Gideon_Strong_Projectile.BP_Gideon_Strong_Projectile_C'"));
	static ConstructorHelpers::FClassFinder<AActor>SecondSkillClassAsset(TEXT("Blueprint'/Game/Blueprint/Skill/BP_GideonSecondSkill.BP_GideonSecondSkill_C'"));
	static ConstructorHelpers::FClassFinder<AActor>ThirdSkillClassAsset(TEXT("Blueprint'/Game/Blueprint/Skill/BP_GideonThirdSkill.BP_GideonThirdSkill_C'"));

	if (AnimMontageAsset.Succeeded())
		CombatMontage = AnimMontageAsset.Object;
	if (SecondSkillDecalInterfaceAsset.Succeeded())
		SecondSkillDecalInterface = SecondSkillDecalInterfaceAsset.Object;

	if (GideonProjectileOneAsset.Succeeded())
		AGideonDefaultProjectileClass_one = GideonProjectileOneAsset.Class;
	if (GideonProjectileTwoAsset.Succeeded())
		AGideonDefaultProjectileClass_two = GideonProjectileTwoAsset.Class;
	if (GideonProjectileThreeAsset.Succeeded())
		AGideonDefaultProjectileClass_three = GideonProjectileThreeAsset.Class;
	if (GideonStrongProjectileAsset.Succeeded())
		AGideonStrongProjectileClass = GideonStrongProjectileAsset.Class;
	if (SecondSkillClassAsset.Succeeded())
		SecondSkillClass = SecondSkillClassAsset.Class;
	if (ThirdSkillClassAsset.Succeeded())
		ThirdSkillClass = ThirdSkillClassAsset.Class;
}
void UGideonCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SecondSkillDecalInterface)
	{
		FActorSpawnParameters FSP;
		FSP.Owner = OwnerCharacter;
		FSP.Instigator = OwnerCharacter;

		SecondSkillDecal = GetWorld()->SpawnActor<ADecalActor>(SecondSkillDecalClass, FVector(0.f, 0.f, 0.f),  FRotator(0.f,0.f,0.f), FSP);
		if (SecondSkillDecal)
		{
			FVector Scale = SecondSkillDecal->GetActorScale();
			SecondSkillDecal->SetActorScale3D(Scale * SecondSkillDecalScale);
			SecondSkillDecal->SetDecalMaterial(SecondSkillDecalInterface);
		}
		if(SecondSkillDecal)
			SecondSkillDecalInst = SecondSkillDecal->CreateDynamicMaterialInstance();
		if(SecondSkillDecalInst)
			SecondSkillDecalInst->SetScalarParameterValue(FName("Alpha"), 0.f);
	}
}
void UGideonCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FlyingWithJump(DeltaTime);

	SetSecondSkillDecalLocation();

	if (bThirdSkillRaiseUp && CurrentTime < RaiseUpTime)
	{
		CurrentTime += DeltaTime;

		RaiseUpCurrentZ = FMath::FInterpTo(RaiseUpCurrentZ, RaiseUpDistance, DeltaTime, RaiseUpInterpSpeed);

		OwnerCharacter->SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, RaiseUpCurrentZ));
	}

	ThirdSkilFallDown(DeltaTime);

	if (bRightClickInterp)
	{
		CurrentLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, RightClickInterpSpeed);
		if ((CurrentLocation - TargetLocation).Size() <= 310.f)
			bRightClickInterp = false;

		OwnerCharacter->SetActorLocation(CurrentLocation);
	}
}
void UGideonCombatComponent::PlayDefaultAttackMontage(EGideonSkill State)
{
	UAnimInstance* AnimInstance = GideonCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);

		FName SectionName;
		switch (State)
		{
		case EGideonSkill::EGS_DefaultAttack:
			switch (DefaultAttackCount)
			{
			case 0:
				SectionName = FName("DefaultAttack1");
				break;

			case 1:
				SectionName = FName("DefaultAttack2");
				break;

			case 2:
				SectionName = FName("DefaultAttack3");
				break;
			}
			break;

		case EGideonSkill::EGS_StrongAttack:
			SectionName = FName("StrongAttack");
			break;

		case EGideonSkill::EGS_FirstSkill:
			SectionName = FName("FirstSkill");
			break;

		case EGideonSkill::EGS_SecondSkill:
			SectionName = FName("SecondSkill");
			break;

		case EGideonSkill::EGS_ThirdSkill:
			SectionName = FName("ThirdSkill");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, CombatMontage);
	}
}
void UGideonCombatComponent::SetCharacter(ARootCharacter* Character)
{
	Super::SetCharacter(Character);

	GideonCharacter = Cast<AGideonCharacter>(Character);
}


void UGideonCombatComponent::Ready()
{
	
}
void UGideonCombatComponent::Death()
{
	Super::Death();

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (SpawnedGideonThirdSkill)
	{
		SpawnedGideonThirdSkill->Destroy();
		SpawnedGideonThirdSkill = nullptr;
	}
}




// 이벤트 그래프에의해 호출되는 함수
void UGideonCombatComponent::DefaultAttackShoot()
{
	FName hand;
	UClass* SpawnClass = nullptr;
	switch (DefaultAttackCount)
	{
	case 0:
		if (AGideonDefaultProjectileClass_one)
			SpawnClass = AGideonDefaultProjectileClass_one;
		hand = FName("hand_r_socket");
		break;

	case 1:
		if (AGideonDefaultProjectileClass_two)
			SpawnClass = AGideonDefaultProjectileClass_two;
		hand = FName("hand_l_socket");
		break;

	case 2:
		if (AGideonDefaultProjectileClass_three)
			SpawnClass = AGideonDefaultProjectileClass_three;
		hand = FName("hand_r_socket");
		break;
	}
	DefaultAttackCount = (DefaultAttackCount + 1) % 3;

	const USkeletalMeshSocket* handsocket = GideonCharacter->GetMesh()->GetSocketByName(hand);
	if (handsocket && SpawnClass)
	{
		FVector ToTarget = CurrentHitResult.ImpactPoint - handsocket->GetSocketLocation(GideonCharacter->GetMesh());
		ToTarget.Normalize();

		FHitResult tempHitResult;

		FActorSpawnParameters FSP;
		FSP.Owner = GideonCharacter;
		AGideonProjectile* GideonProjectile = GetWorld()->SpawnActor<AGideonProjectile>(SpawnClass, handsocket->GetSocketLocation(GideonCharacter->GetMesh()), ToTarget.Rotation(), FSP);

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Emplace(GideonCharacter);
		IgnoreActors.Emplace(GideonProjectile);
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByChannel(tempHitResult, GetCrosshairStartLocation(), GideonProjectile->GetActorLocation(), ECollisionChannel::ECC_Visibility, FQP);
		if (tempHitResult.bBlockingHit)
			GideonProjectile->Hit(tempHitResult.ImpactPoint);
	}
}
void UGideonCombatComponent::DefaultAttack(const TArray<FHitResult>& HitResultsArr)
{
	if (!CanDefaultAttack()) return;

	// 2번째 스킬을 누른상태에서 마우스 공격시 취소하면서 공격
	if (bSecondSkillPressed)
		CancleSecondSkill();

	// 콤보 공격 실패시 원래 공격으로 돌아감
	if (DefaultAttackDelayEnd)
		DefaultAttackCount = 0;

	SetCurrentCombatState(ECombatState::ECS_Attack);

	GideonCharacter = GideonCharacter == nullptr ? Cast<AGideonCharacter>(OwnerCharacter) : GideonCharacter;
	if (GideonCharacter == nullptr || GideonCharacter->GetPlayerController() == nullptr) return;

	PlayDefaultAttackMontage(EGideonSkill::EGS_DefaultAttack);

	CurrentHitResult = HitResultsArr[0];

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (DefaultAttackTimerHandle.IsValid())
		TimerManager.ClearTimer(DefaultAttackTimerHandle);

	DefaultAttackDelayEnd = false;
	TimerManager.SetTimer(DefaultAttackTimerHandle, this, &UGideonCombatComponent::DefaultAttackTimeEnd, DefaultAttackDelay, false);
}
bool UGideonCombatComponent::CanDefaultAttack()
{
	if (CurrentCombatState == ECombatState::ECS_Idle) return true;

	return false;
}


// 우클릭
void UGideonCombatComponent::StrongAttack(const TArray<FHitResult>& HitResultsArr)
{
	if (OwnerCharacter == nullptr) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	FVector Start = CrosshairWorldPosition;
	float DistanceToCharacter = (OwnerCharacter->GetActorLocation() - Start).Size();
	Start += CrosshairWorldDirection * (DistanceToCharacter + 10.f);
	FVector End = Start + CrosshairWorldDirection * RightClickTraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, FQP);
	
	// 장애물이 없다면
	if (!HitResult.bBlockingHit)
	{
		CurrentLocation = OwnerCharacter->GetActorLocation();
		TargetLocation = End;
		bRightClickInterp = true;
	}
	else
	{
		// 해당 벽을 뚫고가면 맵 밖인지 체크
		FHitResult ObjectHitResult;
		FCollisionObjectQueryParams FOQP;
		FOQP.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
		GetWorld()->LineTraceSingleByObjectType(ObjectHitResult, Start, End, FOQP);
		if (ObjectHitResult.ImpactPoint == HitResult.ImpactPoint)
			return;

		Start = Start + CrosshairWorldDirection * RightClickReverseTraceDistance;

		TArray<TEnumAsByte<EObjectTypeQuery>> ETQ;
		ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		TArray<AActor*> IgnoreActors;
		TArray<FHitResult> HitResultArr;
	
		// 캐릭터 앞으로 어떤 장애물이 있는지 조사
		UKismetSystemLibrary::LineTraceMultiForObjects(this, Start, HitResult.ImpactPoint, ETQ, false, IgnoreActors, EDrawDebugTrace::None, HitResultArr, true);

		float CapsuleRadius = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
		TArray<AActor*> ActorsToIgnoreArr;

		int32 Idx = HitResultArr.Num() - 1;

		for (Idx; Idx >= 0; Idx--)
		{
			FHitResult TempHitResult = HitResultArr[Idx];

			FVector CapsuleLocation = TempHitResult.ImpactPoint + (CrosshairWorldDirection * (CapsuleRadius + 50.f));
			bool bCanTeleport = GetWorld()->FindTeleportSpot(OwnerCharacter, CapsuleLocation, OwnerCharacter->GetActorRotation());

			// 해당 위치로 이동가능한지
			if (bCanTeleport)
			{
				GetWorld()->LineTraceSingleByChannel(HitResult, CapsuleLocation, CapsuleLocation + FVector(0.f, 0.f, -RightClickTraceDistance), ECollisionChannel::ECC_Visibility);
				if (HitResult.bBlockingHit)
				{
					CurrentLocation = OwnerCharacter->GetActorLocation();
					TargetLocation = CapsuleLocation + CrosshairWorldDirection * AddDistanceforInterp;
					bRightClickInterp = true;
					break;
				}
			}
		}
	}
}


void UGideonCombatComponent::FirstSkillPressed()
{
	if (OwnerCharacter == nullptr || !CanFirstSkill()) return;

	IsFly = !IsFly;

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);
	// 올라가기
	if (IsFly)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

		GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, FirstSkillHeight), ECollisionChannel::ECC_Visibility, FQP);
		if (HitResult.bBlockingHit)
			TargetZ = HitResult.ImpactPoint.Z;
		else
			TargetZ = FirstSkillHeight + OwnerCharacter->GetActorLocation().Z;
	}
	// 내려가기
	else
	{
		OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		OwnerCharacter->DisableInput(OwnerCharacter->GetPlayerController());
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() - FVector(0.f, 0.f, 10000.f), ECollisionChannel::ECC_Visibility, FQP);
		if (HitResult.bBlockingHit)
			TargetZ = HitResult.ImpactPoint.Z + OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FirstSkillDelayEnd = false;
		GetWorld()->GetTimerManager().SetTimer(FirstSkillTimerHandle, this, &UGideonCombatComponent::FirstSkillTimeEnd, SkillTimerDelay, true);
	}

	bFirstSkillInterp = true;
}
void UGideonCombatComponent::Jump()
{
	if (OwnerCharacter->GetActorLocation().Z > Max_ZHeight) return;

	TargetZ = JumpHeight + OwnerCharacter->GetActorLocation().Z;
	bFirstSkillInterp = true;
}
void UGideonCombatComponent::FlyingWithJump(float DeltaTime)
{
	if (OwnerCharacter && bFirstSkillInterp)
	{
		float CurrentZ = FMath::FInterpTo(OwnerCharacter->GetActorLocation().Z, TargetZ, DeltaTime, FirstSkillInterpSpeed);
		if (FMath::Abs(TargetZ - CurrentZ) <= 10.f)
		{
			OwnerCharacter->EnableInput(OwnerCharacter->GetPlayerController());
			bFirstSkillInterp = false;
		}

		FVector Location = OwnerCharacter->GetActorLocation();
		Location.Z = CurrentZ;
		OwnerCharacter->SetActorLocation(Location, true);
	}
}



void UGideonCombatComponent::CancleSecondSkill()
{
	bSecondSkillPressed = false;

	if (SecondSkillDecal && SecondSkillDecalInst)
	{
		SecondSkillDecalInst->SetScalarParameterValue(FName("Alpha"), 0.f);
	}
}
void UGideonCombatComponent::SecondSkillPressed()
{
	// 팔을 들어올리며 decal(범위표시) 생성
	if (OwnerCharacter == nullptr || !CanSecondSkill()) return;

	bSecondSkillPressed = true;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage);

		AnimInstance->Montage_JumpToSection(FName("SecondSkillStart"), CombatMontage);
	}

	if (SecondSkillDecal && SecondSkillDecalInst)
	{
		SecondSkillDecalInst->SetScalarParameterValue(FName("Alpha"), 0.5f);
		SecondSkillDecal->SetActorLocation(HitResults[0].ImpactPoint);
	}
}
void UGideonCombatComponent::SecondSkillReleased()
{
	if (OwnerCharacter == nullptr || !bSecondSkillPressed) return;

	bSecondSkillPressed = false;

	// 팔을 내리며 스킬 발동
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage);

		AnimInstance->Montage_JumpToSection(FName("SecondSkillEnd"), CombatMontage);
	}

	if (SecondSkillDecal && SecondSkillDecalInst)
		SecondSkillDecalInst->SetScalarParameterValue(FName("Alpha"), 0.f);

	FActorSpawnParameters FSP;
	FSP.Owner = OwnerCharacter;
	FSP.Instigator = OwnerCharacter;
	GetWorld()->SpawnActor<AGideonSecondSkill>(SecondSkillClass, SecondSkillSpawnLocation, FRotator(0.f, 0.f, 0.f), FSP);

	SetCurrentCombatState(ECombatState::ECS_Attack);
	SetSecondSkillCoolTime();
}
void UGideonCombatComponent::SetSecondSkillCoolTime()
{
	SecondSkillDelayEnd = false;

	OwnerCharacter->GetWorldTimerManager().SetTimer(
		SecondSkillTimerHandle,
		this,
		&URootCombatComponent::SecondSkillTimeEnd,
		SkillTimerDelay,
		true
	);
}
void UGideonCombatComponent::SecondSkillJumpToLoop()
{
	if (OwnerCharacter == nullptr) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage);

		AnimInstance->Montage_JumpToSection(FName("SecondSkillLoop"), CombatMontage);
	}
}
void UGideonCombatComponent::SetSecondSkillDecalLocation()
{
	if (bSecondSkillPressed && SecondSkillDecal && HitResults[0].bBlockingHit)
	{
		SecondSkillSpawnLocation = HitResults[0].ImpactPoint;
		SecondSkillDecal->SetActorLocation(SecondSkillSpawnLocation);
	}
}



void UGideonCombatComponent::ThirdSkilFallDown(float DeltaTime)
{
	if (OwnerCharacter && bThirdSkillFallDown)
	{
		float CurrentZ = FMath::FInterpTo(OwnerCharacter->GetActorLocation().Z, TargetZ, DeltaTime, FirstSkillInterpSpeed);
		if (!OwnerCharacter->GetCharacterMovement()->IsFalling())
		{
			bCanRaiseUp = true;
			SpawnThirdSkill();
			bThirdSkillPressed = true;
			bThirdSkillFallDown = false;
		}

		FVector Location = OwnerCharacter->GetActorLocation();
		Location.Z = CurrentZ;
		OwnerCharacter->SetActorLocation(Location, true);
	}
}
void UGideonCombatComponent::SpawnThirdSkill()
{
	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	if (ThirdSkillClass)
	{
		FActorSpawnParameters FSP;
		FSP.Owner = OwnerCharacter;
		FSP.Instigator = OwnerCharacter;
		SpawnedGideonThirdSkill = GetWorld()->SpawnActor<AGideonThirdSkill>(ThirdSkillClass, OwnerCharacter->GetActorLocation(), FRotator(0.f, 0.f, 0.f), FSP);
	}
}
void UGideonCombatComponent::ThirdSkillPressed()
{
	FVector Location = OwnerCharacter->GetActorLocation();
	bool bCanTeleport = GetWorld()->FindTeleportSpot(OwnerCharacter, Location, OwnerCharacter->GetActorRotation());

	if (OwnerCharacter == nullptr || !CanThirdSkill() || !bCanTeleport) return;

	bRightClickInterp = false;
	bFirstSkillInterp = false;
	
	// 공격 상태 및 쿨타임 설정
	Super::ThirdSkillPressed();

	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
	ActiveKeyboardInput(false);

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);
	GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() - FVector(0.f, 0.f, 5000.f), ECollisionChannel::ECC_Visibility, FQP);
	float distance = OwnerCharacter->GetActorLocation().Z - HitResult.ImpactPoint.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 공중에 특정 높이보다 높으면 낙하 후 스킬 실행
	if (distance > NeedToFallingDownDistance)
	{
		bCanRaiseUp = false;
		IsFly = false;

		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		if (HitResult.bBlockingHit)
			TargetZ = HitResult.ImpactPoint.Z + OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 300.f;

		bThirdSkillFallDown = true;
	}
	// 바로 실행
	else
	{
		bThirdSkillPressed = true;
		bCanRaiseUp = true;
		SpawnThirdSkill();
	}
}
void UGideonCombatComponent::ThirdSkillEnd()
{
	Super::ThirdSkillEnd();
	
	ActiveKeyboardInput(true);
	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bThirdSkillPressed = false;
}
// 블루프린트에서 호출됨
void UGideonCombatComponent::ThirdSkillRaiseUp()
{
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	CurrentTime = 0.f;
	bThirdSkillPressed = false;
	bThirdSkillRaiseUp = true;
}
