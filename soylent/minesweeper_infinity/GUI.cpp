#include "GUI.h"
#include "Utils.h"
#include "drawFunctions.h"
#include "SDL.h"
#include "Font.h"
#include "HighScoreSheet.h"
#include <string>
#include <sstream>

bool inTab(int x, int y) {
	if(y >= boardHeight * squareLength)
		if(x >= clockWidth && x <= boardWidth * squareLength - clockWidth) {
			loadGUIPage("Options");
			return true;
		}
	
	return false;
}

class GUIPage {
protected:
	/*
	* Guichan SDL stuff we need
	*/
	gcn::SDLInput* input;             // Input driver
	gcn::SDLGraphics* graphics;       // Graphics driver
	gcn::SDLImageLoader* imageLoader; // For loading images

	/*
	* Guichan stuff we need
	*/
	gcn::Gui* gui;            // A Gui object - binds it all together
	gcn::Container* top;      // A top container
	Font* font;

	bool running;
	string page;

	GUIPage(){
		input = new gcn::SDLInput();
		
		imageLoader = new gcn::SDLImageLoader();
		gcn::Image::setImageLoader(imageLoader);

		top = new gcn::Container(); 
		top->setDimension(gcn::Rectangle(0, 0, screen->w, screen->h));
		top->setBaseColor(gcn::Color(31, 26, 85, 100));
		
		graphics = new gcn::SDLGraphics();
		graphics->setTarget(screen);
		
		gui = new gcn::Gui();
		gui->setGraphics(graphics);
		gui->setInput(input);	
		gui->setTop(top);

		font = new Font(ART_FONT, int(14*scaleFactor));
		gcn::Widget::setGlobalFont(font);
	}

	virtual void handleEvent(SDL_Event& event) {
		if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{  
				quit = true;
				running = false;
                return;
			}
			if(event.key.keysym.sym == SDLK_SPACE && page.compare("options") == 0){
				running = false;
                return;
			}
			if(event.key.keysym.sym == SDLK_s && page.compare("score") == 0) {
				running = false;
				return;
			}
			if(event.key.keysym.sym == SDLK_f && page.compare("options") == 0) {
				flagClear = !flagClear;
				return;
			}
			if(event.key.keysym.sym == SDLK_l && page.compare("options") == 0) {
				rightClickInterface = !rightClickInterface;
				return;
			}
			if(event.key.keysym.sym == SDLK_a && page.compare("options") == 0) {
				autocount = !autocount;
				return;
			}
		}
		else if(event.type == SDL_QUIT)
		{
			quit = true;
			running = false;
			return;
		}

		/*
		* Now that we are done polling and using SDL events we pass
		* the leftovers to the SDLInput object to later be handled by
		* the Gui. (This example doesn't require us to do this 'cause a
		* label doesn't use input. But will do it anyway to show how to
		* set up an SDL application with Guichan.)
		*/
		input->pushInput(event);        		
	}

	virtual void checkInput()
	{
		SDL_Event event;
		/*
		* Poll SDL events
		*/
		while(SDL_PollEvent(&event))
		{
			handleEvent(event);
		}
	}


public:
	virtual void run() {
        running = true;
		stopGameClock();
		screenStack.push(screen);

		while(running) {
			checkInput();
			gui->logic();
			gui->draw();
			
			SDL_Flip(screen);
		}
		
		screenStack.pop(screen);
		SDL_Flip(screen);
		
		startGameClock();
	}
	
	virtual ~GUIPage(){
		delete font;
		delete top;
		delete gui;
		
		delete input;
		delete graphics;
		delete imageLoader;
	}
};

class OptionsPage : public GUIPage {
	class OptionsActionListener : public gcn::ActionListener
	{
		class OptionsPage* oPage;
	public:
		OptionsActionListener(OptionsPage* passedPage){
			oPage = passedPage;
		}
		// Implement the action function in ActionListener to recieve actions
		// The eventId tells us which widget called the action function.
		void action(const gcn::ActionEvent& eventId)
		{
			std::string str;
			std::ostringstream os(str);
            std::string eid = eventId.getId();
			
			if(eid == "autocountCheckBox") {
				autocount = oPage->autocountCheckBox->isMarked();
				refreshGameBoard(screenStack.top());
			}
			else if(eid == "flagClearCheckBox")
				flagClear = oPage->flagClearCheckBox->isMarked();
			else if(eid == "interfaceCheckBox")
				rightClickInterface = oPage->interfaceCheckBox->isMarked();
			else if(eid == "OKButton")
				oPage->running = false;
		}
	};

protected:
	gcn::Button* OKButton;     // A button for actionlistening
	gcn::Label* titleLabel;
	gcn::CheckBox* autocountCheckBox;
	gcn::CheckBox* flagClearCheckBox;
	gcn::CheckBox* interfaceCheckBox;
	Font* optionsFont;
	Font* optionsFont2;

	OptionsActionListener* optionsActionListener;

	void MakeCheckBox(string text, bool checked, int x, int y, string eventID, gcn::CheckBox*& checkBox) {
		checkBox = new gcn::CheckBox(text);
		checkBox->setMarked(checked);
		checkBox->setFocusable(false);
		checkBox->setPosition(x, y);
		// checkBox->setEventId(eventID);
		checkBox->addActionListener(optionsActionListener);
		top->add(checkBox);
	}

	virtual void handleEvent(SDL_Event& event) {
		GUIPage::handleEvent(event);

		if (event.type == SDL_KEYUP)
		{
			autocountCheckBox->setMarked(autocount);
			flagClearCheckBox->setMarked(flagClear);
			interfaceCheckBox->setMarked(rightClickInterface);
		}
	}

public:
	OptionsPage(){
		page = "options";

		optionsActionListener = new OptionsActionListener(this);
		
		optionsFont = new Font(ART_FONT, int(48*scaleFactor));
		optionsFont2 = new Font(ART_FONT, int(32*scaleFactor));
		optionsFont2->setColor(31, 26, 85);
		titleLabel = new gcn::Label("options");
		titleLabel->setFont(optionsFont);
		titleLabel->adjustSize();
		titleLabel->setPosition(top->getWidth()/2-titleLabel->getWidth()/2,0);
		top->add(titleLabel);

		MakeCheckBox("(A)utocount", autocount, titleLabel->getBorderSize(), 
					int(titleLabel->getHeight() + 20*scaleFactor), "autocountCheckBox", autocountCheckBox);

		MakeCheckBox("(F)lag Clear", flagClear, autocountCheckBox->getX(), 
					int(autocountCheckBox->getY() + 20*scaleFactor), "flagClearCheckBox", flagClearCheckBox);

		MakeCheckBox("(L)eft-Click Flags", rightClickInterface, flagClearCheckBox->getX(), 
					int(flagClearCheckBox->getY() + 20*scaleFactor), "interfaceCheckBox", interfaceCheckBox);
		
		OKButton = new gcn::Button("OK");
		OKButton->setFont(optionsFont2);
		OKButton->setSize(int(50*scaleFactor),int(50*scaleFactor));  // XXX 33?
		OKButton->setPosition((top->getWidth()-OKButton->getWidth())/2, int(top->getHeight() - 60*scaleFactor));
		OKButton->setActionEventId("OKButton");
		OKButton->addActionListener(optionsActionListener);
		//OKButton->setBaseColor(gcn::Color(162, 175, 242, 150));
		OKButton->setBaseColor(gcn::Color(255, 255, 255, 0));
		OKButton->setBorderSize(5);
		top->add(OKButton);						
	}

	~OptionsPage(){
		delete optionsFont;
		delete optionsFont2;
		delete optionsActionListener;
		delete autocountCheckBox;
		delete flagClearCheckBox;
		delete interfaceCheckBox;
		delete OKButton;
		delete titleLabel;
	}
};

class HighScorePage : public GUIPage {
	class HighScoreActionListener : public gcn::ActionListener
	{
		class HighScorePage* hPage;
	public:
		HighScoreActionListener(HighScorePage* passedPage){
			hPage = passedPage;
		}
		// Implement the action function in ActionListener to recieve actions
		// The eventId tells us which widget called the action function.
		void action(const gcn::ActionEvent& eventId)
		{
			std::string str;
			std::ostringstream os(str);
			
			if(eventId.getId() == "OKButton") {
				hPage->running = false;
			}
		}
	};

protected:
	gcn::Button* OKButton;     // A button for actionlistening
	gcn::Label* titleLabel;
	gcn::Label* scoreEntry[10][3];
	Font* scoreFont;
	Font* scoreFont2;
	Font* scoreFont3;
	LocalHighScoreSheet* scoreSheet;

	HighScoreActionListener* scoreActionListener;

public:
	HighScorePage(){
		page = "score";

		scoreSheet = new LocalHighScoreSheet;

		scoreActionListener = new HighScoreActionListener(this);
		
		scoreFont = new Font(ART_FONT, int(48*scaleFactor));
		scoreFont2 = new Font(ART_FONT, int(32*scaleFactor));
		scoreFont2->setColor(31, 26, 85);
		scoreFont3 = new Font(ART_FONT, int(16*scaleFactor));
		titleLabel = new gcn::Label("high scores");
		titleLabel->setFont(scoreFont);
		titleLabel->adjustSize();
		titleLabel->setPosition(top->getWidth()/2-titleLabel->getWidth()/2,0);
		top->add(titleLabel);

		set<HighScoreEntry> es = scoreSheet->entries();
		int j = 0;
		for (set<HighScoreEntry>::iterator i = es.begin(); i != es.end(); ++i) {
			float ypos = scoreFont->getHeight("H") + scoreFont3->getHeight("I")*1.1*j;
			std::stringstream ss;
			struct tm* timep = localtime(&i->date);
			char date[64];
			strftime(date, 64, "%b %d, %Y", timep);
			// date[strlen(date)-1] = '\0';   // kill the \n terminator
			ss << j+1 << ": " << std::setw(9) << std::left  << i->name << '\0';
			scoreEntry[j][0] = new gcn::Label(ss.str());
			scoreEntry[j][0]->setFont(scoreFont3);
			scoreEntry[j][0]->adjustSize();
			scoreEntry[j][0]->setPosition(3, ypos);
			ss.seekp(0);
			
			ss << std::setw(4) << std::left << i->score << "seconds\0";
			scoreEntry[j][1] = new gcn::Label(ss.str());
			scoreEntry[j][1]->setFont(scoreFont3);
			scoreEntry[j][1]->adjustSize();
			scoreEntry[j][1]->setPosition(top->getWidth()/4, ypos);
			ss.seekp(0);
			
			ss << std::setw(30) << std::left  << date << '\0';
			scoreEntry[j][2] = new gcn::Label(ss.str());
			scoreEntry[j][2]->setFont(scoreFont3);
			scoreEntry[j][2]->adjustSize();
			scoreEntry[j][2]->setPosition(top->getWidth()/2, ypos);
			
			top->add(scoreEntry[j][0]);
			top->add(scoreEntry[j][1]);
			top->add(scoreEntry[j][2]);
			j++;
		}

		OKButton = new gcn::Button("OK");
		OKButton->setFont(scoreFont2);
		OKButton->setSize(int(60*scaleFactor),int(30*scaleFactor));
		OKButton->setPosition((top->getWidth()-OKButton->getWidth())/2, int(top->getHeight() - OKButton->getHeight()));
		OKButton->setActionEventId("OKButton");
		OKButton->addActionListener(scoreActionListener);
		/*if(OKButton->hasMouse()) {
			OKButton->setBaseColor(gcn::Color(162, 175, 242, 150));
		} else {*/
			OKButton->setBaseColor(gcn::Color(255, 255, 255, 0));
		//}
		OKButton->setBorderSize(5);
		top->add(OKButton);						
	}

	~HighScorePage(){
		delete scoreFont;
		delete scoreFont2;
		delete scoreActionListener;
		delete OKButton;
		delete titleLabel;
		delete[] scoreEntry;
		delete scoreSheet;
	}
};

void loadGUIPage(std::string pageName) {
	GUIPage* page;

	if(pageName == "Options"){
		page = new OptionsPage;
	}
	else if(pageName == "HighScores"){
		page = new HighScorePage;
	}
	else return;

	page->run();
}
