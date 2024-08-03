#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionsSubsystem();

	/**
	 * Create a new online session
	 * @param NumPublicConnections - The number of players that can join this session
	 * @param MatchType - A string describing the type of match being hosted. Filters
	 *  on this value can be applied when clients search for sessions to join.
	 */
	void CreateSession(int32 NumPublicConnections, FString MatchType);

	/**
	 * Search for online sessions.
	 * @param MaxSearchResults - Max number of sessions to include in the search results
	 */
	void FindSessions(int32 MaxSearchResults);

	/**
	 * Joins a session found from a recent search
	 * @param SessionResult - The session identified from a search performed by calling FindSessions()
	 */
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	/**
	 * Destroys and cleans up all resources for the current session.
	 */
	void DestroySession();

	/**
	 * Starts the current session
	 */
	void StartSession();

private:
	IOnlineSessionPtr SessionInterface;

	FOnCreateSessionCompleteDelegate OnCreateSessionDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionDelegate;

	FDelegateHandle OnCreateSessionHandle;
	FDelegateHandle OnFindSessionHandle;
	FDelegateHandle OnJoinSessionHandle;
	FDelegateHandle OnDestroySessionHandle;
	FDelegateHandle OnStartSessionHandle;

	/**
	 * Callback that is triggered when a session has been created
	 * @param SessionName - The name of the session
	 * @param bWasSuccessful - True if creation of the session was successful. False otherwise.
	 */
	void OnCreateSession(FName SessionName, bool bWasSuccessful);

	/**
	 * Callback that is triggered when a search for sessions has completed
	 * @param bWasSuccessful - True if the search finished successfully. False otherwise.
	 */
	void OnFindSessions(bool bWasSuccessful);

	/**
	 * Callback that is triggered after trying to join a session
	 * @param SessionName - The name of the session that has been joined
	 * @param Result - The result
	 */
	void OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * Callback that is triggered after attempting to destroy the current session
	 * @param SessionName - The name of the session
	 * @param bWasSuccessful - True if the session was destroyed. False otherwise.
	 */
	void OnDestroySession(FName SessionName, bool bWasSuccessful);

	/**
	 * Callback that is triggered after attempting to destroy the current session
	 * @param SessionName - The name of the session
	 * @param bWasSuccessful - True if the session was started. False otherwise.
	 */
	void OnStartSession(FName SessionName, bool bWasSuccessful);
};
