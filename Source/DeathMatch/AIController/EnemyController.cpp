#include "DeathMatch/AIController/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "DeathMatch/Enemy/Enemy.h"
#include "DeathMatch/Character/RootCharacter.h"

AEnemyController::AEnemyController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
	Sight->SightRadius = 4000.f;
	Sight->LoseSightRadius = Sight->SightRadius + 500.f;
	Sight->PeripheralVisionAngleDegrees = 360.f;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());
}
void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	AEnemy* Enemy = Cast<AEnemy>(GetPawn());
	if (Enemy)
		OwnerCharacter = Enemy;
}
void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr) return;

	// 감지 상태가 바뀌면 호출됨
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnPerception);

	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
			
			// 비헤이비어 트리 실행
			if (Enemy->GetBehaviorTree())
				RunBehaviorTree(Enemy->GetBehaviorTree());
		}
	}
}


void AEnemyController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<AEnemy>(GetPawn()) : OwnerCharacter;
	ARootCharacter* Chr = Cast<ARootCharacter>(Actor);
	if (Chr && OwnerCharacter)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			OwnerCharacter->CanSeeEnemy(Chr);
			SetFocus(Chr);
		}
		else
		{

			ClearFocus(EAIFocusPriority::Gameplay);
			OwnerCharacter->CanSeeEnemy(nullptr);
		}
	}
}