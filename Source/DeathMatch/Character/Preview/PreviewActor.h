#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewActor.generated.h"

UCLASS()
class DEATHMATCH_API APreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APreviewActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* PreviewMesh;

	UPROPERTY(EditAnywhere)
	FName PreviewCharacterName;

public:
	FORCEINLINE FName GetPreviewCharacterName() { return PreviewCharacterName; }
};
