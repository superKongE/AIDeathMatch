#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "DeathMatch/Enum/AttackState.h"
#include "RootCharacter.generated.h"

UCLASS()
class DEATHMATCH_API ARootCharacter : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	ARootCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Jump() override;

	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed,
		float& OutSightStrength, const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr, int32* UserData = nullptr) const override;
	UPROPERTY(EditAnywhere)
	TArray<FName> ViewTargets;
	int32 Index = 0;

	bool skip = false;

public:
	void MoveFoward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void LookRound(float Value);

public:
	void Init();

	void SetHP(float HP);

	virtual void DefaultAttack();
	virtual void StrongAttack();

	virtual void FirstSkillPressed();
	virtual void FirstSkillRelease();
	virtual void SecondSkillPressed();
	virtual void SecondSkillRelease();
	virtual void ThirdSkillPressed();
	virtual void ThirdSkillRelease();

	//void PlayAttackMontage(EAttackState AttackState);


	void CharacterSelectButtonPressed();
	void CharacterSelect(const int32 index);
	bool CharacterSelectComplete();
	ARootCharacter* GetSpawnCharacter();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	UFUNCTION(BlueprintCallable)
	void FirstSkillEnd();
	UFUNCTION(BlueprintCallable)
	void SecondSkillEnd();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillEnd();


	void ActiveKeyboardInput(bool bActive);
	void ActiveMouseInput(bool bActive);


	void SetHUDHP();
	void SetHUDCurrentHP();
	void SetHUDAmmo(const int32 Ammo, const int32 MaxAmmo);
	UFUNCTION(BlueprintCallable)
	float GetHP() { return CurrentHP / MaxHP; }
	UFUNCTION(BlueprintCallable)
	float GetTargetHP() { return TargetHP / MaxHP; }


	void ReadyToSpawn();
	void Death();
	void Spawn();
	UFUNCTION(BlueprintCallable)
	void DeathEnd();


	UFUNCTION(BlueprintCallable)
	bool FootStepNotify(const FName& BoneName);

	bool GetIsFullBodySkill();

	void OverlapCharacterSelectArea(bool bOverlap);

	void DeleteSpawnedCharacter();

	void ChangeInputExceptMouse(bool IsOff);


	float GetFootLOffset();
	float GetFootROffset();
	float GetHipOffset();

	virtual void DestroyFromCharacterSelect();

protected:
	UPROPERTY(EditAnywhere, Category = "HUD")
	UClass* HUD;

	UPROPERTY()
	class ARootPlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere)
	class URootCombatComponent* CombatComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UFootIKComponent* FootIKComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UCharacterSelectComponent* CharacterSelectComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera = nullptr;
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArm = nullptr;


	UPROPERTY()
	class UAnimInstance* MyAnimInstance = nullptr;

	UPROPERTY(EditAnywhere)
	int32 CharacterNum;

	float Pitch = 0.f;

	bool bCharacterUseFootIK; // 캐릭터 자체가 FootIK를 사용하는지

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAnimInstance> AnimLayerClass;

private:
	UPROPERTY(EditAnywhere, Category=Footstep)
	class USoundBase* GroundQue = nullptr;
	UPROPERTY(EditAnywhere, Category = Footstep)
	class USoundBase* WoodQue = nullptr;
	UPROPERTY(EditAnywhere, Category = Footstep)
	class USoundBase* RockQue = nullptr;

	UPROPERTY(EditAnywhere, Category = Footstep)
	class UNiagaraSystem* GroundParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = Footstep)
	class UNiagaraSystem* WoodParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = Footstep)
	class UNiagaraSystem* RockParticle = nullptr;

	FVector HitTargetLocation;

private:
	UPROPERTY(EditAnywhere, Category = Material)
	UMaterialInterface* DeathProgressMaterial = nullptr;

	FVector SpawnLocation;

	bool IsFowardDeath = false;
	bool IsDeath = false;

	int32 PrevPercent = -1;
	float DeathPercent = 0.f;
	float DeathPercentCharge = 0.1f;

private:
	UPROPERTY(EditAnywhere, Category=Combat)
	float MaxHP = 100.f;
	float CurrentHP;
	float TargetHP;

	UPROPERTY(EditAnywhere, Category = Combat)
	FVector CameraRelativeLocation;
	UPROPERTY(EditAnywhere, Category=Combat)
	float FOV = 50.f;

	bool IsCharacterSelectButtonPressed = false;
	bool bActiveInputExeptMouse = true;
	bool bActiveMouseInput = true;


	bool IsOverlapCharacterSelectArea = false;

public:
	FORCEINLINE class ARootPlayerController* GetPlayerController() { return PlayerController; }
	FORCEINLINE class UCameraComponent* GetCamera() { return Camera; }
	FORCEINLINE class URootCombatComponent* GetCombatComponent() { return CombatComponent; }
	FORCEINLINE class USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE float GetCurrentHP() { return CurrentHP; }
	FORCEINLINE float GetAimOffset() { return Pitch; }
	FORCEINLINE int32 GetCharacterNum() { return CharacterNum; }
	FORCEINLINE void SetCharacterNum(int32 num) { CharacterNum = num; }
	FORCEINLINE void SetHitTargetLocation(FVector& Location) { HitTargetLocation = Location; }
	FORCEINLINE FVector GetHitTargetLocation() { return HitTargetLocation; }
	FORCEINLINE bool GetISDeath() { return IsDeath; }
	FORCEINLINE bool GetIsFowardDeath() { return IsFowardDeath; }
	FORCEINLINE bool GetCharacterUseFootIK() { return bCharacterUseFootIK; }
	FORCEINLINE TSubclassOf<class AHUD> GetHudClass() { return HUD; }
	FORCEINLINE void SetAnimLayerClass(UClass* AnimLayerClass_) { AnimLayerClass = AnimLayerClass_; }
};
