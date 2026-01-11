// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "CapturePlayer.generated.h"

/**
 * BT Task que captura al jugador cuando el guardia lo alcanza
 */
UCLASS()
class THETHIEF_API UCapturePlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCapturePlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
