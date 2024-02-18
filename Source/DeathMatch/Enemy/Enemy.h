#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DeathMatch/Character/RevenantCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	EES_DefaultAttack UMETA(DisplayName = "DefaultAttack"),
	EES_StrongAttack UMETA(DisplayName = "StringAttack"),
	EES_FirstSkill UMETA(DisplayName = "FirstSkill"),
	EES_SecondSkill UMETA(DisplayName = "SecondSkill"),
	EES_ThirdSkill UMETA(DisplayName = "ThirdSkill"),
	EES_Reload UMETA(DisplayName = "Reload"),
	EES_DeathFoward UMETA(DisplayName = "DeathFoward"),
	EES_DeathBackward UMETA(DisplayName = "Reload")
};

UCLASS()
class DEATHMATCH_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void PlayAnimMonatage(EEnemyState EnemyState);

	void CanSeeEnemy(class ACharacter* ViewTargetActor);

	void SetTargetBone(FName& BoneName) const;

	void SetSpendAllAmmo(bool bSpendAll);

	void AISightPerception();
	void Detect(float DeltaTime);

	FVector GetBombSpawnPoint();

	bool Teleport(const FVector& TeleportLocation);
	void SetCanTeleport(bool bCan);


	void Spawn();
	void Death();
	void DeathEnd();


	UFUNCTION(BlueprintCallable)
	void Attack();
	UFUNCTION(BlueprintCallable)
	void Reload();
	UFUNCTION(BlueprintCallable)
	void ReloadEnd();


	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	UFUNCTION(BlueprintCallable)
	void FirstSkillEnd();
	UFUNCTION(BlueprintCallable)
	void SecondSkillEnd();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillEnd();


	UFUNCTION(BlueprintCallable)
	bool FootStepNotify(const FName& BoneName);

private:
	UPROPERTY(EditAnywhere)
	class UEnemyCombatComponent* CombatComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UChildActorComponent* BombPoint = nullptr;

	UPROPERTY()
	class UAnimInstance* AnimInstance = nullptr;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY()
	class AEnemyController* EnemyController = nullptr;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent = nullptr;

	UPROPERTY();
	class AActor* OverlappedActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess="true"))
	class ASmartObject* SmartObject = nullptr;

	// 이동해야 할 지점
	// MakeEditWidget을 통해 해당 FVector가 WorldSpace가 아닌 
	// Enemy 캐릭터의 LocalSpace로 된다
	UPROPERTY(EditAnywhere, Category = BehaviorTree, meta = (AllowPrivateAccess="true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere)
	TArray<FName> ViewTargets;

private:
	FTimerHandle DeathSpawnTimerHandle;
	FTimerHandle DeathEndTimerHandle;
	UPROPERTY(EditAnywhere, Category=Timer)
	float DeathSpawnTimer = 6.f;
	UPROPERTY(EditAnywhere, Category = Timer)
	float DeathEndTimer = 2.5f;
	bool IsDeath = false;
	bool IsSpawned;
	bool IsFowardDeath;

private:
	UPROPERTY()
	class ACharacter* TargetActor = nullptr;
	FVector ViewTargetActorLocation;
	FVector HitTargetLocation;
	bool bViewTarget = false;
	mutable FName TargetBoneName = TEXT("head");

	FVector PrevTargetActorLocation;
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, Category = Timer)
	float MaxDetectEnemyTime = 30.f;
	float CurrentDetectEnemyTime = 0.f;
	bool IsFindPlayerTimeEnd = true;

	float Pitch;

	UPROPERTY(EditAnywhere, Category = Combat)
	float MaxHP = 100.f;
	float CurrentHP;

	UPROPERTY(EditAnywhere, Category = Combat)
	float MaxWalkSpeed = 600;

	bool bSpendAllAmmo = false;

	bool bOverlap = false;

	bool bCanTeleport = true;


private:
	UPROPERTY(EditAnywhere, Category = Footstep)
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


public:
	FORCEINLINE ACharacter* GetTargetActor() { return TargetActor; }
	FORCEINLINE AEnemyController* GetAIController() { return EnemyController; }
	FORCEINLINE AEnemyController* GetEnemyController() { return EnemyController; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }
	FORCEINLINE UEnemyCombatComponent* GetCombatComponent() { return CombatComponent; }
	FORCEINLINE TArray<FName>& GetViewTargets() { return ViewTargets; }
	FORCEINLINE float GetAimOffset() { return Pitch; }
	FORCEINLINE void SetHitTargetLocation(FVector& Location) { HitTargetLocation = Location; }
	FORCEINLINE FVector GetHitTargetLocation() { return HitTargetLocation; }
	FORCEINLINE ASmartObject* GetSmartObject() { return SmartObject; }
	FORCEINLINE const FName GetTargetBoneName() { return TargetBoneName; }
	FORCEINLINE bool GetIsDeath() { return IsDeath; }
	FORCEINLINE bool GetIsFowardDeath() { return IsFowardDeath; }
	FORCEINLINE bool GetIsSpawned() { return IsSpawned; }
	FORCEINLINE void SetIsSpawned(bool bSpawned) { IsSpawned = bSpawned; }
	FORCEINLINE bool GetCanTeleport() { return bCanTeleport; }
	FORCEINLINE bool GetbViewTarget() { return bViewTarget; }
};
