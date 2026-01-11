// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CalculateNewDestination.generated.h"

class APatrolPoint;

UCLASS()
class THETHIEF_API UCalculateNewDestination : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCalculateNewDestination();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DestinationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastPatrolPointKey;
	
private:
	// Puntos de patrulla cacheados
	UPROPERTY()
	TArray<APatrolPoint*> PatrolPoints;
	
	UPROPERTY()
	APatrolPoint* LastPoint = nullptr;
	
	void CachePatrolPoints(UWorld* World);
	APatrolPoint* GetRandomPatrolPoint(APatrolPoint* PreviousPoint);
};
