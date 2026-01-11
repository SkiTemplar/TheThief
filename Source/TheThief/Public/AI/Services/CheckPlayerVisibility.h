// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "CheckPlayerVisibility.generated.h"

/**
 * Servicio BT que verifica si el jugador es visible usando el sistema de percepción
 */
UCLASS()
class THETHIEF_API UCheckPlayerVisibility : public UBTService
{
	GENERATED_BODY()

public:
	UCheckPlayerVisibility();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName CanSeePlayerKeyName = "CanSeeTarget";
};
