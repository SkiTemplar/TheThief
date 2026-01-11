// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerAlertGuard.generated.h"

UCLASS()
class THETHIEF_API UBTTask_VillagerAlertGuard : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerAlertGuard();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetGuardKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ThreatLastLocationKey;
};

