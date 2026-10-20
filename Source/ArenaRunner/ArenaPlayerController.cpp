// Copyright (c) 2026 Arena Runner contributors. MIT licensed.

#include "ArenaPlayerController.h"

#include "ArenaHUD.h"

#include "Kismet/GameplayStatics.h"

AArenaPlayerController::AArenaPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	HideMouseCursor();
}

void AArenaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AArenaPlayerController::TogglePause);
	}
}

void AArenaPlayerController::TogglePause()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const bool bIsPaused = UGameplayStatics::IsGamePaused(this);
	UGameplayStatics::SetGamePaused(this, !bIsPaused);

	if (AArenaHUD* HUD = Cast<AArenaHUD>(GetHUD()))
	{
		if (!bIsPaused)
		{
			HUD->ShowMessage(TEXT("PAUSED  -  P to resume"), FLinearColor(0.8f, 0.9f, 1.f), 4.f);
		}
	}
}

void AArenaPlayerController::HideMouseCursor()
{
	bShowMouseCursor = false;
}
