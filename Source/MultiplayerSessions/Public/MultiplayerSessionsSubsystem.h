#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

//
// Declarations for custom callback delegates exposed by the subsystem
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);


//
// UMultiplayerSessionsSubsystem
//
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	/**
	 * Delegate that is called when this subsystem completes creation of a new session
	 */
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;

	/**
	 * Delegate that is called when this subsystem completes a search for sessions
	 */
	FMultiplayerOnFindSessionComplete MultiplayerOnFindSessionsComplete;

	/**
	 * Delegate that is called when this subsystem completes joining of a session
	 */
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;

	/**
	 * Delegate that is called when this subsystem completes tear-down of a sesssion
	 */
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;

	/**
	 * Delegate that is called when this subsystem completes the starting of a session
	 */
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

	/**
	 * Constructor
	 */
	UMultiplayerSessionsSubsystem();

	/**
	 * @return A reference to the online session interface
	 */
	FORCEINLINE IOnlineSessionPtr GetSessionInterface() const { return SessionInterface; }

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
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnCreateSessionCompleteDelegate OnCreateSessionDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionDelegate;

	FDelegateHandle OnCreateSessionHandle;
	FDelegateHandle OnFindSessionsHandle;
	FDelegateHandle OnJoinSessionHandle;
	FDelegateHandle OnDestroySessionHandle;
	FDelegateHandle OnStartSessionHandle;


	void OnCreateSession(FName SessionName, bool bWasSuccessful);
	void OnFindSessions(bool bWasSuccessful);
	void OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySession(FName SessionName, bool bWasSuccessful);
	void OnStartSession(FName SessionName, bool bWasSuccessful);
};
