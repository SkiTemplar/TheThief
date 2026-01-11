// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HowToPlayWidget.h"
#include "Components/Button.h"

void UHowToPlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UHowToPlayWidget::OnBackClicked);
	}
}

void UHowToPlayWidget::OnBackClicked()
{
	// Simplemente removerse a sí mismo
	RemoveFromParent();
}
