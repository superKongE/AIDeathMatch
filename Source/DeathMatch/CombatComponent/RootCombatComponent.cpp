#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "DeathMatch/Character/RootCharacter.h"
#include "DeathMatch/Character/ReaperTeleport.h"
#include "DeathMatch/HUD/RootHUD.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/Enemy/Enemy.h"

URootCombatComponent::URootCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UTexture2D>CrosshairCenterAsset(TEXT("Texture2D'/Game/Crosshairs/Primary/Crosshair_Center.Crosshair_Center'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D>CrosshairLeftAsset(TEXT("Texture2D'/Game/Crosshairs/Primary/Crosshair_Left.Crosshair_Left'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D>CrosshairRightAsset(TEXT("Texture2D'/Game/Crosshairs/Primary/Crosshair_Right.Crosshair_Right'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D>CrosshairTopAsset(TEXT("Texture2D'/Game/Crosshairs/Primary/Crosshair_Top.Crosshair_Top'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D>CrosshairBottomAsset(TEXT("Texture2D'/Game/Crosshairs/Primary/Crosshair_Bottom.Crosshair_Bottom'"));

	if (CrosshairCenterAsset.Succeeded())
		CenterCrossHair = CrosshairCenterAsset.Object;
	if (CrosshairLeftAsset.Succeeded())
		LeftCrossHair = CrosshairLeftAsset.Object;
	if (CrosshairRightAsset.Succeeded())
		RightCrossHair = CrosshairRightAsset.Object;
	if (CrosshairTopAsset.Succeeded())
		UpCrossHair = CrosshairTopAsset.Object;
	if (CrosshairBottomAsset.Succeeded())
		DownCrossHair = CrosshairBottomAsset.Object;
	
}
void URootCombatComponent::BeginPlay() 
{
	Super::BeginPlay();	
	
	SetCrosshairTransform();
	SetSkillCoolTime();
}
void URootCombatComponent::SetSkillCoolTime()
{
	FirstSkiilProgressBar = (100 / FirstSkillDelay) * SkillTimerDelay; // SkillTimerDelay 마다 프로그레스바가 감소될 양
	FirstSkillCoolTimeMaxCount = 100 / FirstSkiilProgressBar; // 몇번 호출될 지

	SecondSkillProgressBar = (100 / SecondSkillDelay) * SkillTimerDelay;
	SecondSkillCoolTimeMaxCount = 100 / SecondSkillProgressBar;

	ThirdSkillProgressBar = (100 / ThirdSkillDelay) * SkillTimerDelay;
	ThirdSkillCoolTimeMaxCount = 100 / ThirdSkillProgressBar;
}
void URootCombatComponent::SetCrosshairTransform()
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
}
void URootCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CanUseFootIK();
	TraceUnderCrosshair();
	SetCrossHair(DeltaTime);
}
void URootCombatComponent::CanUseFootIK()
{
	if (OwnerCharacter && OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		bUseFootIK = false;
	}
	else
	{
		bUseFootIK = true;
	}
}
void URootCombatComponent::SetCharacter(ARootCharacter* Character)
{
	OwnerCharacter = Character;
}
void URootCombatComponent::SetHUD(AHUD* tempHUD)
{
	HUD = Cast<ARootHUD>(tempHUD);
}
void URootCombatComponent::SetController(class ARootPlayerController* Controller)
{
	PlayerController = Controller;
}




// 크로스헤어로 부터 linetrace
void URootCombatComponent::TraceUnderCrosshair()
{
	if (OwnerCharacter == nullptr) return;

	if (GEngine && OwnerCharacter && HUD)
	{
		SetCrosshairTransform();
		if (CrosshairWorldPosition == FVector::ZeroVector || CrosshairWorldDirection == FVector::ZeroVector) return;

		// 크로스헤어가 3d 공간에서의 시작점
		FVector Start = CrosshairWorldPosition;
		// 크로스헤어와 캐릭터간의 거리
		float DistanceToCharacter = (OwnerCharacter->GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		CrosshairStartLocation = CrosshairWorldPosition + CrosshairWorldDirection * DistanceToCharacter;
		// 크로스헤어가 3d 공간에서의 끝점
		FVector End = Start + CrosshairWorldDirection * TraceDistance;
		
		TArray<AActor*> IgnoreActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> ETQ;
		ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		ETQ.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
		UKismetSystemLibrary::LineTraceMultiForObjects(this, Start, End, ETQ, false, IgnoreActors, EDrawDebugTrace::None, HitResults, true);
		if (!HitResults.IsEmpty())
		{
			OwnerCharacter->SetHitTargetLocation(HitResults[0].ImpactPoint);

			ACharacter* Chr = Cast<ACharacter>(HitResults[0].GetActor());
			if (Chr && HUD)
			{
				AEnemy* Enemy = Cast<AEnemy>(Chr);
				if(Enemy && !Enemy->GetIsDeath())
					HUD->HUDPackage.CrosshairColor = FLinearColor::Red;
				else
					HUD->HUDPackage.CrosshairColor = FLinearColor::White;
			}
			else if (HUD)
			{
				HUD->HUDPackage.CrosshairColor = FLinearColor::White;
			}
		}
		else
		{
			FHitResult Hit;
			Hit.ImpactPoint = End;
			HitResults.Add(Hit);
			OwnerCharacter->SetHitTargetLocation(Hit.ImpactPoint);
			HUD->HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}
// 크로스헤어 값 설정(색깔, 벌어짐 정도 등)
void URootCombatComponent::SetCrossHair(float DeltaTime)
{
	if (OwnerCharacter == nullptr || PlayerController == nullptr) return;

	HUD = HUD == nullptr ? Cast<ARootHUD>(PlayerController->GetHUD()) : HUD;

	if (HUD && LeftCrossHair && RightCrossHair && UpCrossHair && DownCrossHair && CenterCrossHair)
	{
		HUD->HUDPackage.CenterCrossHair = CenterCrossHair;
		HUD->HUDPackage.LeftCrossHair = LeftCrossHair;
		HUD->HUDPackage.RightCrossHair = RightCrossHair;
		HUD->HUDPackage.UpCrossHair = UpCrossHair;
		HUD->HUDPackage.DownCrossHair = DownCrossHair;

		if (OwnerCharacter->GetCharacterMovement()->IsFalling())
		{
			JumpSpread = FMath::FInterpTo(JumpSpread, 10.5f, DeltaTime, 1.5f);
		}
		else
		{
			JumpSpread = FMath::FInterpTo(JumpSpread, 0.f, DeltaTime, 30.f);
		}

		HUD->HUDPackage.CrosshairSpread = 1.f + JumpSpread;
	}
}
void URootCombatComponent::SetCrosshairSpread(float Spread)
{
	
}




void URootCombatComponent::SetCurrentCombatState(ECombatState CombatState)
{
	CurrentCombatState = CombatState;
}


void URootCombatComponent::DefaultAttack(const TArray<FHitResult>& HitResultsArr)
{
	if (OwnerCharacter == nullptr) return;

	SetCurrentCombatState(ECombatState::ECS_Attack);
	//OwnerCharacter->PlayAttackMontage(EAttackState::EAS_DefaultAttack);

	if (DefaultAttackHitParticle != nullptr && HitResultsArr.Num() > 0)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DefaultAttackHitParticle, HitResults[0].ImpactPoint);
	}
	
	DefaultAttackDelayEnd = false;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		DefaultAttackTimerHandle,
		this,
		&URootCombatComponent::DefaultAttackTimeEnd,
		DefaultAttackDelay,
		false
	);
}
void URootCombatComponent::DefaultAttackTimeEnd()
{
	DefaultAttackDelayEnd = true;
}
bool URootCombatComponent::CanDefaultAttack()
{
	if (CurrentCombatState == ECombatState::ECS_Idle && DefaultAttackDelayEnd)
	{
		return true;
	}

	return false;
}


void URootCombatComponent::StrongAttack(const TArray<FHitResult>& HitResultsArr)
{
	if (OwnerCharacter == nullptr) return;

	SetCurrentCombatState(ECombatState::ECS_Attack);

	if (HitResultsArr.Num() > 0)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StrongAttackHitParticle, HitResults[0].ImpactPoint);
	}

	StrongAttackDelayEnd = false;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		StrongAttackTimerHandle,
		this,
		&URootCombatComponent::StrongAttackTimeEnd,
		StrongAttackDealy,
		false
	);
}
void URootCombatComponent::StrongAttackTimeEnd()
{
	StrongAttackDelayEnd = true;
}
bool URootCombatComponent::CanStrongAttack()
{
	if (CurrentCombatState == ECombatState::ECS_Idle && StrongAttackDelayEnd)
	{
		return true;
	}

	return false;
}

// 공격 애니메이션이 끝나면 Anim Notify에 의해 호출되는 함수(공격 상태가 끝남을 알림)
void URootCombatComponent::AttackEnd()
{
	SetCurrentCombatState(ECombatState::ECS_Idle);
}


void URootCombatComponent::FirstSkillPressed()
{
	SetCurrentCombatState(ECombatState::ECS_Attack);

	// 쿨타임 설정
	FirstSkillDelayEnd = false;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		FirstSkillTimerHandle,
		this,
		&URootCombatComponent::FirstSkillTimeEnd,
		SkillTimerDelay,
		true
	);
}
void URootCombatComponent::FirstSkillReleased()
{

}
void URootCombatComponent::FirstSkillEnd()
{
	SetCurrentCombatState(ECombatState::ECS_Idle);
}
void URootCombatComponent::FirstSkillTimeEnd()
{
	float percent = FMath::Clamp(100 - (FirstSkiilProgressBar * FirstSkillCoolTimeCount++), 0.f, 100.f);
	SetFirstSkillProgressBarHUD(percent);
	if (percent == 0.f)
	{
		FirstSkillCoolTimeCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(FirstSkillTimerHandle);
		FirstSkillDelayEnd = true;
	}
}


void URootCombatComponent::SecondSkillPressed()
{
	bSecondSkillPressed = true;
	SecondSkillDelayEnd = false;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		SecondSkillTimerHandle,
		this,
		&URootCombatComponent::SecondSkillTimeEnd,
		SkillTimerDelay,
		true
	);

	SetCurrentCombatState(ECombatState::ECS_Attack);
}
void URootCombatComponent::SecondSkillReleased()
{
	bSecondSkillPressed = false;
}
void URootCombatComponent::SecondSkillEnd()
{
	SetCurrentCombatState(ECombatState::ECS_Idle);
}
void URootCombatComponent::SecondSkillTimeEnd()
{
	float percent = FMath::Clamp(100 - (SecondSkillProgressBar * SecondSkillCoolTimeCount++), 0.f, 100.f);
	SetSecondSkillProgressBarHUD(percent);
	if (percent == 0.f)
	{
		SecondSkillCoolTimeCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(SecondSkillTimerHandle);
		SecondSkillDelayEnd = true;
	}
}


void URootCombatComponent::ThirdSkillPressed()
{
	ThirdSkillDelayEnd = false;
	OwnerCharacter->GetWorldTimerManager().SetTimer(ThirdSkillTimerHandle, this, &URootCombatComponent::ThirdSkillTimeEnd, SkillTimerDelay, true);

	SetCurrentCombatState(ECombatState::ECS_Attack);
}
void URootCombatComponent::ThirdSkillReleased()
{
	
}
void URootCombatComponent::ThirdSkillEnd()
{
	SetCurrentCombatState(ECombatState::ECS_Idle);
}
void URootCombatComponent::ThirdSkillTimeEnd()
{
	float percent = FMath::Clamp(100 - (ThirdSkillProgressBar * ThirdSkillCoolTimeCount++), 0.f, 100.f);
	SetThirdSkillProgressBarHUD(percent);
	if (percent == 0.f)
	{
		ThirdSkillCoolTimeCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(ThirdSkillTimerHandle);
		ThirdSkillDelayEnd = true;
	}
}


void URootCombatComponent::Ready()
{
	
}
void URootCombatComponent::Death()
{
	CurrentCombatState = ECombatState::ECS_Idle;
	bFirstSkillPressed = false;
	bSecondSkillPressed = false;
	bThirdSkillPressed = false;
	IsFullBodySkill = false;
	IsOffUpperarmTransformModify = false;
}



bool URootCombatComponent::CanFirstSkill()
{
	if (FirstSkillDelayEnd && CurrentCombatState == ECombatState::ECS_Idle) return true;

	return false;
}
bool URootCombatComponent::CanSecondSkill()
{
	if (SecondSkillDelayEnd && CurrentCombatState == ECombatState::ECS_Idle) return true;

	return false;
}
bool URootCombatComponent::CanThirdSkill()
{
	if (ThirdSkillDelayEnd && CurrentCombatState == ECombatState::ECS_Idle) return true;

	return false;
}


void URootCombatComponent::SetFirstSkillProgressBarHUD(const float percent)
{
	if (PlayerController)
	{
		HUD = HUD == nullptr ? Cast<ARootHUD>(PlayerController->GetHUD()) : HUD;
		if (HUD)
		{
			HUD->SetFirstSkillProgressBarHUD(percent);
		}
	}
}
void URootCombatComponent::SetSecondSkillProgressBarHUD(const float percent)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(OwnerCharacter->GetPlayerController()->GetHUD()) : HUD;
	if (HUD)
	{
		HUD->SetSecondSkillProgressBarHUD(percent);
	}
}
void URootCombatComponent::SetThirdSkillProgressBarHUD(const float percent)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(OwnerCharacter->GetPlayerController()->GetHUD()) : HUD;
	if (HUD)
	{
		HUD->SetThirdSkillProgressBarHUD(percent);
	}
}


void URootCombatComponent::ActiveKeyboardInput(bool bActive)
{
	OwnerCharacter->ActiveKeyboardInput(bActive);
}
void URootCombatComponent::ActiveMouseInput(bool bActive)
{
	OwnerCharacter->ActiveMouseInput(bActive);
}

