// InventoryComponent.h
// Source/ZombieSurvival/Inventory/InventoryComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<class UItemData> Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Condition = 100.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZOMBIESURVIVAL_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxSlots = 24;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    float MaxWeight = 100.0f;

    UPROPERTY(ReplicatedUsing = OnRep_Inventory, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventorySlot> Slots;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
    int32 HotbarSize = 6;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
    int32 SelectedHotbarSlot = 0;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(TSoftObjectPtr<UItemData> Item, int32 Quantity = 1, float Condition = 100.0f);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(TSoftObjectPtr<UItemData> Item, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasItem(TSoftObjectPtr<UItemData> Item, int32 Quantity = 1) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    float GetCurrentWeight() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void DropItem(int32 SlotIndex, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItem(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SelectHotbarSlot(int32 Slot);

protected:
    UFUNCTION()
    void OnRep_Inventory();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
