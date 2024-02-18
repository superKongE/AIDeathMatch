// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatch/BTTask/BTT_PatrolPoints.h"
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

#include "DeathMatch/Enemy/Enemy.h"
#include "DeathMatch/SmartObject/PatrolPath.h"

UBTT_PatrolPoints::UBTT_PatrolPoints(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Get Path Points";
}

EBTNodeResult::Type UBTT_PatrolPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (MyController == nullptr || MyBlackboard == nullptr) return EBTNodeResult::Failed;

	AEnemy* Chr = Cast<AEnemy>(MyController->GetPawn());
	if (Chr == nullptr) return EBTNodeResult::Failed;

	if (Chr->GetIsSpawned())
	{
		Index = 0;
		Chr->SetIsSpawned(false);
	}

	APatrolPath* PathRef = Cast<APatrolPath>(Chr->GetSmartObject());
	if (PathRef == nullptr || PathRef->Locations.Num() < 1) return EBTNodeResult::Succeeded;

	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>("MoveToLocation", PathRef->Locations[Index]);
	Index = (Index + 1) % PathRef->Locations.Num();

	return EBTNodeResult::Succeeded;
}