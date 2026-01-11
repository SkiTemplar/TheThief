// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/EnemyController.h"
#include "AI/GuardCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Player/ShooterCharacter.h"

AEnemyController::AEnemyController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerceptionComponent);
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
	
	SetupPerceptionSystem();
	
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnTargetPerceptionUpdated);
	}
}

void AEnemyController::SetupPerceptionSystem()
{
	if (!AIPerceptionComponent) return;
	
	// Configurar Solo Vista (sin Hearing)
	UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->SetMaxAge(5.f);
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
}

void AEnemyController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;
	
	AThiefCharacter* Player = Cast<AThiefCharacter>(Actor);
	if (!Player) return;
	
	bool bIsSightStimulus = Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>();
	
	if (Stimulus.WasSuccessfullySensed())
	{
		if (bIsSightStimulus)
		{
			// Marcar al jugador como "visto" para el icono del HUD (solo si no lo veíamos antes)
			if (!bCanSeeTarget)
			{
				bCanSeeTarget = true;
				Player->AddSeenBy();
			}
			LastKnownLocation = Actor->GetActorLocation();
		}
		
		// El guardia persigue al jugador si:
		// 1. El jugador puede ser detectado (corriendo o robando) - SIEMPRE
		// 2. El guardia está investigando o fue alertado - persigue aunque camine
		
		bool bShouldChase = Player->CanBeDetected() ||  // Corriendo o robando
		                    bIsInvestigating ||          // Está investigando
		                    bHasBeenAlerted;             // Fue alertado por aldeano
		
		if (!bShouldChase)
		{
			return; // El jugador camina tranquilo y el guardia no está alerta
		}
		
		GetWorldTimerManager().ClearTimer(LoseTargetTimerHandle);
		GetWorldTimerManager().ClearTimer(InvestigationTimerHandle);
		bIsInvestigating = false;
		
		CurrentTarget = Actor;
		ChaseTarget(Actor);
	}
	else
	{
		if (bIsSightStimulus)
		{
			// Marcar al jugador como "no visto" por este guardia
			if (bCanSeeTarget)
			{
				bCanSeeTarget = false;
				Player->RemoveSeenBy();
			}
			
			if (CurrentTarget == Actor)
			{
				LastKnownLocation = Actor->GetActorLocation();
				
				// Delay más corto antes de perder al objetivo (1.5 segundos en lugar de 5)
				GetWorldTimerManager().SetTimer(
					LoseTargetTimerHandle,
					this,
					&AEnemyController::OnLoseTargetTimer,
					1.5f,
					false
				);
			}
		}
	}
}

void AEnemyController::OnLoseTargetTimer()
{
	if (!bCanSeeTarget && CurrentTarget)
	{
		LoseTarget();
	}
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		
		// Inicializar el Blackboard con estado Patrol
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Patrol));
			BB->SetValueAsBool("CanSeeTarget", false);
			BB->SetValueAsBool("HasBeenAlerted", false);
			
			UE_LOG(LogTemp, Warning, TEXT("Guard %s: Blackboard initialized, CurrentState = Patrol (0)"), *GetName());
		}
		
		if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
		{
			Guard->SetGuardState(EGuardState::Patrol);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Guard %s: No BehaviorTree assigned!"), *GetName());
	}
}

void AEnemyController::SetState(EGuardState NewState)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsEnum("CurrentState", static_cast<uint8>(NewState));
	}
	
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
	{
		Guard->SetGuardState(NewState);
	}
}

void AEnemyController::ChaseTarget(AActor* Target)
{
	if (!Target) return;
	
	CurrentTarget = Target;
	bHasBeenAlerted = false;
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsObject("TargetActor", Target);
		BB->SetValueAsBool("CanSeeTarget", true);
		BB->SetValueAsBool("HasBeenAlerted", false);
		BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Chase));
	}
	
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
	{
		Guard->SetGuardState(EGuardState::Chase);
	}
}

void AEnemyController::LoseTarget()
{
	CurrentTarget = nullptr;
	bCanSeeTarget = false;
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->ClearValue("TargetActor");
		BB->SetValueAsBool("CanSeeTarget", false);
		
		if (bInvestigateOnLoseSight)
		{
			BB->SetValueAsVector("LastKnownLocation", LastKnownLocation);
			BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Investigate));
			
			if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
			{
				Guard->SetGuardState(EGuardState::Investigate);
			}
			
			// Iniciar timer de investigación
			bIsInvestigating = true;
			GetWorldTimerManager().SetTimer(
				InvestigationTimerHandle,
				this,
				&AEnemyController::OnInvestigationComplete,
				InvestigationDuration,
				false
			);
		}
		else
		{
			ReturnToPatrol();
		}
	}
}

void AEnemyController::ReceiveAlert(FVector LastKnownPlayerLocation)
{
	if (bCanSeeTarget) return;
	
	// Validar que la ubicación es válida
	if (LastKnownPlayerLocation.ContainsNaN() || LastKnownPlayerLocation.X > 1e30f || LastKnownPlayerLocation.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Guard %s: ReceiveAlert - Invalid location received: %s"), *GetName(), *LastKnownPlayerLocation.ToString());
		return;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Guard %s: ReceiveAlert - Valid location: %s"), *GetName(), *LastKnownPlayerLocation.ToString());
	
	// Cancelar timers anteriores
	GetWorldTimerManager().ClearTimer(InvestigationTimerHandle);
	GetWorldTimerManager().ClearTimer(ChaseToInvestigateTimerHandle);
	
	bHasBeenAlerted = true;
	LastKnownLocation = LastKnownPlayerLocation;
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsBool("HasBeenAlerted", true);
		BB->SetValueAsVector("LastKnownLocation", LastKnownPlayerLocation);
		
		// Primero poner en Chase brevemente para moverse rápido hacia la ubicación
		BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Chase));
	}
	
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
	{
		Guard->SetGuardState(EGuardState::Chase);
	}
	
	// Después de 0.5 segundos, cambiar a Investigate
	GetWorldTimerManager().SetTimer(
		ChaseToInvestigateTimerHandle,
		this,
		&AEnemyController::OnChaseToInvestigateTimer,
		0.5f,
		false
	);
}

void AEnemyController::OnChaseToInvestigateTimer()
{
	// Si ahora puede ver al objetivo, mantener Chase
	if (bCanSeeTarget)
	{
		return;
	}
	
	bIsInvestigating = true;
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Investigate));
	}
	
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
	{
		Guard->SetGuardState(EGuardState::Investigate);
	}
	
	// Iniciar timer de investigación
	GetWorldTimerManager().SetTimer(
		InvestigationTimerHandle,
		this,
		&AEnemyController::OnInvestigationComplete,
		InvestigationDuration,
		false
	);
}

void AEnemyController::GoToInvestigate(FVector Location)
{
	// Cancelar investigación anterior si la hay
	GetWorldTimerManager().ClearTimer(InvestigationTimerHandle);
	
	LastKnownLocation = Location;
	bIsInvestigating = true;
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsVector("LastKnownLocation", Location);
		BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Investigate));
	}
	
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
	{
		Guard->SetGuardState(EGuardState::Investigate);
	}
	
	// Iniciar timer de investigación
	GetWorldTimerManager().SetTimer(
		InvestigationTimerHandle,
		this,
		&AEnemyController::OnInvestigationComplete,
		InvestigationDuration,
		false
	);
}

void AEnemyController::OnInvestigationComplete()
{
	// Si puede ver al objetivo durante la investigación, no volver a patrullar
	if (bCanSeeTarget) return;
	
	bIsInvestigating = false;
	UE_LOG(LogTemp, Warning, TEXT("Guard: Investigation complete, returning to patrol"));
	ReturnToPatrol();
}

void AEnemyController::ReturnToPatrol()
{
	bHasBeenAlerted = false;
	bIsInvestigating = false;
	CurrentTarget = nullptr;
	
	GetWorldTimerManager().ClearTimer(InvestigationTimerHandle);
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->ClearValue("TargetActor");
		BB->SetValueAsBool("HasBeenAlerted", false);
		BB->SetValueAsBool("CanSeeTarget", false);
		BB->SetValueAsEnum("CurrentState", static_cast<uint8>(EGuardState::Patrol));
	}
	
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(GetPawn()))
	{
		Guard->SetGuardState(EGuardState::Patrol);
	}
}
