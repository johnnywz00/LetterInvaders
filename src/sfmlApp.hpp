#ifndef LETTERINVADERSAPP_HPP
#define LETTERINVADERSAPP_HPP


#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "statemanager.hpp"



class Game
{
public:
    Game ();
    
    ~Game () { }
    
    void update ();
    
    void render ();
    
    void lateUpdate ();
    
    SFGameWindow* getWindow () { return &window; };
    
    Time getElapsed () { return elapsed; };
    
    void restartClock () { elapsed += clock.restart(); };

protected:
	friend class SFGameWindow;
    SFGameWindow 	    window;
    tgui::Gui           gui;
    StateManager        stateManager;
    Clock			    clock;
    Time			    elapsed;
    SharedContext	    context;
};

#endif
