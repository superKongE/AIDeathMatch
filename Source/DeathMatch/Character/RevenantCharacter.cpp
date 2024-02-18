#include "DeathMatch/Character/RevenantCharacter.h"

#include "Components/ChildActorComponent.h"

#include "DeathMatch/CombatComponent/RevenantCombatComponent.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"

ARevenantCharacter::ARevenantCharacter()
{
	BombPoint = CreateDefaultSubobject<UChildActorComponent>(TEXT("Bomb"));
	BombPoint->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<URevenantCombatComponent>(TEXT("RevenantCombatComponent"));

	bCharacterUseFootIK = true;

	CharacterNum = 0;

	ViewTargets.Emplace(FName("head"));
	ViewTargets.Emplace(FName("spine_01"));
	ViewTargets.Emplace(FName("lowerarm_l"));
	ViewTargets.Emplace(FName("hand_l"));
	ViewTargets.Emplace(FName("lowerarm_r"));
	ViewTargets.Emplace(FName("hand_r"));
	ViewTargets.Emplace(FName("leg"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>RevenantSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/ParagonRevenant/Characters/Heroes/Revenant/Meshes/Revenant.Revenant'"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimClassAsset(TEXT("AnimBlueprint'/Game/Blueprint/Animation/Revenant_Animation/BP_RevenantAnimation.BP_RevenantAnimation'"));

	static ConstructorHelpers::FClassFinder<AHUD> HUDAsset(TEXT("Blueprint'/Game/Blueprint/HUD/BP_RevenantHUD.BP_RevenantHUD_C'"));

	if (RevenantSkeletalMeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(RevenantSkeletalMeshAsset.Object);
	if (AnimClassAsset.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimClassAsset.Object->GetAnimBlueprintGeneratedClass());

	if (HUDAsset.Succeeded())
		HUD = HUDAsset.Class;
}
void ARevenantCharacter::BeginPlay()
{
	Super::BeginPlay();
}
void ARevenantCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("Reload"), IE_Pressed, this, &ARevenantCharacter::Reload);
}


void ARevenantCharacter::Reload()
{
	RevenantCombatComponent = RevenantCombatComponent == nullptr ? Cast<URevenantCombatComponent>(GetCombatComponent()) : RevenantCombatComponent;
	if (RevenantCombatComponent)
	{
		RevenantCombatComponent->Reload();
	}
}



void ARevenantCharacter::ThirdSkillForLoopEnd()
{
	RevenantCombatComponent = RevenantCombatComponent == nullptr ? Cast<URevenantCombatComponent>(GetCombatComponent()) : RevenantCombatComponent;
	if (RevenantCombatComponent)
	{
		RevenantCombatComponent->ThirdSkillJumpToLoop();
	}
}