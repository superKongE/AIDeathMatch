// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Revenant_Bomb.generated.h"

UCLASS()
class DEATHMATCH_API ARevenant_Bomb : public AActor
{
	GENERATED_BODY()
	
public:	
	ARevenant_Bomb();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, FVector NormalImpulse,
			const FHitResult& Hit);

	/*UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, 
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/

	

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void Bomb(FVector BombLocation);

private:
	UPROPERTY(EditAnywhere)
	class UParticleSystem* BombParticle = nullptr;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BombMesh = nullptr;
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereArea = nullptr;
	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* BombMovementComponent = nullptr;

private:
	UPROPERTY(EditAnywhere)
	float BombInitialSpeed = 4000.f;
	UPROPERTY(EditAnywhere)
	float BombMaxSpeed = 4000.f;
	UPROPERTY(EditAnywhere)
	float BombGravity = 1.f;

	UPROPERTY(EditAnywhere)
	float Damage = 100.f;
	UPROPERTY(EditAnywhere)
	float DamageRange = 1000.f;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh() { return BombMesh; }
};
