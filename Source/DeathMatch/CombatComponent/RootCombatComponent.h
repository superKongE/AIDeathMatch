#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathMatch/Enum/AttackState.h"
#include "RootCombatComponent.generated.h"

UENUM(BlueprintType)
enum class ECombatState  : uint8{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Attack_Ready UMETA(DisplayName = "AttackReady"),
	ECS_Reload UMETA(DisplayName = "Reload")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHMATCH_API URootCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URootCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;


public:
	virtual void SetCharacter(class ARootCharacter* Character);
	void SetController(class ARootPlayerController* Controller);
	virtual void SetHUD(AHUD* tempHUD);

	virtual void Ready();
	virtual void Death();
	virtual void DefaultAttack(const TArray<FHitResult>& HitResultsArr);
	virtual void StrongAttack(const TArray<FHitResult>& HitResultsArr);

	virtual void FirstSkillPressed();
	virtual void FirstSkillReleased();
	void FirstSkillTimeEnd();
	virtual void SecondSkillPressed();
	virtual void SecondSkillReleased();
	void SecondSkillTimeEnd();
	virtual void ThirdSkillPressed();
	virtual void ThirdSkillReleased();
	void ThirdSkillTimeEnd();

	void SetCurrentCombatState(ECombatState CombatState);



	virtual void AttackEnd();
	virtual void FirstSkillEnd();
	virtual void SecondSkillEnd();
	virtual void ThirdSkillEnd();


	virtual void SetSkillCoolTime();
	void SetCrosshairTransform();


	void TraceUnderCrosshair();
	void SetCrossHair(float DeltaTime);

	virtual bool CanDefaultAttack();
	virtual void DefaultAttackTimeEnd();
	virtual bool CanStrongAttack();
	virtual void StrongAttackTimeEnd();
	
	virtual bool CanFirstSkill();
	virtual bool CanSecondSkill();
	virtual bool CanThirdSkill();

	void ActiveKeyboardInput(bool bActive);
	void ActiveMouseInput(bool bActive);


	void SetFirstSkillProgressBarHUD(const float percent);
	void SetSecondSkillProgressBarHUD(const float percent);
	void SetThirdSkillProgressBarHUD(const float percent);

	void SetCrosshairSpread(float Spread);

	void CanUseFootIK();

private:
	FVector CrosshairStartLocation;

protected:
	UPROPERTY()
	class ARootCharacter* OwnerCharacter = nullptr;
	UPROPERTY()
	class ARootPlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = Montage)
	class UAnimMontage* CombatMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* DefaultAttackParticle;
	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* StrongAttackParticle;
	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* DefaultAttackHitParticle;
	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* StrongAttackHitParticle;


	UPROPERTY()
	class ARootHUD* HUD = nullptr;

	UPROPERTY(EditAnywhere, Category = Crosshair)
	class UTexture2D* LeftCrossHair = nullptr;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	class UTexture2D* RightCrossHair = nullptr;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	class UTexture2D* UpCrossHair = nullptr;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	class UTexture2D* DownCrossHair = nullptr;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	class UTexture2D* CenterCrossHair = nullptr;

protected:
	FVector CrosshairWorldPosition = FVector::ZeroVector;
	FVector CrosshairWorldDirection = FVector::ZeroVector;


	TArray<FHitResult> HitResults;

	FTimerHandle DefaultAttackTimerHandle;
	FTimerHandle StrongAttackTimerHandle;
	FTimerHandle FirstSkillTimerHandle;
	FTimerHandle SecondSkillTimerHandle;
	FTimerHandle ThirdSkillTimerHandle;

	bool DefaultAttackDelayEnd = true;
	bool StrongAttackDelayEnd = true;
	bool FirstSkillDelayEnd = true;
	bool SecondSkillDelayEnd = true;
	bool ThirdSkillDelayEnd = true;

	bool bFirstSkillPressed = false;
	bool bSecondSkillPressed = false;
	bool bThirdSkillPressed = false;

	bool bUseFootIK = true;
	bool IsFullBodySkill = false;
	bool IsOffUpperarmTransformModify = false;
		
	float CrosshairSpread = 0.f;

	// SkillTimerDelay(0.1초)마다 호출될 최대 횟수
	int32 FirstSkillCoolTimeMaxCount;
	int32 SecondSkillCoolTimeMaxCount;
	int32 ThirdSkillCoolTimeMaxCount;

	int32 FirstSkillCoolTimeCount;
	int32 SecondSkillCoolTimeCount;
	int32 ThirdSkillCoolTimeCount;

	UPROPERTY(EditAnywhere, Category = Combat)
	float SkillTimerDelay = 0.1f;

	// SkillTimerDelay(0.1초)마다 스킬 프로그래스바가 차는 양
	float FirstSkiilProgressBar;
	float SecondSkillProgressBar;
	float ThirdSkillProgressBar;

	ECombatState CurrentCombatState;

protected:
	UPROPERTY(EditAnywhere, Category = CoolTime)
	float DefaultAttackDelay = 1.f;
	UPROPERTY(EditAnywhere, Category = CoolTime)
	float StrongAttackDealy = 3.f;
	UPROPERTY(EditAnywhere, Category = CoolTime)
	float FirstSkillDelay = 5.f;
	UPROPERTY(EditAnywhere, Category = CoolTime)
	float SecondSkillDelay = 10.f;
	UPROPERTY(EditAnywhere, Category = CoolTime)
	float ThirdSkillDelay = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultAttackDamage = 100.f;
	float CurrentDefaultAttackDamage = 100.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float StrongAttackDamage = 100.f;
	float CurrentStrongAttackDamage = 100.f;

	UPROPERTY(EditAnywhere, Category = Damage)
	float FirstSkillDamage = 0.f;
	UPROPERTY(EditAnywhere, Category = Damage)
	float SecondSkillDamgge = 0.f;
	UPROPERTY(EditAnywhere, Category = Damage)
	float ThirdSkillDamage = 5.f;


	UPROPERTY(EditAnywhere, Category = Combat)
	float TraceDistance = 16000.f;

	UPROPERTY(EditAnywhere, Category = Crosshair)
	float JumpSpread;

public:
	FORCEINLINE class ARootCharacter* GetOwnerCharacter() { return OwnerCharacter; }
	FORCEINLINE UNiagaraSystem* GetDefaultAttackParticle() { return DefaultAttackParticle; }
	FORCEINLINE UNiagaraSystem* GetStrongAttackParticle() { return StrongAttackParticle; }
	FORCEINLINE TArray<FHitResult> GetHitResults() { return HitResults; }
	FORCEINLINE bool GetIsFullBodySkill() { return IsFullBodySkill; }
	FORCEINLINE void SetIsOffUpperarmTransformModify(bool IsOff) { IsOffUpperarmTransformModify = IsOff; }
	FORCEINLINE bool GetIsOffUpperarmTransformModify() { return IsOffUpperarmTransformModify; }
	FORCEINLINE FVector GetCrosshairWorldPosition() { return CrosshairWorldPosition; }
	FORCEINLINE FVector GetCrosshairStartLocation() { return CrosshairStartLocation; }
	FORCEINLINE bool GetbUseFootIK() { return bUseFootIK; }
};
