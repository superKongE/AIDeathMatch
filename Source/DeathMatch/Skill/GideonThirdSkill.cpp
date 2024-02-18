#include "DeathMatch/Skill/GideonThirdSkill.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "DeathMatch/Character/RootCharacter.h"

AGideonThirdSkill::AGideonThirdSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder Mesh"));
	CylinderMesh->SetWorldScale3D(ThirdSkillScale);
	CylinderMesh->SetVisibility(false);
	SetRootComponent(CylinderMesh);
}

void AGideonThirdSkill::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ARootCharacter>(GetOwner());
	
	if (CylinderMesh)
	{
		CylinderMesh->OnComponentBeginOverlap.AddDynamic(this, &AGideonThirdSkill::OnComponentBeginOverlap);
		CylinderMesh->OnComponentEndOverlap.AddDynamic(this, &AGideonThirdSkill::OnComponentEndOverlap);
	}

	if (ThirdSkillParticleStart && ThirdSkillParticleLoop && ThirdSkillParticleEnd)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThirdSkillParticleStart, GetActorLocation());

		GetWorld()->GetTimerManager().SetTimer(ThirdSkillStartTimerHandle, this, &AGideonThirdSkill::ThirdSkillStartTimeEnd, ThirdSkillStartTime, false);
	}
}

void AGideonThirdSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OwnerCharacter && bEnd)
	{
		for (ACharacter* Chr : CharList)
		{
			if (Chr)
			{
				FVector ForceVector = (GetActorLocation() - Chr->GetActorLocation()) / 3.f;
				Chr->LaunchCharacter(ForceVector, true, true);

				UGameplayStatics::ApplyDamage(Chr, Damage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
			}
		}
	}
}



void AGideonThirdSkill::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Chr = Cast<ACharacter>(OtherActor);
	if (Chr && Chr != Cast<ACharacter>(GetOwner()))
	{
		CharList.AddTail(Chr);
	}
}
void AGideonThirdSkill::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Chr = Cast<ACharacter>(OtherActor);
	if (Chr)
	{
		CharList.RemoveNode(Chr);
	}
}


void AGideonThirdSkill::ThirdSkillStartTimeEnd()
{
	bEnd = true;

	UParticleSystemComponent* particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThirdSkillParticleLoop, GetActorLocation());
	particle->

	GetWorld()->GetTimerManager().SetTimer(ThirdSkillStartTimerHandle, this, &AGideonThirdSkill::ThirdSkillLoopTimeEnd, ThirdSkillLoopTime, false);
}
void AGideonThirdSkill::ThirdSkillLoopTimeEnd()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThirdSkillParticleEnd, GetActorLocation());

	GetWorld()->GetTimerManager().SetTimer(ThirdSkillStartTimerHandle, this, &AGideonThirdSkill::ThirdSkillEndTimeEnd, ThirdSkillEndTime, false);
}
void AGideonThirdSkill::ThirdSkillEndTimeEnd()
{
	bEnd = false;

	Destroy();
}