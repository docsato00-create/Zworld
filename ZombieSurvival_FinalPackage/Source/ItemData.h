// ItemData.h
// Source/ZombieSurvival/Inventory/ItemData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivalTypes.h"
#include "ItemData.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API UItemData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemRarity Rarity = EItemRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float Weight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float Condition = 100.0f; // 0-100, dla części pojazdów / broni

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TSubclassOf<class AActor> WorldActorClass; // BP wyrzuconego itemu

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsConsumable = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bIsConsumable"))
    float HungerRestore = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bIsConsumable"))
    float ThirstRestore = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bIsConsumable"))
    float HealthRestore = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsWeapon = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bIsWeapon"))
    float WeaponDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsBuildingMaterial = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bIsBuildingMaterial"))
    TSubclassOf<class ABuildingPiece> BuildingPieceClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsVehiclePart = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bIsVehiclePart"))
    FName VehiclePartSlotName;
};
