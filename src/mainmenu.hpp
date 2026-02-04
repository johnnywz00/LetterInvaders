//
//  mainmenu.hpp
//  LetterBallsXC
//
//  Created by John Ziegler on 6/29/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#ifndef MAINMENU_STATE_H
#define MAINMENU_STATE_H

#include "basestate.hpp"
#include "eventmanager.hpp"

class MainMenuState : public BaseState {
    
public:

    MainMenuState (StateManager* mgr) : BaseState(mgr) { };
    
    ~MainMenuState () { }

    void draw ();

    void update (const Time& time) { };

    void activate ();

    void deactivate () { };
    
    void onKeyPress (Keyboard::Key) { }

    void onCreate ();

    void onDestroy ();

    void mouseClick (EventDetails* det);

    void escQuit (EventDetails* det);

private:

    static constexpr int    numButtons = 3;
    Text                    text;
    vecF                    buttonSize;
    vecF                    buttonPos;
    uint                    buttonPadding;
    RectangleShape          rects[numButtons];
    Text                    labels[numButtons];
    Font                    font,
                            labelFont;

    Texture                 txBkgd;
    Sprite                  bgSprite;
};

#endif

