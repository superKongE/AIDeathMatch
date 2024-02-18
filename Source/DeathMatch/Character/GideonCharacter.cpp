#include "DeathMatch/Character/GideonCharacter.h"

#include "DeathMatch/CharacterSelectComponent/CharacterSelectComponent.h"
#include "DeathMatch/CombatComponent/FootIKComponent.h"
#include "DeathMatch/CombatComponent/GideonCombatComponent.h"

AGideonCharacter::AGideonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bCharacterUseFootIK = true;

	CombatComponent = CreateDefaultSubobject<UGideonCombatComponent>(TEXT("GideonCombatComponent"));

	CharacterNum = 1;

	ViewTargets.Emplace(FName("head"));
	ViewTargets.Emplace(FName("spine_01"));
	ViewTargets.Emplace(FName("lowerarm_l"));
	ViewTargets.Emplace(FName("hand_l"));
	ViewTargets.Emplace(FName("lowerarm_r"));
	ViewTargets.Emplace(FName("hand_r"));
	ViewTargets.Emplace(FName("leg"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>GideonSkeletalMeshAsset(TEXT("SkeletalMesh'/Game/ParagonGideon/Characters/Heroes/Gideon/Meshes/Gideon.Gideon'"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimClassAsset(TEXT("AnimBlueprint'/Game/Blueprint/Animation/Gideon_Animation/BP_GideonAnimation.BP_GideonAnimation'"));

	static ConstructorHelpers::FClassFinder<AHUD> HUDAsset(TEXT("Blueprint'/Game/Blueprint/HUD/BP_GideonHUD.BP_GideonHUD_C'"));

	if (GideonSkeletalMeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(GideonSkeletalMeshAsset.Object);
	if(AnimClassAsset.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimClassAsset.Object->GetAnimBlueprintGeneratedClass());

	if (HUDAsset.Succeeded())
		HUD = HUDAsset.Class;
}
// ������Ʈ���� �ʱ�ȭ���� BeginPlay()�� ȣ��Ǳ����� 
// �� ĳ���Ͱ� ��ȯ�Ǳ����� �̸� ���� �ʱ�ȭ�۾�
void AGideonCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
void AGideonCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void AGideonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AGideonCharacter::Jump()
{
	GideonCombatComponent = GideonCombatComponent == nullptr ? Cast<UGideonCombatComponent>(CombatComponent) : GideonCombatComponent;
	if (GideonCombatComponent)
	{
		if (GideonCombatComponent->GetIsFly())
		{
			GideonCombatComponent->Jump();
		}
		else
		{
			Super::Jump();
		}
	}
}
