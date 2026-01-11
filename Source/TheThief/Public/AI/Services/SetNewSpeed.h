// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SetNewSpeed.generated.h"

UCLASS()
class THETHIEF_API USetNewSpeed : public UBTService
{
	GENERATED_BODY()

public:
	USetNewSpeed();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	float NewSpeed = 300.f;
};
