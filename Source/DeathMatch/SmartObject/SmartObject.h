// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmartObject.generated.h"

UCLASS()
class DEATHMATCH_API ASmartObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ASmartObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


public:
	// �׻� ī�޶� ���Ͽ� �������Ǵ� 2D �ؽ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBillboardComponent* Billboard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UArrowComponent* FacingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* SubTree = nullptr;
};
