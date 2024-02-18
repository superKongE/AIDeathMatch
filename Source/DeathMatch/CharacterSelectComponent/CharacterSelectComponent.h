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

	void CharacterSelect(const int32 index);
	bool CharacterSelectComplete();

	void DeleteSpawnedCharacter();

	void SetSpawnCharacter();

public:
	FORCEINLINE void SetCharacter(class ARootCharacter* Character) { OwnerCharacter = Character; }
	FORCEINLINE ARootCharacter* GetSpawnCharacter() { return SpawnCharacter; }

private:
	UPROPERTY()
	class ARootCharacter* OwnerCharacter = nullptr;
	UPROPERTY()
	class ARootCharacter* SpawnCharacter = nullptr;

	FVector SpawnLocation;
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CharacterSelectPoint;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ARootCharacter> GideonClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARootCharacter> RevenantClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARootCharacter> SevarogClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARootCharacter> TwinblastClass;

	bool bCharacterSelected = false;
};
