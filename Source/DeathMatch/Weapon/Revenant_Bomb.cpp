#include "DeathMatch/Weapon/Revenant_Bomb.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "DeathMatch/MovementComponent/BombMovementComponent.h"

ARevenant_Bomb::ARevenant_Bomb()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereArea = CreateDefaultSubobject<USphereComponent>(TEXT("SphereArea"));
	SetRootComponent(SphereArea);

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb"));
	BombMesh->SetupAttachment(RootComponent);

	BombMovementComponent = CreateDefaultSubobject<UBombMovementComponent>(TEXT("BombMovementComponent"));
	BombMovementComponent->InitialSpeed = 4000.f;
	BombMovementComponent->MaxSpeed = 4000.f;
	BombMovementComponent->bRotationFollowsVelocity = true;
	BombMovementComponent->ProjectileGravityScale = BombGravity;
}

void ARevenant_Bomb::BeginPlay()
{
	Super::BeginPlay();

	SphereArea->OnComponentBeginOverlap.AddDynamic(this, &ARevenant_Bomb::OnOverlapBegin);
}

void ARevenant_Bomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARevenant_Bomb::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

