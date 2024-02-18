#include "DeathMatch/Skill/SevarogSecondSkill.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASevarogSecondSkill::ASevarogSecondSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	SetActorScale3D(FVector(0.f, 0.f, CurrentZScale));

	ShockWaveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShockWaveMesh"));
}

void ASevarogSecondSkill::BeginPlay()
{
	Super::BeginPlay();
	
	ShockWaveMesh->OnComponentBeginOverlap.AddDynamic(this, &ASevarogSecondSkill::OnComponentBeginOverlap);
}

void ASevarogSecondSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentScale = FMath::FInterpTo(CurrentScale, MaxScale, DeltaTime, ScaleInterpSpeed);
	SetActorScale3D(FVector(CurrentScale, CurrentScale, CurrentZScale));

	if (MaxScale - CurrentScale <= 1.f)
		Destroy();
}


void ASevarogSecondSkill::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* OverlappedChr = Cast<ACharacter>(OtherActor);
	if (OverlappedChr == GetOwner()) return;

	if (!OverlappedCharacterSet.Contains(OverlappedChr))
	{
		OverlappedCharacterSet.Emplace(OverlappedChr);
		UGameplayStatics::ApplyDamage(OverlappedChr, Damage, OverlappedChr->GetController(), GetOwner(), UDamageType::StaticClass());
	}
}
