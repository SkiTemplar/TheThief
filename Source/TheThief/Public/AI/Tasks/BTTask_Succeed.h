// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Succeed.generated.h"

/**
 * Task que siempre retorna Success inmediatamente.
 * Útil para branches opcionales en Selectors.
 */
UCLASS()
class THETHIEF_API UBTTask_Succeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Succeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

