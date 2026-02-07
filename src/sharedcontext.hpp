
/* This file based on work by Raimondas Pupius
 * in SFML Game Development by Example
 */

#ifndef SHARED_CONTEXT_H
#define SHARED_CONTEXT_H

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "sfgamewindow.hpp"

struct SharedContext
{
	SharedContext ()
		: win(nullptr)
        , eventManager(nullptr)
	{ }
    
	SFGameWindow* 	win;
	EventManager* 	eventManager;
    tgui::Gui*      gui;
};

#endif
