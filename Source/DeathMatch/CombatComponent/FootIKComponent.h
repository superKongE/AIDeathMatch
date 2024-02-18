#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootIKComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHMATCH_API UFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFootIKComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void FootLineTrace(const FName& SocketName, FVector& HitLocation, float& FootTraceOffset);

	void SetCapsuleHalfHeight();

private:
	UPROPERTY()
	class ARootCharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = FootIK)
	FName FootL = TEXT("Foot_L");
	UPROPERTY(EditAnywhere, Category = FootIK)
	FName FootR = TEXT("Foot_R");

	FVector FootLHitLocation;
	FVector FootRHitLocation;

	bool bFootLHit = false;
	bool bFootRHit = false;

	UPROPERTY(EditAnywhere, Category=FootIK)
	float StartOffset = 20.f;
	float CurrentLeftFootTraceOffset = 0.f;
	float CurrentRightFootTraceOffset = 0.f;
	float LeftFootTraceOffset = 0.f;
	float RightFootTraceOffset = 0.f;
	float TargetLeftFootTraceOffset;
	float TargetRightFootTraceOffset;
	float HipOffset;
	float CapsuleHalfHeight;
	UPROPERTY(EditAnywhere, Category = FootIK)
	float Distance = 55.f;

	float CurrentHipOffset;

public:
	FORCEINLINE void SetOwnerCharacter(class ARootCharacter* Character) { OwnerCharacter = OwnerCharacter; }
	FORCEINLINE float GetFootLOffset() { return CurrentLeftFootTraceOffset; }
	FORCEINLINE float GetFootROffset() { return CurrentRightFootTraceOffset; }
	FORCEINLINE bool GetFootLHit() { return bFootLHit; }
	FORCEINLINE bool GetFootRHit() { return bFootRHit; }
	FORCEINLINE float GetHitOffset() { return CurrentHipOffset; }
};
