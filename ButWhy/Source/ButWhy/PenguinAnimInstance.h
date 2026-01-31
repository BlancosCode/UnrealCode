// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PenguinAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BUTWHY_API UPenguinAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UPROPERTY(BlueprintReadOnly, Category = "Penguin Movement")
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Penguin Movement")
	bool bIsMoving;
};
