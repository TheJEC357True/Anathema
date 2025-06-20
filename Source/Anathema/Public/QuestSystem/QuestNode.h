// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestSystem/Objective.h"
#include "QuestNode.generated.h"

// Delegate for when ALL objectives within THIS specific quest are completed.
// This is what the UQuestManagerComponent will typically bind to.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, UQuestNode*, CompletedQuest);

/**
 * UQuestNode represents a single quest or a stage within a larger quest chain.
 * It manages a set of objectives that must be completed.
 */
UCLASS()
class ANATHEMA_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:

	// --- CONSTRUCTORS ---
    UQuestNode(const FObjectInitializer& ObjectInitializer);
    UQuestNode(FText InQuestName, FText InQuestDescription, TArray<UObjective*> InObjectives, TArray<UQuestNode*> InPrerequisiteQuests, const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // --- QUEST INFORMATION ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestDescription;

	// --- QUEST STATUS ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    bool bIsAvailable;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    bool bIsCompleted;

	// --- QUEST TREE STRUCTURE ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    TArray<UQuestNode*> PrerequisiteQuests;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    TArray<UQuestNode*> FollowUpQuests;

    // --- QUEST OBJECTIVES ---
    // This array holds instances of UObjective (or its Blueprint/C++ subclasses).
    // UPROPERTY(Instanced) is CRUCIAL here: it tells Unreal to create a unique instance
    // of the selected objective class for EACH quest node, rather than just referencing
    // a shared asset. This allows each objective to maintain its own state (e.g., CurrentKills).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Quest")
    TArray<UObjective*> Objectives;

    // --- FUNCTIONS (PUBLIC API) ---

    // Initializes all objectives associated with this quest.
    // This should be called by the UQuestManagerComponent when the quest becomes active for a player.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void InitializeQuestObjectives(AActor* OwningActor);

    // Uninitializes all objectives, removing any bindings to global events.
    // Call this when the quest is completed, abandoned, or the owning actor is being destroyed.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void UninitializeQuestObjectives();

    UFUNCTION(BlueprintNativeEvent, Category = "Quest")
    bool IsQuestAvailable() const;

    UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Quest")
    bool IsQuestCompleted() const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddFollowup(UQuestNode* FollowUpQuest);

    // Event for when the quest is formally unlocked (e.g., shown in UI as available).
    // Implementable ONLY in Blueprint.
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
    void OnQuestUnlocked();

    // Event for when the quest has been fully completed (all objectives done).
    // BlueprintNativeEvent allows C++ logic and Blueprint override.
    UFUNCTION(BlueprintNativeEvent, Category = "Quest")
    void OnQuestCompleted();

    // --- DELEGATES ---
    // Event broadcast when all objectives for this quest are completed.
    // The QuestManagerComponent should bind to this.
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestCompleted OnQuestCompletedDelegate;

protected:
    // --- INTERNAL HELPER FUNCTIONS ---

    // Callback function for when an individual UObjective reports its completion.
    // This function MUST be a UFUNCTION() to be bindable via AddDynamic.
    UFUNCTION()
    void OnObjectiveCompleted(UObjective* CompletedObjective);

    // Manages binding to the OnObjectiveCompletedDelegate of all objectives in the Objectives array.
    void BindToObjectiveCompletionEvents();

    // Manages unbinding from the OnObjectiveCompletedDelegate of all objectives.
    // Important to prevent memory leaks and unnecessary calls for inactive quests.
    void UnbindFromObjectiveCompletionEvents();

    // Default C++ implementation for IsQuestAvailable (BlueprintNativeEvent).
    virtual bool IsQuestAvailable_Implementation() const;

    // Default C++ implementation for OnQuestCompleted (BlueprintNativeEvent).
    virtual void OnQuestCompleted_Implementation();
};