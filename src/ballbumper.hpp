
#ifndef ballbumper_hpp
#define ballbumper_hpp

#include "vsprite.hpp"
#include "resourcemanager.hpp"



struct WheelBumper: public VSprite
{
    WheelBumper ()
	{
		s.setTexture(gTexture("wheel"));
        rb.setTexture(gTexture("wheel"));
		setup();
    }
    
    void takeHit ()
	{
        ++hits;
        spinRate = hits * 1.25;
		if (flipCoin())
            spinRate *= -1;
        if (hits == 2)
            rb.setColor(Color(110, 110, 255));
        else if (hits == 4)
            rb.setColor(Color::Green);
        else if (hits == 6)
            rb.setColor(Color::Yellow);
        else if (hits == 8)
            rb.setColor(Color(255, 40, 40));
        else if (hits == 9)
            falling = true;
    }
    
    Sprite  rb;
    float   spinRate = 0;
    int     hits = 0;
    bool    falling;
};



struct Bumper: public ZSprite
{
    Bumper ();
};



struct Ball: public VSprite
{
    Ball ();
    
    Ball (string, Color);
	
    void basicInit ();
	
	void swirlExit(Time);

	static unordered_map<string, Keyboard::Key> keyMap;

	Text            txt;
	Time			blastTime;
    Keyboard::Key   key;
    vecF            blastPos;
    float           mass;
    float           blastDestX;
    bool            isActive;
};



struct Block: public ZSprite
{
	Block ()
	{
		setTexture(gTexture("block"));
	}
};


#endif /* ballbumper_hpp */
