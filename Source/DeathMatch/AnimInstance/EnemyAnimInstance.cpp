#include "DeathMatch/AnimInstance/EnemyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "DeathMatch/Enemy/Enemy.h"
#include "DeathMatch/CombatComponent/EnemyCombatComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Enemy = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	Enemy = Enemy == nullptr ? Cast<AEnemy>(TryGetPawnOwner()) : Enemy;
	if (Enemy == nullptr) return;

	Speed = Enemy->GetVelocity().Size();
	Direction = CalculateDirection(Enemy->GetVelocity(), Enemy->GetActorRotation());
	Pitch = Enemy->GetAimOffset();
	IsOffUpperarmTransformModify = Enemy->GetCombatComponent()->GetIsOffUpperarmTransformModify();
	IsDeath = Enemy->GetIsDeath();
	IsFowardDeath = Enemy->GetIsFowardDeath();

	FTransform MuzzleTransform = Enemy->GetMesh()->GetSocketTransform(FName("MuzzlePoint"));
	FVector Start = MuzzleTransform.GetLocation();
	FVector End = Enemy->GetHitTargetLocation();
	FRotator rotate = UKismetMathLibrary::FindLookAtRotation(Start, End);
	Upperarm_rRoatator = FRotator(rotate.Pitch, rotate.Yaw, -60.f);

	IsFalling = Enemy->GetCharacterMovement()->IsFalling();
	//IsCrouch = Enemy->bIsCrouched;
	//IsFullBodySkill = Enemy->GetIsFullBodySkill();
}