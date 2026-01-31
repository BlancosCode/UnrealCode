// Fill out your copyright notice in the Description page of Project Settings.


#include "PenguinAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPenguinAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwningPawn = TryGetPawnOwner();
    if (OwningPawn)
    {
        FVector Velocity = OwningPawn->GetVelocity();
        Velocity.Z = 0;
        GroundSpeed = Velocity.Size();

        bIsMoving = GroundSpeed > 3.f;
    }
}
