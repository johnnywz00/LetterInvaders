//
//  mainmenu.hpp
//  LetterBallsXC
//
//  Created by John Ziegler on 6/29/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

/*
 TO DO:
 
 -music
 -bouncing balls in menu background
 -letter set marked in high score
 
 */

#ifndef MAINMENU_STATE_H
#define MAINMENU_STATE_H

#include "basestate.hpp"
#include "eventmanager.hpp"

class MainMenuState : public BaseState
{
public:
    MainMenuState (StateManager* mgr)
		: BaseState(mgr)
	{ }
    
	void onCreate ();
	
	void onDestroy ();
	
	void activate () { }
	
	void deactivate () { }
	
	void mouseClick (EventDetails* det);
	
	void onKeyPress (Keyboard::Key);

	void escQuit (EventDetails* det);
	
	void update (const Time& time) { }
	
	void draw ();

	/* Hack for my children to each have their own high scores */
	inline static string playerName;
	
private:
	RenderWindow* renWin ();
	
	float SCRW () { return renWin()->getDefaultView().getSize().x; }
	float SCRH () { return renWin()->getDefaultView().getSize().y; }
	float SCRCX () { return renWin()->getDefaultView().getSize().x / 2; }
	float SCRCY () { return renWin()->getDefaultView().getSize().y / 2; }

    static constexpr int    numButtons = 3;
	static const string		instrucsStr;
	
	Sprite                  bgSprite;
	RectangleShape          rects[numButtons];
    Text                    text;
	Text                    labels[numButtons];
	Text					whichPlayer;
	Text					instrucs;

	vecF                    buttonSize;
    vecF                    buttonPos;
    uint                    buttonPadding;
};

#endif

