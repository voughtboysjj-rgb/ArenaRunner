// Copyright (c) 2026 Arena Runner contributors. MIT licensed.

#include "ArenaHUD.h"

#include "ArenaCharacter.h"
#include "ArenaCollectible.h"
#include "ArenaGameState.h"
#include "ArenaHunter.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

namespace ArenaHudStyle
{
	static const FLinearColor Panel(0.02f, 0.03f, 0.05f, 0.72f);
	static const FLinearColor PanelEdge(0.15f, 0.55f, 1.0f, 0.85f);
	static const FLinearColor HealthHigh(0.10f, 0.95f, 0.45f, 1.f);
	static const FLinearColor HealthMid(1.0f, 0.72f, 0.10f, 1.f);
	static const FLinearColor HealthLow(1.0f, 0.18f, 0.18f, 1.f);
	static const FLinearColor Accent(0.25f, 0.85f, 1.0f, 1.f);
	static const FLinearColor Text(0.94f, 0.97f, 1.0f, 1.f);
	static const FLinearColor SubText(0.55f, 0.62f, 0.72f, 1.f);
}

void AArenaHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !PlayerOwner)
	{
		return;
	}

	DrawHealthBar();
	DrawScorePanel();
	DrawMinimap();
	DrawMessages();
	DrawCrosshair();
	DrawDangerVignette();
}

// ---------------------------------------------------------------------------
// Health
// ---------------------------------------------------------------------------

void AArenaHUD::DrawHealthBar()
{
	const AArenaCharacter* Character = PlayerOwner ? Cast<AArenaCharacter>(PlayerOwner->GetPawn()) : nullptr;
	if (!Character)
	{
		return;
	}

	UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;

	const float BarX = 42.f;
	const float BarY = Canvas->ClipY - 96.f;
	const float BarW = 340.f;
	const float BarH = 22.f;

	// Back plate
	DrawRect(ArenaHudStyle::Panel, BarX - 10.f, BarY - 26.f, BarW + 20.f, BarH + 44.f);

	// Label
	DrawTextShadowed(TEXT("VITALS"), BarX, BarY - 22.f, ArenaHudStyle::SubText, Font, 1.f);

	// Track
	DrawRect(FLinearColor(0.08f, 0.10f, 0.13f, 1.f), BarX, BarY, BarW, BarH);

	const float Percent = Character->GetHealthPercent();
	FLinearColor Fill = ArenaHudStyle::HealthHigh;
	if (Percent < 0.3f)
	{
		Fill = ArenaHudStyle::HealthLow;
	}
	else if (Percent < 0.6f)
	{
		Fill = ArenaHudStyle::HealthMid;
	}

	DrawRect(Fill, BarX, BarY, BarW * Percent, BarH);

	// Segment ticks make the bar readable at a glance.
	for (int32 Tick = 1; Tick < 10; ++Tick)
	{
		const float TickX = BarX + (BarW / 10.f) * static_cast<float>(Tick);
		DrawLine(TickX, BarY, TickX, BarY + BarH, FLinearColor(0.f, 0.f, 0.f, 0.35f), 1.f);
	}

	// Border
	DrawLine(BarX, BarY, BarX + BarW, BarY, ArenaHudStyle::PanelEdge, 1.5f);
	DrawLine(BarX, BarY + BarH, BarX + BarW, BarY + BarH, ArenaHudStyle::PanelEdge, 1.5f);

	const FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Character->GetHealth()), FMath::CeilToInt(Character->GetMaxHealth()));
	DrawTextShadowed(HealthText, BarX + BarW + 18.f, BarY + 2.f, ArenaHudStyle::Text, Font, 1.f);

	if (Character->IsSprinting())
	{
		DrawTextShadowed(TEXT("SPRINT"), BarX, BarY + BarH + 6.f, ArenaHudStyle::Accent, Font, 0.9f);
	}

}

// ---------------------------------------------------------------------------
// Score panel
// ---------------------------------------------------------------------------

void AArenaHUD::DrawScorePanel()
{
	const AArenaGameState* ArenaState = GetWorld() ? GetWorld()->GetGameState<AArenaGameState>() : nullptr;
	if (!ArenaState)
	{
		return;
	}

	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	UFont* SmallFont = GEngine ? GEngine->GetMediumFont() : nullptr;

	const float PanelX = Canvas->ClipX - 300.f;
	const float PanelY = 36.f;
	const float PanelW = 260.f;
	const float PanelH = 168.f;

	DrawRect(ArenaHudStyle::Panel, PanelX, PanelY, PanelW, PanelH);
	DrawLine(PanelX, PanelY, PanelX + PanelW, PanelY, ArenaHudStyle::PanelEdge, 2.f);
	DrawLine(PanelX, PanelY, PanelX, PanelY + PanelH, ArenaHudStyle::PanelEdge, 2.f);

	DrawTextShadowed(TEXT("ARENA RUNNER"), PanelX + 16.f, PanelY + 10.f, ArenaHudStyle::Accent, SmallFont, 0.95f);
	DrawTextShadowed(FString::Printf(TEXT("%06d"), ArenaState->Score), PanelX + 16.f, PanelY + 30.f, ArenaHudStyle::Text, Font, 1.15f);

	DrawTextShadowed(
		FString::Printf(TEXT("WAVE %d"), ArenaState->Wave),
		PanelX + 16.f, PanelY + 76.f, ArenaHudStyle::Text, SmallFont, 1.f);

	DrawTextShadowed(
		FString::Printf(TEXT("CORES %d / %d"), ArenaState->Collected, ArenaState->TotalCollectibles),
		PanelX + 16.f, PanelY + 98.f, ArenaHudStyle::SubText, SmallFont, 1.f);

	DrawTextShadowed(
		FString::Printf(TEXT("BEST %d"), ArenaState->BestScore),
		PanelX + 16.f, PanelY + 120.f, ArenaHudStyle::SubText, SmallFont, 1.f);
}

// ---------------------------------------------------------------------------
// Minimap
// ---------------------------------------------------------------------------

void AArenaHUD::DrawMinimap()
{
	UWorld* World = GetWorld();
	const AArenaCharacter* Character = PlayerOwner ? Cast<AArenaCharacter>(PlayerOwner->GetPawn()) : nullptr;
	if (!World || !Character)
	{
		return;
	}

	const float MapSize = 150.f;
	const float MapX = Canvas->ClipX - MapSize - 40.f;
	const float MapY = Canvas->ClipY - MapSize - 40.f;
	const float CenterX = MapX + MapSize * 0.5f;
	const float CenterY = MapY + MapSize * 0.5f;

	// The arena is a circle of radius ~1900uu, a touch of padding is added.
	const float WorldRadius = 2100.f;
	const float Scale = (MapSize * 0.5f) / WorldRadius;

	DrawRect(ArenaHudStyle::Panel, MapX, MapY, MapSize, MapSize);

	// Grid
	for (int32 Grid = 1; Grid < 4; ++Grid)
	{
		const float Offset = (MapSize / 4.f) * static_cast<float>(Grid);
		DrawLine(MapX + Offset, MapY, MapX + Offset, MapY + MapSize, FLinearColor(1.f, 1.f, 1.f, 0.05f), 1.f);
		DrawLine(MapX, MapY + Offset, MapX + MapSize, MapY + Offset, FLinearColor(1.f, 1.f, 1.f, 0.05f), 1.f);
	}

	// Border
	DrawLine(MapX, MapY, MapX + MapSize, MapY, ArenaHudStyle::PanelEdge, 1.f);
	DrawLine(MapX, MapY + MapSize, MapX + MapSize, MapY + MapSize, ArenaHudStyle::PanelEdge, 1.f);
	DrawLine(MapX, MapY, MapX, MapY + MapSize, ArenaHudStyle::PanelEdge, 1.f);
	DrawLine(MapX + MapSize, MapY, MapX + MapSize, MapY + MapSize, ArenaHudStyle::PanelEdge, 1.f);

	// Collectibles
	for (TActorIterator<AArenaCollectible> It(World); It; ++It)
	{
		const FVector Location = It->GetActorLocation();
		const float DotX = CenterX + Location.Y * Scale;
		const float DotY = CenterY - Location.X * Scale;
		DrawRect(FLinearColor(0.1f, 1.f, 0.9f, 0.95f), DotX - 3.f, DotY - 3.f, 6.f, 6.f);
	}

	// Hunters (red)
	for (TActorIterator<AArenaHunter> It(World); It; ++It)
	{
		const FVector Location = It->GetActorLocation();
		const float DotX = CenterX + Location.Y * Scale;
		const float DotY = CenterY - Location.X * Scale;
		DrawRect(FLinearColor(1.f, 0.22f, 0.12f, 0.95f), DotX - 4.f, DotY - 4.f, 8.f, 8.f);
	}

	// Player
	const FVector PlayerLocation = Character->GetActorLocation();
	const float PlayerX = CenterX + PlayerLocation.Y * Scale;
	const float PlayerY = CenterY - PlayerLocation.X * Scale;
	DrawRect(FLinearColor::White, PlayerX - 4.f, PlayerY - 4.f, 8.f, 8.f);
	DrawLine(
		PlayerX, PlayerY,
		PlayerX + Character->GetActorForwardVector().Y * 16.f,
		PlayerY - Character->GetActorForwardVector().X * 16.f,
		ArenaHudStyle::Accent, 2.f);

	UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
	DrawTextShadowed(TEXT("N"), CenterX - 5.f, MapY - 20.f, ArenaHudStyle::SubText, Font, 0.8f);
}

// ---------------------------------------------------------------------------
// Messages
// ---------------------------------------------------------------------------

void AArenaHUD::ShowMessage(const FString& Message, FLinearColor Color, float Duration)
{
	if (!GetWorld())
	{
		return;
	}

	FArenaHudMessage Entry;
	Entry.Text = Message;
	Entry.Color = Color;
	Entry.StartTime = GetWorld()->GetTimeSeconds();
	Entry.ExpireTime = Entry.StartTime + FMath::Max(0.25f, Duration);
	Messages.Add(Entry);

	// Keep the HUD tidy.
	while (Messages.Num() > 4)
	{
		Messages.RemoveAt(0);
	}
}

void AArenaHUD::DrawMessages()
{
	if (!GetWorld())
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font)
	{
		Messages.Reset();
		return;
	}

	for (int32 Index = Messages.Num() - 1; Index >= 0; --Index)
	{
		if (Now >= Messages[Index].ExpireTime)
		{
			Messages.RemoveAt(Index);
		}
	}

	float Y = Canvas->ClipY * 0.28f;
	for (const FArenaHudMessage& Message : Messages)
	{
		const float Life = Message.ExpireTime - Message.StartTime;
		const float Age = Now - Message.StartTime;
		const float Alpha = FMath::Clamp(1.f - (Age / FMath::Max(0.01f, Life)), 0.f, 1.f);

		float TextW = 0.f;
		float TextH = 0.f;
		GetTextSize(Message.Text, TextW, TextH, Font, 1.35f);

		FLinearColor Color = Message.Color;
		Color.A = Alpha;
		DrawTextShadowed(Message.Text, Canvas->ClipX * 0.5f - TextW * 0.5f, Y, Color, Font, 1.35f);
		Y += TextH + 8.f;
	}
}

// ---------------------------------------------------------------------------
// Crosshair + helper
// ---------------------------------------------------------------------------

void AArenaHUD::DrawCrosshair()
{
	const float CX = Canvas->ClipX * 0.5f;
	const float CY = Canvas->ClipY * 0.5f;
	const FLinearColor Color(1.f, 1.f, 1.f, 0.35f);

	DrawLine(CX - 7.f, CY, CX - 2.f, CY, Color, 1.f);
	DrawLine(CX + 2.f, CY, CX + 7.f, CY, Color, 1.f);
	DrawLine(CX, CY - 7.f, CX, CY - 2.f, Color, 1.f);
	DrawLine(CX, CY + 2.f, CX, CY + 7.f, Color, 1.f);
}

void AArenaHUD::DrawDangerVignette()
{
	UWorld* World = GetWorld();
	const AArenaCharacter* Character = PlayerOwner ? Cast<AArenaCharacter>(PlayerOwner->GetPawn()) : nullptr;
	if (!World || !Character)
	{
		return;
	}

	float Nearest = TNumericLimits<float>::Max();
	int32 HunterCount = 0;

	for (TActorIterator<AArenaHunter> It(World); It; ++It)
	{
		++HunterCount;
		Nearest = FMath::Min(Nearest, FVector::Dist2D(It->GetActorLocation(), Character->GetActorLocation()));
	}

	if (HunterCount == 0 || Nearest > 900.f)
	{
		return;
	}

	// 0 far away -> 1 touching the player.
	const float Danger = FMath::Clamp(1.f - (Nearest / 900.f), 0.f, 1.f);
	const float Alpha = Danger * 0.35f;
	const FLinearColor Edge(0.9f, 0.05f, 0.02f, Alpha);
	const float Thickness = 10.f + Danger * 26.f;

	DrawRect(Edge, 0.f, 0.f, Canvas->ClipX, Thickness);
	DrawRect(Edge, 0.f, Canvas->ClipY - Thickness, Canvas->ClipX, Thickness);
	DrawRect(Edge, 0.f, 0.f, Thickness, Canvas->ClipY);
	DrawRect(Edge, Canvas->ClipX - Thickness, 0.f, Thickness, Canvas->ClipY);
}

void AArenaHUD::DrawTextShadowed(const FString& Text, float X, float Y, const FLinearColor& Color, UFont* Font, float Scale)
{
	if (!Font)
	{
		return;
	}

	FLinearColor Shadow = FLinearColor(0.f, 0.f, 0.f, Color.A * 0.8f);
	DrawText(Text, Shadow, X + 2.f, Y + 2.f, Font, Scale);
	DrawText(Text, Color, X, Y, Font, Scale);
}
