// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSystem/Objective.h"

UObjective::UObjective()
    : ObjectiveDescription(FText::FromString(TEXT("Default Objective")))
    , bIsCompleted(false)
{
    // Default constructor. Member initializers are preferred for default values.
}

// Default C++ implementation for BlueprintNativeEvents
// Derived C++ classes can override these using 'override' keyword.
// Blueprint subclasses can override these in their event graphs.

void UObjective::InitializeObjective_Implementation(AActor* OwningActor)
{
    // Reset state when objective is initialized (e.g., when a quest becomes active)
    bIsCompleted = false;
    UE_LOG(LogTemp, Log, TEXT("Base Objective Initialized: %s"), *ObjectiveDescription.ToString());
    // Derived classes will add specific initialization logic (e.g., binding to game events).
}
void UObjective::UninitializeObjective_Implementation() // <--- NEW IMPLEMENTATION
{
    UE_LOG(LogTemp, Log, TEXT("Base UObjective '%s' Uninitialized."), *ObjectiveDescription.ToString());
    // Derived classes will override this to perform specific cleanup (e.g., unbind from events).
}

bool UObjective::IsObjectiveCurrentlyComplete_Implementation() const
{
    // Default base implementation: An objective is complete if bIsCompleted is true.
    // Derived classes will add their specific logic (e.g., CurrentKills >= RequiredKills).
    return bIsCompleted;
}

FText UObjective::GetProgressText_Implementation() const
{
    // Default base implementation: Return "Completed!" if done, otherwise the description.
    if (bIsCompleted)
    {
        return FText::Format(FText::FromString(TEXT("{0} (Completed)")), ObjectiveDescription);
    }
    return ObjectiveDescription; // Fallback to just the description if not overridden
}

void UObjective::CompleteObjective()
{
    if (!bIsCompleted)
    {
        bIsCompleted = true;
        UE_LOG(LogTemp, Log, TEXT("Objective Completed: %s"), *ObjectiveDescription.ToString());
        // Broadcast the delegate to notify any listeners (like the UQuestNode)
        OnObjectiveCompletedDelegate.Broadcast(this);
        // After an objective is completed, it's a good idea to uninitialize it to stop listening.
        // This is often handled by the UQuestNode or UQuestManagerComponent after it gets the completion broadcast,
        // but can also be done here if the objective is truly self-contained.
        // Execute_UninitializeObjective(this); // Uncomment if objective should uninitialize itself
    }
}

void UObjective::ProcessGameEvent_Implementation(const FObjectiveEventData& EventData)
{
    // The base class simply logs, derived classes will implement their specific checks.
    UE_LOG(LogTemp, Verbose, TEXT("Base UObjective '%s' received event"), *ObjectiveDescription.ToString());
}