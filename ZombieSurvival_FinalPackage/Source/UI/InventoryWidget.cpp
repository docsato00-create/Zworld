// InventoryWidget.cpp

#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "SurvivalComponent.h"
#include "SurvivalCharacter.h"
#include "ItemData.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOwningPlayerPawn()))
    {
        PlayerInventory = Character->FindComponentByClass<UInventoryComponent>();
        PlayerSurvival = Character->FindComponentByClass<USurvivalComponent>();
    }

    RefreshInventory();
}

void UInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (WeightText && PlayerInventory)
    {
        WeightText->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f kg"), 
            PlayerInventory->GetCurrentWeight(), PlayerInventory->MaxWeight)));
    }
}

void UInventoryWidget::RefreshInventory()
{
    if (!PlayerInventory || !InventoryGrid) return;

    InventoryGrid->ClearChildren();
    SlotWidgets.Empty();

    for (int32 i = 0; i < PlayerInventory->Slots.Num(); i++)
    {
        if (SlotWidgetClass)
        {
            UUserWidget* SlotWidget = CreateWidget<UUserWidget>(this, SlotWidgetClass);
            if (SlotWidget)
            {
                // Pass slot data to widget
                // SlotWidget->SetSlotData(PlayerInventory->Slots[i], i);
                InventoryGrid->AddChildToUniformGrid(SlotWidget, i / 6, i % 6);
                SlotWidgets.Add(SlotWidget);
            }
        }
    }

    OnInventoryRefreshed();
}

void UInventoryWidget::OnSlotClicked(int32 SlotIndex)
{
    UseItem(SlotIndex);
}

void UInventoryWidget::OnSlotRightClicked(int32 SlotIndex)
{
    // Show context menu: Use, Drop, Split, Equip
}

void UInventoryWidget::OnSlotDragStarted(int32 SlotIndex)
{
    DraggedSlotIndex = SlotIndex;
}

void UInventoryWidget::OnSlotDropped(int32 TargetSlotIndex)
{
    if (DraggedSlotIndex >= 0 && DraggedSlotIndex != TargetSlotIndex)
    {
        // Swap items
        if (PlayerInventory && PlayerInventory->Slots.IsValidIndex(DraggedSlotIndex) 
            && PlayerInventory->Slots.IsValidIndex(TargetSlotIndex))
        {
            Swap(PlayerInventory->Slots[DraggedSlotIndex], PlayerInventory->Slots[TargetSlotIndex]);
            RefreshInventory();
        }
    }
    DraggedSlotIndex = -1;
}

void UInventoryWidget::DropItemToWorld(int32 SlotIndex)
{
    if (PlayerInventory && PlayerInventory->Slots.IsValidIndex(SlotIndex))
    {
        PlayerInventory->DropItem(SlotIndex, PlayerInventory->Slots[lotIndex].Quantity);
        RefreshInventory();
    }
}

void UInventoryWidget::UseItem(int32 SlotIndex)
{
    if (PlayerInventory)
    {
        PlayerInventory->UseItem(SlotIndex);
        OnItemUsed(SlotIndex);
        RefreshInventory();
    }
}

void UInventoryWidget::SplitStack(int32 SlotIndex)
{
    // Split stack in half
}
