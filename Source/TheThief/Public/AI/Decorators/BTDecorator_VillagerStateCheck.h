// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_VillagerStateCheck.generated.h"

UENUM(BlueprintType)
enum class EVillagerStateCheckType : uint8
{
	IsIdle,
	IsFleeing,
	IsAlertingGuard,
	IsTalking,
	IsWalking
};

UCLASS()
class THETHIEF_API UBTDecorator_VillagerStateCheck : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_VillagerStateCheck();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EVillagerStateCheckType StateToCheck = EVillagerStateCheckType::IsIdle;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bInvertCondition = false;
};

