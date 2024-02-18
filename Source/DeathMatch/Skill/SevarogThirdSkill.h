#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SevarogThirdSkill.generated.h"

UCLASS()
class DEATHMATCH_API ASevarogThirdSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	ASevarogThirdSkill();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	bool IsCharacterBehindWall(ACharacter* Chr);

	void SetOwnerInfo(class ASevarogCharacter* Chr);

private:
	UPROPERTY(EditAnywhere, Category = Skill)
	class ASevarogCharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = Skill)
	class UBoxComponent* BoxComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = Skill)
	class UStaticMeshComponent* MeshComponent = nullptr;

	class TSet<ACharacter*> OverlappedChrSet;


	UPROPERTY(EditAnywhere, Category = Skill)
	float Damage = 50.f;
	UPROPERTY(EditAnywhere, Category = Skill)
	float LifeTime = 1.f;
	float CurrentTime = 0.f;
	UPROPERTY(EditAnywhere, Category = Skill)
	float Speed = 1.f;

	FVector StartLocation;
	FVector FowardVector;

	UPROPERTY(EditAnywhere, Category = Skill)
	FVector BoxComponentScale = FVector(0.75f, 11.75f, 5.5f);
	UPROPERTY(EditAnywhere, Category = Skill)
	FVector MeshComponentScale = FVector(0.1f, 0.05f, 0.05f);
};
