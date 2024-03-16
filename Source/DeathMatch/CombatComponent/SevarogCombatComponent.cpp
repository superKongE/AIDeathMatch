#include "DeathMatch/CombatComponent/SevarogCombatComponent.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveVector.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "DeathMatch/Character/SevarogCharacter.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/HUD/SevarogHUD.h"
#include "DeathMatch/Skill/SevarogSecondSkill.h"
#include "DeathMatch/Skill/SevarogThirdSkill.h"
#include "DeathMatch/Enemy/Enemy.h"

USevarogCombatComponent::USevarogCombatComponent()
{
	MoveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MoveTimeline"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage>AnimMontageAsset(TEXT("AnimMontage'/Game/Blueprint/Animation/Sevarog_Animation/BP_SevarogMontage.BP_SevarogMontage'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>DefaultAttackWeaponTrailAsset(TEXT("NiagaraSystem'/Game/Blueprint/Niagara_Trail.Niagara_Trail'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>SecondSkillParticleAsset(TEXT("ParticleSystem'/Game/ParagonSevarog/FX/Particles/Abilities/Subjugate/FX/Sevarog_SecondSkill.Sevarog_SecondSkill'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>DashTrailParticleAsset(TEXT("ParticleSystem'/Game/ParagonSevarog/FX/Particles/Sevarog_DashTrail.Sevarog_DashTrail'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>HitParticleAsset(TEXT("ParticleSystem'/Game/ParagonSevarog/FX/Particles/Abilities/SoulSiphon/FX/Sevarog_HitParticle.Sevarog_HitParticle'"));
	static ConstructorHelpers::FObjectFinder<UCurveVector>SevarogCurveAsset(TEXT("CurveVector'/Game/Blueprint/SevarogDashCurve.SevarogDashCurve'"));

	static ConstructorHelpers::FClassFinder<ASevarogSecondSkill>SecondSkillClassAsset(TEXT("Blueprint'/Game/Blueprint/Skill/BP_SevarogSecondSkill.BP_SevarogSecondSkill_C'"));
	static ConstructorHelpers::FClassFinder<ASevarogThirdSkill>ThirdSkillClassAsset(TEXT("Blueprint'/Game/Blueprint/Skill/BP_SevarogThirdSkill.BP_SevarogThirdSkill_C'"));

	if (AnimMontageAsset.Succeeded())
		CombatMontage = AnimMontageAsset.Object;
	if (DefaultAttackWeaponTrailAsset.Succeeded())
		DefaultAttackWeaponTrail = DefaultAttackWeaponTrailAsset.Object;
	if (SecondSkillParticleAsset.Succeeded())
		SecondSkillParticle = SecondSkillParticleAsset.Object;
	if (DashTrailParticleAsset.Succeeded())
		DashTrailParticle = DashTrailParticleAsset.Object;
	if (SevarogCurveAsset.Succeeded())
		SevarogCurve = SevarogCurveAsset.Object;
	if (HitParticleAsset.Succeeded())
		HitParticle = HitParticleAsset.Object;

	if (SecondSkillClassAsset.Succeeded())
		SecondSkillClass = SecondSkillClassAsset.Class;
	if (ThirdSkillClassAsset.Succeeded())
		ThirdSkillClass = ThirdSkillClassAsset.Class;
}
void USevarogCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(OwnerCharacter)
		InitTargetArmLength = OwnerCharacter->GetSpringArmComponent()->TargetArmLength;

	for (int i = 0; i < 9; i++)
		WeaponSocketArr.Add(FWeaponSocketInfo());

	if (SevarogCurve && MoveTimeline)
	{
		MoveTrack.BindDynamic(this, &USevarogCombatComponent::UpdateMoveCurve);
		MoveTrackEnd.BindDynamic(this, &USevarogCombatComponent::EndMoveCurve);
		MoveTimeline->SetTimelineFinishedFunc(MoveTrackEnd);
		MoveTimeline->AddInterpVector(SevarogCurve, MoveTrack);
		float Min = 0, Max = 0;
		SevarogCurve->GetTimeRange(Min, Max);
		MoveTimeline->SetTimelineLength(1.28);
	}

	if (DefaultAttackWeaponTrail)
	{
		const USkeletalMeshSocket* WeaponTrailSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket_2"));
		FVector SocketLocation = WeaponTrailSocket->GetSocketLocation(OwnerCharacter->GetMesh());

		FMatrix WeaponSocketMartix;
		WeaponTrailSocket->GetSocketMatrix(WeaponSocketMartix, OwnerCharacter->GetMesh());
		const FVector Direction(WeaponSocketMartix.GetUnitAxis(EAxis::X));

		DefaultAttackWeaponTrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(DefaultAttackWeaponTrail, OwnerCharacter->GetMesh(), FName("WeaponSocket_2"),
			SocketLocation, Direction.Rotation(), EAttachLocation::KeepWorldPosition, false);

		DefaultAttackWeaponTrailComponent->SetVisibility(false);
	}
}
void USevarogCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDefaultAttackDelay)
	{	
		// 공격에 딜레이를 주기 위함 
		CurrentDefaultAttackDelayTime += DeltaTime;
		// 공격키를 계속 누르고 있는 경우 DefaultAttackDelay초 이상이 지나야지만 자동 공격이 이루어진다
		if (CurrentDefaultAttackDelayTime > DefaultAttackDelay)
		{
			if (DefaultAttackPressed)
				DefaultAttackPlay();

			bDefaultAttackDelay = false;
		}
	}

	if (bTakeDown)
		SecondSkillTakeDown(DeltaTime);

	if (bWeaponTrace)
		StartWeaponTrace();

	if (bInterpDashCamera && OwnerCharacter && OwnerCharacter->GetSpringArmComponent())
	{
		CurrentTargetArmLength = FMath::FInterpTo(CurrentTargetArmLength, TargetTargetArmLength, DeltaTime, DashCameraInterpSpeed);
		OwnerCharacter->GetSpringArmComponent()->TargetArmLength = CurrentTargetArmLength;

		if (FMath::Abs(TargetTargetArmLength - CurrentTargetArmLength) < 5.f)
			bInterpDashCamera = false;
	}
}
void USevarogCombatComponent::PlayDefaultAttackMontage(ESevarogSkill State)
{
	if (SevarogCharacter == nullptr) return;

	UAnimInstance* AnimInstance = SevarogCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);
	
		FName SectionName;
		switch (State)
		{
		case ESevarogSkill::ESS_DefaultAttack:
			switch (DefaultAttackCnt)
			{
			case 0:
				SectionName = FName("DefaultAttack1_Slow");
				break;

			case 1:
				SectionName = FName("DefaultAttack2_Slow");
				break;
			}
			break;

		case ESevarogSkill::ESS_StrongAttack:
			SectionName = FName("StrongAttack");
			break;

		case ESevarogSkill::ESS_FirstSkill:
			SectionName = FName("FirstSkill");
			break;

		case ESevarogSkill::ESS_SecondSkill:
			SectionName = FName("SecondSkill");
			break;

		case ESevarogSkill::ESS_SecondSkillReady:
			SectionName = FName("SecondSkill_Ready");
			break;

		case ESevarogSkill::ESS_ThirdSkill:
			SectionName = FName("ThirdSkill");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, CombatMontage);
	}
}
void USevarogCombatComponent::SetCharacter(ARootCharacter* Character)
{
	Super::SetCharacter(Character);

	SevarogCharacter = Cast<ASevarogCharacter>(Character);
}
void USevarogCombatComponent::SetSkillCoolTime()
{
	Super::SetSkillCoolTime();

	DashSkillProgressBar = (100 / DashSkillDelay) * SkillTimerDelay;
	DashSkillCoolTimeMaxCount = 100 / DashSkillProgressBar;
}
void USevarogCombatComponent::SetHUD(AHUD* tempHUD)
{
	Super::SetHUD(tempHUD);

	SevarogHUD = Cast<ASevarogHUD>(HUD);
}



bool USevarogCombatComponent::CanDash()
{
	if (CurrentCombatState == ECombatState::ECS_Idle && bDashTimeEnd) return true;

	return false;
}
void USevarogCombatComponent::Dash()
{
	if (OwnerCharacter == nullptr || PlayerController == nullptr || !CanDash()) return;

	OwnerCharacter->DisableInput(PlayerController);

	FowardVector = OwnerCharacter->GetActorForwardVector();
	RightVector = OwnerCharacter->GetActorRightVector();
	UpVector = OwnerCharacter->GetActorUpVector();
	CurrentLocation = OwnerCharacter->GetActorLocation();

	if (SevarogCurve && MoveTimeline)
	{
		SetCurrentCombatState(ECombatState::ECS_Attack);
		CurrentTargetArmLength = InitTargetArmLength;
		TargetTargetArmLength = StartDashLength;
		bInterpDashCamera = true;
		bDash = true;
		OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		MoveTimeline->PlayFromStart();

		bDashTimeEnd = false;
		GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &USevarogCombatComponent::DashTimeEnd, SkillTimerDelay, true);
	}
}
void USevarogCombatComponent::UpdateMoveCurve(FVector Value)
{
	if (OwnerCharacter == nullptr) return;
	
	FVector Vector = FowardVector * Value.X + RightVector * Value.Y;
	CurrentLocation = Vector;

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);

	FVector ActorLocation = OwnerCharacter->GetActorLocation();
	FVector Start = ActorLocation + Vector;
	FVector End = ActorLocation + Vector + FVector(0.f, 0.f, -138.f);

	if (DashTrailParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DashTrailParticle, ActorLocation);

	//// 뚫을 수 있는 벽인지
	TArray<TEnumAsByte<EObjectTypeQuery>> ETQ;
	ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	TArray<AActor*> IgnoreActors;
	UKismetSystemLibrary::LineTraceSingleForObjects(this, ActorLocation, ActorLocation + Vector * 1.f, ETQ, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);
	FVector tempLocation = ActorLocation + Vector * 5.f + FVector(0.f, 0.f, 200.f);
	FRotator tempRotation = OwnerCharacter->GetActorRotation();
	FVector Normal = HitResult.Normal;
	Normal = FVector(FMath::Abs(Normal.X), Normal.Y, Normal.Z);
	FVector XAxis = FVector(1.0f, 0.0f, 0.0f);	
	// 벽을 뚫을 수 없는 경우
	if (HitResult.bBlockingHit)
	{
		MoveTimeline->Stop();
		EndMoveCurve();
		return;
	}

	FVector Location;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, FQP);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);
	if (HitResult.bBlockingHit)
		Location = HitResult.ImpactPoint;
	else
		Location = End;

	OwnerCharacter->SetActorLocation(Location + FVector(0.f,0.f,88.f));
}
void USevarogCombatComponent::EndMoveCurve()
{
	if (OwnerCharacter == nullptr || PlayerController == nullptr) return;

	SetCurrentCombatState(ECombatState::ECS_Idle);
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	TargetTargetArmLength = InitTargetArmLength;
	bDash = false;
	bInterpDashCamera = true;
	OwnerCharacter->EnableInput(PlayerController);
}
void USevarogCombatComponent::DashTimeEnd()
{
	float percent = FMath::Clamp(100 - (DashSkillProgressBar * DashSkillCoolTimeCount++), 0.f, 100.f);

	SetDashSkillProgressBarHUD(percent);
	if (percent == 0.f)
	{
		DashSkillCoolTimeCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
		bDashTimeEnd = true;
	}
}



void USevarogCombatComponent::ReadyForWeaponTrace()
{
	if (OwnerCharacter == nullptr) return;

	const USkeletalMeshSocket* WeaponSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket_1"));
	FVector Location = WeaponSocket->GetSocketLocation(OwnerCharacter->GetMesh());
	WeaponSocketArr[0].CurrentLocation = Location;

	FMatrix WeaponSocketMartix;
	WeaponSocket->GetSocketMatrix(WeaponSocketMartix, OwnerCharacter->GetMesh());
	const FVector Direction(WeaponSocketMartix.GetUnitAxis(EAxis::X));

	for (int i = 1; i < WeaponSocketCnt; i++)
		WeaponSocketArr[i].CurrentLocation = Location + Direction * WeaponSocketTraceDistance * i;
}
void USevarogCombatComponent::StartWeaponTrace()
{
	const USkeletalMeshSocket* WeaponSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket_1"));
	FVector Location = WeaponSocket->GetSocketLocation(OwnerCharacter->GetMesh());

	FMatrix WeaponSocketMartix;
	WeaponSocket->GetSocketMatrix(WeaponSocketMartix, OwnerCharacter->GetMesh());
	const FVector Direction(WeaponSocketMartix.GetUnitAxis(EAxis::X));

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);

	for (int i = 0; i < WeaponSocketCnt; i++)
	{
		FWeaponSocketInfo WeaponSocketInfo = WeaponSocketArr[i];

		// 이전 프레임의 소켓으로부터 현재 프레임의 소켓까지 LineTrace수행
		GetWorld()->LineTraceSingleByChannel(HitResult, WeaponSocketInfo.CurrentLocation, Location + Direction * WeaponSocketTraceDistance * i, ECollisionChannel::ECC_Visibility, FQP);
		if (HitResult.bBlockingHit)
		{
			// impact point에 이펙트 생성
			if (HitParticle && !HitParticleSet.Contains(HitResult.GetActor()))
			{
				HitParticleSet.Emplace(HitResult.GetActor());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint);
			}

			// 맞은 대상이 캐릭터고 벽에 가려져있지 않으면
			ACharacter* Chr = Cast<ACharacter>(HitResult.GetActor());
			if (Chr && !IsCharacterBehindWall(Chr))
				HitCharSet.Emplace(Chr);
		}

		WeaponSocketArr[i].CurrentLocation = Location + Direction * WeaponSocketTraceDistance * i;
	}
}
bool USevarogCombatComponent::IsCharacterBehindWall(ACharacter* Chr)
{
	AEnemy* Enemy = Cast<AEnemy>(Chr);
	if (Enemy == nullptr) return false;

	TArray<FName>& ViewTargetsArr = Enemy->GetViewTargets();
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(Enemy);
	FQP.AddIgnoredActor(OwnerCharacter);

	int32 cnt = 0;
	for (FName& TargetBone : ViewTargetsArr)
	{
		FVector SocketLocation = Enemy->GetMesh()->GetSocketLocation(TargetBone);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, DefaultAttackStartLocation, SocketLocation, ECollisionChannel::ECC_Visibility, FQP);
		//DrawDebugLine(GetWorld(), DefaultAttackStartLocation, SocketLocation, FColor::Red, true);

		if (HitResult.bBlockingHit)
			cnt++;
	}

	if (cnt == ViewTargetsArr.Num()) return true;

	return false;
}



void USevarogCombatComponent::DefaultAttack(const TArray<FHitResult>& HitResultsArr)
{
	DefaultAttackPressed = true;

	// 첫번째 스킬을 써서 공속이 빨라질 시
	// 마우스를 꾹 누르는것 보다 클릭으로 더 빠르게 콤보 공격 가능하게 끔
	if (bDefaultAttackDelay)
	{
		bDefaultAttackDelay = false;

		ReadyForWeaponTrace();

		DefaultAttackPlay();
	}

	if (!CanDefaultAttack()) return;

	ReadyForWeaponTrace();

	DefaultAttackPlay();
}
void USevarogCombatComponent::DefaultAttackPlay()
{
	if (OwnerCharacter == nullptr) return;

	DefaultAttackStartLocation = OwnerCharacter->GetActorLocation();

	bWeaponTrace = true;
	SetCurrentCombatState(ECombatState::ECS_Attack);

	PlayDefaultAttackMontage(ESevarogSkill::ESS_DefaultAttack);

	DefaultAttackWeaponTrailComponent->SetVisibility(true);

	DefaultAttackCnt = (DefaultAttackCnt + 1) % 2;
}
// 무기를 다 휘두르면 이벤트 그래프에서 버프가 켜진상탠지 확인하기 위해 호출됨 + 데미지 주기 
void USevarogCombatComponent::DefaultAttackBuff()
{
	DefaultAttackWeaponTrailComponent->SetVisibility(false);
	bWeaponTrace = false;

	// 버프가 켜져있으면
	if (bFastBuff)
	{
		CurrentDefaultAttackDelayTime = 0.f;
		bDefaultAttackDelay = true;
	}

	for (ACharacter* Chr : HitCharSet)
		UGameplayStatics::ApplyDamage(Chr, WeaponDamage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
	HitCharSet.Reset();
	HitParticleSet.Reset();
}
void USevarogCombatComponent::LeftClickReleased()
{
	DefaultAttackPressed = false;
}
bool USevarogCombatComponent::CanDefaultAttack()
{
	if (CurrentCombatState == ECombatState::ECS_Idle) return true;

	return false;
}


void USevarogCombatComponent::StrongAttack(const TArray<FHitResult>& HitResultsArr)
{
	
}
void USevarogCombatComponent::RightClickReleased()
{
	if (!StrongAttackDelayEnd) return;

	const FVector DirVector = SevarogCharacter->GetControlRotation().Vector();
	float result = FVector::DotProduct(UpVector, DirVector);

	float force = 1.f;
	if (result <= 0.9f)
		force = force * UpForce;
	else
		force = force * FowardForce;
	
	FVector velocity = (SevarogCharacter->GetControlRotation().Vector() * 100.f + SevarogCharacter->GetActorLocation()) - SevarogCharacter->GetActorLocation();	
	FVector LaunchVelocity = velocity * force;

	SevarogCharacter->LaunchCharacter(LaunchVelocity, true, true);

	StrongAttackDelayEnd = false;
	GetWorld()->GetTimerManager().SetTimer(FirstSkillBuffTimerHandle, this, &USevarogCombatComponent::StrongAttackTimeEnd, StrongAttackDealy, false);
}



void USevarogCombatComponent::FirstSkillPressed()
{
	if (!CanFirstSkill()) return;

	// 공격 상태 및 스킬 프로그레스바 업데이트
	Super::FirstSkillPressed();

	PlayDefaultAttackMontage(ESevarogSkill::ESS_FirstSkill);

	// 쿨타임 설정
	bFastBuff = true;
	GetWorld()->GetTimerManager().SetTimer(FirstSkillBuffTimerHandle, this, &USevarogCombatComponent::FirstSkillBuffEnd, FirstSkillBuffTime, false);
}
void USevarogCombatComponent::FirstSkillBuffEnd()
{
	bFastBuff = false;
}



void USevarogCombatComponent::SecondSkillPressed()
{
	if (!CanSecondSkill()) return;

	ActiveKeyboardInput(false);

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(OwnerCharacter);
	GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() - FVector(0.f, 0.f, 5000.f), ECollisionChannel::ECC_Visibility, FQP);
	float distance = OwnerCharacter->GetActorLocation().Z - HitResult.ImpactPoint.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// 공중에 있으면 내려찍기
	if (distance > SecondSkillReadyFallDistance)
	{
		bTakeDownFinish = false;
		bTakeDown = true;
		SecondSkillCurrentLocation = SevarogCharacter->GetActorLocation();
		SecondSkillTargetLocation = HitResults[0].ImpactPoint;

		PlayDefaultAttackMontage(ESevarogSkill::ESS_SecondSkillReady);
	}
	else
	{
		DefaultAttackWeaponTrailComponent->SetVisibility(true);
		PlayDefaultAttackMontage(ESevarogSkill::ESS_SecondSkill);
	}
}
void USevarogCombatComponent::SecondSkillSwingEnd()
{
	DefaultAttackWeaponTrailComponent->SetVisibility(false);

	if (OwnerCharacter == nullptr) return;

	const USkeletalMeshSocket* WeaponSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket_2"));
	if (WeaponSocket)
	{
		if (SecondSkillParticle)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SecondSkillParticle, WeaponSocket->GetSocketLocation(OwnerCharacter->GetMesh()))->SetWorldScale3D(FVector(2.f,2.f,2.f));

		if (SecondSkillClass)
		{
			ASevarogSecondSkill* SecondSkill = GetWorld()->SpawnActor<ASevarogSecondSkill>(SecondSkillClass, WeaponSocket->GetSocketLocation(OwnerCharacter->GetMesh()), FRotator::ZeroRotator);
			SecondSkill->SetOwner(OwnerCharacter);
		}
	}
}
void USevarogCombatComponent::SecondSkillJumpToLoop()
{
	if (OwnerCharacter == nullptr) return;

	UAnimInstance* AnimInstance = SevarogCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_JumpToSection(FName("SecondSkill_Ready"), CombatMontage);
	}
}
void USevarogCombatComponent::SecondSkillEnd()
{
	Super::SecondSkillEnd();

	ActiveKeyboardInput(true);
}
void USevarogCombatComponent::SecondSkillTakeDown(float DeltaTime)
{
	SecondSkillCurrentLocation = FMath::VInterpTo(SecondSkillCurrentLocation, SecondSkillTargetLocation, DeltaTime, SecondSkillInterpSpeed);

	SevarogCharacter->SetActorLocation(SecondSkillCurrentLocation, true);

	if ((SecondSkillTargetLocation - SecondSkillCurrentLocation).Size() <= SecondSkillReadyFallDistance && !bTakeDownFinish)
	{
		bTakeDownFinish = true;
		OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		DefaultAttackWeaponTrailComponent->SetVisibility(true);
		PlayDefaultAttackMontage(ESevarogSkill::ESS_SecondSkill);
	}

	if ((SecondSkillTargetLocation - SecondSkillCurrentLocation).Size() < 5.f)
		bTakeDown = false;
}


void USevarogCombatComponent::ThirdSkillPressed()
{
	if (!CanThirdSkill()) return;

	// 공격 상태 및 쿨타임 설정
	Super::ThirdSkillPressed();

	DefaultAttackWeaponTrailComponent->SetVisibility(true);
	ActiveKeyboardInput(false);
	PlayDefaultAttackMontage(ESevarogSkill::ESS_ThirdSkill);
}
void USevarogCombatComponent::ThirdSkillSwingEnd()
{
	DefaultAttackWeaponTrailComponent->SetVisibility(false);
	ActiveKeyboardInput(true);
}
void USevarogCombatComponent::ThirdSkillAttack() 
{
	if (ThirdSkillClass && SevarogCharacter)
	{
		ASevarogThirdSkill* ThirdSkill = GetWorld()->SpawnActor<ASevarogThirdSkill>(ThirdSkillClass, SevarogCharacter->GetThirdSkillSpawnPoint(), SevarogCharacter->GetThirdSkillRotator());
		if(ThirdSkill)
			ThirdSkill->SetOwnerInfo(SevarogCharacter);
	}
}




void USevarogCombatComponent::SetDashSkillProgressBarHUD(const float percent)
{
	if (SevarogHUD)
	{
		SevarogHUD->SetDashSkillProgressBarHUD(percent);
	}
}