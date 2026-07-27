// InventoryWidget.h
// Source/ZombieSurvival/UI/InventoryWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

UCLASS()
class ZOMBIESURVIVAL_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void RefreshInventory();

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnSlotClicked(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnSlotRightClicked(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnSlotDragStarted(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnSlotDropped(int32 TargetSlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void DropItemToWorld(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void UseItem(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void SplitStack(int32 SlotIndex);

    UPROPERTY(BlueprintReadOnly, Category = "Inventory UI")
    class UInventoryComponent* PlayerInventory;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory UI")
    class USurvivalComponent* PlayerSurvival;

protected:
    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* InventoryGrid;

    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* HotbarGrid;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeightText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CapacityText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory UI")
    TSubclassOf<class UUserWidget> SlotWidgetClass;

    UPROPERTY()
    TArray<class UUserWidget*> SlotWidgets;

    UPROPERTY()
    int32 DraggedSlotIndex = -1;

    UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI")
    void OnInventoryRefreshed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI")
    void OnItemUsed(int32 SlotIndex);
};
