// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerWait.generated.h"

UCLASS()
class THETHIEF_API UBTTask_VillagerWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerWait();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	// Key del Blackboard donde está el último PatrolPoint visitado
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastPatrolPointKey;

	// Tiempo por defecto si no hay PatrolPoint
	UPROPERTY(EditAnywhere, Category = "Wait")
	float DefaultMinWaitTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Wait")
	float DefaultMaxWaitTime = 4.0f;

private:
	float RemainingTime = 0.f;
};
