
/* This file based on work by Raimondas Pupius
 * in SFML Game Development by Example
 */

#ifndef JWZ_GAMEWINDOW
#define JWZ_GAMEWINDOW


#include "jwzsfml.hpp"
#include "eventmanager.hpp"


inline const string defaultTitle { "LetterInvaders" };
inline const string iconPath { "resources/images/icon.png" };


class Game;

class SFGameWindow
{
    friend class Game;
public:
    SFGameWindow ();

    SFGameWindow (const string& title, const vecU& size);
    
    ~SFGameWindow () { destroy(); }
    
    void update ();
    
    void draw (Drawable& d) { window.draw(d); }
    
    void beginDraw () { window.clear(redrawColor); }
    
    void endDraw () { window.display(); }
    
    bool isDone () { return _isDone; }
    
    bool isFullscreen () { return _isFullscreen; }

    bool isFocused () { return _isFocused; }
    
    vecU getWindowSize () { return windowSize; };
    
    RenderWindow* getRenderWindow () { return &window; };
    
    EventManager* getEventManager() { return &eventManager; };
    
    void close (EventDetails* det = nullptr) { _isDone = true; };

    
	Game*                   game;
    
private:
    
	void create ();
	
	void setup (const string& title, const vecU& size);
	
    void destroy () { window.close(); };
    
    Image                   icon;
    RenderWindow            window;
    Color                   redrawColor { Color::Black };
    vecU                    windowSize;
    static const int        defaultWidth { 1280 };
    static const int        defaultHeight { 720 };
    string                  windowTitle;
    EventManager            eventManager;

    bool                    _isDone;
    bool                    _isFullscreen;
    bool                    _isFocused;
    bool                    _isStretched;
};

	
#endif	


