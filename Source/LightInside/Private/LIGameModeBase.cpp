// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "LIGameModeBase.h"
#include "Player/LIBaseCharacter.h"
#include "Player/LIPlayerController.h"

ALIGameModeBase::ALIGameModeBase()
{
	DefaultPawnClass = ALIBaseCharacter::StaticClass();
	PlayerControllerClass = ALIPlayerController::StaticClass();
}
