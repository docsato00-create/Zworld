// BuildingComponent.cpp

#include "BuildingComponent.h"
#include "BuildingPiece.h"
#include "ClaimBlock.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

UBuildingComponent::UBuildingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void UBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsInBuildMode)
    {
        UpdatePlacementPreview();
    }
}

void UBuildingComponent::ToggleBuildMode()
{
    if (bIsInBuildMode)
    {
        CancelBuildMode();
    }
    else
    {
        bIsInBuildMode = true;
        if (CurrentBuildingPieceClass)
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            PlacementPreview = GetWorld()->SpawnActor<AActor>(CurrentBuildingPieceClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
            if (PlacementPreview)
            {
                PlacementPreview->SetActorEnableCollision(false);
            }
        }
    }
}

void UBuildingComponent::SetBuildingPiece(TSubclassOf<ABuildingPiece> PieceClass)
{
    CurrentBuildingPieceClass = PieceClass;
    if (bIsInBuildMode)
    {
        CancelBuildMode();
        ToggleBuildMode();
    }
}

void UBuildingComponent::RotateBuildingPiece(float DeltaYaw)
{
    CurrentRotationYaw += DeltaYaw;
    CurrentRotationYaw = FMath::GridSnap(CurrentRotationYaw, RotationStep);
}

void UBuildingComponent::PlaceBuildingPiece()
{
    if (!bIsInBuildMode || !CurrentBuildingPieceClass) return;

    FVector Location = GetPlacementLocation();
    FRotator Rotation = GetPlacementRotation();

    if (!CanPlaceAtLocation(Location, Rotation)) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ABuildingPiece* NewPiece = GetWorld()->SpawnActor<ABuildingPiece>(CurrentBuildingPieceClass, Location, Rotation, Params);

    if (NewPiece)
    {
        // Odejmij surowce z inventory
    }
}

void UBuildingComponent::CancelBuildMode()
{
    bIsInBuildMode = false;
    if (PlacementPreview)
    {
        PlacementPreview->Destroy();
        PlacementPreview = nullptr;
    }
}

bool UBuildingComponent::CanPlaceAtLocation(FVector Location, FRotator Rotation) const
{
    TArray<AActor*> ClaimBlocks;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClaimBlock::StaticClass(), ClaimBlocks);

    bool bInClaim = false;
    for (AActor* Actor : ClaimBlocks)
    {
        AClaimBlock* Claim = Cast<AClaimBlock>(Actor);
        if (Claim && Claim->IsWithinClaim(Location))
        {
            bInClaim = true;
            break;
        }
    }

    FCollisionShape Shape = FCollisionShape::MakeBox(FVector(50, 50, 50));
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlacementPreview);

    bool bOverlap = GetWorld()->OverlapMultiByChannel(Overlaps, Location, Rotation.Quaternion(), BuildingCollisionChannel, Shape, Params);

    return !bOverlap && bInClaim;
}

void UBuildingComponent::UpdatePlacementPreview()
{
    if (!PlacementPreview) return;

    FVector Location = GetPlacementLocation();
    FRotator Rotation = GetPlacementRotation();

    PlacementPreview->SetActorLocation(Location);
    PlacementPreview->SetActorRotation(Rotation);
}

FVector UBuildingComponent::GetPlacementLocation() const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return FVector::ZeroVector;

    FVector Start = Character->GetActorLocation() + FVector(0, 0, 50);
    FVector End = Start + (Character->GetActorForwardVector() * PlacementRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        FVector Snapped = Hit.Location;
        Snapped.X = FMath::GridSnap(Snapped.X, GridSnapSize);
        Snapped.Y = FMath::GridSnap(Snapped.Y, GridSnapSize);
        Snapped.Z = FMath::GridSnap(Snapped.Z, GridSnapSize);
        return Snapped;
    }

    return End;
}

FRotator UBuildingComponent::GetPlacementRotation() const
{
    return FRotator(0, CurrentRotationYaw, 0);
}
