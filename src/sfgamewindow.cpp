#include "sfmlApp.hpp"
#include "sfgamewindow.hpp"


SFGameWindow::SFGameWindow () {
    
    setup(defaultTitle, vecU(defaultWidth, defaultHeight));
}

SFGameWindow::SFGameWindow (const string& title, const vecU& size) {
    
    setup(title, size);
}
    
void SFGameWindow::setup (const string& title, const vecU& size) {
    
    windowTitle = title;
    windowSize = size;
    _isFullscreen = true;
    _isStretched = false;
    _isDone = false;
    _isFocused = true;
    eventManager.addCallback(StateType(0), "Window_close", &SFGameWindow::close, this);
    eventManager.addCallback(StateType(0), "Fullscreen_toggle", &SFGameWindow::toggleFullscreen, this);
    create();
    if (!loadByMethod(icon, iconPath)) {
        cerr << "Couldn\'t load icon! \n";
    }
    else
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

void SFGameWindow::create () {
    
    VideoMode mode = VideoMode::getDesktopMode();
    auto style = Style::Default;
    
        /* As of SFML 2.6.1, fullscreen modes were not supported on ARM
         * Macs. We need to perform a check before unquestioningly switching
         * to fullscreen mode.
         */
    if (_isFullscreen) {
        auto fsmodes = VideoMode::getFullscreenModes();
        if (fsmodes.size()) {
            // ** Will we ever need to select a mode other than [0]?
            mode = fsmodes[0];
            style = Style::Fullscreen;
        }
        else _isFullscreen = false;
    }
    window.create(mode, windowTitle, style);
    View v { window.getDefaultView() };
    screenOffsetFrom1440x900 = vecf(-(v.getSize().x - 1440) / 2,
                                    -(v.getSize().y - 900) / 2);
    window.setFramerateLimit(60);
    setToggledView(_isStretched);
}
    
void SFGameWindow::toggleFullscreen (EventDetails* d) {
    
    _isFullscreen = !_isFullscreen;
    destroy();
    create();
}

void SFGameWindow::toggleStretchGraphics () {
    
    _isStretched = !_isStretched;
    setToggledView(_isStretched);
}

void SFGameWindow::setToggledView (bool stretched) {
    
    View v { FloatRect(0, 0, 1440, 900) };
    if (!stretched) {
        v = window.getDefaultView();
        v.move(screenOffsetFrom1440x900);
    }
    window.setView(v);
}

void SFGameWindow::adjustViewOnState () {
    
    setToggledView(_isStretched);
}

void SFGameWindow::update () {
    
	Event event;
	while (window.pollEvent(event)) {
        switch(event.type) {
                
            case Event::KeyPressed:
                switch(event.key.code) {
                    case Keyboard::F6:
                        toggleStretchGraphics();
                        break;
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
	
