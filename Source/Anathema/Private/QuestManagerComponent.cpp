// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerComponent.h"
#include "GameFramework/PlayerState.h" // If attaching to PlayerState, helps with logging owner name


// Sets default values for this component's properties
UQuestManagerComponent::UQuestManagerComponent()
{
	// No need to tick, as this component is event-driven
	PrimaryComponentTick.bCanEverTick = false;

	// Set this component to be replicated by default. Crucial if attached to APlayerState
	// and you want quest data to sync to clients.
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UQuestManagerComponent::BeginPlay()
{
	Super::BeginPlay();

    // If this component is attached to a PlayerState, log who owns it.
    APlayerState* OwningPlayerState = GetOwner<APlayerState>();
    if (IsValid(OwningPlayerState))
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent initialized for Player: %s"), *GetNameSafe(OwningPlayerState));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent is not owned by a PlayerState. Consider attaching it to one for proper quest management."));
    }

    // You might load saved quests here, or grant starting quests to the player.
    // Example: (Requires a way to get a reference to a quest Blueprint class)
    // if (GetOwner()->HasAuthority()) // Only run on server for multiplayer
    // {
    //     // UQuestNode* StartingQuest = NewObject<UQuestNode>(this, MyStartingQuestBlueprintClass);
    //     // if (StartingQuest) {
    //     //     AddQuest(StartingQuest);
    //     // }
    // }
	
}

bool UQuestManagerComponent::AddQuest(UQuestNode* QuestToAdd)
{
    if (!IsValid(QuestToAdd))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Attempted to add an invalid quest."));
        return false;
    }

    if (ActiveQuests.Contains(QuestToAdd))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Quest '%s' is already active for this player."), *QuestToAdd->QuestName.ToString());
        return false;
    }

    ActiveQuests.Add(QuestToAdd);

    // Initialize objectives of the newly added quest, passing the owner of this component (e.g., PlayerState).
    QuestToAdd->InitializeQuestObjectives(GetOwner());

    // Bind to this quest's completion delegate so the manager knows when a quest finishes.
    QuestToAdd->OnQuestCompletedDelegate.AddDynamic(this, &UQuestManagerComponent::OnQuestCompleted);

    UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Added quest '%s' for player '%s'."), *QuestToAdd->QuestName.ToString(), *GetNameSafe(GetOwner()));
    return true;
}

bool UQuestManagerComponent::RemoveQuest(UQuestNode* QuestToRemove)
{
    if (!IsValid(QuestToRemove))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Attempted to remove an invalid quest."));
        return false;
    }

    if (!ActiveQuests.Contains(QuestToRemove))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Quest '%s' is not active for this player, cannot remove."), *QuestToRemove->QuestName.ToString());
        return false;
    }

    // Unbind from the quest's completion delegate.
    QuestToRemove->OnQuestCompletedDelegate.RemoveDynamic(this, &UQuestManagerComponent::OnQuestCompleted);

    // Uninitialize the quest's objectives to ensure they stop listening to global events.
    QuestToRemove->UninitializeQuestObjectives();

    ActiveQuests.Remove(QuestToRemove);

    UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Removed quest '%s' for player '%s'."), *QuestToRemove->QuestName.ToString(), *GetNameSafe(GetOwner()));
    return true;
}

bool UQuestManagerComponent::IsQuestActive(UQuestNode* QuestToCheck) const
{
    if (!IsValid(QuestToCheck)) return false;
    return ActiveQuests.Contains(QuestToCheck);
}

// --- EVENT ROUTING IMPLEMENTATIONS ---

void UQuestManagerComponent::NotifyEvent(FObjectiveEventData& E)
{
    UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent for '%s': Received notification for an Event."), *GetNameSafe(GetOwner()));

    // Iterate through THIS player's active quests and route the notification to relevant objectives.
    for (UQuestNode* Quest : ActiveQuests)
    {
        // Only process valid and uncompleted quests
        if (IsValid(Quest) && !Quest->bIsCompleted)
        {
            for (UObjective* Objective : Quest->Objectives)
            {
                if (IsValid(Objective) && !Objective->bIsCompleted) // Only process valid and uncompleted objectives
                {
                    Objective->ProcessGameEvent(E);
                }
            }
        }
    }
}
// --- DELEGATE CALLBACK IMPLEMENTATIONS ---

void UQuestManagerComponent::OnQuestCompleted(UQuestNode* CompletedQuest)
{
    UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent for '%s': Quest '%s' reports all objectives completed!"), *GetNameSafe(GetOwner()), *CompletedQuest->QuestName.ToString());

    // You can now trigger events relevant to the entire quest completion for this player.
    // e.g., Update UI, grant rewards, trigger cinematics.

    OnPlayerQuestCompletedDelegate.Broadcast(CompletedQuest); // Broadcast to UI/other systems

    // Remove the quest from the active list (or move to a 'CompletedQuests' array).
    RemoveQuest(CompletedQuest); // This also unbinds and uninitializes the quest.
}