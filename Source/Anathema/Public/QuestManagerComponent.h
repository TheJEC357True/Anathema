// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestSystem/QuestNode.h"
#include "QuestManagerComponent.generated.h"

// Delegate for when a quest is completed by THIS specific player.
// Useful for updating UI, triggering achievements, etc.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerQuestCompleted, UQuestNode*, CompletedQuest);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent)) // meta=(BlueprintSpawnableComponent) allows adding it in Blueprint editor
class ANATHEMA_API UQuestManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestManagerComponent();

	// --- PLAYER QUEST DATA ---
	// This array holds the quests currently active for THIS specific player.
	// Consider adding ReplicatedUsing if you want clients to be aware of active quests.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest Management")
	TArray<UQuestNode*> ActiveQuests;

    // --- PUBLIC API FOR ADDING/REMOVING/MANAGING QUESTS ---
    // Adds a quest to this player's active quests.
    // Returns true if the quest was successfully added, false otherwise (e.g., already active).
    UFUNCTION(BlueprintCallable, Category = "Quest Management")
    bool AddQuest(UQuestNode* QuestToAdd);

    // Removes a quest from this player's active quests.
    // Call this when a quest is completed, failed, or abandoned.
    UFUNCTION(BlueprintCallable, Category = "Quest Management")
    bool RemoveQuest(UQuestNode* QuestToRemove);

    // Checks if a specific quest is currently active for this player.
    UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Quest Management")
    bool IsQuestActive(UQuestNode* QuestToCheck) const;

    // Checks if a specific quest has been completed by this player (requires a 'CompletedQuests' array)
    // UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Quest Management")
    // bool HasQuestBeenCompleted(UQuestNode* QuestToCheck) const;
    // ... You might add a TArray<UQuestNode*> CompletedQuests; UPROPERTY for this later.

    // --- EVENT ROUTING FUNCTIONS (CALLED BY GAME MODE / GLOBAL LISTENERS) ---
    // These functions act as the entry points for external systems to notify this specific player's quest manager.
    // The GameMode will call these after it filters global events (e.g., when a player gets a kill).

    // Notifies the quest manager that an enemy was killed by this player.
    UFUNCTION(BlueprintCallable, Category = "Quest Management|Events")
    void NotifyEvent(FObjectiveEventData& E);

    // Add similar functions for other objective types as needed:
    // UFUNCTION(BlueprintCallable, Category = "Quest Management|Events")
    // void NotifyItemCollected(AActor* CollectedItem);
    // UFUNCTION(BlueprintCallable, Category = "Quest Management|Events")
    // void NotifyLocationReached(FVector Location);

    // --- DELEGATES ---
    // Event broadcast when a quest managed by this component is completed by the player.
    UPROPERTY(BlueprintAssignable, Category = "Quest Management|Events")
    FOnPlayerQuestCompleted OnPlayerQuestCompletedDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    // Callback function when an individual UQuestNode (managed by this component) completes all its objectives.
    // This function MUST be a UFUNCTION() to be bound using AddDynamic.
    UFUNCTION()
    void OnQuestCompleted(UQuestNode* CompletedQuest);
};
