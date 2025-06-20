// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSystem/QuestNode.h"
#include "QuestSystem/Objective.h" // Include your Objective base class
#include "Engine/World.h" // Needed for GetWorld() or similar contexts

// --- CONSTRUCTORS ---

UQuestNode::UQuestNode(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , QuestName(FText::FromString("Default Quest Name"))
    , QuestDescription(FText::FromString("Default Quest Description"))
    , bIsAvailable(true)
    , bIsCompleted(false)
    , PrerequisiteQuests()
    , FollowUpQuests()
    , Objectives()
{
    UE_LOG(LogTemp, Log, TEXT("UQuestNode default constructor called."));
}
UQuestNode::UQuestNode(FText InQuestName, FText InQuestDescription, TArray<UObjective*> InObjectives, TArray<UQuestNode*> InPrerequisiteQuests, const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , QuestName(InQuestName)
    , QuestDescription(InQuestDescription)
    , bIsAvailable(InPrerequisiteQuests.IsEmpty())
    , bIsCompleted(false)
	, PrerequisiteQuests(InPrerequisiteQuests)
	, FollowUpQuests() // Initialize FollowUpQuests as an empty array
	, Objectives(InObjectives)
{
    for (UQuestNode* Prerequisite : InPrerequisiteQuests)
    {
        if (IsValid(Prerequisite))
        {
            Prerequisite->AddFollowup(this); // Add this quest as a follow-up to each prerequisite
        }
	}

    UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' created with custom constructor."), *QuestName.ToString());
}

// --- FUNCTIONS (IMPLEMENTATIONS) ---

void UQuestNode::InitializeQuestObjectives(AActor* OwningActor)
{
    // First, unbind any existing events to ensure a clean state upon re-initialization
    UninitializeQuestObjectives();

    bIsCompleted = false; // Reset completion status when initializing/re-initializing

    bIsAvailable = true; // You might set this to true here, or have separate logic for availability
    for (UQuestNode* Prerequisite : PrerequisiteQuests)
    {
        if (IsValid(Prerequisite) && !Prerequisite->bIsCompleted)
        {
            bIsAvailable = false; // If any prerequisite is not completed, this quest cannot be available
            UE_LOG(LogTemp, Warning, TEXT("UQuestNode '%s' cannot be initialized because prerequisite '%s' is not completed."), *QuestName.ToString(), *Prerequisite->QuestName.ToString());
            return; // Exit early if prerequisites are not met
        }
	}

    UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' initializing %d objectives."), *QuestName.ToString(), Objectives.Num());

    for (UObjective* Objective : Objectives)
    {
        if (IsValid(Objective))
        {
            // Call the Objective's Initialize method (BlueprintNativeEvent, so use Execute_ prefix if Blueprintable)
            Objective->InitializeObjective(OwningActor);
        }
        else 
        {
			Objectives.Remove(Objective); // Remove invalid objectives from the list
            UE_LOG(LogTemp, Warning, TEXT("UQuestNode '%s' has an invalid objective."), *QuestName.ToString());
        }
    }

    // After all objectives are initialized, bind to their completion events.
    BindToObjectiveCompletionEvents();
}

void UQuestNode::UninitializeQuestObjectives()
{
    // Unbind all objective completion events to prevent memory leaks and unnecessary calls.
    UnbindFromObjectiveCompletionEvents();

    UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' uninitializing all objectives."), *QuestName.ToString());

    // Call uninitialize on each objective as well
    for (UObjective* Objective : Objectives)
    {
        if (IsValid(Objective))
        {
            Objective->UninitializeObjective(); // Call the Objective's Uninitialize
        }
    }
}

bool UQuestNode::IsQuestCompleted() const
{
    for (const UObjective* Objective : Objectives)
    {
        if (IsValid(Objective) && !Objective->IsObjectiveCurrentlyComplete())
        {
            // Found at least one objective that is not yet complete.
            return false;
        }
    }
    // All objectives are complete.
    return true;
}

void UQuestNode::AddFollowup(UQuestNode* InFollowUpQuest)
{
    if (IsValid(InFollowUpQuest) && !FollowUpQuests.Contains(InFollowUpQuest))
    {
        FollowUpQuests.Add(InFollowUpQuest);
        // You might also want to add 'this' quest as a prerequisite to the follow-up quest
        // InFollowUpQuest->Prerequisites.Add(this);
        UE_LOG(LogTemp, Log, TEXT("Quest '%s' now has '%s' as a follow-up."), *QuestName.ToString(), *InFollowUpQuest->QuestName.ToString());
    }
}

// Default C++ implementation for BlueprintNativeEvent
bool UQuestNode::IsQuestAvailable_Implementation() const
{
    // Default logic: a quest can be taken if it's available and all its prerequisites are met.
    if (!bIsAvailable)
    {
        return false;
    }

    // You would typically check if the player's QuestManagerComponent indicates prerequisites are complete.
    // For now, let's assume they are if the quest is available.
    // Example:
    // UQuestManagerComponent* QuestManager = ... get from PlayerController->PlayerState ...
    // for (UQuestNode* Prereq : Prerequisites)
    // {
    //     if (IsValid(Prereq) && !QuestManager->HasQuestCompleted(Prereq))
    //     // This would require HasQuestCompleted on QuestManagerComponent.
    //     {
    //         return false;
    //     }
    // }

    return true; // Placeholder: Add real prerequisite checking logic here.
}

// Default C++ implementation for BlueprintNativeEvent
void UQuestNode::OnQuestCompleted_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' C++ OnQuestCompleted_Implementation called."), *QuestName.ToString());
    // Example: Trigger follow-up quests availability
    for (UQuestNode* FollowUp : FollowUpQuests)
    {
        if (IsValid(FollowUp))
        {
            FollowUp->bIsAvailable = true; // Make follow-up quests available
            FollowUp->OnQuestUnlocked(); // Call its unlock event (BlueprintImplementableEvent)
            UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' unlocked follow-up quest '%s'."), *QuestName.ToString(), *FollowUp->QuestName.ToString());
        }
    }

    // Any other C++ specific completion logic
}

// --- INTERNAL HELPER FUNCTIONS ---

void UQuestNode::OnObjectiveCompleted(UObjective* CompletedObjective)
{
    UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s': Received completion for objective '%s'."), *QuestName.ToString(), *CompletedObjective->ObjectiveDescription.ToString());

    // After an objective completes, immediately unbind from its specific delegate
    // to prevent further calls and cleanup.
    if (IsValid(CompletedObjective))
    {
        CompletedObjective->OnObjectiveCompletedDelegate.RemoveDynamic(this, &UQuestNode::OnObjectiveCompleted);
        // Also call Uninitialize on the specific objective if it has bindings to global events
        CompletedObjective->UninitializeObjective();
    }

    // Check if all objectives are now complete.
    if (IsQuestCompleted())
    {
        bIsCompleted = true; // Mark the quest as completed
        UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s': All objectives completed!"), *QuestName.ToString());

        // Call the BlueprintNativeEvent for quest completion
        OnQuestCompleted();

        // Broadcast the delegate to notify the UQuestManagerComponent (or other listeners)
        OnQuestCompletedDelegate.Broadcast(this);

        // Crucial: Unbind from ALL remaining objective events for this quest,
        // as the quest is now fully completed.
        UnbindFromObjectiveCompletionEvents();
    }
}

void UQuestNode::BindToObjectiveCompletionEvents()
{
    for (UObjective* Objective : Objectives)
    {
        if (IsValid(Objective))
        {
            // Bind our callback function to each objective's completion delegate.
            Objective->OnObjectiveCompletedDelegate.AddDynamic(this, &UQuestNode::OnObjectiveCompleted);
            UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' bound to objective '%s'."), *QuestName.ToString(), *Objective->ObjectiveDescription.ToString());
        }
    }
}

void UQuestNode::UnbindFromObjectiveCompletionEvents()
{
    for (UObjective* Objective : Objectives)
    {
        if (IsValid(Objective))
        {
            // Remove the binding. This is important for garbage collection and preventing calls
            // on invalid objects if the objective or quest is destroyed.
            Objective->OnObjectiveCompletedDelegate.RemoveDynamic(this, &UQuestNode::OnObjectiveCompleted);
            UE_LOG(LogTemp, Log, TEXT("UQuestNode '%s' unbound from objective '%s'."), *QuestName.ToString(), *Objective->ObjectiveDescription.ToString());
        }
    }
}