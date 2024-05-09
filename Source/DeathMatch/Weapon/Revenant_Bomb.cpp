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

//                                        ������ ��(�Ѿ�, Į ��), ���� ����, ���� ������Ʈ
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
			Damage, // �ִ� ������
			GetActorLocation(), // ���� �߽� ��ġ
			DamageRange,
			UDamageType::StaticClass(), // ������ Ÿ��
			TArray<AActor*>(), // �� �������� ���� ���� ������ �迭 (������� �߻��� �ڽ� �Ǵ� ������), �� �迭�� �����ν� �ڽ� ���� ��� �������� �޴´�
			this, // ���ظ� �����ϴ� ����
			GetInstigatorController() // ���ظ� �����ϴ� ������ ��Ʈ�ѷ�
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

