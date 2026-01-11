// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPoint.generated.h"

UCLASS()
class THETHIEF_API APatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolPoint();

	// Radio de espera en este punto (el NPC puede pararse en cualquier lugar dentro)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float WaitRadius = 50.f;

	// Tiempo mínimo y máximo de espera en este punto
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float MinWaitTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float MaxWaitTime = 4.f;

	// Obtener tiempo de espera aleatorio
	UFUNCTION(BlueprintPure, Category = "Patrol")
	float GetRandomWaitTime() const { return FMath::RandRange(MinWaitTime, MaxWaitTime); }

	// Obtener posición aleatoria dentro del radio
	UFUNCTION(BlueprintPure, Category = "Patrol")
	FVector GetRandomPositionInRadius() const;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "Patrol")
	class UBillboardComponent* Billboard;
	
	UPROPERTY(VisibleAnywhere, Category = "Patrol")
	class USphereComponent* RadiusVisualizer;
#endif
};

