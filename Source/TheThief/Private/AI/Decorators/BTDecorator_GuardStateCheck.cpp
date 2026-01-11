// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorators/BTDecorator_GuardStateCheck.h"
#include "AI/GuardCharacter.h"
#include "AI/EnemyStates.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"

UBTDecorator_GuardStateCheck::UBTDecorator_GuardStateCheck()
{
	NodeName = "Guard State Check";
	
	// CRÍTICO: Hacer que el decorator observe cambios en el Blackboard
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	
	// Configurar la key por defecto
	CurrentStateKey.SelectedKeyName = "CurrentState";
	CurrentStateKey.AddEnumFilter(this, TEXT("CurrentStateKey"), StaticEnum<EGuardState>());
}

void UBTDecorator_GuardStateCheck::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		CurrentStateKey.ResolveSelectedKey(*BBAsset);
	}
}

void UBTDecorator_GuardStateCheck::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

void UBTDecorator_GuardStateCheck::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

bool UBTDecorator_GuardStateCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// Leer el estado desde el BLACKBOARD, no desde el Character
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;
	
	// Obtener el estado actual del Blackboard
	uint8 CurrentStateValue = BB->GetValueAsEnum("CurrentState");
	EGuardState CurrentState = static_cast<EGuardState>(CurrentStateValue);
	
	bool bResult = CurrentState == StateToCheck;
	
	UE_LOG(LogTemp, Verbose, TEXT("GuardStateCheck: CurrentState=%d, StateToCheck=%d, Result=%s"), 
		(int32)CurrentState, (int32)StateToCheck, bResult ? TEXT("TRUE") : TEXT("FALSE"));

	return bInvertCondition ? !bResult : bResult;
}

FString UBTDecorator_GuardStateCheck::GetStaticDescription() const
{
	FString StateStr;
	switch (StateToCheck)
	{
	case EGuardState::Patrol: StateStr = TEXT("Patrol"); break;
	case EGuardState::Investigate: StateStr = TEXT("Investigate"); break;
	case EGuardState::Chase: StateStr = TEXT("Chase"); break;
	case EGuardState::Attack: StateStr = TEXT("Attack"); break;
	}

	return FString::Printf(TEXT("%s: %s"), bInvertCondition ? TEXT("State is NOT") : TEXT("State is"), *StateStr);
}
