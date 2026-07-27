// BuildingComponent.h
// Source/ZombieSurvival/Building/BuildingComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZOMBIESURVIVAL_API UBuildingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBuildingComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    float PlacementRange = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    float GridSnapSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TSubclassOf<class ABuildingPiece> CurrentBuildingPieceClass;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Building")
    bool bIsInBuildMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "Building")
    AActor* PlacementPreview = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Building")
    void ToggleBuildMode();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetBuildingPiece(TSubclassOf<ABuildingPiece> PieceClass);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void RotateBuildingPiece(float DeltaYaw);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void PlaceBuildingPiece();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void CancelBuildMode();

    UFUNCTION(BlueprintPure, Category = "Building")
    bool CanPlaceAtLocation(FVector Location, FRotator Rotation) const;

protected:
    void UpdatePlacementPreview();
    FVector GetPlacementLocation() const;
    FRotator GetPlacementRotation() const;

    UPROPERTY(EditAnywhere, Category = "Building")
    float RotationStep = 90.0f;

    float CurrentRotationYaw = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Building")
    TEnumAsByte<ECollisionChannel> BuildingCollisionChannel = ECC_WorldStatic;
};
