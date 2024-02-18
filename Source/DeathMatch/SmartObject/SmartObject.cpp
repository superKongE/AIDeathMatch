#include "DeathMatch/SmartObject/SmartObject.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"

ASmartObject::ASmartObject()
{
	PrimaryActorTick.bCanEverTick = true;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SetRootComponent(Billboard);

	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingDirection"));
	FacingDirection->SetupAttachment(Billboard);
}

void ASmartObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASmartObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

