// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCombatComponent.generated.h"

UENUM(BlueprintType)
enum class EAICombatState : uint8 {
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Attack_Ready UMETA(DisplayName = "AttackReady"),
	ECS_Reload UMETA(DisplayName = "Reload")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHMATCH_API UEnemyCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void TraceUnderCrosshair(); //

	void SpendAmmo(int32 Ammo);
	
	void Attack();
	void AttackCoolTime();
	bool CanAttack();
	void SpawnDefaultAttackEffect(const FVector& SpawnLocation);
	void SpawnStrongAttackEffect(const FVector& SpawnLocation);

	void AttackLineTrace(FHitResult& HitResult, const ACharacter* TargetActor, const FName& TargetBoneName);


	void AttackEnd();
	UFUNCTION(BlueprintCallable)
	void FirstSkillEnd();
	UFUNCTION(BlueprintCallable)
	void SecondSkillEnd();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillEnd();

	void Ready();

	void Jump();


	bool CanReload();
	void Reload();
	void ReloadEnd();

	bool CanTeleport();
	bool Teleport(const FVector& Location);

	void AttackTimerEnd();
	void FirstSKillTimerEnd();
	void SecondSkillTimerEnd();

private:
	UPROPERTY()
	class AEnemy* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARevenant_Bomb> BombClass;
	/*UPROPERTY(EditAnywhere)
	TSubclassOf<class AReaperTeleport> ReaperTeleportClass;*/

private:
	UPROPERTY(EditAnywhere, Category=Precentage)
	float FirstSkillPercentage = 100.f;
	UPROPERTY(EditAnywhere, Category = Precentage)
	float DefaultAttackPercentage = 45.f;
	UPROPERTY(EditAnywhere, Category = Precentage)
	float StrongAttackPercentage = 90.f;

private:
	FTimerHandle AttackTimerHandle;
	FTimerHandle FirstSkillTimerHandle;
	FTimerHandle SecondSkillTimerHandle;

	FVector TeleportLocation;

	UPROPERTY(EditAnywhere, Category = Timer)
	float DefaultAttackTimer = 0.79f;
	UPROPERTY(EditAnywhere, Category = Timer)
	float StrongAttackTimer = 1.4f;
	UPROPERTY(EditAnywhere, Category = Timer)
	float FirstSkillTimer = 5.f;
	UPROPERTY(EditAnywhere, Category = Timer)
	float SecondSkillTimer = 7.f;

	bool FirstSkillCoolTimeEnd = true;
	bool SecondSkillCoolTimeEnd = true;
	bool ThirdSkillCoolTimeEnd = true;

	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* DefaultAttackParticle;
	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* StrongAttackParticle;
	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* DefaultAttackHitParticle;
	UPROPERTY(EditAnywhere, Category = Particle)
	class UNiagaraSystem* StrongAttackHitParticle;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultDamage = 10.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float StrongDamage = 30.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackSphereRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	int32 MaxAmmo = 10;
	UPROPERTY(EditAnywhere, Category = Combat)
	int32 DefaultAttackSpendAmmo = 1;
	UPROPERTY(EditAnywhere, Category = Combat)
	int32 StrongAttackSpendAmmo = 3;
	int32 CurrentAmmo;

private:
	UPROPERTY()
	TArray<FHitResult> HitResults;

	EAICombatState CurrentCombatState = EAICombatState::ECS_Idle;

	UPROPERTY(EditAnywhere, Category = Animation)
	bool IsOffUpperarmTransformModify = true;

public:
	FORCEINLINE void SetOwnerCharacter(AEnemy* Enemy) { OwnerCharacter = Enemy; }
	FORCEINLINE bool GetIsOffUpperarmTransformModify() { return IsOffUpperarmTransformModify; }
	FORCEINLINE void SetIsOffUpperarmTransformModify(bool IsOff) { IsOffUpperarmTransformModify = IsOff; }
};
