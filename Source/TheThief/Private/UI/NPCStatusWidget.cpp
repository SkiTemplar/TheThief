// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/NPCStatusWidget.h"
#include "Components/Image.h"
#include "AI/VillagerCharacter.h"
#include "AI/GuardCharacter.h"
#include "Kismet/GameplayStatics.h"

void UNPCStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Ocultar al inicio
	if (IMG_Icon)
	{
		IMG_Icon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UNPCStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (IsValid(OwnerNPC))
	{
		ENPCWidgetIcon NewIcon = ENPCWidgetIcon::None;
		
		if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(OwnerNPC))
		{
			NewIcon = Villager->GetCurrentWidgetIcon();
		}
		else if (AGuardCharacter* Guard = Cast<AGuardCharacter>(OwnerNPC))
		{
			NewIcon = Guard->GetCurrentWidgetIcon();
		}
		
		UpdateDisplay(NewIcon);
		UpdateVisibilityByDistance();
	}
}

void UNPCStatusWidget::InitializeWidget(AActor* NewOwner)
{
	OwnerNPC = NewOwner;
	
	if (OwnerNPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCStatusWidget: Inicializado con Owner: %s"), *OwnerNPC->GetName());
	}
}

void UNPCStatusWidget::UpdateDisplay(ENPCWidgetIcon NewIcon)
{
	if (CurrentIcon == NewIcon)
	{
		return;
	}
	
	CurrentIcon = NewIcon;
	
	if (!IMG_Icon)
	{
		UE_LOG(LogTemp, Error, TEXT("NPCStatusWidget: IMG_Icon es NULL! Crea una imagen llamada 'IMG_Icon' en el Widget"));
		return;
	}
	
	// Seleccionar textura según el icono
	UTexture2D* TextureToUse = nullptr;
	
	switch (NewIcon)
	{
		case ENPCWidgetIcon::None:
			IMG_Icon->SetVisibility(ESlateVisibility::Hidden);
			return;
			
		case ENPCWidgetIcon::Coin:
			TextureToUse = T_Coin;
			break;
			
		case ENPCWidgetIcon::TalkBubble:
			TextureToUse = T_TalkBubble;
			break;
			
		case ENPCWidgetIcon::TalkBubbleWithCoin:
			TextureToUse = T_TalkBubbleGold;
			break;
			
		case ENPCWidgetIcon::Alert:
			TextureToUse = T_Alert;
			break;
			
		case ENPCWidgetIcon::Investigating:
			TextureToUse = T_Question;
			break;
			
		case ENPCWidgetIcon::AlertWithCoin:
			TextureToUse = T_AlertWithCoin;
			break;
	}
	
	if (TextureToUse)
	{
		IMG_Icon->SetBrushFromTexture(TextureToUse);
		IMG_Icon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NPCStatusWidget: Textura no asignada para icono %d"), (int32)NewIcon);
		IMG_Icon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UNPCStatusWidget::UpdateVisibilityByDistance()
{
	if (!IMG_Icon || !OwnerNPC || CurrentIcon == ENPCWidgetIcon::None) return;
	
	// Si MaxVisibilityDistance es 0, siempre visible
	if (MaxVisibilityDistance <= 0.f)
	{
		IMG_Icon->SetRenderOpacity(1.f);
		return;
	}
	
	// Obtener jugador
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;
	
	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;
	
	// Calcular distancia
	float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), OwnerNPC->GetActorLocation());
	
	// Si está más lejos que la distancia máxima, ocultar
	if (Distance > MaxVisibilityDistance)
	{
		IMG_Icon->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	
	// Si está visible, mostrar
	IMG_Icon->SetVisibility(ESlateVisibility::Visible);
	
	// Calcular opacidad con fade
	if (Distance > FadeStartDistance)
	{
		float FadeRange = MaxVisibilityDistance - FadeStartDistance;
		float FadeProgress = (Distance - FadeStartDistance) / FadeRange;
		float Opacity = 1.f - FMath::Clamp(FadeProgress, 0.f, 1.f);
		IMG_Icon->SetRenderOpacity(Opacity);
	}
	else
	{
		IMG_Icon->SetRenderOpacity(1.f);
	}
}
