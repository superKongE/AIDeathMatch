// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatch/MovementComponent/BombMovementComponent.h"


// 시뮬레이션이 진행되는 동안 Block(충돌) 되면 처리한다
// 충돌 후에도 시뮬래이션을 할지 말지 결정
// 충돌 후에도 시뮬레이션을 할려는 경우 내부적으로 "HandleImpact()"를 호출한 다음 Deflect를 반환하여 HandleDelfection()을 호출하여 multi-bound 와 sliding 을 지원한다
// 충돌 후에 시뮬레이션을 하지 않을 경우 Abort를 반환한다
// EHandleBlockingHitResult : HandleBlockingHit() 이 호출된 후에 시뮬레이션이 어떻게 진행되어야 하는지를 나타내는 enum
UBombMovementComponent::EHandleBlockingHitResult UBombMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);

	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

// 충돌시 속도에 영향을 미치도록 활성화된 경우 bounce 로직을 적용하고
// 활성화 되지 않았거나 속도가 BounceVelocityStopSimulatingThreshold 미만인 경우 발사체를 중단시킨다
void UBombMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// Projectile Movement Component는 기본적으로 물체가 어딘가에 부딪히면 멈추게 하는 기능이 있기 때문에
	// 이 기능을 꺼버린다
	//Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}