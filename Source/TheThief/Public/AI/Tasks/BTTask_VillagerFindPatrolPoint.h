// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerFindPatrolPoint.generated.h"

class APatrolPoint;

UCLASS()
class THETHIEF_API UBTTask_VillagerFindPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerFindPatrolPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DestinationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastPatrolPointKey;

private:
	UPROPERTY()
	TArray<APatrolPoint*> CachedPatrolPoints;

	void CachePatrolPoints(UWorld* World);
	APatrolPoint* GetRandomPatrolPoint(APatrolPoint* LastPoint);
};

