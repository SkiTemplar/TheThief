// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"

#include "IsInCaptureRange.generated.h"

/**
 * Decorator BT que verifica si el jugador está en rango de captura
 */
UCLASS()
class THETHIEF_API UIsInCaptureRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UIsInCaptureRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
