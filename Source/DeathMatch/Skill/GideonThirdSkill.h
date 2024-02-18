#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GideonThirdSkill.generated.h"

UCLASS()
class DEATHMATCH_API AGideonThirdSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	AGideonThirdSkill();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ThirdSkillStartTimeEnd();
	void ThirdSkillLoopTimeEnd();
	void ThirdSkillEndTimeEnd();

public:
	UPROPERTY()
	class ARootCharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	class UStaticMeshComponent* CylinderMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	class UParticleSystem* ThirdSkillParticleStart = nullptr;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	class UParticleSystem* ThirdSkillParticleLoop = nullptr;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	class UParticleSystem* ThirdSkillParticleEnd = nullptr;

	//UPROPERTY()
	TDoubleLinkedList<class ACharacter*> CharList;

	FTimerHandle ThirdSkillStartTimerHandle;
	FTimerHandle ThirdSkillLoopTimerHandle;
	FTimerHandle ThirdSkillEndTimerHandle;

	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float ThirdSkillStartTime = 1.f;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float ThirdSkillLoopTime = 4.8;
	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float ThirdSkillEndTime = 0.55f;

	bool bEnd = false;

	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	float Damage = 0.5f;

	UPROPERTY(EditAnywhere, Category = ThirdSkill)
	FVector ThirdSkillScale = FVector(15.f, 15.f, 1.f);
};
