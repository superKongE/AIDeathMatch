#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoverActor.generated.h"

UCLASS()
class DEATHMATCH_API ACoverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoverActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void Debug();

private:
	UPROPERTY()
	class AEnemy* Character;

	UPROPERTY(EditAnywhere, Category = Components)
	class UBoxComponent* Colider;

	UPROPERTY(EditAnywhere, Category=Components)
	TArray<class UArrowComponent*> CoverSpots;

	bool Available = true;
};
