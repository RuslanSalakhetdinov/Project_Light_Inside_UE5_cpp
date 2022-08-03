// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Components/LICharacterMovementComponent.h"
#include <Player/LIBaseCharacter.h>

float ULICharacterMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	const ALIBaseCharacter* Player = Cast<ALIBaseCharacter>(GetPawnOwner());
	return Player && Player->IsSpeedBuffed ? MaxSpeed * Player->RunModifier : MaxSpeed;
}
