// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatch/AnimInstance/RootAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

#include "DeathMatch/Character/RootCharacter.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"

void URootAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ARootCharacter>(TryGetPawnOwner());
	if (Character)
		CharacterMovementComponent = Character->GetCharacterMovement();
}
void URootAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Character == nullptr || Character->GetPlayerController() == nullptr) return;
	
	Speed = CharacterVelocity.Size();
	
	GetLocomotionAngleData();
	GetAccelerateData(DeltaTime);
	GetLeanAngleData(DeltaTime);
	GetFootIKData();

	// cycle 스테이트의 디스턴스 매칭 적용을 위한 
	// 이전 프레임으로 부터 현재 프레임까지 이동거리
	//DisplacementSinceLastUpdate = UKismetMathLibrary::SafeDivide((WorldLocation2D - PrevWorldLocation2D).Length(), DeltaTime);
	
	// 캐릭터 정지 위치 지점 표시
	/*FVector vel = CharacterVelocity;
	vel.Normalize();
	DrawDebugSphere(GetWorld(), Character->GetActorLocation() + vel * StopDistance, 50.f, 15.f, FColor::Red);*/
	
	//Pitch = WorldRotation.Pitch;
	Pitch = Character->GetAimOffset();
	IsDeath = Character->GetISDeath();
	IsFowardDeath = Character->GetIsFowardDeath();

	IsFalling = Character->GetCharacterMovement()->IsFalling();
	IsCrouch = Character->bIsCrouched;
	IsFullBodySkill = Character->GetIsFullBodySkill();
}


// 게임 스레드와 독립적인 스레드에서 실행되는 함수
void URootAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	GetLocomotionDirectionAndHipFacing(LocomotionAngle, LocomotionDirection, LocomotionDirectionSettings);
}
ELocomotionDirection URootAnimInstance::GetLocomotionDirectionAndHipFacing(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings)
{
	// 현재 방향이 deadzone 범위내에 있는지
	// 카메라가 좌우로 움직임에 따라서 LocomtionAngle값이 변하므로 
	// 카메라 움직임에 따라 방향이 달라지지 않도록 deadzone만큼 범위 추가
	CaculateLocomotionDirectionWithDeadZone(CurrentLocomotionAngle, CurrentDirection, Settings);

	// 방향이 바뀌었는지 체크
	CaculateLocomotionDirection(CurrentLocomotionAngle, CurrentDirection, Settings);

	return LocomotionDirection;
}
void URootAnimInstance::CaculateLocomotionDirectionWithDeadZone(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings)
{
	switch (CurrentDirection)
	{
	case ELocomotionDirection::ELD_F:
		if (UKismetMathLibrary::InRange_FloatFloat(CurrentLocomotionAngle, Settings.FMin - Settings.DeadZone, Settings.FMax + Settings.DeadZone))
			LocomotionDirection = ELocomotionDirection::ELD_F;
		break;

	case ELocomotionDirection::ELD_B:
		if (UKismetMathLibrary::InRange_FloatFloat(CurrentLocomotionAngle, Settings.BMin + Settings.DeadZone, Settings.BMax - Settings.DeadZone))
			LocomotionDirection = ELocomotionDirection::ELD_B;
		break;

	case ELocomotionDirection::ELD_R:
		if (UKismetMathLibrary::InRange_FloatFloat(CurrentLocomotionAngle, Settings.FMax - Settings.DeadZone, Settings.BMax + Settings.DeadZone))
			LocomotionDirection = ELocomotionDirection::ELD_R;
		break;

	case ELocomotionDirection::ELD_L:
		if (UKismetMathLibrary::InRange_FloatFloat(CurrentLocomotionAngle, Settings.FMin + Settings.DeadZone, Settings.BMin - Settings.DeadZone))
			LocomotionDirection = ELocomotionDirection::ELD_L;
		break;
	}
}
void URootAnimInstance::CaculateLocomotionDirection(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings)
{
	if (!UKismetMathLibrary::InRange_FloatFloat(CurrentLocomotionAngle, Settings.BMin, Settings.BMax))
	{
		HipFacingDirections = EHipFacing::EHF_B;
		LocomotionDirection = ELocomotionDirection::ELD_B;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(CurrentLocomotionAngle, Settings.FMin, Settings.FMax))
	{
		HipFacingDirections = EHipFacing::EHF_F;
		LocomotionDirection = ELocomotionDirection::ELD_F;
	}
	else if (CurrentLocomotionAngle < 0.f)
	{
		switch (CurrentDirection)
		{
		case ELocomotionDirection::ELD_F:
			HipFacingDirections = EHipFacing::EHF_F;
			break;

		case ELocomotionDirection::ELD_B:
			HipFacingDirections = EHipFacing::EHF_B;
			break;

		case ELocomotionDirection::ELD_R:
			if (HipFacingDirections == EHipFacing::EHF_F)
				HipFacingDirections = EHipFacing::EHF_B;
			else
				HipFacingDirections = EHipFacing::EHF_F;
			break;

		case ELocomotionDirection::ELD_L:
			break;
		}

		LocomotionDirection = ELocomotionDirection::ELD_L;
	}
	else
	{
		switch (CurrentDirection)
		{
		case ELocomotionDirection::ELD_F:
			HipFacingDirections = EHipFacing::EHF_F;
			break;

		case ELocomotionDirection::ELD_B:
			HipFacingDirections = EHipFacing::EHF_B;
			break;

		case ELocomotionDirection::ELD_R:
			break;

		case ELocomotionDirection::ELD_L:
			if(HipFacingDirections == EHipFacing::EHF_F)
				HipFacingDirections = EHipFacing::EHF_B;
			else
				HipFacingDirections = EHipFacing::EHF_F;
			break;
		}

		LocomotionDirection = ELocomotionDirection::ELD_R;
	}
}



void URootAnimInstance::GetLeanAngleData(float DeltaTime)
{
	PrevYaw = Yaw;
	Yaw = WorldRotation.Yaw;

	if (HipFacingDirections == EHipFacing::EHF_F)
	{
		//                                                                     나누는 값에 따라 캐릭터의 기울어짐 정도가 달라짐
		LeanAngle = (UKismetMathLibrary::SafeDivide(Yaw - PrevYaw, DeltaTime) / 4.f) * 1.f;
	}
	else if (HipFacingDirections == EHipFacing::EHF_B)
	{
		//                                                                     나누는 값에 따라 캐릭터의 기울어짐 정도가 달라짐
		LeanAngle = (UKismetMathLibrary::SafeDivide(Yaw - PrevYaw, DeltaTime) / 4.f) * -1.f;
	}

	LeanAngle = FMath::Clamp(LeanAngle, -90.f, 90.f);
}
void URootAnimInstance::GetLocomotionAngleData()
{
	PrevWorldLocation = WorldLocation;
	WorldLocation = Character->GetActorLocation();
	WorldRotation = Character->GetActorRotation();

	WorldLocation2D = WorldLocation * FVector(1.f, 1.f, 0.f);
	PrevWorldLocation2D = PrevWorldLocation * FVector(1.f, 1.f, 0.f);

	CharacterVelocity = Character->GetVelocity();
	PrevCharacterVelocity2D = CharacterVelocity2D;
	CharacterVelocity2D = CharacterVelocity * FVector(1.f, 1.f, 0.f);

	LocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
}
void URootAnimInstance::GetAccelerateData(float DeltaTime)
{
	Acceleration = Character->GetCharacterMovement()->GetCurrentAcceleration();
	Acceleration2D = Acceleration * FVector(1.f, 1.f, 0.f);

	if (!FMath::IsNearlyZero(Acceleration2D.Length()))
		IsAccelerating = true;
	else
		IsAccelerating = false;


	// 캐릭터에 상대적인게 아니라
	// 월드에 대해 상대적
	// 즉 캐릭터가 x축 방향으로 정면으로 달리면 x값이 올라가고, x축 방향을 보면서 좌우로 움직이면 y값이 변함
	// y축 방향으로 정면으로 달리면 y값이 올라감, y축 방향을 보면서 앞뒤로 움직이면 x값이 변함
	PhysicalAcceleration2D = (CharacterVelocity2D - PrevCharacterVelocity2D) / DeltaTime;

	// 속도가 일정해도 가속도가 0이지만
	// 정지해 있어도 가속도가 0이므로 
	// 현재 가속하고 있는지 체크
	if (IsAccelerating)
	{
		float MaxAcceleration = Character->GetCharacterMovement()->GetMaxAcceleration();
		PhysicalAcceleration2D = UKismetMathLibrary::Vector_ClampSizeMax(PhysicalAcceleration2D, MaxAcceleration) / MaxAcceleration;

		// Velocity 벡터는 World Space 기준이기 때문에
		// 캐릭터의 Local Space 상의 벡터로 바꾸기 위해 
		// 캐릭터의 World Rotation만큼 반시계 방향으로 회전시켜준다 (UnRotate Vector)
		RelativeAccelerationAmount = WorldRotation.UnrotateVector(PhysicalAcceleration2D);
	}
	else
	{
		float MaxBrakingAcceleration = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
		PhysicalAcceleration2D = UKismetMathLibrary::Vector_ClampSizeMax(PhysicalAcceleration2D, MaxBrakingAcceleration) / MaxBrakingAcceleration;

		// Velocity 벡터는 World Space 기준이기 때문에
		// 캐릭터의 Local Space 상의 벡터로 바꾸기 위해 
		// 캐릭터의 World Rotation만큼 반시계 방향으로 회전시켜준다 (UnRotate Vector)
		RelativeAccelerationAmount = WorldRotation.UnrotateVector(PhysicalAcceleration2D);
	}
}
void URootAnimInstance::GetFootIKData()
{
	if (!Character->GetCharacterUseFootIK()) return;

	FootLOffset = Character->GetFootLOffset();
	FootROffset = Character->GetFootROffset();
	HipOffset = Character->GetHipOffset();
	bUseFootIK = Character->GetCombatComponent()->GetbUseFootIK();
}