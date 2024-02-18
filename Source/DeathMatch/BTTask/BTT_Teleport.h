// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_Teleport.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API UBTT_Teleport : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_Teleport(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FVector TeleportLocation = FVector::ZeroVector;
};
