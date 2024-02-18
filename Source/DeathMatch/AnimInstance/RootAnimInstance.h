// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DeathMatch/Struct/DirectionAnimStruct.h"
#include "DeathMatch/Enum/HipFacingEnum.h"
#include "RootAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FLocomotionDirectionSettings
{
	GENERATED_USTRUCT_BODY()

	float FMin = -50.f;
	float FMax = 50.f;
	float BMin = -130.f;
	float BMax = 130.f;
	float DeadZone = 20.f;
};

UENUM(BlueprintType)
enum class ELocomotionDirection : uint8 {
	ELD_F UMETA(DisplayName = "Foward"),
	ELD_B UMETA(DisplayName = "Backward"),
	ELD_R UMETA(DisplayName = "Right"),
	ELD_L UMETA(DisplayName = "Left")
};


UCLASS()
class DEATHMATCH_API URootAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Meta = (BlueprintThreadSafe))
	ELocomotionDirection GetLocomotionDirectionAndHipFacing(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings);
	UFUNCTION(Meta = (BlueprintThreadSafe))
	void CaculateLocomotionDirectionWithDeadZone(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings);
	UFUNCTION(Meta = (BlueprintThreadSafe))
	void CaculateLocomotionDirection(const float& CurrentLocomotionAngle, const ELocomotionDirection& CurrentDirection, const FLocomotionDirectionSettings& Settings);

public:
	void GetLeanAngleData(float DeltaTime);
	void GetLocomotionAngleData();
	void GetAccelerateData(float DeltaTime);
	void GetFootIKData();
	
private:
	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	FDirectionAnimations DirectionAnimStruct;

	FVector WorldLocation;
	FVector PrevWorldLocation;
	FVector WorldLocation2D;
	FVector PrevWorldLocation2D;
	FRotator WorldRotation;


	FVector CharacterVelocity;
	FVector PrevCharacterVelocity2D;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FVector CharacterVelocity2D;


	FVector Acceleration;
	FVector Acceleration2D;
	FVector PhysicalAcceleration2D;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FVector RelativeAccelerationAmount;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsAccelerating;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float LocomotionAngle;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	EHipFacing HipFacingDirections;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FLocomotionDirectionSettings LocomotionDirectionSettings;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	ELocomotionDirection LocomotionDirection;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float DisplacementSinceLastUpdate;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Speed;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Pitch;
	float Yaw;
	float PrevYaw;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float LeanAngle;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsFalling;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsCrouch;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsFullBodySkill;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsDeath;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsFowardDeath;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float FootLOffset = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float FootROffset = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float HipOffset;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bUseFootIK = true;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsFoward;


	// Distance Matching
	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float StopDistance;


	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class ARootCharacter* Character = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UCharacterMovementComponent* CharacterMovementComponent = nullptr;

public:
	FORCEINLINE void SetIsFalling(bool bFall) { IsFalling = bFall; }
};
