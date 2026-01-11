// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardInvestigateArea.generated.h"

UCLASS()
class THETHIEF_API UBTTask_GuardInvestigateArea : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GuardInvestigateArea();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DestinationKey;

	UPROPERTY(EditAnywhere, Category = "Investigate")
	float InvestigationRadius = 300.f;
};

