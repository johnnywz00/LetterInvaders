#ifndef SPHERECOLLISION_H
#define SPHERECOLLISION_H

#include "basestate.hpp"
#include "eventmanager.hpp"
#include "ballbumper.hpp"




class SphereCollision: public BaseState {
    
public:
    
    SphereCollision (StateManager* mgr) :
            BaseState(mgr) { }
    
    ~SphereCollision () { }
        
    void activate ();
    
    void deactivate ();
    
    void draw ();
    
    void update (const Time&);
    
    void loadTextures ();
    
    void loadSounds ();
    
    void onCreate ();
    
    void onDestroy ();
    
    void onEscape (EventDetails*);
    
    void mouseClick (EventDetails*);
    
    void mouseRelease (EventDetails*);
    
    void onKeyPress(Keyboard::Key);
    
    void reset ();
    
    
    void getLetters (bool inclNums = false);
    
    void getNumbers ();
    
    void incGrav ();
    
    void decGrav ();
    
    void incLoss ();
    
    void decLoss ();
    
    bool checkBallCollision(float radius1, vecf pos1, Ball&);

    Font			        ballFont;
    Text			        txt;
    Text                    ballLets[5];
    vector<Texture>         texs;
static const vector<string> texList;
 
    Sprite                  bkgd,
                            bkgdFrame;
    Ball                    balls[5];
    
    vector<WheelBumper>     bumpers;
	vector<Block>			blocks;
    vector<string>          curLetters;
    vector<Keyboard::Key>   curKeys;
    ZSprite*                gClickedOn = nullptr;

    static constexpr int    numSounds = 6;
    SoundBuffer             buffers[numSounds];
    Sound                   sounds[numSounds];
static const vector<string> soundFileList;

    float                   ballMasses[5];
    float                   gravAccel = .6;
    float                   bumperLoss = 0.;
    float                   bkgdScaleDegree = 0;
    bool                    running { false };
    bool                    slow { false };

	RenderWindow* renWin () ;
	
	float SCRW () { return renWin()->getView().getSize().x; }
	float SCRH () { return renWin()->getView().getSize().y; }
	float SCRCX () { return renWin()->getView().getSize().x / 2; }
	float SCRCY () { return renWin()->getView().getSize().y / 2; }

};



#endif



