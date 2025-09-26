// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Preview/PreviewActor.h"

// Sets default values
APreviewActor::APreviewActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APreviewActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

