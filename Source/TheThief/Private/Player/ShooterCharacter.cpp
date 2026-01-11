// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/ShooterCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ThiefGameMode.h"
#include "Game/ThiefGameInstance.h"
#include "DrawDebugHelpers.h"
#include "AI/GuardCharacter.h"
#include "AI/VillagerCharacter.h"

AThiefCharacter::AThiefCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize state
	bIsSprinting = false;
	bIsAttacking = false;
	bIsHidden = false;
	bIsStealing = false;
	bCanJump = true;  // Inicializar el control de salto
	CurrentArmLength = DefaultArmLength;
	CurrentCameraOffset = DefaultCameraOffset;

	// Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = DefaultArmLength;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Movement config - Configuración para third-person con cámara
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;  // TRUE para que mire a la cámara
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;  // FALSE para movimiento lateral
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void AThiefCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	CurrentArmLength = DefaultArmLength;
	CurrentCameraOffset = DefaultCameraOffset;
	
	if (SpringArm)
	{
		SpringArm->TargetArmLength = DefaultArmLength;
		SpringArm->SocketOffset = DefaultCameraOffset;
	}

	// Configurar Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputMapping)
			{
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}
	}

	// Inicializar monedas desde GameInstance
	if (UThiefGameInstance* GI = Cast<UThiefGameInstance>(GetGameInstance()))
	{
		CollectedCoins = GI->CollectedCoins;
		RequiredCoins = GI->RequiredCoins;
	}

	OnCoinsChanged.Broadcast(CollectedCoins, RequiredCoins);
}

void AThiefCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateCameraPosition(DeltaTime);
	UpdateStealthState();
	UpdateFootsteps(DeltaTime);
}

void AThiefCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input) return;

	if (MoveAction) Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThiefCharacter::Move);
	if (LookAction) Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThiefCharacter::Look);
	if (JumpAction)
	{
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AThiefCharacter::StartJump);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AThiefCharacter::StopJump);
	}
	if (SprintAction)
	{
		Input->BindAction(SprintAction, ETriggerEvent::Started, this, &AThiefCharacter::StartSprint);
		Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &AThiefCharacter::StopSprint);
	}
	if (AttackAction) Input->BindAction(AttackAction, ETriggerEvent::Started, this, &AThiefCharacter::Attack);
	if (InteractAction) Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AThiefCharacter::Interact);
	if (PauseAction) Input->BindAction(PauseAction, ETriggerEvent::Started, this, &AThiefCharacter::TogglePause);
}

// ==================== INPUT HANDLERS ====================

void AThiefCharacter::Move(const FInputActionValue& InputActionValue)
{
	if (bIsDead) return;

	const FVector2D InputVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), InputVector.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), InputVector.X);
}

void AThiefCharacter::Look(const FInputActionValue& InputActionValue)
{
	if (bIsDead) return;

	const FVector2D InputVector = InputActionValue.Get<FVector2D>();
	AddControllerYawInput(InputVector.X);
	AddControllerPitchInput(-InputVector.Y);
}

void AThiefCharacter::StartJump()
{
	if (bIsDead || !bCanJump) return;
	
	Jump();
	
	// Activar cooldown
	bCanJump = false;
	GetWorldTimerManager().SetTimer(JumpCooldownTimerHandle, this, &AThiefCharacter::ResetJumpCooldown, JumpCooldown, false);
}

void AThiefCharacter::StopJump()
{
	StopJumping();
}

void AThiefCharacter::StartSprint()
{
	if (bIsDead) return;
	bIsSprinting = true;
	UpdateMovementSpeed();
}

void AThiefCharacter::StopSprint()
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void AThiefCharacter::Attack()
{
	if (bIsDead || bIsAttacking) return;

	bIsAttacking = true;
	
	// Reproducir animación de ataque rotativa
	if (AttackMontages.Num() > 0)
	{
		UAnimMontage* CurrentMontage = AttackMontages[CurrentAttackIndex];
		if (CurrentMontage)
		{
			PlayAnimMontage(CurrentMontage);
		}
		
		// Reproducir sonido de combo correspondiente
		if (ComboAttackSounds.Num() > 0)
		{
			int32 SoundIndex = CurrentAttackIndex % ComboAttackSounds.Num();
			if (ComboAttackSounds[SoundIndex])
			{
				UGameplayStatics::PlaySoundAtLocation(this, ComboAttackSounds[SoundIndex], GetActorLocation());
			}
		}
		
		// Rotar al siguiente ataque
		CurrentAttackIndex = (CurrentAttackIndex + 1) % AttackMontages.Num();
	}
	else
	{
		// Si no hay montages, ejecutar el hit directamente
		OnAttackHit();
		bIsAttacking = false;
	}
}

void AThiefCharacter::OnAttackHit()
{
	if (bIsDead) return;
	
	// Determinar la posición del ataque según el socket configurado
	FVector AttackLocation = GetActorLocation() + GetActorForwardVector() * 100.f; // Por defecto adelante
	
	// Verificar si hay un socket configurado para este ataque
	// El índice que usamos es el anterior porque ya rotamos en Attack()
	int32 AttackIndexForSocket = (CurrentAttackIndex == 0) ? AttackMontages.Num() - 1 : CurrentAttackIndex - 1;
	
	if (GetMesh())
	{
		FName SocketName = RightHandSocket; // Por defecto mano derecha
		
		if (ComboAttackSocketIndex.IsValidIndex(AttackIndexForSocket))
		{
			int32 SocketType = ComboAttackSocketIndex[AttackIndexForSocket];
			
			switch (SocketType)
			{
				case 0: SocketName = RightHandSocket; break;
				case 1: SocketName = LeftHandSocket; break;
				case 2: SocketName = RightFootSocket; break;
				default: SocketName = RightHandSocket; break;
			}
		}
		
		if (GetMesh()->DoesSocketExist(SocketName))
		{
			AttackLocation = GetMesh()->GetSocketLocation(SocketName);
		}
	}
	
	TArray<FOverlapResult> OverlapResults;
	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		AttackLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius)
	);

	bool bHitSomeone = false;

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();
			if (HitActor && HitActor != this)
			{
				bHitSomeone = true;
				
				// Verificar si es un guardia y si le golpeamos por detrás
				if (AGuardCharacter* Guard = Cast<AGuardCharacter>(HitActor))
				{
					FVector ToPlayer = GetActorLocation() - Guard->GetActorLocation();
					ToPlayer.Normalize();
					FVector GuardForward = Guard->GetActorForwardVector();
					
					float DotProduct = FVector::DotProduct(ToPlayer, GuardForward);
					
					// Si el jugador está detrás del guardia (dot < 0), daño x2 (crítico)
					if (DotProduct < -0.3f)
					{
						UGameplayStatics::ApplyDamage(HitActor, AttackDamage * 2.f, GetController(), this, nullptr);
					}
					else
					{
						// Daño normal al guardia
						UGameplayStatics::ApplyDamage(HitActor, AttackDamage, GetController(), this, nullptr);
					}
				}
				else
				{
					UGameplayStatics::ApplyDamage(HitActor, AttackDamage, GetController(), this, nullptr);
				}
			}
		}
	}
	
	// Si golpeamos a alguien, activar estado de crimen y alertar testigos
	if (bHitSomeone)
	{
		bIsCommittingCrime = true;
		AlertNearbyWitnesses();
		
		// El crimen dura 3 segundos
		GetWorldTimerManager().ClearTimer(CrimeTimerHandle);
		GetWorldTimerManager().SetTimer(CrimeTimerHandle, this, &AThiefCharacter::StopCrime, 3.0f, false);
	}
	
	bIsAttacking = false;
}

void AThiefCharacter::Interact()
{
	if (bIsDead) return;
	
	// Primero intentar robar a un villager cercano
	if (TryStealFromNearbyVillager())
	{
		return; // Robo exitoso, no continuar con otras interacciones
	}
	
	// Si no hay villager para robar, hacer la interacción normal (raycast)
	PerformInteractionCheck();
}

void AThiefCharacter::TogglePause()
{
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->TogglePause();
	}
}

// ==================== COINS SYSTEM ====================

void AThiefCharacter::AddCoin(int32 Amount)
{
	CollectedCoins += Amount;
	
	// Guardar en GameInstance
	if (UThiefGameInstance* GI = Cast<UThiefGameInstance>(GetGameInstance()))
	{
		GI->CollectedCoins = CollectedCoins;
	}
	
	OnCoinsChanged.Broadcast(CollectedCoins, RequiredCoins);
	
	// Al robar, el jugador es visible temporalmente
	SetStealing(true);
	
	// Dejar de robar después de un momento usando Timer
	GetWorldTimerManager().SetTimer(StealingTimerHandle, this, &AThiefCharacter::StopStealing, 1.5f, false);
	
	CheckWinCondition();
}

void AThiefCharacter::StopStealing()
{
	SetStealing(false);
}

void AThiefCharacter::ResetJumpCooldown()
{
	bCanJump = true;
}

void AThiefCharacter::SetStealing(bool bStealing)
{
	bIsStealing = bStealing;
}

bool AThiefCharacter::CanBeDetected() const
{
	// El jugador puede ser detectado si:
	// - Está robando (bIsStealing)
	// - Está corriendo (bIsSprinting)
	
	if (bIsStealing) return true;
	if (bIsSprinting) return true;
	
	return false;
}

void AThiefCharacter::CheckWinCondition()
{
	if (CollectedCoins >= RequiredCoins)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player won! Collected %d/%d coins"), CollectedCoins, RequiredCoins);
		OnPlayerWon.Broadcast();
		
		if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->OnPlayerWon();
		}
	}
}

void AThiefCharacter::OnCaptured()
{
	// Evitar múltiples llamadas
	if (bIsDead) return;
	bIsDead = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Player was captured!"));
	
	// Desactivar input del jugador
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
	
	// Detener movimiento
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}
	
	// Reproducir animación de captura (si está asignada) en vez de ragdoll
	if (CapturedMontage)
	{
		PlayAnimMontage(CapturedMontage);
	}
	
	// Reproducir sonido de muerte UNA sola vez
	if (DeathScreamSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathScreamSound, GetActorLocation());
	}
	
	OnPlayerCaptured.Broadcast();
	
	// El GameMode se encarga de mostrar el widget de Game End
	// NO reiniciamos el nivel automáticamente aquí
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnPlayerCaptured();
	}
}

// ==================== HELPER FUNCTIONS ====================

void AThiefCharacter::UpdateCameraPosition(float DeltaTime)
{
	CurrentArmLength = FMath::FInterpTo(CurrentArmLength, DefaultArmLength, DeltaTime, 5.f);
	CurrentCameraOffset = FMath::VInterpTo(CurrentCameraOffset, DefaultCameraOffset, DeltaTime, 5.f);
	
	if (SpringArm)
	{
		SpringArm->TargetArmLength = CurrentArmLength;
		SpringArm->SocketOffset = CurrentCameraOffset;
	}
}

void AThiefCharacter::PerformInteractionCheck()
{
	if (!GetController()) return;
	
	FVector ViewLocation;
	FRotator ViewRotation;
	GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, ViewLocation + ViewRotation.Vector() * InteractionRange, ECC_Visibility, QueryParams))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActor->Implements<UInteractable>() && IInteractable::Execute_CanInteract(HitActor, this))
			{
				IInteractable::Execute_Interact(HitActor, this);
			}
		}
	}
}

void AThiefCharacter::UpdateMovementSpeed()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return;
	
	if (bIsSprinting)
		Movement->MaxWalkSpeed = SprintSpeed;
	else
		Movement->MaxWalkSpeed = WalkSpeed;
}

void AThiefCharacter::UpdateStealthState()
{
	// El jugador está oculto si NO está robando ni corriendo
	bIsHidden = !bIsStealing && !bIsSprinting;
}

void AThiefCharacter::OnDeath()
{
	Super::OnDeath();
	
	// Reproducir grito de muerte
	if (DeathScreamSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathScreamSound, GetActorLocation());
	}
	
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnPlayerCaptured();
	}
}

float AThiefCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Usar el sistema direccional de BaseCharacter
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// ==================== AUDIO SYSTEM ====================

void AThiefCharacter::UpdateFootsteps(float DeltaTime)
{
	if (bIsDead) return;
	
	// Solo reproducir pasos si está en el suelo
	if (!GetCharacterMovement() || !GetCharacterMovement()->IsMovingOnGround()) return;
	
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float CurrentSpeed = Velocity.Size();
	
	if (CurrentSpeed > 10.f)
	{
		FootstepTimer -= DeltaTime;
		
		if (FootstepTimer <= 0.f)
		{
			USoundBase* Sound = bIsSprinting ? FootstepRunSound : FootstepWalkSound;
			FootstepTimer = bIsSprinting ? FootstepInterval * 0.6f : FootstepInterval;
			
			if (Sound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation(), 0.5f);
			}
		}
	}
	else
	{
		FootstepTimer = 0.f;
	}
	
	LastSpeed = CurrentSpeed;
}

void AThiefCharacter::StopCrime()
{
	bIsCommittingCrime = false;
}

void AThiefCharacter::AlertNearbyWitnesses()
{
	// Buscar todos los guardias y aldeanos que puedan ver al jugador
	TArray<AActor*> FoundGuards;
	TArray<AActor*> FoundVillagers;
	
	UGameplayStatics::GetAllActorsOfClass(this, AGuardCharacter::StaticClass(), FoundGuards);
	UGameplayStatics::GetAllActorsOfClass(this, AVillagerCharacter::StaticClass(), FoundVillagers);
	
	FVector PlayerLocation = GetActorLocation();
	
	// Alertar guardias que pueden ver
	for (AActor* Actor : FoundGuards)
	{
		AGuardCharacter* Guard = Cast<AGuardCharacter>(Actor);
		if (!Guard || Guard->IsDead()) continue;
		
		float Distance = FVector::Dist(PlayerLocation, Guard->GetActorLocation());
		if (Distance > WitnessAlertRange) continue;
		
		bool bCanAlert = true;
		
		// Verificar línea de visión si está habilitado
		if (bRequireLineOfSight)
		{
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(Guard);
			
			bool bBlocked = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Guard->GetActorLocation() + FVector(0, 0, 50),
				PlayerLocation + FVector(0, 0, 50),
				ECC_Visibility,
				QueryParams
			);
			
			bCanAlert = !bBlocked || HitResult.GetActor() == this;
		}
		
		if (bCanAlert)
		{
			Guard->OnAlerted(this);
			UE_LOG(LogTemp, Warning, TEXT("Guard %s witnessed the crime at distance %.1f!"), *Guard->GetName(), Distance);
		}
	}
	
	// Alertar aldeanos que pueden ver
	for (AActor* Actor : FoundVillagers)
	{
		AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Actor);
		if (!Villager || Villager->IsDead()) continue;
		
		float Distance = FVector::Dist(PlayerLocation, Villager->GetActorLocation());
		if (Distance > WitnessAlertRange) continue;
		
		bool bCanAlert = true;
		
		// Verificar línea de visión si está habilitado
		if (bRequireLineOfSight)
		{
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(Villager);
			
			bool bBlocked = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Villager->GetActorLocation() + FVector(0, 0, 50),
				PlayerLocation + FVector(0, 0, 50),
				ECC_Visibility,
				QueryParams
			);
			
			bCanAlert = !bBlocked || HitResult.GetActor() == this;
		}
		
		if (bCanAlert)
		{
			Villager->AlertGuards(this);
			UE_LOG(LogTemp, Warning, TEXT("Villager %s witnessed the crime at distance %.1f!"), *Villager->GetName(), Distance);
		}
	}
}

void AThiefCharacter::EnableRagdoll()
{
	// Detener cualquier animación en curso
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		MeshComp->SetSimulatePhysics(true);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		MeshComp->WakeAllRigidBodies();
	}
	
	// Desactivar cápsula de colisión
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	// Desactivar movimiento
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
		Movement->StopMovementImmediately();
	}
}

void AThiefCharacter::RestartLevelAfterDeath()
{
	// Reiniciar el nivel actual
	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = World->GetMapName();
		CurrentLevelName.RemoveFromStart(World->StreamingLevelsPrefix);
		
		UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
	}
}

bool AThiefCharacter::TryStealFromNearbyVillager()
{
	// Buscar villagers cercanos
	TArray<AActor*> FoundVillagers;
	UGameplayStatics::GetAllActorsOfClass(this, AVillagerCharacter::StaticClass(), FoundVillagers);
	
	FVector MyLocation = GetActorLocation();
	AVillagerCharacter* ClosestVillager = nullptr;
	float ClosestDistance = StealRange;
	
	for (AActor* Actor : FoundVillagers)
	{
		AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Actor);
		if (!Villager || !Villager->CanBeRobbed()) continue;
		
		float Distance = FVector::Dist(MyLocation, Villager->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestVillager = Villager;
		}
	}
	
	if (ClosestVillager)
	{
		// Robar la moneda
		ClosestVillager->OnRobbed(this);
		AddCoin(1);
		
		// Reproducir sonido de pickup si existe
		if (CoinPickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CoinPickupSound, MyLocation);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Robbed coin from %s! Distance: %.1f"), *ClosestVillager->GetName(), ClosestDistance);
		return true;
	}
	
	return false;
}
