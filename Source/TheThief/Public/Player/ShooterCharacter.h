// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Characters/BaseCharacter.h"
#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UAnimMontage;
class UWidgetComponent;

// Delegates para comunicación con UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoinsChanged, int32, CurrentCoins, int32, RequiredCoins);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCaptured);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerWon);

UCLASS()
class THETHIEF_API AThiefCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AThiefCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ========== DELEGATES ==========
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCoinsChanged OnCoinsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerCaptured OnPlayerCaptured;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerWon OnPlayerWon;

	// ========== GETTERS ==========
	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintPure, Category = "Stealth")
	bool IsStealthed() const { return bIsHidden; }

	UFUNCTION(BlueprintPure, Category = "Stealth")
	bool IsStealing() const { return bIsStealing; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const { return bIsAttacking; }

	// Devuelve true si el jugador está cometiendo un crimen visible
	UFUNCTION(BlueprintPure, Category = "Stealth")
	bool IsCommittingCrime() const { return bIsCommittingCrime; }

	// Devuelve true si algún enemigo está viendo al jugador
	UFUNCTION(BlueprintPure, Category = "Stealth")
	bool IsBeingSeen() const { return SeenByCount > 0; }

	// Llamado por los enemigos cuando empiezan/dejan de ver al jugador
	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void AddSeenBy() { SeenByCount++; }
	
	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void RemoveSeenBy() { SeenByCount = FMath::Max(0, SeenByCount - 1); }

	UFUNCTION(BlueprintPure, Category = "Coins")
	int32 GetCollectedCoins() const { return CollectedCoins; }

	UFUNCTION(BlueprintPure, Category = "Coins")
	int32 GetRequiredCoins() const { return RequiredCoins; }

	// ========== CALLABLE FUNCTIONS ==========
	UFUNCTION(BlueprintCallable, Category = "Coins")
	void AddCoin(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnCaptured();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnAttackHit();

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void SetStealing(bool bStealing);

	// Verificar si el jugador puede ser detectado
	UFUNCTION(BlueprintPure, Category = "Stealth")
	bool CanBeDetected() const;

	// Override para reproducir sonidos de daño
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnDeath() override;

	// ========== COMPONENTS ==========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	// ========== INPUT ACTIONS ==========
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMapping;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PauseAction;

	// ========== MOVEMENT ==========
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float JumpCooldown = 0.5f; // Tiempo de espera entre saltos en segundos

	// ========== CAMERA ==========
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float DefaultArmLength = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector DefaultCameraOffset = FVector(0.f, 50.f, 50.f);

	// ========== COMBAT ==========
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackDamage = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRadius = 80.f;

	// Socket names para ataques específicos (puño derecho, izquierdo, pie)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Sockets")
	FName RightHandSocket = TEXT("hand_r");

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Sockets")
	FName LeftHandSocket = TEXT("hand_l");

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Sockets")
	FName RightFootSocket = TEXT("foot_r");

	// Qué socket usar para cada ataque del combo (índice del array = número de combo)
	// 0 = RightHand (por defecto), 1 = LeftHand, 2 = RightFoot
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Sockets")
	TArray<int32> ComboAttackSocketIndex;

	// ========== INTERACTION ==========
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange = 100.f;

	// Rango para robar a villagers (por proximidad, no raycast)
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float StealRange = 200.f;

	// ========== AUDIO ==========
	// Footsteps
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Footsteps")
	USoundBase* FootstepWalkSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Footsteps")
	USoundBase* FootstepRunSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Footsteps")
	float FootstepInterval = 0.5f;

	// Combat sounds
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Combat")
	TArray<USoundBase*> ComboAttackSounds;

	// Muerte (scream)
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Damage")
	USoundBase* DeathScreamSound;

	// Animación de muerte (capturado)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* CapturedMontage;

	// Pickup
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Pickup")
	USoundBase* CoinPickupSound;

	// ========== GAME ==========
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 RequiredCoins = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 CollectedCoins = 0;

	// ========== STEALTH ==========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stealth")
	bool bIsHidden = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stealth")
	bool bIsStealing = false;

	// Distancia máxima a la que los NPCs pueden ser alertados cuando el jugador comete un crimen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	float WitnessAlertRange = 1500.f;

	// Si true, los NPCs necesitan línea de visión para ser alertados
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	bool bRequireLineOfSight = true;

private:
	// State
	bool bIsSprinting = false;
	bool bIsAttacking = false;
	bool bIsCommittingCrime = false; // True cuando el jugador ataca/mata a alguien
	int32 SeenByCount = 0; // Contador de enemigos que están viendo al jugador
	bool bCanJump = true; // Control de cooldown de salto
	float CurrentArmLength;
	FVector CurrentCameraOffset;
	int32 CurrentAttackIndex = 0; // Índice del ataque actual (rotativo)

	// Audio
	float FootstepTimer = 0.f;
	float LastSpeed = 0.f;
	
	// Timers
	FTimerHandle StealingTimerHandle;
	FTimerHandle JumpCooldownTimerHandle;
	FTimerHandle CrimeTimerHandle; // Timer para el estado de crimen
	FTimerHandle RestartLevelTimerHandle; // Timer para reiniciar nivel después de muerte

	// Input handlers
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void StartJump();
	void StopJump();
	void StartSprint();
	void StopSprint();
	void Attack();
	void Interact();
	void TogglePause();

	// Systems
	void UpdateCameraPosition(float DeltaTime);
	void PerformInteractionCheck();
	void UpdateMovementSpeed();
	void UpdateStealthState();
	void CheckWinCondition();
	void UpdateFootsteps(float DeltaTime);
	void StopStealing();
	void ResetJumpCooldown();
	void StopCrime(); // Terminar estado de crimen
	void AlertNearbyWitnesses(); // Alertar a NPCs que pueden ver el crimen
	void EnableRagdoll(); // Activar físicas ragdoll
	void RestartLevelAfterDeath(); // Reiniciar nivel después de muerte
	bool TryStealFromNearbyVillager(); // Intentar robar a un villager cercano
};
