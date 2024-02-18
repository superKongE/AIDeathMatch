#include "DeathMatch/Skill/GideonSecondSkill.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#include "DeathMatch/Character/RootCharacter.h"

AGideonSecondSkill::AGideonSecondSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	CylinderComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder Component"));
	CylinderComponent->SetVisibility(false);
	CylinderComponent->SetWorldScale3D(CylinderScale);
	SetRootComponent(CylinderComponent);

	SecondSkillTopLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SecondSkillTopLocation"));
	SecondSkillTopLocation->SetupAttachment(RootComponent);
	SecondSkillTopLocation->SetRelativeLocation(FVector(0.f, 0.f, 500.f));

	SecondSkillBottomLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SecondSkillBottomLocation"));
	SecondSkillBottomLocation->SetupAttachment(RootComponent);
}

void AGideonSecondSkill::BeginPlay()
{
	Super::BeginPlay();
	
	if (SecondSkillBottom && SecondSkillTop)
	{
		UParticleSystemComponent* SecondSkillBottomParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SecondSkillBottom, GetActorLocation());
		FVector Scale = SecondSkillBottomParticle->GetComponentScale();
		SecondSkillBottomParticle->SetWorldScale3D(Scale * SecondSkillBottomScale);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SecondSkillTop, GetActorLocation() + FVector(0.f,0.f,500.f));
	}

	if (CylinderComponent)
	{
		CylinderComponent->OnComponentBeginOverlap.AddDynamic(this, &AGideonSecondSkill::OnComponentBeginOverlap);
		CylinderComponent->OnComponentEndOverlap.AddDynamic(this, &AGideonSecondSkill::OnComponentEndOverlap);
	}

	OwnerCharacter = Cast<ARootCharacter>(GetOwner());
	if(OwnerCharacter)
		PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AGideonSecondSkill::DestroyTimeEnd, DestroyTime);
}

void AGideonSecondSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OverlappedChrArray.Num() > 0 && PlayerController)
	{
		for (ACharacter* Chr : OverlappedChrArray)
		{
			UGameplayStatics::ApplyDamage(Chr, Damage, PlayerController, GetOwner(), UDamageType::StaticClass());
		}
	}
}

void AGideonSecondSkill::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Chr = Cast<ACharacter>(OtherActor);
	if (Chr)
	{
		OverlappedChrArray.AddTail(Chr);
	}
}
void AGideonSecondSkill::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Chr = Cast<ACharacter>(OtherActor);
	if (Chr)
	{
		OverlappedChrArray.RemoveNode(Chr);
	}
}


void AGideonSecondSkill::DestroyTimeEnd()
{
	Destroy();
}