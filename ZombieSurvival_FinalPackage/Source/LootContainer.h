// LootContainer.h
// Source/ZombieSurvival/Inventory/LootContainer.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootContainer.generated.h"

USTRUCT(BlueprintType)
struct FLootEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<class UItemData> Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DropChance = 0.5f; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinCondition = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxCondition = 100.0f;
};

UCLASS()
class ZOMBIESURVIVAL_API ALootContainer : public AActor
{
    GENERATED_BODY()

public:
    ALootContainer();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    FName ContainerName = FName("Container");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 MaxLootSlots = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    TArray<FLootEntry> PossibleLoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    float RespawnTime = 300.0f; // 5 min

    UPROPERTY(ReplicatedUsing = OnRep_LootGenerated, BlueprintReadOnly, Category = "Loot")
    bool bHasBeenLooted = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Loot")
    TArray<struct FInventorySlot> CurrentLoot;

    // === FUNCTIONS ===
    UFUNCTION(BlueprintCallable, Category = "Loot")
    void GenerateLoot();

    UFUNCTION(BlueprintCallable, Category = "Loot")
    void TakeItem(int32 SlotIndex, AActor* Taker);

    UFUNCTION(BlueprintCallable, Category = "Loot")
    void RespawnLoot();

    UFUNCTION(BlueprintCallable, Category = "Loot")
    bool IsEmpty() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot")
    class UStaticMeshComponent* ContainerMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot")
    class USphereComponent* InteractionRadius;

    UPROPERTY()
    FTimerHandle RespawnTimerHandle;

    UFUNCTION()
    void OnRep_LootGenerated();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
