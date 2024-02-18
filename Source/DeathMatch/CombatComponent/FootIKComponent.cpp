#include "DeathMatch/CombatComponent/FootIKComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DeathMatch/Character/RootCharacter.h"

UFootIKComponent::UFootIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UFootIKComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ARootCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter)
		CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}
void UFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerCharacter && OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		LeftFootTraceOffset = 0.f;
		RightFootTraceOffset = 0.f;
		HipOffset = 0.f;
		return;
	}

	FootLineTrace(FootL, FootLHitLocation, LeftFootTraceOffset);
	FootLineTrace(FootR, FootRHitLocation, RightFootTraceOffset);
	
	// 왼발이 올라감
	if (LeftFootTraceOffset <= RightFootTraceOffset)
	{
		HipOffset = RightFootTraceOffset;
		TargetLeftFootTraceOffset = FootLHitLocation.Z - FootRHitLocation.Z;
		TargetRightFootTraceOffset = 0.f;
	}
	else
	{
		HipOffset = LeftFootTraceOffset;
		TargetLeftFootTraceOffset = 0.f;
		TargetRightFootTraceOffset = FootRHitLocation.Z - FootLHitLocation.Z;
	}

	CurrentHipOffset = FMath::FInterpTo(CurrentHipOffset, HipOffset, DeltaTime, 15.f);
	SetCapsuleHalfHeight();

	CurrentLeftFootTraceOffset = FMath::FInterpTo(CurrentLeftFootTraceOffset, TargetLeftFootTraceOffset, DeltaTime, 15.f);
	CurrentRightFootTraceOffset = FMath::FInterpTo(CurrentRightFootTraceOffset, TargetRightFootTraceOffset, DeltaTime, 15.f);
}


void UFootIKComponent::FootLineTrace(const FName& SocketName, FVector& HitLocation, float& FootTraceOffset)
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ARootCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter)
	{
		const USkeletalMeshSocket* FootLSocket = OwnerCharacter->GetMesh()->GetSocketByName(SocketName);
		FVector FootStartLocation = FootLSocket->GetSocketLocation(OwnerCharacter->GetMesh());
		FootStartLocation.Z = OwnerCharacter->GetActorLocation().Z - StartOffset;
		FVector FootEndLocation = FootStartLocation;
		FootEndLocation.Z = OwnerCharacter->GetActorLocation().Z - CapsuleHalfHeight * 2;

		FHitResult HitResult;
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActor(OwnerCharacter);
		GetWorld()->LineTraceSingleByChannel(HitResult, FootStartLocation, FootEndLocation, ECollisionChannel::ECC_Visibility, FQP);
		//DrawDebugLine(GetWorld(), FootStartLocation, FootEndLocation, FColor::Red);
		if (HitResult.bBlockingHit)
		{
			//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 15, FColor::Blue);	
			//DrawDebugSphere(GetWorld(), OwnerCharacter->GetMesh()->GetComponentLocation(), 5.f, 15, FColor::Red);
			FootTraceOffset = OwnerCharacter->GetMesh()->GetComponentLocation().Z - HitResult.ImpactPoint.Z;
			HitLocation = HitResult.ImpactPoint;
		}
		else
		{
			FootTraceOffset = 0.f;
		}
	}
}
void UFootIKComponent::SetCapsuleHalfHeight()
{
	UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
	if (Capsule == nullptr) return;

	float CapsuleHalf = CapsuleHalfHeight - CurrentHipOffset * 0.5f;

	Capsule->SetCapsuleHalfHeight(CapsuleHalf);
}
