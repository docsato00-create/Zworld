// LootContainer.cpp

#include "LootContainer.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ItemData.h"
#include "InventoryComponent.h"

ALootContainer::ALootContainer()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    ContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
    RootComponent = ContainerMesh;

    InteractionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRadius"));
    InteractionRadius->SetupAttachment(RootComponent);
    InteractionRadius->SetSphereRadius(200.0f);
    InteractionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALootContainer::BeginPlay()
{
    Super::BeginPlay();
    if (GetLocalRole() == ROLE_Authority)
    {
        GenerateLoot();
    }
}

void ALootContainer::GenerateLoot()
{
    CurrentLoot.Empty();
    bHasBeenLooted = false;

    for (const FLootEntry& Entry : PossibleLoot)
    {
        float Roll = FMath::FRand();
        if (Roll <= Entry.DropChance)
        {
            FInventorySlot Slot;
            Slot.Item = Entry.Item;
            Slot.Quantity = FMath::RandRange(Entry.MinQuantity, Entry.MaxQuantity);
            Slot.Condition = FMath::FRandRange(Entry.MinCondition, Entry.MaxCondition);
            CurrentLoot.Add(Slot);

            if (CurrentLoot.Num() >= MaxLootSlots) break;
        }
    }

    OnRep_LootGenerated();
}

void ALootContainer::TakeItem(int32 SlotIndex, AActor* Taker)
{
    if (!CurrentLoot.IsValidIndex(SlotIndex) || !Taker) return;

    UInventoryComponent* InvComp = Taker->FindComponentByClass<UInventoryComponent>();
    if (!InvComp) return;

    FInventorySlot& Slot = CurrentLoot[SlotIndex];
    if (InvComp->AddItem(Slot.Item, Slot.Quantity, Slot.Condition))
    {
        CurrentLoot.RemoveAt(SlotIndex);

        if (CurrentLoot.Num() == 0)
        {
            bHasBeenLooted = true;
            GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ALootContainer::RespawnLoot, RespawnTime, false);
        }
    }
}

void ALootContainer::RespawnLoot()
{
    GenerateLoot();
}

bool ALootContainer::IsEmpty() const
{
    return CurrentLoot.Num() == 0;
}

void ALootContainer::OnRep_LootGenerated()
{
    // Update UI / visual state
    if (bHasBeenLooted)
    {
        // Set empty material / open lid
    }
}

void ALootContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALootContainer, bHasBeenLooted);
    DOREPLIFETIME(ALootContainer, CurrentLoot);
}
