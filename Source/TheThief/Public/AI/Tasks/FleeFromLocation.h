// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "FleeFromLocation.generated.h"

/**
 * BT Task que hace que el NPC huya de una ubicación específica
 */
UCLASS()
class THETHIEF_API UFleeFromLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UFleeFromLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName ThreatLocationKey = "ThreatLocation";

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName FleeDestinationKey = "FleeDestination";

	UPROPERTY(EditAnywhere, Category = "AI")
	float FleeDistance = 1000.f;
};
