#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SevarogSecondSkill.generated.h"

UCLASS()
class DEATHMATCH_API ASevarogSecondSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	ASevarogSecondSkill();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ShockWaveMesh = nullptr;

	class TSet<ACharacter*> OverlappedCharacterSet;

	UPROPERTY(EditAnywhere)
	float Damage = 30.f;

	UPROPERTY(EditAnywhere)
	float CurrentZScale = 2.f;
	float CurrentScale = 0.f;
	UPROPERTY(EditAnywhere)
	float MaxScale = 2.f;
	UPROPERTY(EditAnywhere)
	float ScaleInterpSpeed = 5.f;
};
