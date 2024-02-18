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


// 캐릭터 선탱창에서 캐릭터 선택시
void UCharacterSelectComponent::CharacterSelect(const int32 index)
{
	if (SpawnCharacter != nullptr)
	{
		// 기존에 소환된 캐릭
		SpawnCharacter->GetMesh()->SetVisibility(false);
		SpawnCharacter->DestroyFromCharacterSelect();
		SpawnCharacter->Destroy();
	}

	// 같은 캐릭 선택시
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
	case ECharacterSelectIndex::ECSI_Revenant://리퍼
		if (RevenantClass)
			SpawnCharacter = GetWorld()->SpawnActor<ARootCharacter>(RevenantClass, SpawnLocation, SpawnRotation, SpawnParams);
		break;

	case ECharacterSelectIndex::ECSI_Gideon://가디언
		if (GideonClass)
			SpawnCharacter = GetWorld()->SpawnActor<ARootCharacter>(GideonClass, SpawnLocation, SpawnRotation, SpawnParams);
		break;

	case ECharacterSelectIndex::ECSI_Sevarog://전사
		if (SevarogClass)
			SpawnCharacter = GetWorld()->SpawnActor<ARootCharacter>(SevarogClass, SpawnLocation, SpawnRotation, SpawnParams);
		break;
	}

	if (SpawnCharacter)
		SetSpawnCharacter();
}
// 캐릭터 선택창에서 확인 버튼 누를 시 PlayerController를 거쳐 호출되는 함수
bool UCharacterSelectComponent::CharacterSelectComplete()
{
	// 캐릭터를 선택하지 않았으면
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