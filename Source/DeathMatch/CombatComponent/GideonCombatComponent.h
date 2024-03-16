// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "GideonCombatComponent.generated.h"

UENUM(BlueprintType)
enum class EGideonSkill : uint8 {
	EGS_DefaultAttack UMETA(DisplayName = "DefaultAttack"),
	EGS_StrongAttack UMETA(DisplayName = "StringAttack"),
	EGS_FirstSkill UMETA(DisplayName = "FirstSkill"),
	EGS_SecondSkill UMETA(DisplayName = "SecondSkill"),
	EGS_ThirdSkill UMETA(DisplayName = "ThirdSkill"),
	EGS_FowardDeath UMETA(DisplayName = "FowardDeath"),
	EGS_BackwardDeath UMETA(DisplayName = "BackwardDeath"),
};

UCLASS()
class DEATHMATCH_API UGideonCombatComponent : public URootCombatComponent
{
	GENERATED_BODY()

public:
	UGideonCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	virtual void BeginPlay() override;
	
public:
	virtual void SetCharacter(ARootCharacter* Character);

	virtual void Ready();
	virtual void Death();


	UFUNCTION(BlueprintCallable)
	void DefaultAttackShoot();
	virtual void DefaultAttack(const TArray<FHitResult>& HitResultsArr);
	virtual bool CanDefaultAttack();


	virtual void StrongAttack(const TArray<FHitResult>& HitResultsArr);


	virtual void FirstSkillPressed();
	void Jump();
	void FlyingWithJump(float DeltaTime);


	virtual void SecondSkillPressed();
	virtual void SecondSkillReleased();
	void CancleSecondSkill();
	void SetSecondSkillCoolTime();
	UFUNCTION(BlueprintCallable)
	void SecondSkillJumpToLoop();
	void SetSecondSkillDecalLocation();


	virtual void ThirdSkillPressed();
	virtual void ThirdSkillEnd();
	UFUNCTION(BlueprintCallable)
	void ThirdSkillRaiseUp();
	void SpawnThirdSkill();
	void ThirdSkilFallDown(float DeltaTime);


	void PlayDefaultAttackMontage(EGideonSkill State);

private:
	UPROPERTY()
	class AGideonCharacter* GideonCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	UClass* AGideonDefaultProjectileClass_one = nullptr;
	UPROPERTY(EditAnywhere)
	UClass* AGideonDefaultProjectileClass_two = nullptr;
	UPROPERTY(EditAnywhere)
	UClass* AGideonDefaultProjectileClass_three = nullptr;

	UPROPERTY(EditAnywhere)
	UClass* AGideonStrongProjectileClass = nullptr;

private:
	FHitResult CurrentHitResult;

private:// 우클릭 관련 변수들
	UPROPERTY(EditAnywhere, Category = RightClick)
	float RightClickTraceDistance = 4000.f;
	UPROPERTY(EditAnywhere, Category = RightClick)
	float RightClickReverseTraceDistance = 4000.f;
	UPROPERTY(EditAnywhere, Category = RightClick)
	float AddDistanceforInterp = 300.f;

	UPROPERTY(EditAnywhere, Category = RightClick)
	float RightClickInterpSpeed = 2.f;

	bool bRightClickInterp = false;

	FVector CurrentLocation;
	FVector TargetLocation;

private: // First Skill 관련 변수들
	bool IsFly;
	bool bFirstSkillInterp;
	bool bJumpIntrep;

	UPROPERTY(EditAnywhere, Category = FirstSkill)
	float FirstSkillInterpSpeed = 6.f;
	float TargetZ;

	
	UPROPERTY(EditAnywhere, Category = FirstSkill)
	float FirstSkillHeight = 500.f;
	UPROPERTY(EditAnywhere, Category = FirstSkill)
	float JumpHeight = 500.f;
	UPROPERTY(EditAnywhere, Category = FirstSkill)
	float Max_ZHeight = 3950.f;

	UPROPERTY(EditAnywhere, Category = FirstSkill)
	float FlyDownAmount = -1.f;
	float DownTargetZ;

private: // second skill 관련 변수들
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	UClass* SecondSkillClass = nullptr;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	TSubclassOf<class ADecalActor> SecondSkillDecalClass = nullptr;
	class ADecalActor* SecondSkillDecal = nullptr;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class UMaterialInterface* SecondSkillDecalInterface = nullptr;

	class UMaterialInstanceDynamic* SecondSkillDecalInst = nullptr;

	FVector SecondSkillSpawnLocation;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	float SecondSkillDecalScale = 1.5f;

private: // Third skill 관련 변수들
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	UClass* ThirdSkillClass = nullptr;

	UPROPERTY()
	class AGideonThirdSkill* SpawnedGideonThirdSkill = nullptr;

	bool bCanRaiseUp = true;
	bool bThirdSkillRaiseUp = false;
	bool bThirdSkillFallDown = false;

	float RaiseUpCurrentZ;

	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float RaiseUpDistance = 10.f;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float RaiseUpTime = 0.5f;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float RaiseUpInterpSpeed = 2.f;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float NeedToFallingDownDistance = 100.f;
	float CurrentTime;

private:
	int32 DefaultAttackCount = 0;
	
public:
	FORCEINLINE bool GetIsFly() { return IsFly; }
	FORCEINLINE bool GetThirdSkillPressed() { return bThirdSkillPressed; }
	FORCEINLINE bool GetbRightClickInterp() { return bRightClickInterp; }
};
