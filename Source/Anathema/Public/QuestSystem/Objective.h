// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Objective.generated.h"

// Define a delegate to notify when an objective is completed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, UObjective*, CompletedObjective);

/**
 * Base class for all quest objectives.
 * This class serves as an abstract base for different objective types.
 *
 * UCLASS() specifiers:
 *   - Blueprintable: Allows creating Blueprint classes from this C++ class.
 *   - BlueprintType: Allows this C++ class to be used as a type in Blueprints (e.g., for variables).
 *   - DefaultToInstanced: Objects of this class will be instanced by default when added to arrays/maps that don't specify Instanced.
 *   - EditInlineNew: Allows creating new instances of objects of this class directly in the editor's details panel. 
 */
UCLASS()
class ANATHEMA_API UObjective : public UObject
{
	GENERATED_BODY()

public:

	UObjective();

    // --- Properties common to all objectives ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    FText ObjectiveName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    FText ObjectiveDescription;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
    bool bIsCompleted;

    // --- Functions (BlueprintNativeEvent allows C++ implementation and Blueprint override) ---

    // Initializes the objective (e.g., binds to game events, resets internal state).
    // This is a BlueprintNativeEvent, meaning it has a C++ default implementation
    // (InitializeObjective_Implementation) and can be overridden in Blueprints.
    UFUNCTION(BlueprintNativeEvent, Category = "Objective")
    void InitializeObjective(AActor* OwningActor); // Pass context like the quest owner

    // Checks if the objective is currently complete.
    // This is BlueprintPure as it just checks state, no side effects.
    // It's also a BlueprintNativeEvent for C++ and Blueprint overrides.
    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Objective")
    bool IsObjectiveCurrentlyComplete() const;

    // Returns current progress text (e.g., "0/5 Goblins Killed").
    // BlueprintPure, BlueprintNativeEvent.
    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Objective")
    FText GetProgressText() const;

    // Marks the objective as complete and triggers relevant events.
    // This usually contains common logic and broadcasts the completion delegate.
    UFUNCTION(BlueprintCallable, Category = "Objective")
    void CompleteObjective();

    // --- Delegates ---
    // Event triggered when this objective is completed.
    UPROPERTY(BlueprintAssignable, Category = "Objective")
    FOnObjectiveCompleted OnObjectiveCompletedDelegate;

protected:
    // --- C++ Implementation for BlueprintNativeEvents ---
    // You MUST provide a C++ body for BlueprintNativeEvents with _Implementation suffix.
    // These are also 'virtual' to allow C++ subclasses to override them further.

    virtual void InitializeObjective_Implementation(AActor* OwningActor);
    virtual bool IsObjectiveCurrentlyComplete_Implementation() const;
    virtual FText GetProgressText_Implementation() const;
};
