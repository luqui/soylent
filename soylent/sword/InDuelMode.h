#ifndef InDuelMode_h
#define InDuelMode_h

#include "DuelMode.h"
#include "FlockController.h"
#include "InputNexus.h"
#include "InFighterInput.h"

// Controllable descendant that contains a DuelMode.
class InDuelMode : public MultiPlayer
{
public:

	InDuelMode(DuelMode *aDm)
		:mDm(aDm), mIfiPlayer1(&aDm->player1_), mIfiPlayer2(&aDm->player2_)
	{
		AddControllable(&mIfiPlayer1, "Player 1 - ");
		AddControllable(&mIfiPlayer2, "Player 2 - ");
	}

private:
	DuelMode *mDm;
	InFighterInput mIfiPlayer1;
	InFighterInput mIfiPlayer2;
};

#endif
