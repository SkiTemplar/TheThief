// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/PatrolPoint.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"

APatrolPoint::APatrolPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	// Crear root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

#if WITH_EDITORONLY_DATA
	// Billboard para ver el punto en el editor
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
	
	// Esfera para visualizar el radio
	RadiusVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("RadiusVisualizer"));
	RadiusVisualizer->SetupAttachment(RootComponent);
	RadiusVisualizer->SetSphereRadius(WaitRadius);
	RadiusVisualizer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RadiusVisualizer->SetHiddenInGame(true);
#endif
}

FVector APatrolPoint::GetRandomPositionInRadius() const
{
	FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.f, WaitRadius);
	RandomOffset.Z = 0.f; // Mantener en el suelo
	return GetActorLocation() + RandomOffset;
}

