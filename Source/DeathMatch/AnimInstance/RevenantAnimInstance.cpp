#include "DeathMatch/AnimInstance/RevenantAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

#include "DeathMatch/Character/RevenantCharacter.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "DeathMatch/CombatComponent/RevenantCombatComponent.h"

void URevenantAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ARevenantCharacter>(TryGetPawnOwner());
	//OwnerCombatComponent = Cast<URevenantCombatComponent>(OwnerCharacter->GetCombatComponent());
}

void URevenantAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(OwnerCharacter && OwnerCharacter->GetCombatComponent())
		OwnerCombatComponent = OwnerCombatComponent == nullptr ? Cast<URevenantCombatComponent>(OwnerCharacter->GetCombatComponent()) : OwnerCombatComponent;
	//if (OwnerCharacter)
	//{
	//	/*FTransform MuzzleTransform = OwnerCharacter->GetMesh()->GetSocketTransform(FName("MuzzlePoint"));
	//	FVector Start = MuzzleTransform.GetLocation();
	//	FVector End = OwnerCharacter->GetHitTargetLocation();
	//	FRotator rotate = UKismetMathLibrary::FindLookAtRotation(Start, End);
	//	Upperarm_rRoatator = FRotator(rotate.Pitch, rotate.Yaw, -60.f);*/
	//	//IsOffUpperarmTransformModify = OwnerCharacter->GetCombatComponent()->GetIsOffUpperarmTransformModify();
	//	//IsOffUpperarmTransformModify = true;
	//}
}