// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InteractionPromptWidget.h"
#include "Components/TextBlock.h"

void UInteractionPromptWidget::SetPromptText(const FText& Text)
{
	if (PromptText)
	{
		PromptText->SetText(Text);
	}
}

