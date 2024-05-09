#include "DeathMatch/Weapon/Revenant_Bomb.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ARevenant_Bomb::ARevenant_Bomb()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereArea = CreateDefaultSubobject<USphereComponent>(TEXT("SphereArea"));
	SetRootComponent(SphereArea);

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb"));
	BombMesh->SetupAttachment(RootComponent);

	BombMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BombMovementComponent"));
	BombMovementComponent->InitialSpeed = 4000.f;
	BombMovementComponent->MaxSpeed = 4000.f;
	BombMovementComponent->ProjectileGravityScale = BombGravity;
}

void ARevenant_Bomb::BeginPlay()
{
	Super::BeginPlay();

	SphereArea->OnComponentHit.AddDynamic(this, &ARevenant_Bomb::OnHit);
}

void ARevenant_Bomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//                                        때리는 것(총알, 칼 등), 맞은 액터, 맞은 컴포넌트
void ARevenant_Bomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor != GetOwner())
	{
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActor(this);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombParticle, GetActorLocation());
		UGameplayStatics::ApplyRadialDamage(
			this,
			Damage, // 최대 데미지
			GetActorLocation(), // 원의 중심 위치
			DamageRange,
			UDamageType::StaticClass(), // 데미지 타입
			TArray<AActor*>(), // 이 데미지를 받지 않을 액터의 배열 (예를들어 발사한 자신 또는 팀원들), 빈 배열을 줌으로써 자신 포함 모두 데미지를 받는다
			this, // 피해를 유발하는 액터
			GetInstigatorController() // 피해를 유발하는 액터의 컨트롤러
		);

		Destroy();
	}
}

void ARevenant_Bomb::Bomb(FVector BombLocation)
{
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(this);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombParticle, BombLocation);
	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage, 
		GetActorLocation(), 
		DamageRange,
		UDamageType::StaticClass(),
		TArray<AActor*>(), 
		this, 
		GetInstigatorController()
	);

	Destroy();
}

