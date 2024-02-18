#include "DeathMatch/Skill/SevarogThirdSkill.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "DeathMatch/Enemy/Enemy.h"
#include "DeathMatch/Character/SevarogCharacter.h"

ASevarogThirdSkill::ASevarogThirdSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetWorldScale3D(BoxComponentScale);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetWorldScale3D(MeshComponentScale);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASevarogThirdSkill::BeginPlay()
{
	Super::BeginPlay();

	FowardVector = GetActorForwardVector();
	
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASevarogThirdSkill::OnComponentBeginOverlap);
	}
}
void ASevarogThirdSkill::SetOwnerInfo(ASevarogCharacter* Chr)
{
	SetOwner(Chr);
	OwnerCharacter = Chr;
	StartLocation = Chr->GetActorLocation();
}

void ASevarogThirdSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OwnerCharacter)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime <= LifeTime)
			SetActorLocation(GetActorLocation() + FowardVector * Speed);
		else
			Destroy();
	}
}



void ASevarogThirdSkill::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Chr = Cast<ACharacter>(OtherActor);
	if (Chr)
	{
		if (!OverlappedChrSet.Contains(Chr))
		{
			OverlappedChrSet.Emplace(Chr);

			ACharacter* OwnerChr = Cast<ACharacter>(GetOwner());
			if (OwnerChr)
			{
				if (!IsCharacterBehindWall(Chr))
					UGameplayStatics::ApplyDamage(Chr, Damage, OwnerChr->GetController(), OwnerChr, UDamageType::StaticClass());
			}
		}
	}
}


bool ASevarogThirdSkill::IsCharacterBehindWall(ACharacter* Chr)
{
	AEnemy* Enemy = Cast<AEnemy>(Chr);
	if (Enemy == nullptr) return false;

	TArray<FName>& ViewTargetsArr = Enemy->GetViewTargets();
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(Enemy);
	FQP.AddIgnoredActor(GetOwner());
	
	int32 cnt = 0;
	for (FName& TargetBone : ViewTargetsArr)
	{
		FVector SocketLocation = Enemy->GetMesh()->GetSocketLocation(TargetBone);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, SocketLocation, ECollisionChannel::ECC_Visibility, FQP);
		//DrawDebugLine(GetWorld(), StartLocation, SocketLocation, FColor::Red, true);

		if (HitResult.bBlockingHit)
			cnt++;
	}

	if (cnt == ViewTargetsArr.Num()) return true;

	return false;
}