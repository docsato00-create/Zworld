// SurvivalCharacter.cpp

#include "SurvivalCharacter.h"
#include "SurvivalComponent.h"
#include "InventoryComponent.h"
#include "BuildingComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ASurvivalCharacter::ASurvivalCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    // Movement defaults
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
    GetCharacterMovement()->MaxSprintSpeed = 600.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    SurvivalComp = CreateDefaultSubobject<USurvivalComponent>(TEXT("SurvivalComp"));
    InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
    BuildingComp = CreateDefaultSubobject<UBuildingComponent>(TEXT("BuildingComp"));
}

void ASurvivalCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void ASurvivalCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSprinting && SurvivalComp)
    {
        SurvivalComp->DrainStaminaForSprint(DeltaTime);
        if (SurvivalComp->GetStaminaPercent() <= 0.0f)
        {
            StopSprint();
        }
    }
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Move);
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Look);
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &ASurvivalCharacter::StartSprint);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASurvivalCharacter::StopSprint);
        EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ASurvivalCharacter::PerformInteraction);
        EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &ASurvivalCharacter::FireWeapon);
        EnhancedInput->BindAction(AimAction, ETriggerEvent::Started, this, &ASurvivalCharacter::StartAim);
        EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, this, &ASurvivalCharacter::StopAim);
        EnhancedInput->BindAction(BuildModeAction, ETriggerEvent::Started, this, &ASurvivalCharacter::ToggleBuildMode);
    }
}

void ASurvivalCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ASurvivalCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void ASurvivalCharacter::StartSprint()
{
    if (!CanSprint()) return;
    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxSprintSpeed;
}

void ASurvivalCharacter::StopSprint()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

bool ASurvivalCharacter::CanSprint() const
{
    return SurvivalComp && SurvivalComp->GetStaminaPercent() > 0.0f && !bIsAiming;
}

void ASurvivalCharacter::PerformInteraction()
{
    if (!Controller) return;

    FVector CameraLocation;
    FRotator CameraRotation;
    Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + (CameraRotation.Vector() * InteractionRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            Server_PerformInteraction(HitActor);
        }
    }
}

void ASurvivalCharacter::Server_PerformInteraction_Implementation(AActor* Target)
{
    // Server-side interaction logic
    // Open door, loot container, install vehicle part, etc.
}

void ASurvivalCharacter::PerformMeleeAttack()
{
    Server_PerformMeleeAttack();
}

void ASurvivalCharacter::Server_PerformMeleeAttack_Implementation()
{
    Multicast_PlayMeleeAnim();

    FVector TraceStart = GetActorLocation() + FVector(0, 0, 50);
    FVector TraceEnd = TraceStart + (GetActorForwardVector() * 200.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, Params))
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            UGameplayStatics::ApplyDamage(HitActor, MeleeDamage, GetController(), this, nullptr);
        }
    }
}

void ASurvivalCharacter::Multicast_PlayMeleeAnim_Implementation()
{
    // Play melee montage
}

void ASurvivalCharacter::FireWeapon()
{
    // Weapon firing logic
}

void ASurvivalCharacter::StartAim()
{
    bIsAiming = true;
    if (bIsSprinting) StopSprint();
}

void ASurvivalCharacter::StopAim()
{
    bIsAiming = false;
}

void ASurvivalCharacter::ToggleBuildMode()
{
    if (BuildingComp)
    {
        BuildingComp->ToggleBuildMode();
    }
}

void ASurvivalCharacter::ApplyDamageToCharacter(float Damage, AActor* DamageCauser)
{
    if (SurvivalComp)
    {
        SurvivalComp->ModifyHealth(-Damage);
    }
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASurvivalCharacter, bIsAiming);
    DOREPLIFETIME(ASurvivalCharacter, bIsSprinting);
}
