//
//  mainmenu.cpp
//  LetterBallsXC
//
//  Created by John Ziegler on 6/29/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#include "mainmenu.hpp"
#include "statemanager.hpp"

void MainMenuState::activate () {
    
}

void MainMenuState::onDestroy () {
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->removeCallback(StateType::MainMenu, "Mouse_Left");
    evMgr->removeCallback(StateType::MainMenu, "Key_Escape");
}

void MainMenuState::onCreate () {
    
    stateManager->getContext()->win->adjustViewOnState();
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->addCallback(StateType::MainMenu, "Mouse_Left", 
            &MainMenuState::mouseClick, this);
    evMgr->addCallback(StateType::MainMenu, "Key_Escape", 
            &MainMenuState::escQuit, this);
    
    loadByMethod(txBkgd, "resources/graybkgd.png");
    txBkgd.setRepeated(true);
    bgSprite.setTexture(txBkgd);
    bgSprite.setColor(Color::Yellow);
    bgSprite.setTextureRect(IntRect(0, 0, ScrW, ScrH));
    centerOrigin(bgSprite);
    bgSprite.setPosition(ScrCX, ScrCY);
    
    loadByMethod(font, "resources/TwoForJuanNF.otf");
    loadByMethod(labelFont, "resources/TestarossaNF.otf");
    text.setFont(font);
    text.setString(String("L E T T E R B A L L S!"));
    text.setCharacterSize(100);
    text.setOutlineColor(Color(128, 128, 128));
    text.setOutlineThickness(3);
    centerOrigin(text);
    text.sP(ScrCX, 50);
    
    buttonSize = vecF(500, 55);
    buttonPadding = 15;
    buttonPos = vecF(ScrCX - buttonSize.x / 2 - buttonPadding / 2, ScrCY - 180);
    
    string str[numButtons];
    str[0] = "Letter Balls";
    str[1] = "Letter Invaders";
    // Update `numButtons` if adding
    str[numButtons - 1] = "Exit";
    
    for (int i = 0; i <  numButtons; ++i) {
        vecF bp(i > 8 ? buttonPos.x + buttonSize.x + buttonPadding : buttonPos.x,
            buttonPos.y + (i % 9 * (buttonSize.y + buttonPadding)));
        rects[i].setSize(buttonSize);
        rects[i].setFillColor(Color(132, 164, 224));
        if (i == numButtons - 1) rects[i].setFillColor(Color(225, 165, 71));
        rects[i].setOutlineColor(Color(54, 60, 25));
        rects[i].setOutlineThickness(5);
        rects[i].setOrigin(buttonSize.x / 2., buttonSize.y / 2.);
        rects[i].setPosition(bp);
        labels[i].setFont(labelFont);
        labels[i].setString(String(str[i]));
        labels[i].setCharacterSize(36);
        labels[i].setFillColor(Color(50, 50, 50));
        FloatRect rect = labels[i].gLB();
        labels[i].setOrigin(rect.left + rect.width / 2., rect.top + rect.height / 2.);
        labels[i].sP(bp);
        }
    }

void MainMenuState::mouseClick (EventDetails* det) {
    
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
            ////////
            else if (i == numButtons-1)
                stateManager->getContext()->win->close();
        }
    }
}

void MainMenuState::draw () {
    
    RenderWindow* w = stateManager->getContext()->win->getRenderWindow();
    w->draw(bgSprite);
    w->draw(text);
    for (int i = 0; i <  numButtons; ++i) {
        w->draw(rects[i]);
        w->draw(labels[i]);
    }
}

void MainMenuState::escQuit (EventDetails *det) {
    
    stateManager->getContext()->win->close();
}
