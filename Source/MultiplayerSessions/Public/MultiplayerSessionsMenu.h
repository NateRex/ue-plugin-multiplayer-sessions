#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsMenu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/**
	 * Adds this menu to the viewport
	 * @param NumberOfPublicConnections - Number of players allowed to join sessions created from this menu.
	 *  Defaults to 4.
	 * @param TypeOfMatch - Type of match represented by sessions created from this menu.
	 *  Defaults to "FreeForAll".
	 */
	UFUNCTION(BlueprintCallable)
	void Setup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));

protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:

	int32 NumPublicConnections{ 4 };

	FString MatchType{TEXT("FreeForAll")};

	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void OnHostButtonClick();

	UFUNCTION()
	void OnJoinButtonClick();

	void TearDown();
};
