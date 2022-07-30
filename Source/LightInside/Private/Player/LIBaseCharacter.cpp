// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Player/LIBaseCharacter.h"

ALIBaseCharacter::ALIBaseCharacter()
{

	PrimaryActorTick.bCanEverTick = true;
}

void ALIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
