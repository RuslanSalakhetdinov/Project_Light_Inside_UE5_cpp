// Project "Lignt Inside" Game. RedBuff Studios copyright.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LICharacterMovementComponent.generated.h"

/**
 *
 */
UCLASS()
class LIGHTINSIDE_API ULICharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;
};
