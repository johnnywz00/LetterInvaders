#include "spherecollision.hpp"
#include "statemanager.hpp"


RenderWindow* SphereCollision::renWin ()
{
	return stateManager->getContext()->win->getRenderWindow();
}

void SphereCollision::activate () {
    
}

void SphereCollision::deactivate () {
    
    reset();
}

void SphereCollision::getLetters (bool inclNums) {
    
	int sz = inclNums ? 36 : 26;  //36 if including nums
    vector<string> bs {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
    };
    vector<Keyboard::Key> v {
        Keyboard::A, Keyboard::B, Keyboard::C, Keyboard::D, Keyboard::E, Keyboard::F, Keyboard::G, Keyboard::H,
        Keyboard::I, Keyboard::J, Keyboard::K, Keyboard::L, Keyboard::M, Keyboard::N, Keyboard::O, Keyboard::P,
        Keyboard::Q, Keyboard::R, Keyboard::S, Keyboard::T, Keyboard::U, Keyboard::V, Keyboard::W, Keyboard::X,
        Keyboard::Y, Keyboard::Z
    };
	if (inclNums) {
		vector<string> nv {
			"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
		};
		vector<Keyboard::Key> kv {
			Keyboard::Num1, Keyboard::Num2, Keyboard::Num3, Keyboard::Num4, Keyboard::Num5,
			Keyboard::Num6, Keyboard::Num7, Keyboard::Num8, Keyboard::Num9, Keyboard::Num0
		};
		bs = vecPlusVec(bs, nv);
		v = vecPlusVec(v, kv);
	}
	curLetters.clear();
	curKeys.clear();
    while (curLetters.size() != 5) {
        int idx = randRange(0, sz - 1);
        string cand = bs[idx];
        if (std::find(curLetters.begin(), curLetters.end(), cand) == curLetters.end()) {
            curLetters.push_back(cand);
            curKeys.push_back(v[idx]);
        }
    }
}

void SphereCollision::getNumbers () {
    
	string bs[10] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };
    Keyboard::Key v[] = {
        Keyboard::Num1, Keyboard::Num2, Keyboard::Num3, Keyboard::Num4, Keyboard::Num5,
        Keyboard::Num6, Keyboard::Num7, Keyboard::Num8, Keyboard::Num9, Keyboard::Num0
    };
	curLetters.clear();
	curKeys.clear();
	while (curLetters.size() != 5) {
		int idx = randRange(0, 9);
		string cand = bs[idx];
		if (std::find(curLetters.begin(), curLetters.end(), cand) == curLetters.end()) {
			curLetters.push_back(cand); 
			curKeys.push_back(v[idx]);
        }
    }
}

void SphereCollision::onEscape (EventDetails* det) {
    
    stateManager->switchTo(StateType::MainMenu);
}

void SphereCollision::onDestroy () {
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->removeCallback(StateType::SphereCollision, "Mouse_Left");
    evMgr->removeCallback(StateType::SphereCollision, "Mouse_Left_Up");
    evMgr->removeCallback(StateType::SphereCollision, "Key_Escape");
}

void SphereCollision::loadTextures () {
    
    texs.clear();
    Texture tex;
    string basePath = "resources/";
    int sz = int(texList.size());
    for (int i = 0; i <  sz ; ++i) {
        string fileName = basePath + texList[i];
//        loadByMethod(tex, fileName);
		tex.loadFromFile(fileName);
        texs.push_back(tex);
    }
    texs[0].setRepeated(true);
}

void SphereCollision::loadSounds () {
    
    string path = "resources/";
    forNum(numSounds) {
        loadByMethod(buffers[i], path + soundFileList[i]);
        sounds[i].setBuffer(buffers[i]);
    }
    sounds[0].setVolume(25);
}

void SphereCollision::onCreate () {
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->addCallback(StateType::SphereCollision, "Mouse_Left",
                       &SphereCollision::mouseClick, this);
    evMgr->addCallback(StateType::SphereCollision, "Mouse_Left_Up",
                       &SphereCollision::mouseRelease, this);
    evMgr->addCallback(StateType::SphereCollision, "Key_Escape",
                       &SphereCollision::onEscape, this);
    

    loadByMethod(ballFont, "resources/Futura.ttc");

    loadTextures();
    loadSounds();
    
    bkgd.setTexture(texs[0]);
    bkgd.setTextureRect(IntRect(0, 0, 2800, 2800));
    cO(bkgd);
    bkgd.setPosition(ScrCX, ScrCY);
    
    bkgdFrame.setTexture(texs[3]);
    bkgdFrame.sP(-550, -550);
       
    txt = Text("", ballFont, 15);
    txt.setPosition(10, 80);
    txt.setOutlineColor(Color::Black);
    txt.setFillColor(Color::Black);
                
    reset();
} //end onCreate

void SphereCollision::reset () {
    
    running = false;
        // Give the gravity strength a random value each reset
    int g = randRange(0, 150);
    gravAccel = float(g) / 100;

    bkgd.setColor(randomColor());
 
    txt.setString( "Gravity: " + floatStr(gravAccel, 2) + "\n" +
                   "Collision loss: " + floatStr(bumperLoss, 2) + "\n");

    vector<Color> cv {
        Color(255, 0, 0),
        Color(0, 255, 0),
        Color(100, 100, 255),
        Color(255, 255, 0),
        Color(255, 0, 255)
    };
    if (isOptionPressed())
        getNumbers();
	else getLetters(isShiftPressed());
    for (int i = 0; i <  5; ++i) {
        balls[i].velocity = vecF(0., 0.);
        balls[i].isActive = false;
        Sprite* s = &balls[i].s;
        s->setTexture(texs[1]);
        centerOrigin(*s);
        s->setColor(cv[i]);
        s->setPosition((ScrW - 100) / 5 * i + .5 * (ScrW - 100) / 5 + 50,
                       50);
        ballLets[i] = Text("", ballFont, 45);
        centerOrigin(ballLets[i]);
        ballLets[i].setPosition(s->gP() - vecF(15, 30));
        ballLets[i].setOutlineColor(Color::Black);
        ballLets[i].setOutlineThickness(3);
        ballLets[i].setFillColor(Color::White);
        ballLets[i].setString(curLetters[i]);
        ballLets[i].setStyle(Text::Bold);
        if (ballLets[i].getString() == 'W')
            ballLets[i].setCharacterSize(36);
        float scale = float(randRange(80, 150)) / 100;
        s->setScale(scale, scale);
        float r = s->gGB().width / 2;
        ballMasses[i] = (4 / 3) * pi * r * r * r;
    }

        // Populate the screen with wheels for the letter balls to bounce off of
    bumpers.clear();
	blocks.clear();
	for (int i = 0; i <  20; ++i) {
		float thirdX = ScrW / 3;
		float thirdY = (ScrH - 95) / 3;
		auto x = i % 3 * thirdX + randRange(0, int(thirdX - 45));
		auto mult = i / 7;
		int noBumpersAbove = 125;
		auto y = mult * thirdY + noBumpersAbove + randRange(0, int(thirdY - 25));
		if (i % 2) {
			WheelBumper bumper(texs[2]);
			centerOrigin(bumper.s);
			bumper.falling = false;
			bumper.s.sP(x, y);
			centerOrigin(bumper.rb);
			bumper.rb.sP(x, y);
			int sci = randRange(85, 160);
			float sc = float(sci) / 100;
			bumper.s.setScale(sc, sc);
			bumper.rb.setScale(sc, sc);
			bumpers.push_back(bumper);
		}
		else {
			Block block(texs[4]);
			centerOrigin(block);
			block.s.sP(x, y);
			blocks.push_back(block);
		}
	}
}

void SphereCollision::decGrav () {
    
    gravAccel = max(0.f, gravAccel - .01f);
}

void SphereCollision::incGrav () {
    
    gravAccel += .01;
}

void SphereCollision::decLoss () {
    
    bumperLoss = max(0.f, bumperLoss - .01f);
}

void SphereCollision::incLoss () {
    
    bumperLoss += .01;
}

void SphereCollision::mouseClick (EventDetails* det) {
    
	vecI mp = det->mouse;
	for (auto& obj : bumpers) {
		if (!(obj.s.gGB().contains(mp.x, mp.y)))
            continue;
		obj.clickedOn = true;
		gClickedOn = &obj;
		return;
    }
}
	
void SphereCollision::mouseRelease (EventDetails* det) {
    
	if (gClickedOn)
        gClickedOn->clickedOn = false;
	gClickedOn = nullptr;
}


bool SphereCollision::checkBallCollision (float radius1, vecf pos1, Ball& b2) {
    
    float radius2 = b2.s.gGB().width / 2;
    vecF pos2 = b2.s.gP();
    float hyp1 = hyp(pos1, pos2);
    return hyp1 <= radius1 + radius2;
}

void SphereCollision::update (const sf::Time& l_time) {


    float factor = cos(toRad(bkgdScaleDegree))*.3 + 1;
    bkgd.setScale(factor, factor);
    bkgd.rotate(.05);
    bkgdScaleDegree += .1;
    if (bkgdScaleDegree >= 360) bkgdScaleDegree -= 360;
  
  
    auto mx = Mouse::getPosition(*(stateManager->getContext()->win->getRenderWindow())).x;
    auto my = Mouse::getPosition(*(stateManager->getContext()->win->getRenderWindow())).y;
    
    if (gClickedOn)  {
        gClickedOn->s.setPosition(mx, my);
    }
    
    
    if (iKP(Num1) && iKP(LShift))
        incGrav();
    if (iKP(Num2) && iKP(LShift))
        decGrav();
    if (iKP(Num3) && iKP(LShift))
        incLoss();
    if (iKP(Num4) && iKP(LShift))
        decLoss();

    for (int i = 0; i <  5; ++i) {
		if (Keyboard::isKeyPressed(curKeys[i]))  {
            float rad1 = balls[i].s.gGB().width / 2;
            vecf pos1 = balls[i].s.gP();
            bool needToWait = false;
            forNumJ(5) {
                    // Don't activate this ball if another one is passing over it
                if (j != i && checkBallCollision(rad1, pos1, balls[j])) {
                    needToWait = true;
                    break;
                }
            }
            if (needToWait)
                continue;
            running = true;
			balls[i].isActive = true;
			balls[i].velocity = vecF(float(randRange(-12, 12)), float(randRange(-12, 12)));	
        }
    }

    if (!running) return;

    
    float phi = 0;
    float ax1, ay1, bx1, by1;

    float highSpd = 0;
    for (int b = 0; b < 5; ++b) {
        if (!balls[b].isActive)
            continue;
        balls[b].velocity.y += gravAccel;
        vecF vp = toRPolar(balls[b].velocity);
        if (vp.x > highSpd)
            highSpd = vp.x;
    }
    highSpd = ceil(highSpd);
    vector<vecF> savePosns(5);
    
    for (int i = 0; i < highSpd; ++i) {
        for (int b = 0; b < 5; ++b) {
            if (!balls[b].isActive)
                continue;
            savePosns[b] = balls[b].s.gP();
            balls[b].s.move(balls[b].velocity / highSpd);
        }
        for (int b = 0; b < 5; ++b) {
            if (!balls[b].isActive)
                continue;
            bool hitBallOrWall = false;
            VSprite& c = balls[b];
            Sprite& s = balls[b].s;
            ax1 = c.velocity.x;
            ay1 = c.velocity.y;
            vecF ap1 = toRPolar(c.velocity);
			float cradius = s.gGB().width / 2;
			vecF cpos = s.gP();
            
            vecF vp = toPolar(c.velocity);
            float oldAdjst = (vp.y + 180 < 360 ? vp.y + 180 : vp.y + 180 - 360);
            float newAng, contactAng;
            
            for (auto& obj : bumpers) {
                if (obj.falling)
                    continue;
                float radius = obj.s.gGB().width / 2;
                vecF pos = obj.s.gP();
                if (hyp(cpos, pos) > cradius + radius)
                    continue;
                
                intvec iv {0, 1, 2, 3};
                int k = randElemVal(iv);
                sounds[k].setPitch( float(randRange(80, 120)) / 100 );
                sounds[k].setVolume(randRange(65, 100));
                sounds[k].play();
                if (obj.hits == 8) // will be 9th hit after takeHit called
                    sounds[5].play();
                
                obj.takeHit();
                s.sP(savePosns[b]);
                cpos = s.gP();
                contactAng = toDeg(atan2(cpos.y - pos.y, cpos.x - pos.x));
                newAng = contactAng + (contactAng - oldAdjst);
                c.velocity = vecF(toRect(vp.x - bumperLoss, newAng));
            }
			
				/* Check whether current ball contacts any square blocks */
			for (auto& obj : blocks) {
					/* No contact: pass */
				if (!s.gGB().intersects(obj.gGB()))
					continue;
					
					/* If the center of the sphere falls between any two corners
					 * of the block, it's guaranteed contact and the bounce will
					 * behave just like a flat surface
					 */
				if (isOrBetween(cpos.x, obj.left(), obj.right())) {
					c.setDy(c.dy() * -1);
				}
				else if (isOrBetween(cpos.y, obj.top(), obj.bottom())) {
					c.setDx(c.dx() * -1);
				}
				
					/* The ball's bounding rectangle overlaps the block but the
					 * ball's center does not lie between corners of the block.
					 * Use radius to check for contact with the nearest corner
					 * and calculate angle if contact is made.
					 */
				else {
					vecf pdif;
					if (cpos.y < obj.top()) {
						if (cpos.x < obj.left()) {
							if (hyp(obj.topLeft(), cpos) > cradius)
								continue;
							pdif = toPolar(cpos - obj.topLeft());
						}
						else {
							if (hyp(obj.topRight(), cpos) > cradius)
								continue;
							pdif = toPolar(cpos - obj.topRight());
						}
					}
					else {
						if (cpos.x < obj.left()) {
							if (hyp(obj.bottomLeft(), cpos) > cradius)
								continue;
							pdif = toPolar(cpos - obj.bottomLeft());
						}
						else {
							if (hyp(obj.bottomRight(), cpos) > cradius)
								continue;
							pdif = toPolar(cpos - obj.bottomRight());
						}
					}
					float newAng = czdg(pdif.y + (pdif.y - oldAdjst));
					c.setVelocityP(max(vp.x - bumperLoss, 0.f), newAng);
					s.sP(savePosns[b]);
					cpos = savePosns[b];
				}
				sounds[0].play();
			}
            
			for (int bb = b + 1; bb < 5; ++bb) {
                if (!balls[bb].isActive)
                    continue;
                if (!checkBallCollision(cradius, cpos, balls[bb]))
                    continue;
                    
				hitBallOrWall = true;
				s.sP(savePosns[b]);
                cpos = s.gP();
                vecf pos = balls[bb].s.gP();
                float radius2 = balls[bb].s.gLB().width / 2;
                if (hyp(cpos, pos) <= cradius + radius2) {
                    balls[bb].s.sP(savePosns[bb]);
                    pos = savePosns[bb];
                }

                bx1 = balls[bb].velocity.x;
                by1 = balls[bb].velocity.y;
                vecF bp1 = toRPolar(balls[bb].velocity);
                phi = atan2(pos.y - cpos.y, pos.x - cpos.x);
                float ag = cos(ap1.y - phi) * ap1.x;
                float ah = sin(ap1.y - phi) * ap1.x;
                float bg = cos(bp1.y - phi) * bp1.x;
                float bh = sin(bp1.y - phi) * bp1.x;

                float mA = ballMasses[b];
                float mB = ballMasses[bb];
                float mdifAB = mA - mB;
                float mdifBA = mB - mA;
                float msum = mA + mB;
                float ag2 = (mdifAB * ag + 2 * mB * bg) / msum;     // = bg if masses the same
                float bg2 = (mdifBA * bg + 2 * mA * ag) / msum;     // = ag if masses the same

                float magA = hyp(ah, ag2);
                float dirA = atan2(ah, ag2) + phi;
                float magB = hyp(bh, bg2);
                float dirB = atan2(bh, bg2) + phi;
                
				balls[b].velocity = vecF(toRRect(magA, dirA));
                balls[bb].velocity = vecF(toRRect(magB - bumperLoss, dirB));
            }       //end for bb
            vecF pos = s.gP();
            vector<bool> hitWall = {false, false};
            if (pos.x > ScrW) {
                s.sP(ScrW, pos.y);
                hitWall[0] = true;
                }
            else if (pos.x < 0) {
                s.sP(0, pos.y);
                hitWall[0] = true;
                }
            if (pos.y > ScrH) {
                s.sP(pos.x, ScrH);
                hitWall[1] = true;
                }
            else if (pos.y < 0) {
                s.sP(pos.x, 0);
                hitWall[1] = true;
                }
            
            if (hitWall[0])
                balls[b].velocity.x *= -1;
            if (hitWall[1])
                balls[b].velocity.y *= -1;
            if (hitWall[0] || hitWall[1])
                hitBallOrWall = true;
            if (hitBallOrWall) {
                vecF vp = toPolar(balls[b].velocity);
                if (vp.x > 2) {
                    sounds[4].setPitch( float(randRange(85, 115)) / 100 );
                    sounds[4].setVolume(randRange(75, 100));
                    sounds[4].play();
                }
                vp.x -= bumperLoss;
                balls[b].velocity = toRect(vp);
            }
            ballLets[b].sP(balls[b].s.gP()-vecF(15, 30));

            txt.setString(
                 "Gravity: " + floatStr(gravAccel, 2) + "\n" +
                 "Collision loss: " + floatStr(bumperLoss, 2) + "\n" +
                "");
        } //end for b
    } //end for i
    if (slow)
        sleep(seconds(.1));

    for (auto& obj : bumpers) {
        obj.rb.rotate(obj.spinRate);
        if (obj.falling && obj.s.gP().y < 1000) {
            obj.velocity.y += .6;
            obj.s.move(obj.velocity);
            obj.rb.move(obj.velocity);
        }
    }
}
  
	    												
void SphereCollision::draw () {
    
	RenderWindow* w = stateManager->getContext()->win->getRenderWindow();
	w->draw(bkgd);
    for (auto& obj : bumpers) {
        w->draw(obj.rb);
    }
	for (auto& obj : blocks) {
		w->draw(obj);
	}
	for (int i = 0; i < 5; ++i) {
		w->draw(balls[i].s);
		w->draw(ballLets[i]);
    }
    w->draw(txt);
    if (!stateManager->getContext()->win->isStretched())
        w->draw(bkgdFrame);
}
	

void SphereCollision::onKeyPress(Keyboard::Key k) {
    
    switch(k) {
            
        case Keyboard::Space:
            reset(); break;
           
        case Keyboard::RAlt:
            slow = !slow; break;
            
        case Keyboard::RShift:
            running = !running; break;
            
        default:
            break;
    }
}

const vector<string> SphereCollision::texList {
    "graybkgd.png",     // 0
    "grayball.png",     // 1
    "wheel.png",        // 2
    "bkgdFrame.png",    // 3
	"squareblock.png"   // 4
};

const vector<string> SphereCollision::soundFileList {
    "boing1.wav",       // 0
    "boing2.wav",       // 1
    "boing3.wav",       // 2
    "boing4.wav",       // 3
    "thump1.wav",       // 4
    "lose5.wav"         // 5
    // Update `numSounds` if adding to
};
