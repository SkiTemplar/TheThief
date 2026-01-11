// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerCheckForTalkPartner.generated.h"

UCLASS()
class THETHIEF_API UBTTask_VillagerCheckForTalkPartner : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerCheckForTalkPartner();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TalkPartnerKey;

	UPROPERTY(EditAnywhere, Category = "Talk")
	float TalkDistance = 200.f;
};

