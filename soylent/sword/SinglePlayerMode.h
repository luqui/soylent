#ifndef SinglePlayerMode_h
#define SinglePlayerMode_h

#include "DuelMode.h"

class SinglePlayerMode : public DuelMode
{
public:
	//class Args : public DuelMode::Args
	//{
	//public:
	//	const NVThing *mNvtPlaybackFiles;
	//	int mIDeathTarget;
	//};

	typedef DuelMode Ancestor;

	SinglePlayerMode(const NVThing &aNvtPrefs)
		:DuelMode(aNvtPrefs)
	{
		NVThing *lNvtSingle;
		if (!aNvtPrefs.Find("SinglePlayerPrefs", lNvtSingle))
			DIE("SinglePlayer section was not found in prefs");

		NVThing *lNvtPbf;
		if (!lNvtSingle->Find("PlaybackFiles", lNvtPbf))
			DIE("PlaybackFiles section was not found in prefs");

		if (!lNvtSingle->Find("DeathTarget", mIDeathTarget))
			DIE("DeathTarget was not found in single player prefs!");

		mNvtPlaybackFiles = *lNvtPbf;
		if (!mNvtPlaybackFiles.Find("Count", mIFileCount))
			DIE("'Count' was not found!");

		mIDrawRoundCount = 0;

		// advance to the next round!
		std::ostringstream lOss;
		mIFileIndex = 0;
		lOss << "Round " << mIFileIndex + 1 << "; '" << CurrentPlaybackFile() << "'";
		DisplayRoundMessage(lOss.str(), 250);

		// Start playback with this file.
		player2_.LoopPlayback(CurrentPlaybackFile());

		mOfsScore.open("Score.txt");
		mOfsScore << "Your Sword Game score sheet: " << endl;
	}
	virtual ~SinglePlayerMode()
	{
		if (mOfsScore.is_open())
		{
			mOfsScore << "You gave up on opponent: " << mIFileIndex + 1 << "; '" << CurrentPlaybackFile() << "'";
			mOfsScore.close();
		}
	}

	static void SetupDefaultPrefs(NVThing &aNvt)
	{
		NVThing *lNvtPlayback = new NVThing;
		lNvtPlayback->Add("Count", 1);
		lNvtPlayback->Add("PlaybackFile", 1, "Replace this with a real filename");

		aNvt.Add("PlaybackFiles", lNvtPlayback);
		aNvt.Add("DeathTarget", 5);
	}

	virtual void draw_scores()
	{
		Ancestor::draw_scores();

		if (mIDrawRoundCount)
		{
			--mIDrawRoundCount;

            glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			glLoadIdentity();
			gluOrtho2D(-INIT.width()/2,INIT.width()/2,INIT.height()/2,-INIT.height()/2);

			// upper left is (0, 0)
			glTranslatef(-INIT.width()/2, -INIT.height()/2, 0);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			// Draw the 'round' message.
			// If multiple lines, center each line 15 units below the previous line.
			int lIHeight = INIT.height() / 3.0 + 15/2;
			int i = 0;
			while (i < mSRoundMessage.size())
			{
				int iret = mSRoundMessage.find_first_of("\n", i);
				if (iret == -1)
					iret = mSRoundMessage.size();
				
				glRasterPos3d(INIT.width()/2 - (iret - i) * 9.0/2.0, lIHeight, 0);
				for (size_t j = i; j < iret; j++) 
				{
					glutBitmapCharacter(GLUT_BITMAP_9_BY_15, mSRoundMessage[j]);
				}
				i = iret + 1;
				lIHeight += 15;
			}
			

			glPopMatrix();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
		}
	}

private:
	virtual void OnMeatSackSworded(MeatSack *aM, Sword* aS)
	{
		Ancestor::OnMeatSackSworded(aM, aS);

		if (aM->get_deaths() >= mIDeathTarget)
		{
			if (aM == player1_.get_meatsack())
			{
				mOfsScore << "You lost against robot " << mIFileIndex + 1 << " '" << CurrentPlaybackFile() << "'" << endl ;
				mOfsScore << "\tYour score: " << player2_.get_meatsack()->get_deaths() << " kills, " 
											<< player2_.get_meatsack()->get_touches() << " touches" << endl;
				mOfsScore << "\tRobot score: " << player1_.get_meatsack()->get_deaths() << " kills, " 
											<< player1_.get_meatsack()->get_touches() << " touches" << endl;
				// Start the same round again.
				player1_.get_meatsack()->set_deaths(0);
				player1_.get_meatsack()->set_touches(0);
				player2_.get_meatsack()->set_deaths(0);
				player2_.get_meatsack()->set_touches(0);

				std::ostringstream lOss;
				lOss << "You lost to the robot!" << endl << endl;
				lOss << "Restarting round: " << mIDrawRoundCount + 1 << "; '" << CurrentPlaybackFile() << "'";
				DisplayRoundMessage(lOss.str(), 250);
			}
			else
			{
				mOfsScore << "You won against robot " << mIFileIndex + 1 << ", '" << CurrentPlaybackFile() << "'" << endl;
				mOfsScore << "\tYour score: " << player2_.get_meatsack()->get_deaths() << " kills, " 
											<< player2_.get_meatsack()->get_touches() << " touches" << endl;
				mOfsScore << "\tRobot score: " << player1_.get_meatsack()->get_deaths() << " kills, " 
											<< player1_.get_meatsack()->get_touches() << " touches" << endl;
				// advance to the next round!
				player1_.get_meatsack()->set_deaths(0);
				player1_.get_meatsack()->set_touches(0);
				player2_.get_meatsack()->set_deaths(0);
				player2_.get_meatsack()->set_touches(0);

				std::ostringstream lOss;
				lOss << "Robot '" << CurrentPlaybackFile() << "' defeated!" << endl << endl;
				++mIFileIndex;
				if (mIFileIndex < mIFileCount)
				{
					lOss << "Starting round " << mIFileIndex + 1 << "; '" << CurrentPlaybackFile() << "'";
					DisplayRoundMessage(lOss.str(), 250);
					player2_.LoopPlayback(CurrentPlaybackFile());
				}
				else
				{
					mOfsScore << "You won all rounds!  Congrats." << endl;
					mOfsScore.close();
					player2_.StopPlayback();
					DisplayRoundMessage("Hey you won!", 250);
				}
			}
		}
	}

	const char* CurrentPlaybackFile() const
	{
		const char *lC;
		if (mNvtPlaybackFiles.Find("PlaybackFile", mIFileIndex + 1, lC))
			return lC;
		else
		{
			DIE("Playback file not found in NVT!");
			return 0;	// never reached!
		}
	}

	void DisplayRoundMessage(const string &aS, int aICount)
	{
		mSRoundMessage = aS;
		mIDrawRoundCount = aICount;
	}

	string mSRoundMessage;
	int mIDrawRoundCount;

	// How many deaths until you start the next round?
	int mIDeathTarget;

	int mIFileIndex;
	int mIFileCount;
	NVThing mNvtPlaybackFiles;

	ofstream mOfsScore;

};

#endif