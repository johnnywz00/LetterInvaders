//
//  letterinvaders.cpp
//  LetterBallsXC
//
//  Created by John Ziegler on 6/29/24.
//  Copyright © 2024 John Ziegler. All rights reserved.
//

#include "letterinvaders.hpp"
#include "statemanager.hpp"



void swirlExit (Ball& b, float p) {
    
    vecf dest {b.blastDestX, -100};
    vecf vecDif = dest - b.blastPos;
    float hypot = hyp(vecDif);
    float radius = 80;
    float ratio = radius / hypot;
    vecf offset = vecDif * ratio;
    vecf initAxis = b.blastPos + offset;
    vecDif = dest - initAxis;
    vecf axis = initAxis + vecDif * p;
    vecf offs2 {b.blastPos - initAxis};
    float initRads = toPolar(offs2).y;
    float rads = initRads + 360 * (fmod(p, 5) / .2);
    vecf v = toRect(radius, rads);
    b.s.sP(axis + v);
}


LetterInvadersState::~LetterInvadersState () {
    
}

void LetterInvadersState::onEscape (EventDetails* det) {
    
	reset();
	// remove gui? or check that deactivate is called by switchTo
    stateManager->switchTo(StateType::MainMenu);
}

void LetterInvadersState::onDestroy () {
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->removeCallback(StateType::tLetterInvaders, "Mouse_Left");
    evMgr->removeCallback(StateType::tLetterInvaders, "Mouse_Left_Up");
    evMgr->removeCallback(StateType::tLetterInvaders, "Key_Escape");
    

}

void LetterInvadersState::deactivate () {
    
    auto w = stateManager->getContext()->win->getRenderWindow();
    w->setMouseCursorVisible(true);
    auto gui = stateManager->getContext()->gui;
    gui->get("scores")->setVisible(false);
    
    gui->removeAllWidgets();
}

void LetterInvadersState::activate () {
    
    auto w = stateManager->getContext()->win->getRenderWindow();
    w->setMouseCursorVisible(false);
        
    reset();
}

void LetterInvadersState::setUpHighScores () {

    ifstream scores {
        "liscores.txt"
    };
    string line;
    while (getline(scores, line)) {
        HighScore hs;
        stringstream ss(line);
        ss >> hs.score >> hs.player >> hs.date >> hs.level;
        topTen.push_back(hs);
    }
    scores.close();

    auto gui = stateManager->getContext()->gui;
    auto p = tgui::Panel::create();
    p->setSize(700, 700);
    p->setPosition(350, 100);
    gui->add(p, "scores");
    auto l = tgui::Label::create();
    l->setOrigin(.5, .5);
    l->setPosition("50 % ", 50);
    l->setTextSize(48);
    l->setText("HIGH SCORES!");
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

void LetterInvadersState::loadTextures () {
    
    texs.clear();
    texs.reserve(10);
    
    Texture tex;
    string basePath = "resources/";
    int sz = int(texList.size());
    for (int i = 0; i <  sz ; ++i) {
        string fileName = basePath + texList[i];
        loadByMethod(tex, fileName);
        texs.push_back(tex);
    }
    texs[0].setRepeated(true);
}

void LetterInvadersState::loadSounds () {
    
    string path = "resources/";
	forNum(numSounds) {
        loadByMethod(buffers[i], path + soundFileList[i]);
        sounds[i].setBuffer(buffers[i]);
    }
    sounds[0].setVolume(25);
	
	levelUpBuf.loadFromFile("resources/Bell3.wav");
	levelUpSnd.setBuffer(levelUpBuf);
}

void LetterInvadersState::onCreate () {
    
    EventManager* evMgr = stateManager->getContext()->eventManager;
    evMgr->addCallback(StateType::tLetterInvaders, "Mouse_Left", 
            &LetterInvadersState::mouseClick, this);
    evMgr->addCallback(StateType::tLetterInvaders, "Mouse_Left_Up", 
            &LetterInvadersState::mouseRelease, this);
    evMgr->addCallback(StateType::tLetterInvaders, "Key_Escape", 
            &LetterInvadersState::onEscape, this);


    loadByMethod(gLetFont, "resources/Futura.ttc");
    loadByMethod(font, "resources/Tahoma.ttf");
  
    loadTextures();
    loadSounds();
    
    gui = stateManager->getContext()->gui;
    
    bkgd.setTexture(texs[0]);
    bkgd.setTextureRect(IntRect(0, 0, ScrW * 2, ScrW * 2));
    cO(bkgd);
    bkgd.setPosition(ScrCX, ScrCY);
    
    bkgdFrame.setTexture(texs[3]);
    bkgdFrame.sP(-550, -550);
    
    beam.setFillColor(Color(255, 255, 255, 60));
    beam.setSize(vecf(0, 5));
    beam.setOrigin(0, 2);
    beam.sP(ScrCX, ScrH);
    
	curLetterSet = sets[0];
	curSetLabel = setsLabels[0];
    txt = Text("", font, 20);
    txt.setPosition(10, 80);
    txt.setOutlineColor(Color(40, 40, 40));
    txt.setOutlineThickness(1);
    txt.setFillColor(Color(210, 210, 210));
    txt.setString(
                  "SCORE: " + tS(curScore) + "\n\n" +
                  "LEVEL: " + tS(curLevel) +
				  "\n\n" + curSetLabel);

    
    animator.addAnimation("swirlexit", &swirlExit, seconds(1.f));
    animator.playAnimation("swirlexit");
       
    setUpHighScores();

    reset();
} //end onCreate



void LetterInvadersState::reset () {

    bumpers.clear();
    balls.clear();
    
    running = false;
    gameOver = false;
    auto gui = stateManager->getContext()->gui;
    gui->get("scores")->setVisible(false);
    stateManager->getContext()->win->getRenderWindow()->setMouseCursorVisible(false);

    gravAccel = initGrav;
    bumperLoss = initBumperLoss;
    numBumperColumns = initBumperCols;
    numBumperRows = initBumperRows;
    curScore = 0;
    curLevel = 1;
    spawnInterval = initSpawnInterval;
    lastSpawnTime = Time::Zero;
    bkgd.setColor(randomColor());
    
    placeBumpers();
    beam.setSize(vecf(0, 5));
    beam.setRotation(0);
}


void LetterInvadersState::placeBumpers () {

    forNum(numBumperColumns * numBumperRows) {
        Bumper b { &texs[2] };
        float cellW = (ScrW - wallThickness * 2) / numBumperColumns;
        float cellH = (ScrH - upperDeadZone - lowerDeadZone) / numBumperRows;
        auto x = i % numBumperColumns * cellW + wallThickness + randRange(0, int(cellW - 50));
        auto y = i / numBumperColumns * cellH + upperDeadZone + randRange(0, int(cellH - 50));
        b.s.sP(x, y);

        bumpers.push_back(b);
    }
}


void LetterInvadersState::draw () {
    
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
    if (!stateManager->getContext()->win->isStretched())
        w->draw(bkgdFrame);
}



void LetterInvadersState::update (const Time& elapsed) {

    float factor = cos(toRad(bkgdScaleDegree)) * .3 + 1;
    bkgd.setScale(factor, factor);
    bkgd.rotate(.05);
    bkgdScaleDegree += .1;
    if (bkgdScaleDegree >= 360)
        bkgdScaleDegree -= 360;
  
    auto gui = stateManager->getContext()->gui;
    auto sc = gui->get("scores");
    if (sc->isVisible() && sc->isFocused()) {
        return;
    }
 
	txt.setString( "SCORE: " + tS(curScore) + "\n\n" +
				  "LEVEL: " + tS(curLevel) +
				  "\n\n" + curSetLabel);
	
    if (!running || gameOver) return;

    animator.update(animClock.restart());
    
    for (auto itr = balls.begin(); itr != balls.end(); ++itr) {
        Ball& b = *itr;
        if (b.isActive && Keyboard::isKeyPressed(b.key)) {
            destroyBall(b);
            fadeTime = elapsed + seconds(.15);
        }
        
            /* Currently only using the animator to make the ball
             * swirl around off of the screen after being shot
             */
        if (!b.isActive) {
            animator.animate(b);
        }
    }

    auto p = find_if(balls.begin(), balls.end(),
                     [&](auto& x) { return !x.isActive && x.s.gP().y <= -15; });
    if (p != balls.end())
        balls.erase(p);

    if (elapsed.asSeconds() > lastSpawnTime.asSeconds() + spawnInterval)
        spawnBall(elapsed);

    if (elapsed > fadeTime)
        drawBeam = false;

    
        // COLLISION LOGIC

    float phi = 0, hyp1 = 0;
    float ax1, ay1, bx1, by1;

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
    vector<vecF> savePosns(numBalls);
    for (int i = 0; i <  highSpd; ++i) {
        for (int b = 0; b < numBalls; ++b) {
            if (!balls[b].isActive)
                continue;
            savePosns[b] = balls[b].s.gP();
            balls[b].s.move(balls[b].velocity / highSpd);
        }
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
            for (auto& obj : bumpers) {
                float radius = obj.s.gGB().width / 2;
                vecF pos = obj.s.gP();
                if (hyp(cpos, pos) > cradius + radius)
                    continue;
                s.sP(savePosns[b]);
                cpos = s.gP();
                contactAng = toDeg(atan2(cpos.y - pos.y, cpos.x - pos.x));
                newAng = contactAng + (contactAng - oldAdjst);
                c.velocity = vecF(toRect(vp.x - bumperLoss, newAng));
                
                sounds[0].setPitch( float(randRange(80, 120))/100 );
                sounds[0].play();
            }
            
            for (int bb = b + 1; bb < numBalls; ++bb) {
                if (!balls[bb].isActive)
                    continue;
                float radius = balls[bb].s.gGB().width / 2;
                vecF pos = balls[bb].s.gP();
                hyp1 = hyp(cpos, pos);
                if (hyp1 > cradius + radius)
                    continue;  // No collision
                s.sP(savePosns[b]); cpos = s.gP();
                if (hyp(cpos, pos) <= cradius + radius) balls[bb].s.sP(savePosns[bb]);

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
                
                sounds[1].play();
            } //end for bb
            
            vecF pos = s.gP();
            bool hitWall = false;
            if (pos.x > ScrW) {
                s.sP(ScrW, pos.y);
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

            balls[b].txt.sP(balls[b].s.gP() - vecF(15, 30));
      
            if (balls[b].s.gGB().intersects(FloatRect(0, ScrH + 1, ScrW, 5))) {
                endGame();
                return;
            }
        } //end for b
    } //end for i
} // end update()



void LetterInvadersState::mouseClick (EventDetails* det) {
    
    vecI mp = det->mouse;
    for (auto& obj : bumpers) {
        if (!(obj.s.gGB().contains(mp.x, mp.y)))
            continue;
        obj.clickedOn = true;
        gClickedOn = &obj;
        return;
    }
}

void LetterInvadersState::mouseRelease (EventDetails* det) {
    
    if (gClickedOn)
        gClickedOn->clickedOn = false;
    gClickedOn = nullptr;
}

void LetterInvadersState::onKeyPress (Keyboard::Key k) {
    
	vector<Keyboard::Key> setsKeys {
		Keyboard::Num1, Keyboard::Num2, Keyboard::Num3, Keyboard::Num4,
		Keyboard::Num5, Keyboard::Num6, Keyboard::Num7, Keyboard::Num8,
		Keyboard::Num9
	};
	forNum(9) {
		if (k == setsKeys[i]) {
			curLetterSet = sets[i];
			curSetLabel = setsLabels[i];
			return;
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


void LetterInvadersState::spawnBall (Time tm) {

    string let(1, toupper( curLetterSet[randRange(0, int(curLetterSet.length() - 1))] ) );
    Ball b { &texs[1], let, randElemVal(ballColors), };
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


void LetterInvadersState::destroyBall (Ball& b) {
    
    curScore += 1;
    if (!(curScore % 10)) {
        ++curLevel;
        if (spawnInterval > 1)
            spawnInterval -= 1;
        else if (spawnInterval > .4)
            spawnInterval -= .1;
        gravAccel = min(2.5, gravAccel + .01);
        bkgd.setColor(randomColor());
		levelUpSnd.play();
		sleep(seconds(.2));
		levelUpSnd.play();
		sleep(seconds(.2));
		levelUpSnd.play();
    }
    intvec iv {2, 4, 5};
    int i = randElemVal(iv);
    sounds[i].setVolume(randRange(65, 100));
    sounds[i].play();
    b.isActive = false;
    b.blastPos = b.s.gP();
    animator.playAnimation("swirlexit");
    auto q = beam.gP() - b.blastPos;
    beam.setSize(vecf(toPolar(q).x, beam.getSize().y));
    beam.setRotation(toPolar(q).y - 180);
    drawBeam = true;

}

void LetterInvadersState::endGame () {
    
    running = false;
    gameOver = true;
    sounds[3].play();
    showHighScores(curScore, curLevel);
}

void LetterInvadersState::showHighScores (int score, int level) {

    stateManager->getContext()->win->getRenderWindow()->setMouseCursorVisible(true);
    auto gui = stateManager->getContext()->gui;
    auto sc = gui->get<tgui::Panel>("scores");
    sc->setVisible(true);
    if (score >= topTen[9].score) {
//        boost::gregorian::date d(boost::gregorian::day_clock::local_day());
//        string date = tS(d.month()) + "/" + tS(d.day()) + "/" + tS(d.year());
		string date {}; ////
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
        sc->setFocused(true);
    }
    else {
        sc->setFocused(false);
        for (auto& e : eboxes) {
            e->setFocused(false);
            e->setEnabled(false);
        }
    }

}

void LetterInvadersState::enterNewScore (tgui::String str) {
    
    string s = str.toStdString();
    forNum(10) {
        if (topTen[i].player == "") {
            topTen[i].player = s == "" ? "Nameless One" : s;
            eboxes[i]->setText(highScoreToText(topTen[i]));
            eboxes[i]->setEnabled(false);
            gui->get("scores")->setFocused(false);
        }
    }
    topTenToFile();
}

string LetterInvadersState::highScoreToText (HighScore &hs) {
    
    stringstream ss {};
    ss << "  " << std::left << std::setw(10) << tS(hs.score) << std::setw(20) << hs.player << std::setw(10)
                << hs.date << std::setw(10) << tS(hs.level);
    return ss.str();
}

void LetterInvadersState::topTenToFile () {
    
    ofstream f { "liscores.txt", ios_base::trunc};
    if (!f.is_open())
        return;
    for (auto& t : topTen) {
        f << t.score << " " << t.player << " " << t.date
        << " " << t.level << "\n";
    }
    f.close();
}

const vector<string> LetterInvadersState::texList {
    "graybkgd.png",     // 0
    "grayball.png",     // 1
    "bumper.png",       // 2
    "bkgdFrame.png"     // 3
};

const vector<string> LetterInvadersState::soundFileList {
    "bumperhit.wav",    // 0
    "thump1.wav",       // 1
    "explosion1.wav",   // 2
    "gameover4.wav",    // 3
    "explosion2.wav",   // 4
    "hurt5.wav"         // 5
	
//	"Bell3.wav"
    // Update `numSounds` if adding to list
};
