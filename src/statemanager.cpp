#include "statemanager.hpp"

StateManager::StateManager(SharedContext* ctx) :
        context(ctx) {
            
    registerState<MainMenuState>(StateType::MainMenu);
	registerState<SphereCollision>(StateType::SphereCollision);
    registerState<LetterInvadersState>(StateType::tLetterInvaders);
}
	
StateManager::~StateManager () {
    
	for (auto& itr : states) {
		itr.second->onDestroy();
		delete itr.second;
    }
}
	
void StateManager::draw () {
    
	if (states.empty())
        return;
	if (states.back().second->isTransparent() && states.size() > 1) {
		auto itr = states.end();
		while (itr != states.begin()) {
			if (itr != states.end()) {
				if (!itr->second->isTransparent())
                    break;
            }
			--itr;
        }
		for ( ; itr != states.end(); ++itr) {
			context->win->getRenderWindow()->setView(itr->second->getView());
			itr->second->draw();
        }
    }
	else  states.back().second->draw();
}
	
void StateManager::update (const Time& time) {
    
	if (states.empty())
        return;
	if (states.back().second->isTranscendent() && states.size() > 1) {
		auto itr = states.end();
		while (itr != states.begin()) {
			if (itr != states.end()) {
				if (!itr->second->isTranscendent())
                    break;
            }
			--itr;
        }
		for ( ; itr != states.end(); ++itr)
            itr->second->update(time);
    }
	else  states.back().second->update(time);
}
	
bool StateManager::hasState (const StateType& typ) {
    
	for (auto itr = states.begin(); itr != states.end(); ++itr) {
		if (itr->first == typ) {
			auto removed = std::find(toRemove.begin(), toRemove.end(), typ);
			if (removed == toRemove.end())
                return true;
			return false;
        }
    }
	return false;
}
	
void StateManager::switchTo (const StateType& typ) {
    
	context->eventManager->setCurrentState(typ);
	for (auto itr = states.begin(); itr != states.end(); ++itr) {
		if (itr->first == typ) {
			states.back().second->deactivate();
			StateType tmpTyp = itr->first;
			BaseState* tmpStat = itr->second;
			states.erase(itr);
			states.emplace_back(tmpTyp, tmpStat);
			tmpStat->activate();
			//context->win->getRenderWindow()->setView(tmpStat->getView());
			return;
        }
    }
	if (!states.empty())
        states.back().second->deactivate();
	createState(typ);
	states.back().second->activate();
	//context->win->getRenderWindow()->setView(states.back().second->getView());
}
	
void StateManager::createState (const StateType& typ) {
    
    auto newState = stateFactory.find(typ);
    if (newState == stateFactory.end())
        return;
    BaseState* state = newState->second();
    state->view = context->win->getRenderWindow()->getDefaultView();
    states.emplace_back(typ, state);
    state->onCreate();
}
	
void StateManager::removeState (const StateType& typ) {
    
	for (auto itr = states.begin(); itr != states.end(); ++itr) {
		if (itr->first == typ) {
			itr->second->onDestroy();
			delete itr->second;
			states.erase(itr);
			return;
        }
    }
}
