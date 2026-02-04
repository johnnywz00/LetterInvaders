#ifndef JWZ_EVENTMANAGER
#define JWZ_EVENTMANAGER

#include "jwzsfml.hpp"

#define _EMBEDDING
#include "embedded.hpp"
////////////////////////////////////////////////////////////////


/*
enum EventType
{
   Closed, 0
   Resized, 1
   LostFocus, 2
   GainedFocus, 3
   TextEntered, 4
   KeyPressed, 5
   KeyReleased, 6
   MouseWheelMoved, 7
   MouseWheelScrolled, 8
   MouseButtonPressed, 9
   MouseButtonReleased, 10
   MouseMoved, 11
   MouseEntered, 12
   MouseLeft, 13
   JoystickButtonPressed, 14
   JoystickButtonReleased, 15
   JoystickMoved, 16
   JoystickConnected, 17
   JoystickDisconnected, 18
   TouchBegan, 19
   TouchMoved, 20
   TouchEnded, 21
   SensorChanged, 22
   Count
};


    //Keyboard::Key
 Unknown = -1, A = 0, B, C,
  3- D, E, F, G, 
  7- H, I, J, K, 
  11- L, M, N, O, 
  15- P, Q, R, S, 
  19- T, U, V, W, 
  23- X, Y, Z, Num0, 
  27- Num1, Num2, Num3, Num4, 
  31- Num5, Num6, Num7, Num8, 
  35- Num9, Escape, LControl, LShift, 
  39- LAlt, LSystem, RControl, RShift, 
  43- RAlt, RSystem, Menu, LBracket, 
  47- RBracket, Semicolon, Comma, Period, 
  51- Quote, Slash, Backslash, Tilde, 
  55- Equal, Hyphen, Space, Enter, 
  59- Backspace, Tab, PageUp, PageDown, 
  63-  End, Home, Insert, Delete, 
  67- Add, Subtract, Multiply, Divide, 
  71- Left, Right, Up, Down, 
  75- Numpad0, Numpad1, Numpad2, Numpad3, 
  79- Numpad4, Numpad5, Numpad6, Numpad7, 
  83- Numpad8, Numpad9, F1, F2, 
  87- F3, F4, F5, F6, 
  91- F7, F8, F9, F10, 
  95- F11, F12, F13, F14, 
  99- F15, Pause, KeyCount, Dash = Hyphen, 
  BackSpace = Backspace, BackSlash = Backslash, SemiColon = Semicolon, Return = Enter
*/


enum class EventType {
	KeyDown = Event::KeyPressed, 
	KeyUp = Event::KeyReleased, 
	ButtonDown = Event::MouseButtonPressed, 
	ButtonUp = Event::MouseButtonReleased, 
	MouseWheel = Event::MouseWheelMoved, 
	Resize = Event::Resized, 
	GotFocus = Event::GainedFocus, 
	LostFocus = Event::LostFocus, 
	MouseIn = Event::MouseEntered, 
	MouseOut = Event::MouseLeft, 
	Closed = Event::Closed, 
	TextEntered = Event::TextEntered, 
	Keyboard = Event::Count + 1, 
	Mouse, 
	Joystick
};


struct EventInfo {
    
	EventInfo () { code = 0; }
    
	EventInfo (int evt) { code = evt; }
    
	union { int code; };
};


struct EventDetails {
    
	EventDetails (const string& str) : name(str) { clear(); }
    
	void clear () {
		size = vecI(0, 0);
		textEntered = 0;
		mouse = vecI(0, 0);
		wheelDelta = 0;
		keyCode = -1;
    }
    
	string 		name;
	vecI		size;
	Uint32		textEntered;
	vecI		mouse;
	int			wheelDelta;
	int			keyCode;
};
	

using eventVec = vector<pair<EventType, EventInfo>>;


struct Binding {
    
	Binding (const string& str) : name(str), details(str), ct(0) { }
    
	void bindEvent (EventType typ, EventInfo inf = EventInfo()) {
        
		events.emplace_back(typ, inf);
    }
    
	eventVec		events;
	string			name;
	EventDetails	details;
	int				ct;
};
	

enum class StateType;
using bindingTab = unordered_map<string, Binding*>;
using CallbackContainer = unordered_map<string, function<void(EventDetails*)>>;
using callbackTab = unordered_map<StateType, CallbackContainer>;

////////////////////////////////////////////////////////////////


class EventManager {

public:
    
    EventManager ();
    
    ~EventManager ();
    
    bool addBinding (Binding* bdg);
    
    bool removeBinding (string name);
    
    void setFocus (const bool& fcs) { hasFocus = fcs; };
    
    void handleEvent (Event& evt);
    
    void update ();
    
    void setCurrentState (StateType stat) {
        curState = stat;
    }
    
    vecI getMouse (RenderWindow* win = nullptr) {
        return (win ? Mouse::getPosition(*win) : Mouse::getPosition());
    }
    
    template<class T>
    bool addCallback (StateType stat, const string& name,
                      void(T::*func)(EventDetails*), T* inst) {
        
        auto itr = callbacks.emplace(stat, CallbackContainer()).first;
        auto temp = bind(func, inst, std::placeholders::_1);
        return itr->second.emplace(name, temp).second;
    }
    
    bool removeCallback (StateType stat, const string& name) {
        
        auto itr = callbacks.find(stat);
        if (itr == callbacks.end())
            return false;
        auto itr2 = itr->second.find(name);
        if (itr2 == itr->second.end())
            return false;
        itr->second.erase(name);
        return true;
    }
        
    string			bindingsFilename = "bindings.cfg";

private:
    
    void loadBindings ();
    
    bindingTab		bindings;
    callbackTab		callbacks;
    StateType		curState;
    bool			hasFocus;
};

////////////////////////////////////////////////////////////////

	
#endif

