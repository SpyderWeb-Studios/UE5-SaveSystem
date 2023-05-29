// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SaveSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

class USaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDataLoaded, USaveGame*, PlayerSaveObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDataSaved, bool, bSuccess);

/**
 * The Save Subsystem is a Game Instance Subsystem that handles the saving and loading of the Player Data. It is a base class that should be extended to add functionality.
 *
 * It needs to be both Abstract and NotBlueprintType because it is a base class but we don't want it to be used directly, nor do we want it to automatically be created.
 */
UCLASS(Abstract, NotBlueprintType)
class SAVESYSTEM_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	/**
	 * @brief Event Dispatcher for when the Player Data is loaded, and passes the Save Game Object
	 */
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers")
	FOnPlayerDataLoaded OnPlayerDataLoaded;

	/**
	 * @brief Event Dispatcher for when the Player Data is saved, and passes whether the save was successful or not
	 */
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers")
	FOnPlayerDataSaved OnPlayerDataSaved;

	/**
	 * @brief Creates a New Save Game, and overwrites the old one if it exists
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void StartNewSave(bool bLoad = false, bool bVerbose = true);
	
	/**
	 * @brief Saves the current Player Data to the Save Slot. Creates a new instance if the current one is invalid or non existent
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveData(bool bAsync = true, bool bVerbose = true);

	/**
	 * @brief Loads the Player Data from the Save Slot. Creates a new instance if the current one is invalid or non existent
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadData(bool bVerbose = true);

	UFUNCTION(BlueprintCallable, Category="Save System")
	void ClearSave(bool bVerbose = true);
	
	/**
	 * @brief Is called when the Async Load is finished, and calls the OnPlayerDataLoaded Event
	 * @param SlotName The name of the Slot that was loaded
	 * @param UserIndex The User Index that was loaded
	 * @param SaveGame The Save Game Object that was loaded
	 */
	UFUNCTION()
	virtual void OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame);

	/**
	 * @brief Is called when the Async Save is finished, and logs the result
	 * @param SlotName The name of the Slot that was saved
	 * @param UserIndex The User Index that was saved
	 * @param bSuccess Whether the Save was successful or not
	 */
	UFUNCTION()
	virtual void OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	/**
	 * @brief Sets the Save Game Class to use for the Player, and resets the Save Game Object with the new class
	 * @param SaveGameSubClass The Save Game Class to use for the Player
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SetSaveGameClass(TSubclassOf<USaveGame> SaveGameSubClass, bool bResetSaveObject = true);

	/**
	 * @brief Overridable function to get the name of the Save Slot to use for the Player, if you want to use a different one
	 * @return The name of the Save Slot to use for the Player
	 */
	UFUNCTION(BlueprintPure, Category = "Save System")
	virtual FString GetPlayerSaveSlot();

	/**
	 * @brief Retrieves the Save Game Object for the Player,
	 * BP Only - casts it to the specified class if it is valid
	 * @param SaveGameClass The Class to cast the Save Game Object to
	 * @param bIsValid Whether the Save Game Object is valid or not. Reference so that it can be used in Blueprints with multiple return values
	 * @return The Save Game Object for the Player
	 */
	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "SaveGameClass"))
	USaveGame* GetValidatedSaveGameObject(const TSubclassOf<USaveGame> SaveGameClass, bool& bIsValid);

	/**
	 * @brief Retrieves the Save Game Object for the Player
	 * BP Only - casts it to the specified class if it is valid
	 * @param SaveGameClass The Class to cast the Save Game Object to
	 * @return The Save Game Object for the Player
	 */
	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "SaveGameClass"))
	USaveGame* GetSaveGameObject(const TSubclassOf<USaveGame> SaveGameClass, bool bVerbose = true);

	/**
	 * @brief Retrieves the Save Game Object for the Player, without casting it to the specified class
	 * @return The Save Game Object for the Player
	 */
	UFUNCTION(BlueprintPure)
	USaveGame* GetRawSaveGameObject();

	UFUNCTION(BlueprintPure)
	TSubclassOf<USaveGame> GetSaveGameClass();

protected:
	/**
	 * @brief Assigns the Save Game Object for the Player, and calls the OnPlayerDataLoaded Event
	 * @param SaveGameObject The Save Game Object to assign to the Player Data
	 * @return Whether the Assignment was valid or not
	 */
	bool AssignSaveGameObject(USaveGame* SaveGameObject);
	
private:
	/**
	 * @brief The Class to use for the Save Game Object
	 */
	TSubclassOf<USaveGame> _SaveGameClass;
	
	/**
	* @brief The Current Save Game Object for the Player Data. This is the one that is saved and loaded
	*/
	UPROPERTY()
	TObjectPtr<USaveGame> PlayerSaveObject;
};
