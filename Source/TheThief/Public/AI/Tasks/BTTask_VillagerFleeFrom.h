// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerFleeFrom.generated.h"

UCLASS()
class THETHIEF_API UBTTask_VillagerFleeFrom : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerFleeFrom();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ThreatActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FleeDestinationKey;

	UPROPERTY(EditAnywhere, Category = "Flee")
	float FleeDistance = 500.f;
};

