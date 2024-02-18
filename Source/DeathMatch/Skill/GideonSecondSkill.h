#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GideonSecondSkill.generated.h"

UCLASS()
class DEATHMATCH_API AGideonSecondSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	AGideonSecondSkill();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DestroyTimeEnd();

public:
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class UStaticMeshComponent* CylinderComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class USceneComponent* SecondSkillTopLocation = nullptr;
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class USceneComponent* SecondSkillBottomLocation = nullptr;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class UParticleSystem* SecondSkillBottom = nullptr;
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	class UParticleSystem* SecondSkillTop = nullptr;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	float SecondSkillBottomScale = 1.5f;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	float Damage = 1.f;

	UPROPERTY(EditAnywhere, Category = SecondSkill)
	FVector CylinderScale = { 10.f,10.f,5.f };
	
	//UPROPERTY()
	TDoubleLinkedList<class ACharacter*> OverlappedChrArray;

	class ARootCharacter* OwnerCharacter = nullptr;
	class APlayerController* PlayerController = nullptr;

	FTimerHandle DestroyTimerHandle;
	UPROPERTY(EditAnywhere, Category = SecondSkill)
	float DestroyTime = 2.55f;
};
