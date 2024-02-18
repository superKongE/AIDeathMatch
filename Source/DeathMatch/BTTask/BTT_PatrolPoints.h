// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_PatrolPoints.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API UBTT_PatrolPoints : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_PatrolPoints(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	int32 Index = 0;
};
