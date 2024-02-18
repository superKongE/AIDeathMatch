#include "DeathMatch/Weapon/GideonProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "DeathMatch/Enemy/Enemy.h"

AGideonProjectile::AGideonProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereArea = CreateDefaultSubobject<USphereComponent>(TEXT("SphereArea"));

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2048.f;
	ProjectileMovementComponent->MaxSpeed = 2048.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}
void AGideonProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereArea->OnComponentHit.AddDynamic(this, &AGideonProjectile::OnComponentHit);

	if (Particle)
		UGameplayStatics::SpawnEmitterAttached(Particle, SphereArea, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
}
void AGideonProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGideonProjectile::Hit(const FVector& ImpactPoint)
{
	if (HitParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);

	Destroy();
}
void AGideonProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ARootCharacter* OwnerCharacter = Cast<ARootCharacter>(GetOwner());
	if (OwnerCharacter == nullptr) return;

	UParticleSystemComponent* SpawnedParticle = nullptr;
	if(HitParticle)
		SpawnedParticle =  UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Hit.ImpactPoint);

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy == nullptr)
	{
		Destroy();
		return;
	}

	UGameplayStatics::ApplyDamage(Enemy, Damage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());

	/*if (SpawnedParticle)
	{
		SpawnedParticle->SetVisibility(false);
		SpawnedParticle->DestroyComponent();
		SpawnedParticle = nullptr;
	}*/

	Destroy();
}
