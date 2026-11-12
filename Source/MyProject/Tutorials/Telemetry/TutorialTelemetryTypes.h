
#pragma once

#include "CoreMinimal.h"
#include "TutorialTelemetryTypes.generated.h"

// Holds information about a single gameplay event.
// Example: Player attempted to open a locked door.

USTRUCT(BlueprintType)
struct MYPROJECT_API FTUTGameplayEvent
{
    GENERATED_BODY()

    // Each recorded event receives a sequence number.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    int32 Sequence = 0;

    // Human-readable UTC timestamp.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString TimestampUTC;

    // Time expressed as a Unix timestamp.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    int64 UnixTime = 0;

    // Time elapsed since the session started.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    int64 SecondsFromStart = 0;

    // Name of the map where the event occurred.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString MapName;

    // General category, such as Interaction or Navigation.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString EventCategory;

    // Specific event, such as StationDoorLocked.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString EventName;

    // Additional descriptive information.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString Details;

    // Optional numeric measurement associated with this event.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    double Value = 0.0;

    // Player position when the event occurred.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    double PlayerX = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    double PlayerY = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    double PlayerZ = 0.0;
};

/**
* Holds information about an entire playtesting session.
* Contains session metadata and recorded gameplay events.
*/
USTRUCT(BlueprintType)
struct MYPROJECT_API FTUTPlaytestSession
{
    GENERATED_BODY()

    // Version of the telemetry data format.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    int32 SchemaVersion = 1;

    // Identifies which game build was tested.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString BuildLabel;

    // Unique identifier for this playtesting session.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString SessionID;

    // Anonymous or assigned participant identifier.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString ParticipantCode;

    // Human-readable UTC session start time.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString SessionStartUTC;

    // Session start time as a Unix timestamp.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    int64 SessionStartUnix = 0;

    // UTC timestamp of the most recent data export.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    FString LastExportUTC;

    // Collection of gameplay events recorded during this session.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
    TArray<FTUTGameplayEvent> Events;
};