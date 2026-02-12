
#ifndef letterinvaders_hpp
#define letterinvaders_hpp

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "basestate.hpp"
#include "eventmanager.hpp"

#include "ballbumper.hpp"
#include "highscore.hpp"


// (Hopefully) temporary disabling of TGUI/high scores on Windows
#if defined(_WIN32) || defined(_WIN64)
	#define WINDOWS
#endif

class LetterInvadersState: public BaseState
{
public:
    LetterInvadersState (StateManager* mgr)
		: BaseState(mgr)
	{ }
    
	void onCreate ();
	
	void onDestroy ();

	void activate ();

    void deactivate ();

	void mouseClick (EventDetails*) { }
	
	void mouseRelease (EventDetails*) { }
	
	void onKeyPress (Keyboard::Key);

	void onEscape (EventDetails*);
	
	void update (const Time&);

	void draw ();

private:
	const string LHLetters 		{ "qwertasdfgzxcvb" };
	const string RHLetters 		{ "yuiophjklnm" };
	const string AllLets 		{ "abcdefghijklmnopqrstuvwxyz" };
	const string AllNums 		{ "1234567890" };
	const string TopRow 		{ "qwertyuiop" };
	const string MiddleRow 		{ "asdfghjkl" };
	const string BottomRow 		{ "zxcvbnm" };
	const string Punctuation 	{ "[]\\;',./=-~" };
	const string AllKeys 		{ AllLets + AllNums + Punctuation };
	static constexpr uint   	initBumperCols = 7;
	static constexpr uint   	initBumperRows = 3;
	static constexpr uint   	wallThickness = 0;
	static constexpr float  	initGrav = .1;
	static constexpr float  	initBumperLoss = .15;
	static constexpr float  	upperDeadZone = 95;
	static constexpr float  	lowerDeadZone = 95;
	static constexpr float  	initSpawnInterval = 6;

	const vector<string> sets {
		AllLets, AllNums, LHLetters, RHLetters, TopRow, MiddleRow,
		BottomRow, Punctuation, AllKeys
	};
	
	const vector<string> setsLabels {
		"Letters", "Numbers", "LH", "RH", "TopRow", "MiddleRow",
		"BottomRow", "Punctuation", "All"
	};
	
	const vector<Keyboard::Key> setsKeys {
		Keyboard::Num1, Keyboard::Num2, Keyboard::Num3, Keyboard::Num4,
		Keyboard::Num5, Keyboard::Num6, Keyboard::Num7, Keyboard::Num8,
		Keyboard::Num9
	};
	
	const vector<Color> ballColors {
		ORANGE, ORANGE50, ORANGE75, MEDORANGE, MEDORANGE50, MEDORANGE75,
		AZURE, AZURE50, AZURE75, MEDAZURE, MEDAZURE50, MEDAZURE75, DKAZURE,
		DKAZURE75, DKAZURE50, PURPLE, PURPLE50, PURPLE75
	};

	void setUpHighScores ();

	void reset ();

	void placeBumpers ();

	void spawnBall (Time);

    void destroyBall (Ball&, Time);

    void endGame ();

    void showHighScores (int, int);

    void enterNewScore (tgui::String);

    string highScoreToText (HighScore&);

    void topTenToFile ();
	
	RenderWindow* renWin ();


	tgui::Gui*                      gui;
    Text                            txt;
    ZSprite                         bkgd;
	Sprite							ground
									, lWall
									, rWall
	;
    Clock                           animClock;

    vector<Bumper>                  bumpers;
    vector<Ball>                    balls;
    vector<HighScore>               topTen;
    vector<tgui::EditBox::Ptr>      eboxes;
    RectangleShape                  beam;
    ZSprite*                        gClickedOn = nullptr;

    string                          curLetterSet;
	string							curSetLabel;
    string                          customLetters {};

    Time                            fadeTime;
    Time                            lastSpawnTime;
    float                           gravAccel;     //.6, 1.3
    float                           bumperLoss;    //.6
    float                           bkgdScaleDegree = 0;
    float                           spawnInterval;
    uint                            numBumperColumns;
    uint                            numBumperRows;
    uint                            curScore;
    uint                            curLevel;
    bool                            drawBeam;
    bool                            running;
    bool                            gameOver;
};

#endif /* letterinvaders_hpp */

