#include "DeathMatch/AnimInstance/GideonAnimInstance.h"

#include "DeathMatch/Character/GideonCharacter.h"
#include "DeathMatch/CombatComponent/GideonCombatComponent.h"

void UGideonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<AGideonCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
		OwnerCombatComponent = Cast<UGideonCombatComponent>(OwnerCharacter->GetCombatComponent());
}

void UGideonAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OwnerCharacter == nullptr) return;

	if (OwnerCombatComponent)
	{
		if (OwnerCombatComponent->GetbRightClickInterp())
			SetIsFalling(true);

		IsFly = OwnerCombatComponent->GetIsFly();
		bThirdSkillPressed = OwnerCombatComponent->GetThirdSkillPressed();
	}
}
