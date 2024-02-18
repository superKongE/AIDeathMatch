#include "DeathMatch/Character/SevarogCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"

#include "DeathMatch/CombatComponent/SevarogCombatComponent.h"

ASevarogCharacter::ASevarogCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bCharacterUseFootIK = false;

	ShadowActorSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ShadowActorSpawnPoint"));
	ShadowActorSpawnPoint->SetupAttachment(RootComponent);

	ThirdSkillSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ThirdSkillSpawnPoint"));
	ThirdSkillSpawnPoint->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<USevarogCombatComponent>(TEXT("SevarogCombatComponent"));

	CharacterNum = 2;

	ViewTargets.Emplace(FName("head"));
	ViewTargets.Emplace(FName("spine_01"));
	ViewTargets.Emplace(FName("lowerarm_l"));
	ViewTargets.Emplace(FName("hand_l"));
	ViewTargets.Emplace(FName("lowerarm_r"));
	ViewTargets.Emplace(FName("hand_r"));
	ViewTargets.Emplace(FName("leg"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SevarogSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Meshes/Sevarog.Sevarog'"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimClassAsset(TEXT("AnimBlueprint'/Game/Blueprint/Animation/Sevarog_Animation/BP_SevarogAnimation.BP_SevarogAnimation'"));

	static ConstructorHelpers::FClassFinder<AHUD> HUDAsset(TEXT("Blueprint'/Game/Blueprint/HUD/BP_SevarogHUD.BP_SevarogHUD_C'"));
	static ConstructorHelpers::FClassFinder<AActor> shadowActorAsset(TEXT("Blueprint'/Game/Blueprint/BP_ShadowActor.BP_ShadowActor_C'"));

	if (SevarogSkeletalMeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(SevarogSkeletalMeshAsset.Object);
	if (AnimClassAsset.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimClassAsset.Object->GetAnimBlueprintGeneratedClass());

	if (HUDAsset.Succeeded())
		HUD = HUDAsset.Class;
	if (shadowActorAsset.Succeeded())
		SevarogShadowClass = shadowActorAsset.Class;
}
// 컴포넌트들이 초기화된후 BeginPlay()가 호출되기전에 
// 즉 캐릭터가 소환되기전에 미리 변수 초기화작업
void ASevarogCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
void ASevarogCharacter::BeginPlay()
{
	Super::BeginPlay();

	SevarogCombatComponent = Cast<USevarogCombatComponent>(CombatComponent);

	if (SevarogShadowClass)
	{
		const USkeletalMeshSocket* ShadowSocket = GetMesh()->GetSocketByName(FName("ShadowPoint"));
		if (ShadowSocket)
		{
			ShadowActor = GetWorld()->SpawnActor<AActor>(SevarogShadowClass, ShadowActorSpawnPoint->GetComponentLocation(), FRotator::ZeroRotator);
			if (ShadowActor)
			{
				ShadowActor->SetOwner(this);
				ShadowActor->AttachToComponent(ShadowActorSpawnPoint, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}
void ASevarogCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
void ASevarogCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("DefaultAttack"), IE_Released, this, &ASevarogCharacter::LeftClickReleased);
	PlayerInputComponent->BindAction(FName("StrongAttack"), IE_Released, this, &ASevarogCharacter::RightClickReleased);
}


void ASevarogCharacter::Jump()
{
	if (SevarogCombatComponent)
	{
		SevarogCombatComponent->Dash();
	}
}


void ASevarogCharacter::LeftClickReleased()
{
	if (SevarogCombatComponent)
	{
		SevarogCombatComponent->LeftClickReleased();
	}
}

void ASevarogCharacter::RightClickReleased()
{
	if (SevarogCombatComponent)
	{
		SevarogCombatComponent->RightClickReleased();
	}
}


FVector ASevarogCharacter::GetThirdSkillSpawnPoint()
{
	return ThirdSkillSpawnPoint->GetComponentLocation();
}
FRotator ASevarogCharacter::GetThirdSkillRotator()
{
	return ThirdSkillSpawnPoint->GetComponentRotation();
}


void ASevarogCharacter::DestroyFromCharacterSelect()
{
	if (ShadowActor)
	{
		ShadowActor->SetHidden(true);
		ShadowActor->Destroy();
	}
}