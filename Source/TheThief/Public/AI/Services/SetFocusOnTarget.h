// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SetFocusOnTarget.generated.h"

/**
 * Service que hace que el enemigo mire al target y active strafing
 */
UCLASS()
class THETHIEF_API USetFocusOnTarget : public UBTService
{
	GENERATED_BODY()

public:
	USetFocusOnTarget();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	FName TargetKeyName = "Target";
};
