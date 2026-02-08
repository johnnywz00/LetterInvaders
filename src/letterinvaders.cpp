
#include "statemanager.hpp"
#include "letterinvaders.hpp"


void LetterInvadersState::onCreate ()
{
	/* Register callbacks */
	EventManager* evMgr = stateManager->getContext()->eventManager;
	evMgr->addCallback(StateType::tLetterInvaders, "Mouse_Left",
			&LetterInvadersState::mouseClick, this);
	evMgr->addCallback(StateType::tLetterInvaders, "Mouse_Left_Up",
			&LetterInvadersState::mouseRelease, this);
	evMgr->addCallback(StateType::tLetterInvaders, "Key_Escape",
			&LetterInvadersState::onEscape, this);

	gui = stateManager->getContext()->gui;
	
	gSound("hitBumper").setVolume(25);
	
	/* Background features */
	bkgd.setTexture(gTexture("bkgd"));
	bkgd.setTextureRect(IntRect(0, 0, scrw * 2, scrw * 2));
	centerOrigin(bkgd);
	bkgd.setPosition(scrcx, scrcy);
	
	ground.setTexture(gTexture("ground"));
	auto texY = ground.getTexture()->getSize().y;
	ground.setTextureRect(IntRect(0, 0, scrw, texY));
	ground.setPosition(0, scrh - texY);
	
	lWall.setTexture(gTexture("wall"));
	auto texX = lWall.getTexture()->getSize().x;
	lWall.setTextureRect(IntRect(0, 0, texX, scrh));
	lWall.setPosition({10.f - texX, 0});

	rWall.setTexture(gTexture("wall"));
	rWall.setTextureRect(IntRect(0, 0, texX, scrh));
	rWall.setPosition(scrw - 11, 0);

	/* Ball-zapping laser beam */
	beam.setFillColor(Color(255, 255, 255, 60));
	beam.setSize({0, 5});
	beam.setOrigin(0, 2);
	beam.sP(scrcx, scrh);
	
	/* Stats text */
	curLetterSet = sets[0];
	curSetLabel = setsLabels[0];
	txt = Text("", gFont("stats"), 20);
	txt.setPosition(18, 80);
	txt.setOutlineColor(Color(40, 40, 40));
	txt.setOutlineThickness(1);
	txt.setFillColor(Color(210, 210, 210));
	txt.setString(
				  "SCORE: " + tS(curScore) + "\n\n" +
				  "LEVEL: " + tS(curLevel) +
				  "\n\n" + curSetLabel);
} //end onCreate


void LetterInvadersState::onDestroy ()
{
	EventManager* evMgr = stateManager->getContext()->eventManager;
	evMgr->removeCallback(StateType::tLetterInvaders, "Mouse_Left");
	evMgr->removeCallback(StateType::tLetterInvaders, "Mouse_Left_Up");
	evMgr->removeCallback(StateType::tLetterInvaders, "Key_Escape");
}

void LetterInvadersState::activate ()
{
	renWin()->setMouseCursorVisible(false);
	setUpHighScores();
	reset();
}

void LetterInvadersState::deactivate ()
{
	renWin()->setMouseCursorVisible(true);
	auto gui = stateManager->getContext()->gui;
	auto scoresPtr = gui->get("scores");
	if (scoresPtr)
		scoresPtr->setVisible(false);
	gui->removeAllWidgets();
}

void LetterInvadersState::onKeyPress (Keyboard::Key k)
{
	/* Don't register game-level keystrokes when a high score
	 * is being entered
	 */
	auto scoresPtr = gui->get<tgui::Panel>("scores");
	if (scoresPtr) {
		if (scoresPtr->isVisible()) {
			forNum(10) {
				auto eb = gui->get<tgui::EditBox>("EBox" + tS(i));
				if (eb->isFocused())
					return;
			}
		}
	}
	
	/* Allow choosing of key set before game start */
	if (!running) {
		forNum(9) {
			if (k == setsKeys[i]) {
				curLetterSet = sets[i];
				curSetLabel = setsLabels[i];
				return;
			}
		}
	}
	
	switch(k) {
		case Keyboard::Y:
			if (!running)
				reset();
			break;
		
		case Keyboard::Space:
			running = !running;
			break;
				
		default:
			break;
	}
}

void LetterInvadersState::onEscape (EventDetails* det)
{
	reset();
    stateManager->switchTo(StateType::MainMenu);
}

void LetterInvadersState::update (const Time& elapsed)
{
	/* Do nothing here during high scores display */
	auto sc = gui->get("scores");
	if (sc && sc->isVisible() && sc->isFocused()) {
		return;
	}
 
	txt.setString( "SCORE: " + tS(curScore) + "\n\n" +
				  "LEVEL: " + tS(curLevel) +
				  "\n\n" + curSetLabel);
	
	if (!running || gameOver)
		return;

	/* Animate background */
	float factor = cos(toRad(bkgdScaleDegree)) * .3 + 1;
	bkgd.setScale({factor, factor});
	bkgd.rotate(.05);
	bkgdScaleDegree += .1;
	if (bkgdScaleDegree >= 360)
		bkgdScaleDegree -= 360;

	/* Destroy a falling ball if the corresponding key is pressed */
	for (auto itr = balls.begin(); itr != balls.end(); ++itr) {
		Ball& b = *itr;
		if (b.isActive && Keyboard::isKeyPressed(b.key)) {
			destroyBall(b, elapsed);
			fadeTime = elapsed + seconds(.15);
		}
		/* Do a swirling animation as the ball is blasted away */
		if (!b.isActive) {
			b.swirlExit(elapsed);
		}
	}
	balls.erase(
				remove_if(balls.begin(), balls.end(),
					 [&](auto& x) { return !x.isActive && x.s.gP().y < -50; }),
				balls.end()
				);

	/* Generate new falling ball based on time elapsed and level difficulty */
	if (elapsed.asSeconds() > lastSpawnTime.asSeconds() + spawnInterval)
		spawnBall(elapsed);

	/* Rudimentary Fuse behavior for when to turn off last laser shot*/
	if (elapsed > fadeTime)
		drawBeam = false;
	
/* Animate bouncing balls */
	
	float phi = 0, hyp1 = 0;
	float ax1, ay1, bx1, by1;

	/* Use the fastest-moving ball as a gauge for incremental
	 * collision checking
	 */
	float highSpd = 0;
	int numBalls = int(balls.size());
	for (int b = 0; b < numBalls; ++b) {
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
	vector<vecF> savePosns(numBalls);
	
	for (int i = 0; i <  highSpd; ++i) {
		/* First tentatively move each ball */
		for (int b = 0; b < numBalls; ++b) {
			if (!balls[b].isActive)
				continue;
			savePosns[b] = balls[b].s.gP();
			balls[b].s.move(balls[b].velocity / highSpd);
		}
		
		/* Collision check loop for each ball */
		for (int b = 0; b < numBalls; ++b) {
			if (!balls[b].isActive)
				continue;
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
			
			/* Check whether cur ball is hitting a bumper */
			for (auto& obj : bumpers) {
				float radius = obj.s.gGB().width / 2;
				vecF pos = obj.s.gP();
				if (hyp(cpos, pos) > cradius + radius)
					continue;
				
				/* Hit a wheel */
				s.sP(savePosns[b]);
				cpos = s.gP();
				contactAng = toDeg(atan2(cpos.y - pos.y, cpos.x - pos.x));
				newAng = contactAng + (contactAng - oldAdjst);
				c.velocity = vecF(toRect(vp.x - bumperLoss, newAng));
				
				gSound("hitBumper").setPitch( float(randRange(80, 120)) / 100 );
				gSound("hitBumper").play();
			}
			
			/* Check if ball collides with any other ball */
			for (int bb = b + 1; bb < numBalls; ++bb) {
				if (!balls[bb].isActive)
					continue;
				float radius = balls[bb].s.gGB().width / 2;
				vecF pos = balls[bb].s.gP();
				hyp1 = hyp(cpos, pos);
				if (hyp1 > cradius + radius)
					continue;  // No collision
				
				s.sP(savePosns[b]);
				cpos = s.gP();
				if (hyp(cpos, pos) <= cradius + radius)
					balls[bb].s.sP(savePosns[bb]);

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

				float mA = balls[b].mass;
				float mB = balls[bb].mass;
				float mdifAB = mA - mB;
				float mdifBA = mB - mA;
				float msum = mA + mB;
				float ag2 = (mdifAB * ag + 2 * mB * bg) / msum;
				float bg2 = (mdifBA * bg + 2 * mA * ag) / msum;

				float magA = hyp(ah, ag2);
				float dirA = atan2(ah, ag2) + phi;
				float magB = hyp(bh, bg2);
				float dirB = atan2(bh, bg2) + phi;
				
				balls[b].velocity = vecF(toRRect(magA - bumperLoss, dirA));
				balls[bb].velocity = vecF(toRRect(magB - bumperLoss, dirB));
				
				gSound("ballsCollide").play();
			} //end for bb
			
			/* Check for contact with borders of the screen */
			vecF pos = s.gP();
			bool hitWall = false;
			if (pos.x > scrw) {
				s.sP(scrw, pos.y);
				hitWall = true;
			}
			else if (pos.x < 0) {
				s.sP(0, pos.y);
				hitWall = true;
			}
			if (hitWall) {
				balls[b].velocity.x *= -1;
				vecF vp = toPolar(balls[b].velocity);
				vp.x -= bumperLoss;
				balls[b].velocity = toRect(vp);
			}
			/* Make the letter catch up with the new ball position */
			balls[b].txt.sP(balls[b].s.gP() - vecF(15, 30));
	  
			if (balls[b].s.gGB().intersects(FloatRect(0, scrh + 1, scrw, 5))) {
				endGame();
				return;
			}
		} //end for b
	} //end for i
} // end update()

void LetterInvadersState::draw ()
{
	RenderWindow* w = stateManager->getContext()->win->getRenderWindow();
	w->draw(bkgd);
	for (auto& obj : bumpers) {
		w->draw(obj.s);
	}
	for (auto& b : balls) {
		w->draw(b.s);
		if (b.isActive)
			w->draw(b.txt);
	}
	w->draw(txt);
	if (drawBeam)
		w->draw(beam);
	w->draw(ground);
	w->draw(lWall);
	w->draw(rWall);
}

void LetterInvadersState::setUpHighScores ()
{
	string whichFile = "liscores.txt";
	if (MainMenuState::playerName == "Ravenna")
		whichFile = "liscores-rav.txt";
	else if (MainMenuState::playerName == "Laith")
		whichFile = "liscores-lai.txt";
    ifstream scores {
		Resources::executingDir() / "resources" / whichFile
    };
	if (scores.is_open()) {
		topTen.clear();
		string line;
		while (getline(scores, line)) {
			HighScore hs;
			stringstream ss(line);
			ss >> hs.score >> hs.player >> hs.date >> hs.level;
			topTen.push_back(hs);
		}
		scores.close();
	}

    auto p = tgui::Panel::create();
    p->setSize(800, 700);
	p->setPosition(scrcx - (p->getSize().x / 2), scrcy - (p->getSize().y / 2));
    gui->add(p, "scores");
    auto l = tgui::Label::create();
    l->setOrigin(.5, .5);
    l->setPosition("50 % ", 50);
    l->setTextSize(48);
    l->setText("GREATEST HEROES");
    p->add(l, "scoreslabel");
    auto vl = tgui::VerticalLayout::create();
    vl->setSize("90 % ", "80 % ");
    vl->setPosition("50 % ", 100);
    vl->setOrigin(.5, 0);
    p->add(vl, "vl");
    forNum(10) {
        auto tt = topTen[i];
        auto eb = tgui::EditBox::create();
        eb->setTextSize(24);
        eb->setText(highScoreToText(tt));
        eb->onReturnKeyPress(&LetterInvadersState::enterNewScore, this);
        vl->add(eb, "EBox" + tS(i));
        eboxes.push_back(eb);
        if (i != 9)
            vl->addSpace(.1);
    }
    p->setVisible(false);
}

void LetterInvadersState::reset ()
{
    bumpers.clear();
    balls.clear();
    
    running = false;
    gameOver = false;
	auto hscorePtr = gui->get("scores");
	if (hscorePtr)
		hscorePtr->setVisible(false);
    renWin()->setMouseCursorVisible(false);

    gravAccel = initGrav;
    bumperLoss = initBumperLoss;
    numBumperColumns = initBumperCols;
    numBumperRows = initBumperRows;
    curScore = 0;
    curLevel = 1;
    spawnInterval = initSpawnInterval;
    lastSpawnTime = Time::Zero;
    bkgd.setColor(randomColorMaxSat(80));
    
    placeBumpers();
    beam.setSize(vecf(0, 5));
    beam.setRotation(0);
}


void LetterInvadersState::placeBumpers ()
{
    forNum(numBumperColumns * numBumperRows) {
        Bumper b;
        float cellW = (scrw - wallThickness * 2) / numBumperColumns;
        float cellH = (scrh - upperDeadZone - lowerDeadZone) / numBumperRows;
        auto x = i % numBumperColumns * cellW + wallThickness + randRange(0, int(cellW - 50));
        auto y = i / numBumperColumns * cellH + upperDeadZone + randRange(0, int(cellH - 50));
        b.s.sP({x, y});
        bumpers.push_back(b);
    }
}

void LetterInvadersState::spawnBall (Time tm)
{
    string let(1, toupper(curLetterSet[randRange(0, int(curLetterSet.length() - 1))] ) );
    Ball b {let, randElemVal(ballColors)};
	/* Make sure the ball doesn't spawn on top of an existing bouncing ball */
    bool openSpot;
    do {
        openSpot = true;
        for (auto& bb : balls) {
            if (b.s.gGB().intersects(bb.s.gGB())) {
                openSpot = false;
                b.s.move(0, -50);
                break;
            }
        }
    }
    while (!openSpot);
    balls.push_back(b);
    lastSpawnTime = tm;
}

void LetterInvadersState::destroyBall (Ball& b, Time tm)
{
    curScore += 1;
    if (!(curScore % 10)) {
        ++curLevel;
        if (spawnInterval > 1)
            spawnInterval -= 1;
        else if (spawnInterval > .4)
            spawnInterval -= .1;
        gravAccel = min(2.5, gravAccel + .01);
        bkgd.setColor(randomColorMaxSat(80));
		gSound("levelUp").play();
		sleep(seconds(.2));
		gSound("levelUp").play();
		sleep(seconds(.2));
		gSound("levelUp").play();
    }
    strvec sv {"zap1", "zap2", "zap3"};
    string key = randElemVal(sv);
    gSound(key).setVolume(randRange(65, 100));
    gSound(key).play();
    b.isActive = false;
    b.blastPos = b.s.gP();
	b.blastTime = tm;
	
	/* Create a "laser beam" zapping the ball */
    auto q = beam.gP() - b.blastPos;
    beam.setSize(vecf(toPolar(q).x, beam.getSize().y));
    beam.setRotation(toPolar(q).y - 180);
    drawBeam = true;
}

void LetterInvadersState::endGame ()
{
    running = false;
    gameOver = true;
    gSound("gameOver").play();
    showHighScores(curScore, curLevel);
}

void LetterInvadersState::showHighScores (int score, int level)
{
    renWin()->setMouseCursorVisible(true);
    auto scoresPtr = gui->get<tgui::Panel>("scores");
    if (!scoresPtr)
		return;
	
	scoresPtr->setVisible(true);
    if (score >= topTen[9].score) {
		string date {LocalTime().slashDate()};
        HighScore hs {score, level, date};
        auto p = find_if(topTen.begin(), topTen.end(),
                         [&](auto x) { return hs.score >= x.score; });
        if (p != topTen.end())
            topTen.insert(p, hs);
        topTen.resize(10);
        forNum(10) {
            HighScore& tt = topTen[i];
            auto eb = gui->get<tgui::EditBox>("EBox" + tS(i));
            if (tt.player == "") {
                eb->setText(tS(hs.score));
                eb->selectText();
                eb->setEnabled(true);
                eb->setFocused(true);
            }
            else {
                eb->setText(highScoreToText(tt));
                eb->setEnabled(false);
            }
        }
		scoresPtr->setFocused(true);
    }
    else {
		scoresPtr->setFocused(false);
        for (auto& e : eboxes) {
            e->setFocused(false);
            e->setEnabled(false);
        }
    }
}

void LetterInvadersState::enterNewScore (tgui::String str)
{
	string s = str.toStdString();
	/* Truncate the entered name if it's too long */
	if (s.length() > 18)
		s.resize(18);
	/* Get rid of potential spaces in the entered name
	 * just to keep our parsing simple.
	 */
	for (auto& ch : s)
		if (ch == ' ')
			ch = '-';
	
    forNum(10) {
        if (topTen[i].player == "") {
            topTen[i].player = s == "" ? "NamelessOne" : s;
            eboxes[i]->setText(highScoreToText(topTen[i]));
            eboxes[i]->setEnabled(false);
            gui->get("scores")->setFocused(false);
        }
    }
    topTenToFile();
}

string LetterInvadersState::highScoreToText (HighScore &hs)
{
    stringstream ss {};
    ss << "  " << std::left << std::setw(10) << tS(hs.score) << std::setw(20)
		<< hs.player << std::setw(10) << hs.date << std::right << std::setw(10) << tS(hs.level);
    return ss.str();
}

void LetterInvadersState::topTenToFile ()
{
	string whichFile = "liscores.txt";
	if (MainMenuState::playerName == "Ravenna")
		whichFile = "liscores-rav.txt";
	else if (MainMenuState::playerName == "Laith")
		whichFile = "liscores-lai.txt";
    ofstream f {Resources::executingDir() / "resources" / whichFile, ios_base::trunc};
    if (!f.is_open())
        return;
    for (auto& t : topTen) {
        f << t.score << " " << t.player << " " << t.date
        << " " << t.level << "\n";
    }
    f.close();
}

RenderWindow* LetterInvadersState::renWin ()
{
	return stateManager->getContext()->win->getRenderWindow();
}
