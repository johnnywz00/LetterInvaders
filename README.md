# LetterInvaders

## SFML (C++) typing challenge game for children

### ABOUT THE PROJECT
This game is one of many projects that I undertook in the early days of my programming self-teaching (ca. 2020). The projects of this period were motivated primarily by the desire to know if I could "solve" the challenge of bringing this or that mechanic or concept into being through programming language, largely without consulting any other sources to see how it was done (this was also pre-AI era). The projects were characterized by a lack of concern for code organization, best practices, readability, or maintainability, because my free time was very limited and my chief objective was getting the satisfaction of knowing I got the idea to work. Most of the time, when I achieved that much of the goal, I was ready to move on to the next logical challenge rather than pore over the details of making a well-rounded product fit for public consumption. I've taken a little time to restructure some things before posting the code here; nonetheless, much about the program remains primitive or suboptimal as it was originally written. 

This program was basically just an exercise for myself when I was teaching myself sphere collision physics, and I put it to some use by throwing in the typing elements so my (at the time) 2- and 3-year-old children could start finding letters on the keyboard. The balls' collisions respond to the balls' relative sizes: a fast-moving small ball will only nudge a large one a little bit, etc.

"Letter Balls" is the original iteration; it simply sets a ball bouncing when the child presses its corresponding key. The spinning wheels theme came from my son's obsession with them at the time. 

"Letter Invaders" is a step up in difficulty, where the player has to type the letter shown on a descending ball before it ricochets its way to the bottom of the screen. It has levels of increasing difficulty and high scores. It was tacked on much later (ca. 2024) to accommodate the children's increasing abilities.

The event managing and game state registration are probably a bit of overkill for what this program is, but I was adopting someone else's approach for setting up an abstract SFML app when I was brand-new to GUI programming. I used the TGUI library to facilitate entering and displaying high scores, and the Thor library for its animator.

### FILE DESCRIPTIONS
* **sfmlApp:**  Implements `main()` and the abstract app
* **sfgamewindow:**  Wrapper class for sf::RenderWindow
* **sharedcontext:**  Small class for providing access to key objects without using globals
* **eventmanager:**  Manager class to handle keystrokes, mouse activity, etc.
* **basestate:**  Base class for different game states
* **statemanager:**  Manager class for switching game states
* **mainmenu:**  Opening menu "state"
* **spherecollision:**  Implements primary graphical elements, physics, game logic for the basic "Letter Balls" state
* **letterinvaders:**  Implements primary graphical elements, physics, game logic for the slighty more involved "Letter Invaders" state
* **ballbumper:**  Small structs representing balls, bumpers, etc.
  
(From my "reusable modules" repo: https://github.com/johnnywz00/SFML-shared-headers)
* **jwz:**  C++ utility functions, #defines, shortcuts
* **jwzsfml:**  Like above, but SFML-specific
* **zsprite:**  Wrapper class for SFML sprite with many extra methods, particularly related to collision checking
* **vsprite:**  Subclass of ZSprite that uses velocity
* **resourcemanager:**  Static class for accessing resource files globally
* **highscore:**  Struct representing data for one high score entry

### BUILDING INSTRUCTIONS
Ready-made program files are available on the Releases page of this repository, with versions for MacOS, Windows, and Linux. If your OS isn't supported by the pre-made versions, or if you have other reasons for building from source:
- Clone this repository, and navigate to the root folder of that clone in a terminal window.
- Run:
<pre>
   cmake -B build
   cmake --build build
</pre>
