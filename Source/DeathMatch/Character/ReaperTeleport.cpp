#include "DeathMatch/Character/ReaperTeleport.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

AReaperTeleport::AReaperTeleport()
{
	PrimaryActorTick.bCanEverTick = false;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleRadius(34.f);
	Capsule->SetCapsuleHalfHeight(88.f);
	SetRootComponent(Capsule);

	TeleportMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Mesh"));
	TeleportMesh->SetupAttachment(RootComponent);
}

void AReaperTeleport::BeginPlay()
{
	Super::BeginPlay();
	
}

void AReaperTeleport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AReaperTeleport::SetMeshHide(bool bHide)
{
	if (bHide)
	{
		TeleportMesh->SetHiddenInGame(true);
	}
	else
	{
		TeleportMesh->SetHiddenInGame(false);
	}
}
