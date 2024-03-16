// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "RevenantCombatComponent.generated.h"

UENUM(BlueprintType)
enum class ERevenantSkill : uint8 {
	ERS_DefaultAttack UMETA(DisplayName = "DefaultAttack"),
	ERS_StrongAttack UMETA(DisplayName = "StringAttack"),
	ERS_FirstSkill UMETA(DisplayName = "FirstSkill"),
	ERS_SecondSkill UMETA(DisplayName = "SecondSkill"),
	ERS_ThirdSkill UMETA(DisplayName = "ThirdSkill"),
	ERS_FowardDeath UMETA(DisplayName = "FowardDeath"),
	ERS_BackwardDeath UMETA(DisplayName = "BackwardDeath"),
};

UCLASS()
class DEATHMATCH_API URevenantCombatComponent : public URootCombatComponent
{
	GENERATED_BODY()

public:
	URevenantCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	
public: 
	virtual void SetCharacter(ARootCharacter* Character);

	virtual void Ready();
	virtual void Death();
	virtual void DefaultAttack(const TArray<FHitResult>& HitResultsArr);
	virtual bool CanDefaultAttack();

	virtual void StrongAttack(const TArray<FHitResult>& HitResultsArr);
	virtual bool CanStrongAttack();

	virtual void FirstSkillPressed();
	virtual void FirstSkillReleased();
	virtual void SecondSkillPressed();
	virtual void SecondSkillReleased();
	virtual void ThirdSkillPressed();
	virtual void ThirdSkillReleased();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillJumpToLoop();
	void ThirdSkillJumpToEnd();


	void ThrowBomb();



	void PlayAttackMontage(ERevenantSkill State);
	void PlayReloadMontage();


	void SetAmmoHUD(const int32 Ammo);
	void SpendAmmo(const int32 Ammo);

	void Reload();
	bool CanReload();
	UFUNCTION(BlueprintCallable)
	void ReloadEnd();

	bool CanTeleport();
	bool Teleport();
	void CancelTeleport();
	void SpawnTeleport();
	
	void ThirdSkillAttack();

	virtual void FirstSkillEnd();
	virtual void SecondSkillEnd();
	virtual void ThirdSkillEnd();

	virtual bool CanFirstSkill();
	virtual bool CanSecondSkill();
	virtual bool CanThirdSkill();

private:
	UPROPERTY()
	class ARevenantCharacter* RevenantCharacter = nullptr;
	UPROPERTY()
	class ARevenantHUD* RevenantHUD = nullptr;
	UPROPERTY(EditAnywhere)
	UClass* BombClass;
	UPROPERTY(EditAnywhere)
	UClass* ReaperTeleportClass;
	UPROPERTY()
	class AReaperTeleport* ReaperTeleport = nullptr;

	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* ThirdSkillParticle;

	UPROPERTY()
	class UNiagaraComponent* ThirdSkillEffect = nullptr;

	FTimerHandle ThirdSkillReleasedHandle;

private:
	UPROPERTY(EditDefaultsOnly)
	float TeleportDistance = 5000.f; // 순간이동 최대 이동거리

	bool bTeleportButtonPressed = false;

	UPROPERTY(EditAnywhere, Category = Ammo)
	int32 MaxAmmo = 10;
	UPROPERTY(EditAnywhere, Category = Ammo)
	int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, Category = Ammo)
	int32 DefaultAttackSpendAmmo = 1;
	UPROPERTY(EditAnywhere, Category = Ammo)
	int32 StrongAttackSpendAmmo = 3;

	UPROPERTY(EditAnywhere, Category = CoolTime)
	float ThirdSkillReleasedDelay = 5.f;

	UPROPERTY(EditAnywhere, Category = Skill)
	float ThirdSkillLength = 1800.f;
};
