#include "DeathMatch/CoverActor/CoverActor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

ACoverActor::ACoverActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Colider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Colider->SetupAttachment(RootComponent);
}

void ACoverActor::BeginPlay()
{
	Super::BeginPlay();
	
	Colider->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::OnOverlapBegin);
	Colider->OnComponentEndOverlap.AddDynamic(this, &ACoverActor::OnOverlapEnd);

	GetComponents<UArrowComponent>(CoverSpots, true);
	Debug();
}

void ACoverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ACoverActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<AEnemy>(OtherActor);
	if (Character)
	{
		Available = true;
	}
}
void ACoverActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AEnemy>(OtherActor) == Character)
	{
		Character = nullptr;
		Available = false;
	}
}
