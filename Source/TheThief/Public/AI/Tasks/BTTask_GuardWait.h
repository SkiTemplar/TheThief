// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardWait.generated.h"

UCLASS()
class THETHIEF_API UBTTask_GuardWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GuardWait();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastPatrolPointKey;

	UPROPERTY(EditAnywhere, Category = "Wait")
	float DefaultMinWaitTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Wait")
	float DefaultMaxWaitTime = 4.0f;

private:
	float RemainingTime = 0.f;
};

