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

	// cycle ������Ʈ�� ���Ͻ� ��Ī ������ ���� 
	// ���� ���������� ���� ���� �����ӱ��� �̵��Ÿ�
	//DisplacementSinceLastUpdate = UKismetMathLibrary::SafeDivide((WorldLocation2D - PrevWorldLocation2D).Length(), DeltaTime);
	
	// ĳ���� ���� ��ġ ���� ǥ��
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


// ���� ������� �������� �����忡�� ����Ǵ� �Լ�
void URootAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	GetLocomotionDirectionAndHipFacing(LocomotionAngle, LocomotionDirection, LocomotionDirectionSettings);
}
ELocomotionDirection URootAnimInstance::GetLocomotionDirectionAndHipFacing(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings)
{
	// ���� ������ deadzone �������� �ִ���
	// ī�޶� �¿�� �����ӿ� ���� LocomtionAngle���� ���ϹǷ� 
	// ī�޶� �����ӿ� ���� ������ �޶����� �ʵ��� deadzone��ŭ ���� �߰�
	CaculateLocomotionDirectionWithDeadZone(CurrentLocomotionAngle, CurrentDirection, Settings);

	// ������ �ٲ������ üũ
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
		//                                                                     ������ ���� ���� ĳ������ ������ ������ �޶���
		LeanAngle = (UKismetMathLibrary::SafeDivide(Yaw - PrevYaw, DeltaTime) / 4.f) * 1.f;
	}
	else if (HipFacingDirections == EHipFacing::EHF_B)
	{
		//                                                                     ������ ���� ���� ĳ������ ������ ������ �޶���
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


	// ĳ���Ϳ� ������ΰ� �ƴ϶�
	// ���忡 ���� �����
	// �� ĳ���Ͱ� x�� �������� �������� �޸��� x���� �ö󰡰�, x�� ������ ���鼭 �¿�� �����̸� y���� ����
	// y�� �������� �������� �޸��� y���� �ö�, y�� ������ ���鼭 �յڷ� �����̸� x���� ����
	PhysicalAcceleration2D = (CharacterVelocity2D - PrevCharacterVelocity2D) / DeltaTime;

	// �ӵ��� �����ص� ���ӵ��� 0������
	// ������ �־ ���ӵ��� 0�̹Ƿ� 
	// ���� �����ϰ� �ִ��� üũ
	if (IsAccelerating)
	{
		float MaxAcceleration = Character->GetCharacterMovement()->GetMaxAcceleration();
		PhysicalAcceleration2D = UKismetMathLibrary::Vector_ClampSizeMax(PhysicalAcceleration2D, MaxAcceleration) / MaxAcceleration;

		// Velocity ���ʹ� World Space �����̱� ������
		// ĳ������ Local Space ���� ���ͷ� �ٲٱ� ���� 
		// ĳ������ World Rotation��ŭ �ݽð� �������� ȸ�������ش� (UnRotate Vector)
		RelativeAccelerationAmount = WorldRotation.UnrotateVector(PhysicalAcceleration2D);
	}
	else
	{
		float MaxBrakingAcceleration = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
		PhysicalAcceleration2D = UKismetMathLibrary::Vector_ClampSizeMax(PhysicalAcceleration2D, MaxBrakingAcceleration) / MaxBrakingAcceleration;

		// Velocity ���ʹ� World Space �����̱� ������
		// ĳ������ Local Space ���� ���ͷ� �ٲٱ� ���� 
		// ĳ������ World Rotation��ŭ �ݽð� �������� ȸ�������ش� (UnRotate Vector)
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