#include "DeathMatch/Character/RootCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/RotationMatrix.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInterface.h"

#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "DeathMatch/CombatComponent/RevenantCombatComponent.h"
#include "DeathMatch/CombatComponent/FootIKComponent.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/CharacterSelectComponent/CharacterSelectComponent.h"
#include "DeathMatch/Enemy/Enemy.h"
#include "DeathMatch/HUD/RootHUD.h"


ARootCharacter::ARootCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.f, 100.f, 80.f));

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, -1.f, 0.f);

	GetMesh()->SetWorldLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetWorldRotation(FRotator(0.f, -90.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	CharacterSelectComponent = CreateDefaultSubobject<UCharacterSelectComponent>("CharacterSelectComponent");
	FootIKComponent = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIKComponent"));
}
// 컴포넌트들이 초기화된후 BeginPlay()가 호출되기전에 
// 즉 캐릭터가 소환되기전에 미리 변수 초기화작업
void ARootCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
		CombatComponent->SetCharacter(this);
	if (CharacterSelectComponent)
		CharacterSelectComponent->SetCharacter(this);
	if (FootIKComponent)
		FootIKComponent->SetOwnerCharacter(this);
}
 void ARootCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 애니메이션 블루프린트의 애니메이션 레이어를
	// AnimLayerClass의 레이어로 연결시키기 위함
	// unlink 와 link를 통해 애니메이션 레이어를 바꾸면서
	// 간단하게 애니메이션을 바꿀수 있다 (예를들어 걷기 모드 -> 달리기 모드)
	if (AnimLayerClass)
		GetMesh()->LinkAnimClassLayers(AnimLayerClass);

	PlayerController = Cast<ARootPlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->ClientSetHUD(HUD);
		TargetHP = CurrentHP = MaxHP;
		SetHUDHP();

		if (CombatComponent)
		{
			CombatComponent->SetController(PlayerController);
			CombatComponent->SetHUD(PlayerController->GetHUD());
			CombatComponent->Ready();
		}

		PlayerController->SetDeathProgress();
	}

	SpawnLocation = GetActorLocation();

	IsOverlapCharacterSelectArea = true;
	bActiveInputExeptMouse = true;
}
void ARootCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pitch = Camera->GetComponentRotation().Pitch;
	
	if (CurrentHP != TargetHP)
	{
		CurrentHP = FMath::FInterpTo(CurrentHP, TargetHP, DeltaTime, 1.5f);
		SetHUDCurrentHP();
	}

	if (IsDeath && DeathPercent < 1.f)
	{
		DeathPercent = FMath::Clamp(DeathPercent + DeathPercentCharge * DeltaTime, 0.f, 1.f);
		PlayerController->SetDeathProgressPercent(DeathPercent);

		if (PrevPercent != FMath::CeilToInt(DeathPercent * 100.f))
		{
			PrevPercent = FMath::CeilToInt(DeathPercent * 100.f);
			PlayerController->SetDeathProgressText(PrevPercent);
		}

		if (DeathPercent >= 1.f)
		{
			PrevPercent = -1;
			DeathPercent = 0.f;

			PlayerController->SetVisibleDeathProgress(false);

			// 캐릭터 부활하기
			Spawn();

			IsDeath = false;
		}
	}
}
void ARootCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("DefaultAttack"), IE_Pressed, this, &ARootCharacter::DefaultAttack);
	PlayerInputComponent->BindAction(FName("StrongAttack"), IE_Pressed, this, &ARootCharacter::StrongAttack);
	PlayerInputComponent->BindAction(FName("FirstSkill"), IE_Pressed, this, &ARootCharacter::FirstSkillPressed);
	PlayerInputComponent->BindAction(FName("FirstSkill"), IE_Released, this, &ARootCharacter::FirstSkillRelease);
	PlayerInputComponent->BindAction(FName("SecondSkill"), IE_Pressed, this, &ARootCharacter::SecondSkillPressed);
	PlayerInputComponent->BindAction(FName("SecondSkill"), IE_Released, this, &ARootCharacter::SecondSkillRelease);
	PlayerInputComponent->BindAction(FName("ThirdSkill"), IE_Pressed, this, &ARootCharacter::ThirdSkillPressed);
	PlayerInputComponent->BindAction(FName("ThirdSkill"), IE_Released, this, &ARootCharacter::ThirdSkillRelease);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("CharacterSelect"), IE_Pressed, this, &ARootCharacter::CharacterSelectButtonPressed);

	PlayerInputComponent->BindAxis(FName("MoveFoward"), this, &ARootCharacter::MoveFoward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ARootCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ARootCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRound"), this, &ARootCharacter::LookRound);
}
// 캐릭터 선택창에서 캐릭터를 골라 소환했을때는 PlayerController가 Possess된 상태가 아니므로
// Possess한후 PlayerController에서 따로 초기화 작업을 위해 호출함
void ARootCharacter::Init()
{
	PlayerController = Cast<ARootPlayerController>(Controller);

	if (PlayerController)
	{
		SpawnLocation = GetActorLocation();

		SetHUDHP();
		if (CombatComponent)
		{
			CombatComponent->SetCharacter(this);
			CombatComponent->SetController(PlayerController);
			CombatComponent->SetHUD(PlayerController->GetHUD());
			CombatComponent->Ready();
		}
		if (CharacterSelectComponent != nullptr)
			CharacterSelectComponent->SetCharacter(this);
		if (FootIKComponent != nullptr)
			FootIKComponent->SetOwnerCharacter(this);

		PlayerController->SetDeathProgress();
	}
}


float ARootCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDeath) return DamageAmount;

	TargetHP = FMath::Clamp(TargetHP - DamageAmount, 0.f, MaxHP);

	SetHUDHP();
	if (TargetHP <= 0.f)
	{
		// 공격이 앞에서 온지 뒤에서 온지
		if (FVector::DotProduct(GetActorForwardVector(), DamageCauser->GetActorLocation()) >= 0.f)
			IsFowardDeath = true;
		else
			IsFowardDeath = false;

		Death();
	}

	return DamageAmount;
}
void ARootCharacter::Death()
{
	IsDeath = true;
	bActiveInputExeptMouse = false;
	GetCharacterMovement()->DisableMovement();

	if (CombatComponent)
		CombatComponent->Death();


	if(PlayerController != nullptr)
		PlayerController->SetVisibleDeathProgress(true);
}
void ARootCharacter::Spawn()
{
	ReadyToSpawn();
	SetActorLocation(SpawnLocation);
	GetMesh()->SetVisibility(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (CombatComponent)
		CombatComponent->Ready();
	SetHUDHP();
	bActiveInputExeptMouse = true;
}
void ARootCharacter::ReadyToSpawn()
{
	TargetHP = CurrentHP = MaxHP;
	IsOverlapCharacterSelectArea = true;
}
// 사망 애니메이션이 끝난 후에 호출됨
void ARootCharacter::DeathEnd()
{
	GetMesh()->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



void ARootCharacter::Jump()
{
	if (!bActiveInputExeptMouse) return;

	Super::Jump();
}
void ARootCharacter::MoveFoward(float Value)
{
	if (!bActiveInputExeptMouse) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

	AddMovementInput(Direction, Value);
}
void ARootCharacter::MoveRight(float Value)
{
	if (!bActiveInputExeptMouse) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	AddMovementInput(Direction, Value);
}
void ARootCharacter::LookUp(float Value)
{
	if (Value == 0.f) return;

	AddControllerPitchInput(Value);
}
void ARootCharacter::LookRound(float Value)
{
	if (Value == 0.f) return;

	AddControllerYawInput(Value);
}




// 기본 공격
void ARootCharacter::DefaultAttack()
{
	skip = !skip;
	if (!bActiveInputExeptMouse) return;
	/*
	* 공격 애니메이션 재생
	* 공격 LinTrace
	* 이펙트 재생, 소리 재생
	*/
	if (CombatComponent)
	{
		CombatComponent->DefaultAttack(CombatComponent->GetHitResults());
	}
}
// 강화 기본 공격
void ARootCharacter::StrongAttack()
{
	if (!bActiveInputExeptMouse) return;

	if (CombatComponent)
	{
		CombatComponent->StrongAttack(CombatComponent->GetHitResults());
	}
}
void ARootCharacter::FirstSkillPressed()
{
	if (!bActiveInputExeptMouse) return;

	if (CombatComponent)
	{
		CombatComponent->FirstSkillPressed();
	}
}
void ARootCharacter::FirstSkillRelease()
{
	if (!bActiveInputExeptMouse) return;

	if (CombatComponent)
	{
		CombatComponent->FirstSkillReleased();
	}
}
void ARootCharacter::SecondSkillPressed()
{
	if (!bActiveInputExeptMouse) return;

	if (CombatComponent)
	{
		CombatComponent->SecondSkillPressed();
	}
}
void ARootCharacter::SecondSkillRelease()
{
	if (!bActiveInputExeptMouse) return;

	if (CombatComponent)
	{
		CombatComponent->SecondSkillReleased();
	}
}
void ARootCharacter::ThirdSkillPressed()
{
	if (!bActiveInputExeptMouse) return;

	if (CombatComponent)
	{
		CombatComponent->ThirdSkillPressed();
	}
}
void ARootCharacter::ThirdSkillRelease()
{
	if (CombatComponent)
	{
		CombatComponent->ThirdSkillReleased();
	}
}

void ARootCharacter::AttackEnd()
{
	if (CombatComponent)
	{
		CombatComponent->AttackEnd();
	}
}
void ARootCharacter::FirstSkillEnd()
{
	if (CombatComponent)
	{
		CombatComponent->FirstSkillEnd();
	}
}
void ARootCharacter::SecondSkillEnd()
{
	if (CombatComponent)
	{
		CombatComponent->SecondSkillEnd();
	}
}
void ARootCharacter::ThirdSkillEnd()
{
	if (CombatComponent)
	{
		CombatComponent->ThirdSkillEnd();
	}
}



void ARootCharacter::CharacterSelectButtonPressed()
{
	if (!IsOverlapCharacterSelectArea || !bActiveInputExeptMouse) return;

	PlayerController = PlayerController == nullptr ? Cast<ARootPlayerController>(Controller) : PlayerController;
	if (PlayerController)
	{
		//IsCharacterSelectButtonPressed = !IsCharacterSelectButtonPressed;
		PlayerController->ShowCharacterSelectMenu(true);
	}
}
void ARootCharacter::CharacterSelect(const int32 index)
{
	CharacterSelectComponent->CharacterSelect(index);
}
bool ARootCharacter::CharacterSelectComplete()
{
	PlayerController->ShowCharacterSelectMenu(false);

	// 캐릭터 선택을 완료해 새로 소환할 캐릭터를 현재 캐릭터 위치로 변경
	if (!CharacterSelectComponent->CharacterSelectComplete())
	{
		DeleteSpawnedCharacter();
		return false;
	}

	return true;
}
ARootCharacter* ARootCharacter::GetSpawnCharacter()
{
	return CharacterSelectComponent->GetSpawnCharacter();
}


bool ARootCharacter::GetIsFullBodySkill()
{
	if (CombatComponent == nullptr) return false;

	return CombatComponent->GetIsFullBodySkill();
}


void ARootCharacter::ActiveKeyboardInput(bool bActive)
{
	bActiveInputExeptMouse = bActive;
}
void ARootCharacter::ActiveMouseInput(bool bActive)
{
	bActiveMouseInput = bActive;
}


bool ARootCharacter::FootStepNotify(const FName& BoneName)
{
	const USkeletalMeshSocket* foot_l = GetMesh()->GetSocketByName(BoneName);
	const FVector Start = foot_l->GetSocketLocation(GetMesh()) + FVector(0.f, 0.f, 50.f);
	const FVector End = Start + FVector(0.f, 0.f, -100.f);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.bReturnPhysicalMaterial = true;
	FQP.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, FQP);
	
	if (HitResult.PhysMaterial.Get() == nullptr) return false;

	auto HitSurface = HitResult.PhysMaterial->SurfaceType;
	if (HitSurface == EPhysicalSurface::SurfaceType_Default)
	{
		if (GroundQue && GroundParticle)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, GroundQue, HitResult.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GroundParticle, HitResult.ImpactPoint);
			return true;
		}
	}
	else if (HitSurface == EPhysicalSurface::SurfaceType1)
	{
		if (WoodQue && WoodParticle)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, WoodQue, HitResult.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, WoodParticle, HitResult.ImpactPoint);
			return true;
		}
	}
	else if (HitSurface == EPhysicalSurface::SurfaceType2)
	{
		if (RockQue && RockParticle)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, RockQue, HitResult.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, RockParticle, HitResult.ImpactPoint);
			return true;
		}
	}

	return false;
}


// Sight Perception에서 이 함수를 호출한다음
// 이 함수의 return 값을 통해 감지된지 여부를 결정
bool ARootCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed,
	float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	if (IsDeath || skip) return false;

	const AEnemy* Enemy = Cast<AEnemy>(IgnoreActor);
	if (Enemy == nullptr) return false;
	
	// AI의 Foward Vector와
	// AI에서 적으로의 벡터를 내적해 AI 앞쪽에 적이 있는지 체크
	FVector AIForwardVector = IgnoreActor->GetActorForwardVector();
	AIForwardVector.Normalize();

	FVector AIToCharacter = GetActorLocation() - IgnoreActor->GetActorLocation();
	AIToCharacter.Normalize();

	// 음수일 경우 적이 AI 뒤에 있다
	if (FVector::DotProduct(AIForwardVector, AIToCharacter) < 0.f) return false;
	
	for (int32 i = 0; i < ViewTargets.Num(); i++)
	{
		FName TargetBone = ViewTargets[i];
		FVector SocketLocation = GetMesh()->GetSocketLocation(TargetBone);
		FHitResult HitResult;
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActor(IgnoreActor);
		GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, SocketLocation, ECollisionChannel::ECC_Visibility, FQP);
		//DrawDebugLine(GetWorld(), ObserverLocation, SocketLocation, FColor::Red, false);
		NumberOfLoSChecksPerformed++;
		if (HitResult.bBlockingHit)
		{
			if (Cast<ARootCharacter>(HitResult.GetActor()))
			{
				Enemy->SetTargetBone(TargetBone);
				OutSeenLocation = SocketLocation;
				OutSightStrength = 1;
				return true;
			}
		}
	}

	OutSightStrength = 0;
	return false;
}


void ARootCharacter::SetHUDHP()
{
	PlayerController = PlayerController == nullptr ? Cast<ARootPlayerController>(GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUDHP(MaxHP, TargetHP);
	}
}
void ARootCharacter::SetHUDCurrentHP()
{
	PlayerController = PlayerController == nullptr ? Cast<ARootPlayerController>(GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUDCurrentHP(MaxHP, CurrentHP);
	}
}
void ARootCharacter::SetHUDAmmo(const int32 Ammo, const int32 MaxAmmo)
{
	PlayerController = PlayerController == nullptr ? Cast<ARootPlayerController>(GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUDAmmo(Ammo, MaxAmmo);
	}
}



void ARootCharacter::OverlapCharacterSelectArea(bool bOverlap)
{
	if (bOverlap)
	{
		IsOverlapCharacterSelectArea = true;
	}
	else
	{
		IsOverlapCharacterSelectArea = false;
	}
}


void ARootCharacter::DeleteSpawnedCharacter()
{
	if (CharacterSelectComponent)
	{
		CharacterSelectComponent->DeleteSpawnedCharacter();
	}
}

void ARootCharacter::ChangeInputExceptMouse(bool IsOff)
{
	bActiveInputExeptMouse = IsOff;
}



float ARootCharacter::GetFootLOffset()
{
	if (FootIKComponent)
		return FootIKComponent->GetFootLOffset();
	
	return 0.f;
}
float ARootCharacter::GetFootROffset()
{
	if (FootIKComponent)
		return FootIKComponent->GetFootROffset();

	return 0.f;
}
float ARootCharacter::GetHipOffset()
{
	if (FootIKComponent)
		return FootIKComponent->GetHitOffset();

	return 0.f;
}


void ARootCharacter::SetHP(float HP)
{
	CurrentHP = HP;
	TargetHP = HP;
}


void ARootCharacter::DestroyFromCharacterSelect()
{

}