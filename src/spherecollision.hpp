#ifndef SPHERECOLLISION_H
#define SPHERECOLLISION_H

#include "basestate.hpp"
#include "eventmanager.hpp"
#include "ballbumper.hpp"



class SphereCollision: public BaseState
{
public:
    SphereCollision (StateManager* mgr)
		: BaseState(mgr)
	{ }
            
	void onCreate ();
	
	void onDestroy ();
	
	void activate () { }
    
    void deactivate ();
    
    void update (const Time&);
    
    void mouseClick (EventDetails*);
    
    void mouseRelease (EventDetails*);
    
    void onKeyPress(Keyboard::Key);
    
	void onEscape (EventDetails*);
	
	void draw ();
	
private:
	const vector<Color> colorsVec {
		Color(255, 20, 20),
		Color(20, 255, 20),
		Color(100, 100, 255),
		Color(255, 255, 20),
		Color(255, 20, 255)
	};
	
	const vector<string> lettersVec {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
		"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
	};
	
	const vector<Keyboard::Key> letKeysVec {
		Keyboard::A, Keyboard::B, Keyboard::C, Keyboard::D, Keyboard::E, Keyboard::F,
		Keyboard::G, Keyboard::H, Keyboard::I, Keyboard::J, Keyboard::K, Keyboard::L,
		Keyboard::M, Keyboard::N, Keyboard::O, Keyboard::P, Keyboard::Q, Keyboard::R,
		Keyboard::S, Keyboard::T, Keyboard::U, Keyboard::V, Keyboard::W, Keyboard::X,
		Keyboard::Y, Keyboard::Z
	};
	
	const vector<string> numbersVec {
		"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
	};
	
	vector<Keyboard::Key> numKeysVec {
		Keyboard::Num1, Keyboard::Num2, Keyboard::Num3, Keyboard::Num4,
		Keyboard::Num5, Keyboard::Num6, Keyboard::Num7, Keyboard::Num8,
		Keyboard::Num9, Keyboard::Num0
	};
	
	void reset ();
    
    void getLetters (bool inclNums = false);
    
    void getNumbers ();
    
    void incGrav ();
    
    void decGrav ();
    
    void incLoss ();
    
    void decLoss ();
    
    bool checkBallCollision(float radius1, vecf pos1, Ball&);

	RenderWindow* renWin ();

	float SCRW () { return renWin()->getDefaultView().getSize().x; }
	float SCRH () { return renWin()->getDefaultView().getSize().y; }
	float SCRCX () { return renWin()->getDefaultView().getSize().x / 2; }
	float SCRCY () { return renWin()->getDefaultView().getSize().y / 2; }
	
	Sprite                  bkgd;
	Ball                    balls[5];
	vector<WheelBumper>     bumpers;
	vector<Block>			blocks;
    Text			        txt;
    Text                    ballLets[5];
     
	vector<string>          curLetters;
    vector<Keyboard::Key>   curKeys;
    WheelBumper*            gClickedOn = nullptr;

    float                   ballMasses[5];
    float                   gravAccel = .6;
    float                   bumperLoss = 0.;
    float                   bkgdScaleDegree = 0;
    bool                    running { false };
    bool                    slow { false };
};

#endif



