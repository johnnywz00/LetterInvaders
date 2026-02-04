//
//  letterinvaders.hpp
//  LetterBallsXC
//
//  Created by John Ziegler on 6/29/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#ifndef letterinvaders_hpp
#define letterinvaders_hpp

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <Thor/Animations.hpp>
#include <SFML/Audio.hpp>
#include "basestate.hpp"
#include "eventmanager.hpp"

#include "ballbumper.hpp"
#include "highscore.hpp"


inline Font gLetFont;

class LetterInvadersState: public BaseState {
    
public:

    LetterInvadersState (StateManager* mgr) :
            BaseState(mgr) { }
    
    ~LetterInvadersState () ;

    void activate () ;

    void deactivate () ;

    void draw ();

    void update (const Time&);

    void loadTextures ();
    
    void loadSounds();
    
    void onCreate ();

    void onDestroy ();

    void onEscape (EventDetails*);

    void mouseClick (EventDetails*);

    void mouseRelease (EventDetails*);
    
    void onKeyPress (Keyboard::Key);

    void reset ();

    void setUpHighScores ();

    void spawnBall (Time);

    void placeBumpers ();

    void destroyBall (Ball&);

    void endGame ();

    void showHighScores (int, int);

    void enterNewScore (tgui::String);

    string highScoreToText (HighScore&);

    void topTenToFile ();

    enum LetterGroup { All, LH, RH, Letters, Numbers, Custom };
    
    Font                            font;
    Text                            txt;
    ZSprite                         bkgd;
    Sprite                          bkgdFrame;
    vector<Texture>                 texs;
    static const vector<string>     texList;
    tgui::Gui*                      gui;
    thor::Animator<Ball, string>    animator;
    Clock                           animClock;

    vector<Bumper>                  bumpers;
    vector<Ball>                    balls;
    vector<HighScore>               topTen;
    vector<tgui::EditBox::Ptr>      eboxes;
    RectangleShape                  beam;
    ZSprite*                        gClickedOn = nullptr;

    LetterGroup                     group = All;
    string                          curLetterSet;
	string							curSetLabel;
    string                          customLetters {};
    
    static constexpr int            numSounds = 6;
    SoundBuffer                     buffers[numSounds];
    Sound                           sounds[numSounds];
    static const vector<string>     soundFileList;
	
	SoundBuffer						levelUpBuf;
	Sound							levelUpSnd;

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


const string LHLetters { "qwertasdfgzxcvb" };
const string RHLetters { "yuiophjklnm" };
const string AllLets { "abcdefghijklmnopqrstuvwxyz" };
const string AllNums { "1234567890" };
const string TopRow { "qwertyuiop"};
const string MiddleRow { "asdfghjkl"};
const string BottomRow { "zxcvbnm"};
const string Punctuation { "[]\\;',./=-~"};
const string All = AllLets + AllNums + Punctuation;

const vector<string> sets {
	AllLets, AllNums, LHLetters, RHLetters, TopRow, MiddleRow,
	BottomRow, Punctuation, All
};
const vector<string> setsLabels {
	"Letters", "Numbers", "LH", "RH", "TopRow", "MiddleRow",
	"BottomRow", "Punctuation", "All"
};

const vector<Color> ballColors {
    ORANGE, ORANGE50, ORANGE75, MEDORANGE, MEDORANGE50, MEDORANGE75, AZURE, AZURE50,
    AZURE75, MEDAZURE, MEDAZURE50, MEDAZURE75, DKAZURE, DKAZURE75, DKAZURE50, PURPLE,
    PURPLE50, PURPLE75
};


constexpr float     initGrav = .12;
constexpr float     initBumperLoss = .15;
constexpr uint      initBumperCols = 7;
constexpr uint      initBumperRows = 3;
constexpr uint      wallThickness = 0;
constexpr float     upperDeadZone = 95;
constexpr float     lowerDeadZone = 95;
constexpr float     initSpawnInterval = 6.;


#endif /* letterinvaders_hpp */

