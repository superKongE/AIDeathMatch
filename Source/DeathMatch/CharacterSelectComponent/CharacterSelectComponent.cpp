#include "DeathMatch/CharacterSelectComponent/CharacterSelectComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DeathMatch/Character/RootCharacter.h"

UCharacterSelectComponent::UCharacterSelectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCharacterSelectComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UCharacterSelectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ĳ���� ����â���� ĳ���� ���ý�
void UCharacterSelectComponent::CharacterSelect(const int32 index)
{
	if (SpawnCharacter != nullptr)
	{
		// ������ ��ȯ�� ĳ��
		SpawnCharacter->GetMesh()->SetVisibility(false);
		SpawnCharacter->DestroyFromCharacterSelect();
		SpawnCharacter->Destroy();
	}

	// ���� ĳ�� ���ý�
	if (OwnerCharacter == nullptr || index == OwnerCharacter->GetCharacterNum())
	{
		bCharacterSelected = false;
		return;
	}

	TArray<AActor*> Actor;
	UGameplayStatics::GetAllActorsOfClass(this, CharacterSelectPoint, Actor);

	SpawnLocation = Actor[0]->GetActorLocation();
	SpawnRotation = Actor[0]->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;

	bCharacterSelected = true;

	switch (index)
	{
	case ECharacterSelectIndex::ECSI_Revenant://����
		if (RevenantClass)
			SpawnCharacter = GetWorld()->SpawnActor<ARootCharacter>(RevenantClass, SpawnLocation, SpawnRotation, SpawnParams);
		break;

	case ECharacterSelectIndex::ECSI_Gideon://�����
		if (GideonClass)
			SpawnCharacter = GetWorld()->SpawnActor<ARootCharacter>(GideonClass, SpawnLocation, SpawnRotation, SpawnParams);
		break;

	case ECharacterSelectIndex::ECSI_Sevarog://����
		if (SevarogClass)
			SpawnCharacter = GetWorld()->SpawnActor<ARootCharacter>(SevarogClass, SpawnLocation, SpawnRotation, SpawnParams);
		break;
	}

	if (SpawnCharacter)
		SetSpawnCharacter();
}
// ĳ���� ����â���� Ȯ�� ��ư ���� �� PlayerController�� ���� ȣ��Ǵ� �Լ�
bool UCharacterSelectComponent::CharacterSelectComplete()
{
	// ĳ���͸� �������� �ʾ�����
	if (!bCharacterSelected) return false;

	bCharacterSelected = false;

	FVector Location = OwnerCharacter->GetActorLocation();
	SpawnCharacter->SetActorLocation(Location + FVector(0.f,0.f,50.f));
	SpawnCharacter->SetActorRotation(FRotator(0.f, 370.f, 0.f));

	return true;
}
void UCharacterSelectComponent::DeleteSpawnedCharacter()
{
	if (SpawnCharacter != nullptr)
	{
		SpawnCharacter->Destroy();
		SpawnCharacter = nullptr;
	}
}


void UCharacterSelectComponent::SetSpawnCharacter()
{
	SpawnCharacter->SetActorLocation(SpawnLocation);
	SpawnCharacter->SetActorRotation(SpawnRotation);
}