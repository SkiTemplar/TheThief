// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardAlertNearby.generated.h"

UCLASS()
class THETHIEF_API UBTTask_GuardAlertNearby : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GuardAlertNearby();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Alert")
	float AlertRadius = 1500.f;
};

