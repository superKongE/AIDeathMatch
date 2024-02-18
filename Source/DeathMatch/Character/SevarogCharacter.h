#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/Character/RootCharacter.h"
#include "SevarogCharacter.generated.h"


UCLASS()
class DEATHMATCH_API ASevarogCharacter : public ARootCharacter
{
	GENERATED_BODY()

public:
	ASevarogCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;

	virtual void DestroyFromCharacterSelect();

public:
	void LeftClickReleased();

	void RightClickReleased();

	FVector GetThirdSkillSpawnPoint();
	FRotator GetThirdSkillRotator();

private:
	UPROPERTY(EditAnywhere);
	class USevarogCombatComponent* SevarogCombatComponent = nullptr;

	UPROPERTY(EditAnywhere);
	class UClass* SevarogShadowClass = nullptr;

	UPROPERTY(EditAnywhere);
	class UArrowComponent* ShadowActorSpawnPoint = nullptr;

	UPROPERTY(EditAnywhere);
	class UArrowComponent* ThirdSkillSpawnPoint = nullptr;

	UPROPERTY()
	class AActor* ShadowActor = nullptr;
};
