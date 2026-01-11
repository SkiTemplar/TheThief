// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "VillagerController.generated.h"

class UAIPerceptionComponent;
class UBehaviorTree;
class UBlackboardComponent;
class APatrolPoint;
class AGuardCharacter;

UCLASS()
class THETHIEF_API AVillagerController : public AAIController
{
	GENERATED_BODY()

public:
	AVillagerController();

	// Funciones llamadas desde el Character o externamente
	UFUNCTION(BlueprintCallable, Category = "AI")
	void FleeFrom(AActor* DangerActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopFleeing();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void FleeToGuard(AGuardCharacter* Guard, FVector ThiefLastLocation);

	// Función para actualizar el Blackboard desde el Character
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetBlackboardThreat(AActor* ThreatActor, FVector ThreatLocation);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float FleeCalmnessTime = 5.0f;

	// Configuración de visión del Villager
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float SightRadius = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float LoseSightRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float PeripheralVisionAngle = 60.f;

private:
	UFUNCTION()
	void OnSensed(AActor* Actor, FAIStimulus Stimulus);

	void SetupBlackboard();

	bool bIsFleeing = false;
	bool bIsAlertingGuard = false;
	bool bCanSeePlayer = false; // Si el aldeano puede ver al jugador actualmente
	FTimerHandle CalmDownTimerHandle;
};
