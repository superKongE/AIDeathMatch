#include "DeathMatch/CharacterSelectComponent/CharacterSelectComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/Character/RootCharacter.h"
#include "DeathMatch/Character/Preview/PreviewActor.h"

UCharacterSelectComponent::UCharacterSelectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UCharacterSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> SpawnPointActor;
	UGameplayStatics::GetAllActorsOfClass(this, CharacterSelectPoint, SpawnPointActor);
	SpawnLocation = SpawnPointActor[0]->GetActorLocation();
	SpawnRotation = SpawnPointActor[0]->GetActorRotation();

	// 맵 어딘가에 배치된 모든 캐릭터를 가져와 CharacterMap에 저장
	TArray<AActor*> CharActors;
	UGameplayStatics::GetAllActorsOfClass(this, ARootCharacter::StaticClass(), CharActors);
	for (auto& Actor : CharActors)
	{
		ARootCharacter* Char = Cast<ARootCharacter>(Actor);
		CharacterMap.Emplace(Char->GetCharacterName(), Char);
	}

	// PreviewActor는 캐릭터 선택창에서 캐릭터 선택시 캐릭터를 미리 보기용으로 만들어진 가벼운 Actor
	// PreviewActor도 마찬가지로 맵 어딘가에 미리 배치후 게임 시작시 모든 캐릭터를 가져와 Map에 저장시킨다
	TArray<AActor*> PreviewActors;
	UGameplayStatics::GetAllActorsOfClass(this, APreviewActor::StaticClass(), PreviewActors);
	for (auto& Actor : PreviewActors)
	{
		APreviewActor* PreviewCharacter = Cast<APreviewActor>(Actor);
		PreviewActorMap.Emplace(PreviewCharacter->GetPreviewCharacterName(), PreviewCharacter);
	}
}
void UCharacterSelectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ĳ���� ����â���� ĳ���� ���ý�
// 캐릭터 선택창에서 캐릭터를 고르기만 한 상태
void UCharacterSelectComponent::CharacterSelect(const FName SelectCharacterName)
{
	if (CharacterSelectPoint == nullptr) return;

	if (SelectedPreviewActor != nullptr)
	{
		FVector Location = SelectedPreviewActor->GetActorLocation();
		SelectedPreviewActor->SetActorLocation(FVector(Location.X, Location.Y, -100000));
	}

	// SpawnCharacter : 유저가 캐릭터를 고른 후 확인 버튼을 누를시 소환할 캐릭터
    // SelectedPreviewActor : 유저가 선택한 캐릭터의 미리 보기용 Actor, Map에서 가져와 위치와 회전 값만 수정
	SpawnCharacter = CharacterMap[SelectCharacterName];
	SelectedPreviewActor = PreviewActorMap[SelectCharacterName];
	SelectedPreviewActor->SetActorLocation(SpawnLocation);
	SelectedPreviewActor->SetActorRotation(SpawnRotation);

	if (SelectCharacterName == OwnerCharacter->GetCharacterName())
	{
		bCharacterSelected = false;
		return;
	}

	bCharacterSelected = true;
}
// ĳ���� ����â���� Ȯ�� ��ư ���� �� PlayerController�� ���� ȣ��Ǵ� �Լ�
// 캐릭터 선택창에서 캐릭터를 고른 후 확인 버튼을 누름
bool UCharacterSelectComponent::CharacterSelectComplete()
{
	if (!bCharacterSelected)
	{
		return false;
	}

	bCharacterSelected = false;

	// 유저가 고른 캐릭터를 Map에서 가져와 위치와 회전 값만 수정
	SpawnCharacter->SetActorLocation(OwnerCharacter->GetActorLocation());
	SpawnCharacter->SetActorRotation(OwnerCharacter->GetActorRotation());
	OwnerCharacter = SpawnCharacter;

	return true;
}