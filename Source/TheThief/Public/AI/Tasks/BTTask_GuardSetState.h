// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AI/EnemyStates.h"
#include "BTTask_GuardSetState.generated.h"

UCLASS()
class THETHIEF_API UBTTask_GuardSetState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GuardSetState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Guard")
	EGuardState NewState = EGuardState::Patrol;
};

