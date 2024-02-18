// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyController();

	virtual void BeginPlay();
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY()
	class AEnemy* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = AIBehavior, meta=(AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = AIBehavior, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UAIPerceptionComponent* AIPerceptionComponent = nullptr;

	UPROPERTY()
	class UAISenseConfig_Sight* Sight = nullptr;

public:
	uint8 EnemyKeyId;
	uint8 LocationKeyId;
	uint8 ContactKeyId;

public:
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() { return BlackboardComponent; }
};
