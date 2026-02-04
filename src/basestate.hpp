#ifndef BASE_STATE_H
#define BASE_STATE_H

#include <SFML/Graphics.hpp>

using namespace sf;

class StateManager;


class BaseState {
    
	friend class StateManager;
public:
    
    BaseState (StateManager* mgr) :
        stateManager(mgr),
        transparent(false),
        transcendent(false) { }
    
    virtual ~BaseState () { };
    
    virtual void onCreate () = 0;
    
    virtual void onDestroy () = 0;
    
    virtual void activate () = 0;
    
    virtual void deactivate () = 0;
    
    virtual void update (const Time& time) = 0;
    
    virtual void draw () = 0;
    
    virtual void onKeyPress (Keyboard::Key) = 0;
    
    void setTransparent (const bool& trans) { transparent = trans; }
    
    bool isTransparent () const{ return transparent; }
    
    void setTranscendent (const bool& trans) { transcendent = trans; }
    
    bool isTranscendent () const{ return transcendent; }
    
    StateManager* getStateManager () { return stateManager; }
    
    View& getView () { return view; }

protected:
    
    StateManager*	stateManager;
    View			view;
    bool 			transparent,
                    transcendent;
};
	
	
#endif
