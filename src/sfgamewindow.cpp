
/* This file based on work by Raimondas Pupius
 * in SFML Game Development by Example
 */

#include "sfmlApp.hpp"
#include "sfgamewindow.hpp"


SFGameWindow::SFGameWindow ()
{
    setup(defaultTitle, vecU(defaultWidth, defaultHeight));
}

SFGameWindow::SFGameWindow (const string& title, const vecU& size)
{
    setup(title, size);
}
    
void SFGameWindow::update ()
{
	Event event;
	while (window.pollEvent(event)) {
		switch(event.type) {
				
			case Event::KeyPressed:
				switch(event.key.code) {

					default:
						game->stateManager.states.back().second->onKeyPress(event.key.code);
						break;
				}
				break;
				
			case Event::Closed:
				close();
				break;
				
			case Event::LostFocus:
				_isFocused = false;
				eventManager.setFocus(false);
				break;
				
			case Event::GainedFocus:
				_isFocused = true;
				eventManager.setFocus(true);
				break;
				
			default:
				break;
		}
		if (event.type == Event::MouseButtonPressed ||
				event.type == Event::MouseButtonReleased) {
			vecf adj = window.mapPixelToCoords(vecI(event.mouseButton.x,
														   event.mouseButton.y));
			event.mouseButton.x = int(adj.x);
			event.mouseButton.y = int(adj.y);
		}
		if (!(game->gui.handleEvent(event)))
			eventManager.handleEvent(event);
	}
	eventManager.update();
}

void SFGameWindow::create ()
{
	VideoMode mode = VideoMode::getDesktopMode();
	auto style = Style::Default;
	
	if (_isFullscreen) {
		auto fsmodes = VideoMode::getFullscreenModes();
		if (fsmodes.size()) {
			mode = fsmodes[0];
			style = Style::Fullscreen;
		}
		else _isFullscreen = false;
	}
	window.create(mode, windowTitle, style);
	window.setFramerateLimit(60);
}

void SFGameWindow::setup (const string& title, const vecU& size)
{
    windowTitle = title;
    windowSize = size;
    _isFullscreen = true;
    _isDone = false;
    _isFocused = true;
    eventManager.addCallback(StateType(0), "Window_close", &SFGameWindow::close, this);
    create();
	if (!icon.loadFromFile((Resources::executingDir() / "resources" / "images" / "icon.png").string())) {
        cerr << "Couldn\'t load icon! \n";
    }
    else
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}
