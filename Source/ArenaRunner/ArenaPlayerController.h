// Copyright (c) 2026 Arena Runner contributors. MIT licensed.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArenaPlayerController.generated.h"

/** Plain player controller: game-only input and a pause toggle. */
UCLASS()
class ARENARUNNER_API AArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AArenaPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void TogglePause();
	void HideMouseCursor();
};
