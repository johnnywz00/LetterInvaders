//
//  mainmenu.cpp
//  LetterBallsXC
//
//  Created by John Ziegler on 6/29/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#include "mainmenu.hpp"
#include "statemanager.hpp"

const string MainMenuState::instrucsStr =
"\tLETTER BALLS:\n"
"Just push the keys printed on the balls and watch them bounce!\n"
"SPACE BAR- Resets the balls, using only alphabetic characters.\n"
"ALT/OPTION + SPACE BAR- Resets, using only numbers.\n"
"SHIFT + SPACE BAR- Resets with alphabetic and numeric characters.\n"
"ESCAPE- Returns to the menu.\n"
"-You can twiddle the gravity strength and the collision velocity loss\n"
" by holding SHIFT and pushing 1-4.\n"
"-The wheels can be mouse-dragged around if desired.\n"
" \n \n"
"\tLETTER INVADERS:\n"
"Type the character on the falling ball before it hits the ground!\n"
"SPACE BAR- Starts or pauses the game.\n"
"Y- Resets without immediately starting. While still paused, you can\n"
"   choose which character group to work on by pressing a number key\n"
"   from 1-9.\n"
"-If you get a high score, type your name and hit Enter/Return; then\n"
" hit Y to play again, or Escape to return to the menu."
" \n \n \n\t\t\tJohn Ziegler, 2020-2024\n\t\t\tjohnnywz00@yahoo.com"
;

void MainMenuState::onCreate () {
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->addCallback(StateType::MainMenu, "Mouse_Left", 
            &MainMenuState::mouseClick, this);
    evMgr->addCallback(StateType::MainMenu, "Key_Escape", 
            &MainMenuState::escQuit, this);
    
	bgSprite.setTexture(gTexture("bkgd"));
	bgSprite.setColor(ORANGE);
    bgSprite.setTextureRect(IntRect(0, 0, scrw, scrh));
    centerOrigin(bgSprite);
    bgSprite.setPosition(scrcx, scrcy);
    
    text.setFont(gFont("menuTitle"));
    text.setString(String("L E T T E R B A L L S!"));
    text.setCharacterSize(120);
    text.setOutlineColor(Color(128, 128, 128));
    text.setOutlineThickness(3);
    centerOrigin(text);
    text.sP(scrcx, 200);
	
	whichPlayer.setFont(gFont("menuButton"));
	whichPlayer.setCharacterSize(40);
	whichPlayer.setFillColor(Color(128, 128, 128));
	centerOrigin(whichPlayer);
	whichPlayer.sP(scrcx, scrh - 100);
	
	instrucs.setFont(gFont("ball"));
	instrucs.setCharacterSize(20);
	instrucs.setFillColor(Color(40, 40, 50));
	instrucs.sP(scrcx + 100, 400);
	instrucs.setString(instrucsStr);
    
    buttonSize = vecF(500, 55);
    buttonPadding = 15;
    buttonPos = vecF(scrcx - buttonSize.x / 2 - buttonPadding / 2, scrcy - 80);
    
    string str[numButtons];
    str[0] = "Letter Balls";
    str[1] = "Letter Invaders";
    // Update `numButtons` and mouseClick if adding
    str[numButtons - 1] = "Exit";
    
    for (int i = 0; i < numButtons; ++i) {
        vecF bp(i > 8 ? buttonPos.x + buttonSize.x + buttonPadding : buttonPos.x,
            buttonPos.y + (i % 9 * (buttonSize.y + buttonPadding)));
        rects[i].setSize(buttonSize);
        rects[i].setFillColor(Color(100, 120, 224));
        if (i == numButtons - 1)
			rects[i].setFillColor(Color(225, 165, 71));
        rects[i].setOutlineColor(Color(54, 60, 25));
        rects[i].setOutlineThickness(5);
        rects[i].setOrigin(buttonSize.x / 2., buttonSize.y / 2.);
        rects[i].setPosition(bp);
        labels[i].setFont(gFont("menuButton"));
        labels[i].setString(String(str[i]));
        labels[i].setCharacterSize(36);
        labels[i].setFillColor(Color(50, 50, 50));
        FloatRect rect = labels[i].gLB();
        labels[i].setOrigin(rect.left + rect.width / 2., rect.top + rect.height / 2.);
        labels[i].sP(bp);
        }
    }

void MainMenuState::onDestroy ()
{
	EventManager* evMgr = stateManager->getContext()->eventManager;
	evMgr->removeCallback(StateType::MainMenu, "Mouse_Left");
	evMgr->removeCallback(StateType::MainMenu, "Key_Escape");
}

void MainMenuState::mouseClick (EventDetails* det)
{
    vecI mp = det->mouse;
    float halfx = buttonSize.x / 2.;
    float halfy = buttonSize.y / 2.;
    for (int i = 0; i <  numButtons; ++i) {
        if (mp.x >= rects[i].getPosition().x - halfx &&
                mp.x <= rects[i].getPosition().x + halfx &&
                mp.y >= rects[i].getPosition().y - halfy &&
                mp.y <= rects[i].getPosition().y + halfy) {
            if (i == 0)
                stateManager->switchTo(StateType::SphereCollision);
            else if (i == 1)
                stateManager->switchTo(StateType::tLetterInvaders);
            else if (i == numButtons-1)
                stateManager->getContext()->win->close();
        }
    }
}

void MainMenuState::onKeyPress (Keyboard::Key k)
{
	/* Personal use: children can have their own high scores files */
	if (k == Keyboard::Key::R) {
		MainMenuState::playerName = "Ravenna";
		whichPlayer.setString("Ravenna");
	}
	else if (k == Keyboard::Key::L) {
		MainMenuState::playerName = "Laith";
		whichPlayer.setString("Laith");
	}
	else {
		MainMenuState::playerName = "";
		whichPlayer.setString("");
	}
}

void MainMenuState::escQuit (EventDetails *det)
{
	stateManager->getContext()->win->close();
}

void MainMenuState::draw ()
{
    RenderWindow* w = stateManager->getContext()->win->getRenderWindow();
    w->draw(bgSprite);
    w->draw(text);
	w->draw(whichPlayer);
	w->draw(instrucs);
    for (int i = 0; i < numButtons; ++i) {
        w->draw(rects[i]);
        w->draw(labels[i]);
    }
}

RenderWindow* MainMenuState::renWin ()
{
	return stateManager->getContext()->win->getRenderWindow();
}

