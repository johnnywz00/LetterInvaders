# LetterInvaders

## A C++/SFML typing game implementing sphere collision physics


### ABOUT THE PROJECT

This program started out as an experiment in programming sphere collision physics, and evolved into a simple game that helped my small children find letters on the keyboard. Its first incarnation was simply a screen in which five colored balls appeared at the top with a single letter printed on each: if the children found and pressed that letter on the keyboard, gravity would kick in and the ball would bounce off the bumpers and around the screen. My 2-year-old son was obsessed with wheels (cars, yes, but wheels *specifically*), so the bumpers became wheels which spin and eventually fall. In the second iteration — as my oldest was about turning five — the gameplay changes so that they have to type a ball's letter before it bounces its way to the floor, and high scores are implemented. 

This game is one of many projects that I undertook in the early days of my programming self-teaching. The projects of this period were motivated primarily by the desire to know if I could "solve" the challenge of bringing this or that mechanic or concept into being through programming language, largely without consulting any other sources to see how it was done. The projects are characterized by a lack of concern for code organization, best practices, readability, or maintainability, because my free time was very limited and my chief objective was the satisfaction of knowing I got the idea to work. Most of the time, when I achieved that much of the goal, I was ready to move on to the next logical challenge rather than pore over the details of making a well-rounded product fit for public consumption. 

I've taken a little time to restructure some things before posting the code here; nonetheless, much about the program remains primitive or suboptimal as it was originally written. Most notably, I hardcoded my graphics to the size of the screen I worked on at the time, and have had to write some extra code to make them display acceptably on different screen sizes. 

### FILE DESCRIPTIONS
* **sfmlApp:**  Implements `main()`, an SFML window and the abstract app

* **zsprite:**  A base class that extends the functionality of SFML's Sprite class
* **vsprite:**  A ZSprite that uses velocity
* **fusemanager:**  An an object to manage timed events like fuses and daemons 
* **jwz:**  C++ utility functions/#defines, shortcuts
* **jwzsfml:**  Like above, but SFML-specific

### BUILDING INSTRUCTIONS
TBA

### TECHNICAL NOTES
I wrote the program on an Intel Mac, where the game is meant to be displayed fullscreen and not in a system window. However, it seems to be confirmed that as of SFML 2.6.1, ARM/Silicon Macs such as the one I am now working on are unable to use fullscreen mode. It sounds like this may be fixed when SFML 2.6.2 is released, but in the meantime, the game will have to run in a system window if being played on an ARM Mac. 
