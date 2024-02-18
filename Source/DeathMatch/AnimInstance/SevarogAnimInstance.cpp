#include "DeathMatch/AnimInstance/SevarogAnimInstance.h"

#include "DeathMatch/Character/SevarogCharacter.h"
#include "DeathMatch/CombatComponent/SevarogCombatComponent.h"

void USevarogAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASevarogCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
		OwnerCombatComponent = Cast<USevarogCombatComponent>(OwnerCharacter->GetCombatComponent());
}

void USevarogAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OwnerCharacter == nullptr || OwnerCombatComponent == nullptr) return;

	bDash = OwnerCombatComponent->GetbDash();
}
