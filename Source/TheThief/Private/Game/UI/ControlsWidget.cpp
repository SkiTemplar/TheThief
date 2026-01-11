// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ControlsWidget.h"
#include "Components/Button.h"

void UControlsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UControlsWidget::OnBackClicked);
	}
}

void UControlsWidget::OnBackClicked()
{
	// Simplemente removerse a sí mismo
	RemoveFromParent();
}
