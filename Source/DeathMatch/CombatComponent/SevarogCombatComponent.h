#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "Components/TimelineComponent.h"
#include "SevarogCombatComponent.generated.h"

UENUM()
enum class ESevarogSkill : uint8 {
	ESS_DefaultAttack UMETA(DisplayName = "DefaultAttack"),
	ESS_StrongAttack UMETA(DisplayName = "StringAttack"),
	ESS_FirstSkill UMETA(DisplayName = "FirstSkill"),
	ESS_SecondSkill UMETA(DisplayName = "SecondSkill"),
	ESS_SecondSkillReady UMETA(DisplayName = "SecondSkillReady"),
	ESS_ThirdSkill UMETA(DisplayName = "ThirdSkill"),
	ESS_FowardDeath UMETA(DisplayName = "FowardDeath"),
	ESS_BackwardDeath UMETA(DisplayName = "BackwardDeath"),
};

USTRUCT(Atomic)
struct FWeaponSocketInfo
{
	GENERATED_BODY()

public:
	FWeaponSocketInfo() {}
	FWeaponSocketInfo(FVector CurrentLocation, FVector PrevLocation) : CurrentLocation(CurrentLocation), PrevLocation(PrevLocation) {}

public:
	FVector CurrentLocation;
	FVector PrevLocation;
};

UCLASS()
class DEATHMATCH_API USevarogCombatComponent : public URootCombatComponent
{
	GENERATED_BODY()

public:
	USevarogCombatComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

public:
	virtual void SetCharacter(ARootCharacter* Character);
	virtual void SetHUD(AHUD* tempHUD);

	virtual void SetSkillCoolTime();

	void PlayDefaultAttackMontage(ESevarogSkill State);


	bool CanDash();
	void Dash();
	/*UFUNCTION()
	void UpdateMoveCurve(float Value);*/
	UFUNCTION()
	void UpdateMoveCurve(FVector Value); 
	UFUNCTION()
	void EndMoveCurve();
	void DashTimeEnd();


	void ReadyForWeaponTrace();
	void StartWeaponTrace();
	bool IsCharacterBehindWall(ACharacter* Chr);


	virtual void DefaultAttack(const TArray<FHitResult>& HitResultsArr);
	void DefaultAttackPlay();
	void LeftClickReleased();
	virtual bool CanDefaultAttack();
	UFUNCTION(BlueprintCallable)
	void DefaultAttackBuff();


	virtual void StrongAttack(const TArray<FHitResult>& HitResultsArr);
	void RightClickReleased();
	void ChargeToJump(float DeltaTime);


	virtual void FirstSkillPressed();
	void FirstSkillBuffEnd();


	virtual void SecondSkillPressed();
	UFUNCTION(BlueprintCallable)
	void SecondSkillSwingEnd();
	UFUNCTION(BlueprintCallable)
	void SecondSkillJumpToLoop();
	UFUNCTION(BlueprintCallable)
	virtual void SecondSkillEnd();
	void SecondSkillTakeDown(float DeltaTime);


	virtual void ThirdSkillPressed();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillSwingEnd();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillAttack();



	void SetDashSkillProgressBarHUD(const float percent);


private:
	UPROPERTY()
	class ASevarogCharacter* SevarogCharacter = nullptr;

	UPROPERTY()
	class ASevarogHUD* SevarogHUD = nullptr;


private:// default attack 관련 변수
	UPROPERTY(EditAnywhere, Category = "DefaultAttack")
	class UNiagaraSystem* DefaultAttackWeaponTrail = nullptr;
	UPROPERTY()
	class UNiagaraComponent* DefaultAttackWeaponTrailComponent = nullptr;

	FVector DefaultAttackStartLocation;

	int32 DefaultAttackCnt = 0;

	bool bFastBuff = false;
	bool DefaultAttackPressed = false;
	bool bDefaultAttackDelay = false;
	bool bDefaultAttackSuccess = false;

	float CurrentDefaultAttackDelayTime = 0.f;


private:// strong attack 관련 변수
	UPROPERTY(EditAnywhere, Category = "StrongAttack")
	float UpForce = 25.f;
	UPROPERTY(EditAnywhere, Category = "StrongAttack")
	float FowardForce = 40.f;


private:// First Skill 관련 변수
	FTimerHandle FirstSkillBuffTimerHandle;

	UPROPERTY(EditAnywhere, Category = FirstSkill)
	float FirstSkillBuffTime = 20.f;


private:// second skill 관련 변수
	UPROPERTY(EditAnywhere, Category=SecondSkill)
	class UClass* SecondSkillClass;
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class UParticleSystem* SecondSkillParticle = nullptr;

	FVector SecondSkillTargetLocation;
	FVector SecondSkillCurrentLocation;

	bool bTakeDown = false;
	bool bTakeDownFinish = false;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	float SecondSkillInterpSpeed = 16.f;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	float SecondSkillReadyFallDistance = 1000.f;


private: // third skill 관련 변수
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	class UClass* ThirdSkillClass;


private: // dash 관련 변수
	float DashSkillProgressBar;
	UPROPERTY(EditAnywhere, Category = CoolTime)
	float DashSkillDelay = 3.f;
	float DashSkillCoolTimeMaxCount;
	float DashSkillCoolTimeCount = 0.f;

	UPROPERTY(EditAnywhere, Category = Dash)
	class UParticleSystem* DashTrailParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = Dash)
	class UCurveVector* SevarogCurve = nullptr;
	UPROPERTY(EditAnywhere, Category = Dash)
	class UTimelineComponent* MoveTimeline = nullptr;

	bool bDash = false;

	FOnTimelineVector MoveTrack;
	FOnTimelineEvent MoveTrackEnd;

	//FOnTimelineVector MoveTrackVector;
	//FOnTimelineEvent MoveTrackEndVector;

	FVector FowardVector;
	FVector RightVector;
	FVector UpVector;
	FVector CurrentLocation;


	bool bInterpDashCamera = false;
	float InitTargetArmLength;
	float CurrentTargetArmLength;
	UPROPERTY(EditAnywhere, Category = Dash)
	float StartDashLength = 900.f;
	float TargetTargetArmLength;
	UPROPERTY(EditAnywhere, Category = Dash)
	float DashCameraInterpSpeed = 10.f;


	bool bDashTimeEnd = true;
	FTimerHandle DashTimerHandle;
	float DashTimerDelay;


private:
	UPROPERTY(EditAnywhere, Category = Weapon)
	class UParticleSystem* HitParticle = nullptr;

	TArray<FWeaponSocketInfo> WeaponSocketArr;

	bool bWeaponTrace = false;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float WeaponDamage = 20.f;

	TSet<ACharacter*> HitCharSet;
	TSet<AActor*> HitParticleSet;

public:
	FORCEINLINE bool GetbDash() { return bDash; }
};
