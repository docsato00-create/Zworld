// AssembledVehicle.cpp

#include "AssembledVehicle.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AAssembledVehicle::AAssembledVehicle()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    VehicleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("VehicleCollision"));
    RootComponent = VehicleCollision;

    VehicleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VehicleMesh"));
    VehicleMesh->SetupAttachment(RootComponent);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 500.0f;
    CameraBoom->bUsePawnControlRotation = true;

    VehicleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VehicleCamera"));
    VehicleCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    VehicleMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("VehicleMovement"));
    VehicleMovement->MaxSpeed = MaxSpeed;
    VehicleMovement->Acceleration = Acceleration;
    VehicleMovement->Deceleration = Acceleration * 2.0f;

    CurrentFuel = MaxFuel;
    CurrentHealth = MaxHealth;
}

void AAssembledVehicle::BeginPlay()
{
    Super::BeginPlay();
}

void AAssembledVehicle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsOccupied && HasFuel())
    {
        CurrentFuel -= FuelConsumptionPerSecond * DeltaTime * FMath::Abs(CurrentThrottle);
        CurrentFuel = FMath::Clamp(CurrentFuel, 0.0f, MaxFuel);
        ApplyMovement(DeltaTime);
    }
}

void AAssembledVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AAssembledVehicle::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AAssembledVehicle::MoveRight);
}

void AAssembledVehicle::MoveForward(float Value)
{
    CurrentThrottle = Value;
}

void AAssembledVehicle::MoveRight(float Value)
{
    CurrentSteering = Value;
}

void AAssembledVehicle::ApplyMovement(float DeltaTime)
{
    if (!HasFuel()) return;

    FVector Forward = GetActorForwardVector();
    FVector Right = GetActorRightVector();

    FVector Velocity = Forward * CurrentThrottle * MaxSpeed;
    AddActorWorldOffset(Velocity * DeltaTime, true);

    if (CurrentThrottle != 0.0f)
    {
        FRotator NewRotation = GetActorRotation();
        NewRotation.Yaw += CurrentSteering * TurnSpeed * DeltaTime * FMath::Sign(CurrentThrottle);
        SetActorRotation(NewRotation);
    }
}

void AAssembledVehicle::EnterVehicle(ACharacter* Character)
{
    if (!Character || bIsOccupied) return;

    CurrentDriver = Character;
    bIsOccupied = true;

    Character->AttachToComponent(VehicleMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Seat"));
    Character->SetActorHiddenInGame(true);
    Character->GetCharacterMovement()->Deactivate();

    if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
    {
        PC->Possess(this);
    }
}

void AAssembledVehicle::ExitVehicle(ACharacter* Character)
{
    if (!Character || !bIsOccupied) return;

    bIsOccupied = false;

    FVector ExitLocation = GetActorLocation() + (GetActorRightVector() * 200.0f);
    Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Character->SetActorLocation(ExitLocation);
    Character->SetActorHiddenInGame(false);
    Character->GetCharacterMovement()->Activate();

    if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
    {
        PC->Possess(Character);
    }

    CurrentDriver = nullptr;
}

void AAssembledVehicle::Refuel(float Amount)
{
    CurrentFuel = FMath::Clamp(CurrentFuel + Amount, 0.0f, MaxFuel);
}

void AAssembledVehicle::Repair(float Amount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
}

void AAssembledVehicle::TakeDamage(float Damage)
{
    CurrentHealth -= Damage;
    if (CurrentHealth <= 0.0f)
    {
        // Explode / become wreck again
        // Spawn wrecked vehicle
        Destroy();
    }
}

void AAssembledVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAssembledVehicle, CurrentFuel);
    DOREPLIFETIME(AAssembledVehicle, CurrentHealth);
    DOREPLIFETIME(AAssembledVehicle, bIsOccupied);
    DOREPLIFETIME(AAssembledVehicle, OwnerPlayerState);
}
