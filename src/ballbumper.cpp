
#include "ballbumper.hpp"


Bumper::Bumper ()
{
	s.setTexture(gTexture("bumper"));
    s.setScale({2, 2});
    ::centerOrigin(s);
}


Ball::Ball ()
{
    basicInit();
}

Ball::Ball (string str, Color c)
{
    s.setTexture(gTexture("ball"));
	s.setColor(c);
    basicInit();
    txt.setString(str);
    if (txt.getString() == "W")
		/* A hack to keep 'W' more centered on the ball */
        txt.setCharacterSize(36);
    key = keyMap[txt.getString()];
}

void Ball::basicInit ()
{
	auto scrWid = VideoMode::getDesktopMode().width;
	isActive = true;
	float scale = float(randRange(80, 150)) / 100;
	s.setScale(scale, scale);
	centerOrigin();
    s.setPosition(randRange(50, scrWid - 50), -50);
	float r = s.gGB().width / 2;
	mass = (4 / 3) * pi * r * r * r;
	blastDestX = float(randRange(0, scrWid + 200) - 200);
	
    txt = Text("", gFont("ball"), 45);
    txt.setOutlineColor(Color::Black);
    txt.setOutlineThickness(3);
    txt.setFillColor(Color::White);
    txt.setStyle(Text::Bold);
	// Ball starts out of screen and update() will set txt's position
}


void Ball::swirlExit (Time tm)
{
	float p = (tm - blastTime).asSeconds();
	vecf dest {blastDestX, -100};
	vecf vecDif = dest - blastPos;
	float hypot = hyp(vecDif);
	float radius = 80;
	float ratio = radius / hypot;
	vecf offset = vecDif * ratio;
	vecf initAxis = blastPos + offset;
	vecDif = dest - initAxis;
	vecf axis = initAxis + vecDif * p;
	vecf offs2 {blastPos - initAxis};
	float initDegs = toPolar(offs2).y;
	float degs = initDegs + 360 * (fmod(p, 5) / .2);
	vecf v = toRect(radius, degs);
	s.sP(axis + v);
}

unordered_map<string, Keyboard::Key> Ball::keyMap
{
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
