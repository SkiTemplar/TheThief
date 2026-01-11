// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerCalmDown.generated.h"

UCLASS()
class THETHIEF_API UBTTask_VillagerCalmDown : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerCalmDown();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};

