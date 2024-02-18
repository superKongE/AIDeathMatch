#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GideonProjectile.generated.h"

UCLASS()
class DEATHMATCH_API AGideonProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGideonProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Hit(const FVector& ImpactPoint);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereArea = nullptr;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(EditAnywhere, Category=Particle)
	class UParticleSystem* Particle = nullptr;

	UPROPERTY(EditAnywhere, Category = Particle)
	class UParticleSystem* HitParticle = nullptr;
	
	UPROPERTY(EditAnywhere, Category=Damage)
	float Damage = 10.f;
};
