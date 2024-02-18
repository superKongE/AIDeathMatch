#include "DeathMatch/BTTask/BTT_Teleport.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "DeathMatch/Enemy/Enemy.h"

UBTT_Teleport::UBTT_Teleport(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Teleport";
}

EBTNodeResult::Type UBTT_Teleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if(MyBlackboard == nullptr) return EBTNodeResult::Failed;

	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (MyController == nullptr || MyBlackboard == nullptr) return EBTNodeResult::Failed;

	AEnemy* Chr = Cast<AEnemy>(MyController->GetPawn());
	if (Chr == nullptr || !Chr->GetCanTeleport()) return EBTNodeResult::Failed;

	const FName name = TEXT("TeleportLocation");
	TeleportLocation = MyBlackboard->GetValueAsVector(name);
	//DrawDebugSphere(GetWorld(), TeleportLocation, 100.f, 20, FColor::Red, true);
	//UE_LOG(LogTemp, Error, TEXT("%f / %f / %f"), TeleportLocation.X, TeleportLocation.Y, TeleportLocation.Z);
	if (Chr && TeleportLocation != FVector::ZeroVector)
	{
		if(Chr->Teleport(TeleportLocation))	
			return EBTNodeResult::Succeeded;
		else
			return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}