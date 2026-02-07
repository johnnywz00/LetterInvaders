#include "statemanager.hpp"
#include "spherecollision.hpp"


void SphereCollision::onCreate ()
{
	/* Register callbacks */
	EventManager* evMgr = stateManager->getContext()->eventManager;
	evMgr->addCallback(StateType::SphereCollision, "Mouse_Left",
					   &SphereCollision::mouseClick, this);
	evMgr->addCallback(StateType::SphereCollision, "Mouse_Left_Up",
					   &SphereCollision::mouseRelease, this);
	evMgr->addCallback(StateType::SphereCollision, "Key_Escape",
					   &SphereCollision::onEscape, this);
	
	
	gSound("hitBumper").setVolume(25);
	
	/* Background sprite */
	bkgd.setTexture(gTexture("bkgd"));
	bkgd.setTextureRect(IntRect(0, 0, 2800, 2800));
	centerOrigin(bkgd);
	bkgd.setPosition(scrcx, scrcy);

	/* Gravity/loss stats */
	txt = Text("", gFont("ball"), 18);
	txt.setPosition(10, 80);
	txt.setOutlineColor(Color::Black);
	txt.setFillColor(Color::Black);
				
	reset();
} //end onCreate

void SphereCollision::onDestroy ()
{
	/* Unregister from event manager */
	EventManager* evMgr = stateManager->getContext()->eventManager;
	evMgr->removeCallback(StateType::SphereCollision, "Mouse_Left");
	evMgr->removeCallback(StateType::SphereCollision, "Mouse_Left_Up");
	evMgr->removeCallback(StateType::SphereCollision, "Key_Escape");
}

void SphereCollision::deactivate ()
{
	reset();
}

void SphereCollision::update (const sf::Time& l_time)
{
	/* Animate the background */
	float factor = cos(toRad(bkgdScaleDegree))*.3 + 1;
	bkgd.setScale(factor, factor);
	bkgd.rotate(.05);
	bkgdScaleDegree += .1;
	if (bkgdScaleDegree >= 360)
		bkgdScaleDegree -= 360;
  
	/* Drag wheels around */
	auto mx = Mouse::getPosition(*(renWin())).x;
	auto my = Mouse::getPosition(*(renWin())).y;
	if (gClickedOn)  {
		gClickedOn->s.setPosition(mx, my);
		gClickedOn->rb.setPosition(mx, my);
	}
	
	/* Twiddle physics properties */
	if (iKP(Num1) && isShiftPressed())
		incGrav();
	if (iKP(Num2) && isShiftPressed())
		decGrav();
	if (iKP(Num3) && isShiftPressed())
		incLoss();
	if (iKP(Num4) && isShiftPressed())
		decLoss();

	/* Activate a letter ball if no other balls are currently contacting it */
	for (int i = 0; i < 5; ++i) {
		if (Keyboard::isKeyPressed(curKeys[i]))  {
			float rad1 = balls[i].s.gGB().width / 2;
			vecf pos1 = balls[i].s.gP();
			bool needToWait = false;
			forNumJ(5) {
				/* Don't activate this ball if another one is passing over it */
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

	/* Animate bouncing balls */
	float phi = 0;
	float ax1, ay1, bx1, by1;

	/* Use the fastest-moving ball as a gauge for incremental
	 * collision checking
	 */
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
	
	/* Store current positions of balls for reverting to in
	 * the case of a collision
	 */
	vector<vecF> savePosns(5);
	
	
	for (int i = 0; i < highSpd; ++i) {
		/* First tentatively move each ball */
		for (int b = 0; b < 5; ++b) {
			if (!balls[b].isActive)
				continue;
			savePosns[b] = balls[b].s.gP();
			balls[b].s.move(balls[b].velocity / highSpd);
		}
		
		/* Collision check loop for each ball */
		for (int b = 0; b < 5; ++b) {
			if (!balls[b].isActive)
				continue;
			/* Store some stuff first */
			bool hitBallOrWall = false;
			VSprite& c = balls[b];
			Sprite& s = balls[b].s;
			ax1 = c.velocity.x;
			ay1 = c.velocity.y;
			vecF ap1 = toRPolar(c.velocity);
			float cradius = s.gGB().width / 2;
			vecF cpos = s.gP();
			
			vecF vp = toPolar(c.velocity);
			float oldAdjst = czdg(vp.y + 180);
			float newAng, contactAng;
			
			/* Check whether cur ball is hitting a wheel */
			for (auto& obj : bumpers) {
				if (obj.falling)
					continue;
				float radius = obj.s.gGB().width / 2;
				vecF pos = obj.s.gP();
				if (hyp(cpos, pos) > cradius + radius)
					continue;
				
				/* Hit a wheel */
				strvec sv {"boing1", "boing2", "boing3", "boing4"};
				string key = randElemVal(sv);
				gSound(key).setPitch( float(randRange(80, 120)) / 100 );
				gSound(key).setVolume(randRange(65, 100));
				gSound(key).play();
				if (obj.hits == 8) // will be 9th hit after takeHit called
					gSound("wheelFall").play();
				
				obj.takeHit();
				s.sP(savePosns[b]);
				cpos = s.gP();
				/* Determine ball's new velocity as result of the collision*/
				contactAng = toDeg(atan2(cpos.y - pos.y, cpos.x - pos.x));
				newAng = contactAng + (contactAng - oldAdjst);
				c.velocity = vecF(toRect(vp.x - bumperLoss, newAng));
			}
			
			/* Check whether current ball contacts any square blocks */
			for (auto& obj : blocks) {
				if (!s.gGB().intersects(obj.gGB()))
					/* No contact: pass */
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
				gSound("ballsCollide").play();
			}
			
			/* Check if ball collides with any other ball */
			for (int bb = b + 1; bb < 5; ++bb) {
				if (!balls[bb].isActive)
					continue;
				if (!checkBallCollision(cradius, cpos, balls[bb]))
					continue;
					
				/* Hit another ball: move one or both of the balls
				 * back to saved position till there's no overlap
				 */
				hitBallOrWall = true;
				s.sP(savePosns[b]);
				cpos = s.gP();
				vecf pos = balls[bb].s.gP();
				float radius2 = balls[bb].s.gGB().width / 2;
				if (hyp(cpos, pos) <= cradius + radius2) {
					balls[bb].s.sP(savePosns[bb]);
					pos = savePosns[bb];
				}

				/* Sphere collision math to determine both balls'
				 * new velocities with differences of mass factored
				 * in.
				 */
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
				float ag2 = (mdifAB * ag + 2 * mB * bg) / msum;
				float bg2 = (mdifBA * bg + 2 * mA * ag) / msum;

				float magA = hyp(ah, ag2);
				float dirA = atan2(ah, ag2) + phi;
				float magB = hyp(bh, bg2);
				float dirB = atan2(bh, bg2) + phi;
				
				balls[b].velocity = vecF(toRRect(magA, dirA));
				balls[bb].velocity = vecF(toRRect(magB - bumperLoss, dirB));
			}  //end for bb
			
			/* Finally check for contact with borders of the screen */
			vecF pos = s.gP();
			vector<bool> hitWall = {false, false};
			if (pos.x > scrw) {
				s.sP(scrw, pos.y);
				hitWall[0] = true;
				}
			else if (pos.x < 0) {
				s.sP(0, pos.y);
				hitWall[0] = true;
				}
			if (pos.y > scrh) {
				s.sP(pos.x, scrh);
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
					gSound("ballsCollide").setPitch( float(randRange(85, 115)) / 100 );
					gSound("ballsCollide").setVolume(randRange(75, 100));
					gSound("ballsCollide").play();
				}
				vp.x -= bumperLoss;
				balls[b].velocity = toRect(vp);
			}
			/* Make the letter catch up with the new ball position */
			ballLets[b].sP(balls[b].s.gP() - vecF(15, 30));

			txt.setString(
				 "Gravity: " + floatStr(gravAccel, 2) + "\n" +
				 "Collision loss: " + floatStr(bumperLoss, 2) + "\n" +
				"");
		} //end for b
	} //end for i
	
	/* Slow-mo mode (Right Alt/Option) was used during debugging */
	if (slow)
		sleep(seconds(.1));

	/* Animate the spin and/or fall of the wheels */
	for (auto& obj : bumpers) {
		obj.rb.rotate(obj.spinRate);
		if (obj.falling && obj.s.gP().y < scrh + 300) {
			obj.velocity.y += gravAccel;
			obj.s.move(obj.velocity);
			obj.rb.move(obj.velocity);
		}
	}
}
  
void SphereCollision::mouseClick (EventDetails* det)
{
	/* Make wheels draggable */
	vecI mp = det->mouse;
	for (auto& obj : bumpers) {
		if (!(obj.s.gGB().contains(mp.x, mp.y)))
			continue;
		obj.clickedOn = true;
		gClickedOn = &obj;
		return;
	}
}
	
void SphereCollision::mouseRelease (EventDetails* det)
{
	if (gClickedOn)
		gClickedOn->clickedOn = false;
	gClickedOn = nullptr;
}

void SphereCollision::onKeyPress(Keyboard::Key k)
{
	switch(k) {
			
		case Keyboard::Space:
			reset();
			break;
		   
		case Keyboard::RAlt:
			slow = !slow;
			break;
			
		case Keyboard::RShift:
			running = !running;
			break;
			
		default:
			break;
	}
}

void SphereCollision::onEscape (EventDetails* det)
{
	stateManager->switchTo(StateType::MainMenu);
}

void SphereCollision::draw ()
{
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
}
	
void SphereCollision::reset ()
{
    running = false;
	/* Give the gravity strength a random value each reset */
    int g = randRange(0, 100);
    gravAccel = float(g) / 100;

    bkgd.setColor(randomColorMaxSat(80));
 
    txt.setString( "Gravity: " + floatStr(gravAccel, 2) + "\n" +
                   "Collision loss: " + floatStr(bumperLoss, 2) + "\n");

	/* Configure the balls with new letters/numbers */
    if (isOptionPressed())
        getNumbers();
	else getLetters(isShiftPressed());
	
    for (int i = 0; i < 5; ++i) {
        balls[i].velocity = vecF(0., 0.);
        balls[i].isActive = false;
        Sprite* s = &balls[i].s;
		s->setTexture(gTexture("ball"));
        centerOrigin(*s);
        s->setColor(colorsVec[i]);
        s->setPosition((scrw - 100) / 5 * i + .5 * (scrw - 100) / 5 + 50,
                       70);
        ballLets[i] = Text("", gFont("ball"), 45);
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

	/* Populate the screen with wheels and square blocks
	 * for the letter balls to bounce off of
	 */
    bumpers.clear();
	blocks.clear();
	for (int i = 0; i < 20; ++i) {
		float thirdX = scrw / 3;
		float thirdY = (scrh - 95) / 3;
		auto x = i % 3 * thirdX + randRange(0, int(thirdX - 45));
		auto mult = i / 7;
		int noBumpersAbove = 125;
		auto y = mult * thirdY + noBumpersAbove + randRange(0, int(thirdY - 25));
		if (randRange(7) > 1) { /* More wheels than blocks */
			WheelBumper bumper;
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
			Block block;
			centerOrigin(block);
			block.s.sP(x, y);
			blocks.push_back(block);
		}
	}
}

void SphereCollision::getLetters (bool inclNums)
{
	int sz = inclNums ? 36 : 26;
	auto curVec = lettersVec;
	auto keysVec = letKeysVec;
	if (inclNums) {
		curVec = vecPlusVec(lettersVec, numbersVec);
		keysVec = vecPlusVec(letKeysVec, numKeysVec);
	}
	curLetters.clear();
	curKeys.clear();
	while (curLetters.size() != 5) {
		int idx = randRange(0, sz - 1);
		string cand = lettersVec[idx];
		if (std::find(curLetters.begin(), curLetters.end(), cand) == curLetters.end()) {
			curLetters.push_back(cand);
			curKeys.push_back(keysVec[idx]);
		}
	}
}

void SphereCollision::getNumbers ()
{
	curLetters.clear();
	curKeys.clear();
	while (curLetters.size() != 5) {
		int idx = randRange(0, 9);
		string cand = numbersVec[idx];
		if (std::find(curLetters.begin(), curLetters.end(), cand) == curLetters.end()) {
			curLetters.push_back(cand);
			curKeys.push_back(numKeysVec[idx]);
		}
	}
}

void SphereCollision::incGrav () { gravAccel += .01; }

void SphereCollision::decGrav () { gravAccel = max(0.f, gravAccel - .01f); }

void SphereCollision::incLoss () { bumperLoss += .01; }

void SphereCollision::decLoss () { bumperLoss = max(0.f, bumperLoss - .01f); }

bool SphereCollision::checkBallCollision (float radius1, vecf pos1, Ball& b2)
{
    float radius2 = b2.s.gGB().width / 2;
    vecF pos2 = b2.s.gP();
    float hyp1 = hyp(pos1, pos2);
    return hyp1 <= radius1 + radius2;
}

RenderWindow* SphereCollision::renWin ()
{
	return stateManager->getContext()->win->getRenderWindow();
}

