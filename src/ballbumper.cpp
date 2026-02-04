//
//  ballbumper.cpp
//  LetterBallsXC
//
//  Created by John Ziegler on 7/1/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#include "ballbumper.hpp"


Bumper::Bumper (Texture* tx) {

    s.setTexture(*tx);
    s.setScale(2, 2);
    ::cO(s);
}


Ball::Ball (Texture* tx) {

    basicInit();
}

Ball::Ball (Texture* tx, string str, Color c) {

    s.setTexture(*tx);
    basicInit();
    s.setColor(c);
    txt.setString(str);
    if (txt.getString() == "W")
        txt.setCharacterSize(36);
    key = keyMap[txt.getString()];
}

Font gLetFont;

void Ball::basicInit () {

    velocity = vecF(0., 0.);
    isActive = false;
    ::centerOrigin(s);
    s.setPosition(randRange(50, ScrW - 50), -50);
    txt = Text("", gLetFont, 45);
    ::centerOrigin(txt);
    txt.setPosition(s.gP() - vecF(15, 30));
    txt.setOutlineColor(Color::Black);
    txt.setOutlineThickness(3);
    txt.setFillColor(Color::White);
    txt.setStyle(Text::Bold);

    float scale = float(randRange(80, 150)) / 100;
    s.setScale(scale, scale);
    float r = s.gGB().width / 2;
    mass = (4 / 3) * pi * r * r * r;
    blastDestX = float(randRange(0, ScrW + 200) - 200);

    isActive = true;
}


lutab<string, Keyboard::Key> Ball::keyMap {
    
    { "A", Keyboard::A },
    { "B", Keyboard::B },
    { "C", Keyboard::C },
    { "D", Keyboard::D },
    { "E", Keyboard::E },
    { "F", Keyboard::F },
    { "G", Keyboard::G },
    { "H", Keyboard::H },
    { "I", Keyboard::I },
    { "J", Keyboard::J },
    { "K", Keyboard::K },
    { "L", Keyboard::L },
    { "M", Keyboard::M },
    { "N", Keyboard::N },
    { "O", Keyboard::O },
    { "P", Keyboard::P },
    { "Q", Keyboard::Q },
    { "R", Keyboard::R },
    { "S", Keyboard::S },
    { "T", Keyboard::T },
    { "U", Keyboard::U },
    { "V", Keyboard::V },
    { "W", Keyboard::W },
    { "X", Keyboard::X },
    { "Y", Keyboard::Y },
    { "Z", Keyboard::Z },
    { "1", Keyboard::Num1 },
    { "2", Keyboard::Num2 },
    { "3", Keyboard::Num3 },
    { "4", Keyboard::Num4 },
    { "5", Keyboard::Num5 },
    { "6", Keyboard::Num6 },
    { "7", Keyboard::Num7 },
    { "8", Keyboard::Num8 },
    { "9", Keyboard::Num9 },
    { "0", Keyboard::Num0 },
    { ";", Keyboard::Semicolon },
    { "/", Keyboard::Slash },
    { ",", Keyboard::Comma },
    { ".", Keyboard::Period },
	{ "'", Keyboard::Apostrophe },
	{ "[", Keyboard::LBracket },
	{ "]", Keyboard::RBracket },
	{ "\\", Keyboard::Backslash },
	{ "-", Keyboard::Hyphen },
	{ "=", Keyboard::Equal },
	{ "`", Keyboard::Tilde }

};
