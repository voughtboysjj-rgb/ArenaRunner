// Copyright (c) 2026 Arena Runner contributors. MIT licensed.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ArenaHUD.generated.h"

/** One transient message drawn in the centre of the screen (wave clear, game over, ...). */
struct FArenaHudMessage
{
	FString Text;
	FLinearColor Color = FLinearColor::White;
	float ExpireTime = 0.f;
	float StartTime = 0.f;
};

/**
 * Code-only HUD: health bar, score panel, minimap, crosshair and centre messages.
 * No UMG widget assets required.
 */
UCLASS()
class ARENARUNNER_API AArenaHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	/** Queues a centred message. Called by the game mode. */
	void ShowMessage(const FString& Message, FLinearColor Color, float Duration = 2.5f);

private:
	void DrawHealthBar();
	void DrawScorePanel();
	void DrawMinimap();
	void DrawMessages();
	void DrawCrosshair();
	void DrawDangerVignette();

	void DrawTextShadowed(const FString& Text, float X, float Y, const FLinearColor& Color, UFont* Font, float Scale = 1.f);

	TArray<FArenaHudMessage> Messages;
};
