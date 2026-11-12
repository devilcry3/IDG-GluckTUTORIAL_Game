// Fill out your copyright notice in the Description page of Project Settings.

#include "Tutorials/Telemetry/TutorialDataSubsystem.h"
#include "Misc/Guid.h"
#include "Misc/DateTime.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Tutorials/Save/TutorialSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/DateTime.h"


void UTutorialDataSubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Default tutorial objective
	CurrentObjective = NSLOCTEXT(
		"SilentRelay",
		"InitialObjective",
		"Reach the relay station entrance"
	);

	// Defualt checkpoint identifier
	CurrentCheckpoint = FName(TEXT("ExteriorStart"));

	UE_LOG(LogTemp, Log,
		TEXT("Tutorial Data Subsytem initialized."));
}

void UTutorialDataSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Log,
		TEXT("Tutorial Data Subsystem shutting down."));

	Super::Deinitialize();
}


void UTutorialDataSubsystem::StartTutorialSession()
{
    // Prevent starting another session during map transitions.
    if (bSessionStarted)
    {
        UE_LOG(LogTemp, Log,
            TEXT("Tutorial session already running: %s"),
            *SessionID);

        return;
    }

    // Generate a unique session identifier.
    FGuid NewSessionGuid = FGuid::NewGuid();

    SessionID = NewSessionGuid.ToString(
        EGuidFormats::DigitsWithHyphens
    );

    // Record the session start time in UTC.
    SessionStartTime = FDateTime::UtcNow();

    // Convert the start time to an ISO 8601 string.
    SessionStartUTC = SessionStartTime.ToIso8601();

    // Convert the start time to a Unix timestamp.
    SessionStartUnix = SessionStartTime.ToUnixTimestamp();

    // Reset event tracking for the new session.
    EventSequence = 0;
    GameplayEvents.Empty();

    // Clear the previous export path.
    LastTelemetryFilePath.Empty();

    // Activate the tutorial session.
    bTutorialActive = true;
    bSessionStarted = true;

    UE_LOG(LogTemp, Display,
        TEXT("Tutorial session started. ID: %s | UTC: %s"),
        *SessionID,
        *SessionStartUTC);
}


void UTutorialDataSubsystem::LogGameplayEvent(
    const FString& EventCategory,
    const FString& EventName,
    const FString& Details,
    double Value)
{
    // Do not log events outside an active tutorial session.
    if (!bTutorialActive || !bSessionStarted)
    {
        return;
    }

    // Create a new gameplay event record.
    FTUTGameplayEvent NewEvent;

    // Give this event its sequence number.
    ++EventSequence;
    NewEvent.Sequence = EventSequence;

    // Record the current UTC time.
    const FDateTime Now = FDateTime::UtcNow();

    NewEvent.TimestampUTC = Now.ToIso8601();
    NewEvent.UnixTime = Now.ToUnixTimestamp();

    // Calculate elapsed seconds since the session began.
    const FTimespan Elapsed = Now - SessionStartTime;

    NewEvent.SecondsFromStart = FMath::Max<int64>(
        0,
        static_cast<int64>(Elapsed.GetTotalSeconds())
    );

    // Store event details supplied by Blueprint.
    NewEvent.EventCategory = EventCategory;
    NewEvent.EventName = EventName;
    NewEvent.Details = Details;
    NewEvent.Value = Value;

    // Retrieve the current world through our Game Instance.
    UGameInstance* GameInstance = GetGameInstance();
    UWorld* World = GameInstance ? GameInstance->GetWorld() : nullptr;

    if (World)
    {
        // Record the current level name, without the PIE prefix.
        NewEvent.MapName =
            UGameplayStatics::GetCurrentLevelName(World, true);

        // Locate the player's Pawn and record its position.
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (APawn* PlayerPawn = PC->GetPawn())
            {
                const FVector Position = PlayerPawn->GetActorLocation();

                NewEvent.PlayerX = Position.X;
                NewEvent.PlayerY = Position.Y;
                NewEvent.PlayerZ = Position.Z;
            }
        }
    }

    // Add the completed event to our session's event collection.
    GameplayEvents.Add(NewEvent);

    UE_LOG(LogTemp, Display,
        TEXT("Telemetry event %d recorded: %s / %s"),
        EventSequence,
        *EventCategory,
        *EventName);
}

bool UTutorialDataSubsystem::SaveProgress()
{
    // keep this functi8onality limited to our tutorial
    if (!bTutorialActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Save rejected: Tutorial is not active."));
        return false;
    }

    // Create an empty save-game object.
    UTutorialSaveGame* SaveData = Cast<UTutorialSaveGame>(UGameplayStatics::CreateSaveGameObject(UTutorialSaveGame::StaticClass()));

    // check that creation succeeded
    if (!SaveData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create TutorialSaveGame."));
        return false;
    }

    // Copy our current gameplay cprogress into the save.
    SaveData->SavedMap = CurrentMap;
    SaveData->SavedCheckpoint = CurrentCheckpoint;
    SaveData->SavedObjective = CurrentObjective;

    SaveData->bSavedHasTriedEntrance = bHasTriedEntrance;

    SaveData->bSavedHasReadMaintenanceTerminal = bHasReadMaintenanceTerminal;

    // Write the save game data to the selected slot
    const bool bSaved = UGameplayStatics::SaveGameToSlot(
        SaveData,
        SaveSlotName,
        SaveUserIndex
    );

    if (bSaved)
    {
        UE_LOG(LogTemp, Display, TEXT("Tutorial progress saved: %s | Checkpoint: %s"),
            *SaveSlotName,
            *CurrentCheckpoint.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error,
        TEXT("Tutorial progress save failed: %s"),
        *SaveSlotName);
    }

    return bSaved;
}


bool UTutorialDataSubsystem::LoadProgress()
{
    // Prevent loading through the tutorial system
    // when the tutorial is inactive.
    if (!bTutorialActive)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Load rejected: Tutorial is not active."));
        return false;
    }

    // Check whether the save slot exists.
    if (!UGameplayStatics::DoesSaveGameExist(
        SaveSlotName, SaveUserIndex))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("No tutorial save found: %s"),
            *SaveSlotName);
        return false;
    }

    // Read the save file and convert the result
    // to our custom UTutorialSaveGame class.
    UTutorialSaveGame* LoadedData =
        Cast<UTutorialSaveGame>(
            UGameplayStatics::LoadGameFromSlot(
                SaveSlotName,
                SaveUserIndex
            )
        );

    // Reject missing, corrupted, or incompatible data.
    if (!LoadedData)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Failed to load TutorialSaveGame."));
        return false;
    }

    // Check the saved data format version.
    if (LoadedData->SaveVersion != 1)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Unsupported tutorial save version: %d"),
            LoadedData->SaveVersion);
        return false;
    }

    // A map and checkpoint are required for restoration.
    if (LoadedData->SavedMap.IsNull() ||
        LoadedData->SavedCheckpoint.IsNone())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Save is missing a map or checkpoint."));
        return false;
    }

    // Restore gameplay progress into our subsystem.
    CurrentMap = LoadedData->SavedMap;
    CurrentCheckpoint = LoadedData->SavedCheckpoint;
    CurrentObjective = LoadedData->SavedObjective;

    bHasTriedEntrance =
        LoadedData->bSavedHasTriedEntrance;

    bHasReadMaintenanceTerminal =
        LoadedData->bSavedHasReadMaintenanceTerminal;

    // Tell the next map initialization that we want
    // to restore the saved checkpoint.
    bRestoreCheckpointOnNextMap = true;

    UE_LOG(LogTemp, Display,
        TEXT("Tutorial progress loaded: %s | Checkpoint: %s"),
        *SaveSlotName,
        *CurrentCheckpoint.ToString());

    return true;
}


bool UTutorialDataSubsystem::ExportTelemetryJSON()
{
    // Only export an active tutorial session.
    if (!bTutorialActive || !bSessionStarted)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Telemetry export rejected: No active session."));
        return false;
    }

    // ==========================================
    // BUILD THE SESSION DATA
    // ==========================================

    FTUTPlaytestSession SessionData;

    SessionData.SchemaVersion = 1;
    SessionData.BuildLabel = BuildLabel;
    SessionData.SessionID = SessionID;
    SessionData.ParticipantCode = ParticipantCode;

    SessionData.SessionStartUTC = SessionStartUTC;
    SessionData.SessionStartUnix = SessionStartUnix;

    SessionData.LastExportUTC =
        FDateTime::UtcNow().ToIso8601();

    // Copy all recorded gameplay events.
    SessionData.Events = GameplayEvents;

    // ==========================================
    // CONVERT THE STRUCT INTO JSON
    // ==========================================

    FString JsonString;


    const bool bConverted =
        FJsonObjectConverter::UStructToJsonObjectString(
            FTUTPlaytestSession::StaticStruct(),
            &SessionData,
            JsonString,
            0,
            0,
            0,
            nullptr,
            true
        );


    if (!bConverted)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Failed to convert telemetry session to JSON."));
        return false;
    }

    // ==========================================
    // PREPARE THE OUTPUT DIRECTORY
    // ==========================================

    const FString OutputDirectory = FPaths::Combine(
        FPaths::ProjectSavedDir(),
        TEXT("Telemetry")
    );

    if (!IFileManager::Get().MakeDirectory(
        *OutputDirectory, true))
    {
        UE_LOG(LogTemp, Error,
            TEXT("Failed to create telemetry directory: %s"),
            *OutputDirectory);
        return false;
    }

    // Use the session ID as the filename.
    const FString FileName =
        SessionID + TEXT(".json");

    const FString FilePath =
        FPaths::Combine(OutputDirectory, FileName);

    // ==========================================
    // WRITE THE JSON FILE
    // ==========================================

    const bool bSaved = FFileHelper::SaveStringToFile(
        JsonString,
        *FilePath,
        FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM
    );

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Failed to write telemetry JSON: %s"),
            *FilePath);
        return false;
    }

    // Store the location of the successful export.
    LastTelemetryFilePath = FilePath;

    UE_LOG(LogTemp, Display,
        TEXT("Telemetry exported: %s | Events: %d"),
        *FilePath,
        GameplayEvents.Num());

    return true;
}
