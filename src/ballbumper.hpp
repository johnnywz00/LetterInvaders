//
//  ballbumper.hpp
//  LetterBallsXC
//
//  Created by John Ziegler on 7/1/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#ifndef ballbumper_hpp
#define ballbumper_hpp

#include "vsprite.hpp"


struct WheelBumper: public VSprite {

    WheelBumper () { setup(); };
    
    WheelBumper (const Texture& texture) {
        
        s.setTexture(texture);
        rb.setTexture(texture); setup();
    }
    
    void takeHit () {
        ++hits;
        spinRate = hits * 1.25;
        if (randRange(0, 10) < 5)
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



struct Bumper: public ZSprite {

    Bumper (Texture*);
};



struct Ball: public VSprite {

    Ball () { }

    Ball (Texture*) ;
    
    Ball (Texture*, string, Color);
    
    void basicInit () ;

    static lutab<string, Keyboard::Key> keyMap;

    Text            txt;
    Keyboard::Key   key;
    vecf            blastPos {};
    float           mass;
    float           blastDestX;
    bool            isActive;
};



struct Block: public ZSprite
{
	Block (const Texture& tex)
	{
		setTexture(tex);
	}
};


#endif /* ballbumper_hpp */
