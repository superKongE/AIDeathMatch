#include "DeathMatch/Character/Preview/PreviewActor.h"


APreviewActor::APreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetEnableGravity(false);
}
void APreviewActor::BeginPlay()
{
	Super::BeginPlay();
	
}
void APreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

