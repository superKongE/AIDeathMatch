#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathMatch/Character/RootCharacter.h"
#include "CharacterSelectComponent.generated.h"

enum ECharacterSelectIndex : int32
{
	ECSI_Revenant = 0,
	ECSI_Gideon = 1,
	ECSI_Sevarog = 2
};

class ARootCharacter;
class APreviewActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHMATCH_API UCharacterSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterSelectComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CharacterSelect(const FName SelectCharacterName);
	bool CharacterSelectComplete();

public:
	FORCEINLINE void SetCharacter(class ARootCharacter* Character) { OwnerCharacter = Character; }
	FORCEINLINE ARootCharacter* GetSpawnCharacter() { return SpawnCharacter; }

private:
	UPROPERTY()
	ARootCharacter* OwnerCharacter;
	UPROPERTY()
	ARootCharacter* SpawnCharacter;
	UPROPERTY()
	APreviewActor* SelectedPreviewActor;

	UPROPERTY()
	TMap<FName, ARootCharacter*> CharacterMap;
	UPROPERTY()
	TMap<FName, APreviewActor*> PreviewActorMap;

	FVector SpawnLocation;
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CharacterSelectPoint;

	bool bCharacterSelected = false;
};
