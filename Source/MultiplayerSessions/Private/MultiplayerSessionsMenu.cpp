#include "MultiplayerSessionsMenu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UMultiplayerSessionsMenu::Setup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	// Add to viewport
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	// Update controller settings
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// Obtain reference to multiplayer sessions subsystem
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// Bind callbacks to multiplayer sessions subsystem delegates
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
	}
}

bool UMultiplayerSessionsMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClick);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClick);
	}

	return true;
}

void UMultiplayerSessionsMenu::NativeDestruct()
{
	TearDown();
	Super::NativeDestruct();
}

void UMultiplayerSessionsMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// Travel to the lobby level
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString("Failed to create session"));
		}
	}
}

void UMultiplayerSessionsMenu::OnHostButtonClick()
{
	if (MultiplayerSessionsSubsystem)
	{
		// Create session
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMultiplayerSessionsMenu::OnJoinButtonClick()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString("Join Button Clicked"));
	}
}

void UMultiplayerSessionsMenu::TearDown()
{
	// Remove from viewport
	RemoveFromParent();

	// Update controller settings
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
