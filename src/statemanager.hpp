#ifndef JWZ_STATEMANAGER
#define JWZ_STATEMANAGER

#include "sharedcontext.hpp"

#include "spherecollision.hpp"
#include "letterinvaders.hpp"
#include "mainmenu.hpp"


////////////////////////////////////////////////////////////////

enum class StateType {
    
	SphereCollision = 1,
    MainMenu,
    tLetterInvaders
};
	
using stateVec = vector<pair<StateType, BaseState*>>;
using stateTypeVec = vector<StateType>;
using StateFactory = unordered_map<StateType, function<BaseState* (void)>>;

////////////////////////////////////////////////////////////////


class StateManager {
   
    friend class SFGameWindow;
public:
    
    StateManager (SharedContext* ctx);
    
    ~StateManager ();
    
    void update (const Time& time);
    
    void draw ();
    
    SharedContext* getContext () { return context; }
    
    bool hasState (const StateType& typ);
    
    void switchTo (const StateType& typ);
    
    void remove (const StateType& typ) { toRemove.push_back(typ); }
    
    void processRequests () {
        
        while (toRemove.begin() != toRemove.end()) {
            removeState(*toRemove.begin());
            toRemove.erase(toRemove.begin());
        }
    }
        
private:
    
    void createState (const StateType& typ);
    
    void removeState (const StateType& typ);
    
    template<class T>
    void registerState (const StateType& typ) {
        
        stateFactory[typ] = [this]()->BaseState* { return new T(this); } ;
    }
    
    SharedContext*	context;
    stateVec		states;
    stateTypeVec	toRemove;
    StateFactory	stateFactory;
};



	
#endif
