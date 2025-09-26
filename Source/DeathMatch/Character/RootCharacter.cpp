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

	FootIKComponent = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIKComponent"));
}
// ������Ʈ���� �ʱ�ȭ���� BeginPlay()�� ȣ��Ǳ����� 
// �� ĳ���Ͱ� ��ȯ�Ǳ����� �̸� ���� �ʱ�ȭ�۾�
void ARootCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
		CombatComponent->SetCharacter(this);
	if (FootIKComponent)
		FootIKComponent->SetOwnerCharacter(this);
}
 void ARootCharacter::BeginPlay()
{
	Super::BeginPlay();

	// �ִϸ��̼� ��������Ʈ�� �ִϸ��̼� ���̾
	// AnimLayerClass�� ���̾�� �����Ű�� ����
	// unlink �� link�� ���� �ִϸ��̼� ���̾ �ٲٸ鼭
	// �����ϰ� �ִϸ��̼��� �ٲܼ� �ִ� (������� �ȱ� ��� -> �޸��� ���)
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

			// ĳ���� ��Ȱ�ϱ�
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
// ĳ���� ����â���� ĳ���͸� ��� ��ȯ�������� PlayerController�� Possess�� ���°� �ƴϹǷ�
// Possess���� PlayerController���� ���� �ʱ�ȭ �۾��� ���� ȣ����
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
		// ������ �տ��� ���� �ڿ��� ����
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
// ��� �ִϸ��̼��� ���� �Ŀ� ȣ���
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
	if (Value == 0.f || !bActiveInputExeptMouse) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

	AddMovementInput(Direction, Value);
}
void ARootCharacter::MoveRight(float Value)
{
	if (Value == 0.f || !bActiveInputExeptMouse) return;

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




// �⺻ ����
void ARootCharacter::DefaultAttack()
{
	skip = !skip;
	if (!bActiveInputExeptMouse) return;
	/*
	* ���� �ִϸ��̼� ���
	* ���� LinTrace
	* ����Ʈ ���, �Ҹ� ���
	*/
	if (CombatComponent)
	{
		CombatComponent->DefaultAttack(CombatComponent->GetHitResults());
	}
}
// ��ȭ �⺻ ����
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

	OpenSelectWidgetLocation = GetActorLocation();
	OpenSelectWidgetRotator = GetActorRotation();
	PlayerController = PlayerController == nullptr ? Cast<ARootPlayerController>(Controller) : PlayerController;
	if (PlayerController)
	{
		bActiveInputExeptMouse = false;
		PlayerController->ShowCharacterSelectMenu(true);
	}
}
void ARootCharacter::CharacterSelect(const FName SelectCharacterName)
{
	CharacterSelectComponent->CharacterSelect(SelectCharacterName);
}
bool ARootCharacter::CharacterSelectComplete()
{
	PlayerController->ShowCharacterSelectMenu(false);
	bActiveInputExeptMouse = true;
	return CharacterSelectComponent->CharacterSelectComplete();
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


// Sight Perception���� �� �Լ��� ȣ���Ѵ���
// �� �Լ��� return ���� ���� �������� ���θ� ����
bool ARootCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed,
	float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	if (IsDeath || skip) return false;

	const AEnemy* Enemy = Cast<AEnemy>(IgnoreActor);
	if (Enemy == nullptr) return false;
	
	// AI�� Foward Vector��
	// AI���� �������� ���͸� ������ AI ���ʿ� ���� �ִ��� üũ
	FVector AIForwardVector = IgnoreActor->GetActorForwardVector();
	AIForwardVector.Normalize();

	FVector AIToCharacter = GetActorLocation() - IgnoreActor->GetActorLocation();
	AIToCharacter.Normalize();

	// ������ ��� ���� AI �ڿ� �ִ�
	if (FVector::DotProduct(AIForwardVector, AIToCharacter) < 0.f) return false;
	
	for (int32 i = 0; i < ViewTargets.Num(); i++)
	{
		FName TargetBone = ViewTargets[i];
		FVector SocketLocation = GetMesh()->GetSocketLocation(TargetBone);
		FHitResult HitResult;
		FCollisionQueryParams FQP;
		FQP.AddIgnoredActor(IgnoreActor);
		GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, SocketLocation, ECollisionChannel::ECC_Visibility, FQP);
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
