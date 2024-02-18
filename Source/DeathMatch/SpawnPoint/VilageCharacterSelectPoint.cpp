#include "DeathMatch/SpawnPoint/VilageCharacterSelectPoint.h"

#include "Components/SkeletalMeshComponent.h"

AVilageCharacterSelectPoint::AVilageCharacterSelectPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	CharactetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
}

void AVilageCharacterSelectPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVilageCharacterSelectPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


