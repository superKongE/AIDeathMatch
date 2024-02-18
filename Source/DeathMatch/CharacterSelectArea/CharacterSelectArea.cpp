#include "DeathMatch/CharacterSelectArea/CharacterSelectArea.h"
#include "Components/BoxComponent.h"

#include "DeathMatch/Character/RootCharacter.h"

ACharacterSelectArea::ACharacterSelectArea()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxArea"));
	SetRootComponent(BoxArea);
}
void ACharacterSelectArea::BeginPlay()
{
	Super::BeginPlay();
	
	BoxArea->OnComponentBeginOverlap.AddDynamic(this, &ACharacterSelectArea::OnComponentBeginOverlap);
	BoxArea->OnComponentEndOverlap.AddDynamic(this, &ACharacterSelectArea::OnComponentEndOverlap);
}
void ACharacterSelectArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACharacterSelectArea::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARootCharacter* Character = Cast<ARootCharacter>(OtherActor);
	if (Character)
	{
		Character->OverlapCharacterSelectArea(true);
	}
}
void ACharacterSelectArea::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARootCharacter* Character = Cast<ARootCharacter>(OtherActor);
	if (Character)
	{
		Character->OverlapCharacterSelectArea(false);
	}
}
