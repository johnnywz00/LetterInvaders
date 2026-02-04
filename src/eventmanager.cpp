/*
 * This file is mostly copy-paste (with naming changes)
 * from Raimondas Pupius' SFML Game Development by Example
 */

#include "eventmanager.hpp"


EventManager::EventManager () :
        hasFocus(true) {
            
    loadBindings();
}

EventManager::~EventManager () {
    
	for (auto& itr : bindings) {
		delete itr.second;
		itr.second = nullptr;
    }
}

void EventManager::loadBindings () {
    
	string delimiter = ":";
	std::ifstream bindings;
	bindings.open(bindingsFilename);
	if (!bindings.is_open()) {
		cout << "Bindings couldn't be loaded! \n";
		return;
    }
	string line;
	while (std::getline(bindings, line)) {
		std::stringstream keystream(line);
		string callbackName;
		keystream >> callbackName;
		Binding* bind = new Binding(callbackName);
		while (!keystream.eof()) {
			string keyval;
			keystream >> keyval;
			int start = 0;
			int end = int( keyval.find(delimiter) );
			if (end == string::npos) {
				delete bind;
				bind = nullptr;
				break;
            }
			EventType type = EventType(stoi(keyval.substr(start, end-start)));
			int code = stoi(keyval.substr(end + delimiter.length(), 
					keyval.find(delimiter, end + delimiter.length())));
			EventInfo evInf;
			evInf.code = code;
			bind->bindEvent(type, evInf);
        }
		if (!addBinding(bind))
            delete bind;
		bind = nullptr;
    }
	bindings.close();
}
	
bool EventManager::addBinding (Binding* bdg) {
    
	if (bindings.find(bdg->name) != bindings.end())
        return false;
	return bindings.emplace(bdg->name, bdg).second;
}
	
bool EventManager::removeBinding (string name) {
	auto itr = bindings.find(name);
	if (itr == bindings.end())
        return false;
	delete itr->second;
	bindings.erase(itr);
	return true;
}

void EventManager::handleEvent (Event& evt) {
    
	for (auto& bdg : bindings) {
		Binding* bind = bdg.second;
		for (auto& bdgEvt : bind->events) {
			EventType sfmlEvent = (EventType)evt.type;
			if (bdgEvt.first != sfmlEvent)
                continue;
			if (sfmlEvent == EventType::KeyDown ||
					sfmlEvent == EventType::KeyUp) {
				if (bdgEvt.second.code == evt.key.code) {
					if (bind->details.keyCode != -1)
						bind->details.keyCode = bdgEvt.second.code;
					++(bind->ct);
					break;
                }
            }
			else if (sfmlEvent == EventType::ButtonDown ||
					sfmlEvent == EventType::ButtonUp) {
				if (bdgEvt.second.code == evt.mouseButton.button) {
					bind->details.mouse.x = evt.mouseButton.x;
					bind->details.mouse.y = evt.mouseButton.y;
					if (bind->details.keyCode != -1) 
						bind->details.keyCode = bdgEvt.second.code;
					++(bind->ct);
					break;
                }
            }
			else {
				if (sfmlEvent == EventType::MouseWheel)
					bind->details.wheelDelta = evt.mouseWheel.delta;
				else if (sfmlEvent == EventType::Resize) {
					bind->details.size.x = evt.size.width;
					bind->details.size.y = evt.size.height;
                }
				else if (sfmlEvent == EventType::TextEntered) 
					bind->details.textEntered = evt.text.unicode;
				++(bind->ct);
            }
        }
    }
}

void EventManager::update () {
    
	if (!hasFocus)
        return;
	for (auto& bdg : bindings) {
		Binding* bind = bdg.second;
		for (auto& bdgEvt : bind->events) {
			switch (bdgEvt.first) {
                    
				case (EventType::Keyboard):
					if (Keyboard::isKeyPressed( Keyboard::Key(bdgEvt.second.code))) {
						if (bind->details.keyCode != -1)
							bind->details.keyCode = bdgEvt.second.code;
						++(bind->ct);
                    }
					break;
                    
				case (EventType::Mouse):
					if (Mouse::isButtonPressed( Mouse::Button(bdgEvt.second.code))) {
						if (bind->details.keyCode != -1)
							bind->details.keyCode = bdgEvt.second.code;
						++(bind->ct);
                    }
					break;
				default:
                    break;
            }
        }
		if (bind->events.size() == bind->ct) {
			auto stateCallbacks = callbacks.find(curState);
			auto otherCallbacks = callbacks.find(StateType(0));
			if (stateCallbacks != callbacks.end()) {
				auto callItr = stateCallbacks->second.find(bind->name);
				if (callItr != stateCallbacks->second.end())
					callItr->second(&bind->details);
            }
			if (otherCallbacks != callbacks.end()) {
				auto callItr = otherCallbacks->second.find(bind->name);
				if (callItr != otherCallbacks->second.end())
					callItr->second(&bind->details);
            }
        }
		bind->ct = 0;
		bind->details.clear();
    }
}
	
