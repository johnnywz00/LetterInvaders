#include "sfmlApp.hpp"



Game::Game () :
        window("LetterBalls", vecU(1440, 900)),
        stateManager(&context) {
            
    clock.restart();
    srand(unsigned(time(nullptr)));
    context.win = &window;
    context.eventManager = window.getEventManager();
    context.gui = &gui;     // GUI
    window.game = this;
    stateManager.switchTo(StateType::MainMenu);
    
    gui.setTarget(*(window.getRenderWindow()));     // GUI
    tgui::Theme::setDefault("resources/Black.txt"); // GUI
}
 
void Game::update () {
    
    window.update();
    stateManager.update(elapsed);
}

void Game::render () {
    
    window.beginDraw();
    stateManager.draw();
    gui.draw();             // GUI
    window.endDraw();
}

void Game::lateUpdate () {
    
    stateManager.processRequests();
    restartClock();
}



int main (int argc, char **argv) {
 
    /* XCode folly: two instances of the program are launched if
     * we customize the working directory. We can cause the extraneous
     * instance to silently quit immediately by giving it a relative
     * path that it can't find.
     */
    Image img;
    if (!img.loadFromFile(iconPath))
        return EXIT_FAILURE;


	Game game;
	while (!game.getWindow()->isDone()) {
		game.update();
		game.render();
		game.lateUpdate();
    }
}
	
