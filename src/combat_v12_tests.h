#pragma once
#include "combat.h"
#include <iostream>
#include <stdexcept>

inline int combatV12Tests() {
    using namespace pf;
    try {
        auto require=[](bool ok,const char *reason){if(!ok)throw std::runtime_error(reason);};
        constexpr float step=1.f/120.f;

        Match stock(3,8);stock.intro=0;
        require(stock.f[0].bags==BagCapacity && stock.f[1].bags==BagCapacity,"Both players begin with six money bags");
        stock.f[0].stun=.3f;
        require(!stock.start(stock.f[0],Magic) && stock.f[0].bags==BagCapacity,"Rejected launch does not consume stock");
        stock.f[0].stun=0;
        Input magic,duck;magic.magic=true;duck.crouch=true;
        int launches=0;bool connected=false;
        for(int n=0;n<1200;n++) {
            stock.update(step,magic,duck);
            if(stock.f[0].move==Magic && stock.f[0].connected && !connected)++launches;
            connected=stock.f[0].move==Magic && stock.f[0].connected;
        }
        require(launches==BagCapacity && stock.f[0].bags==0 && stock.f[1].bags==BagCapacity,"Held magic launches exactly six bags and uses only its player's stock");
        require(!stock.start(stock.f[0],Magic),"Empty stock rejects new launches");
        for(int n=0;n<1200;n++)stock.update(step,{},{});
        require(stock.f[0].bags==0,"Money does not refill during the round");
        stock.f[0].meter=100;
        require(stock.start(stock.f[0],Ultimate),"Ultimate remains available with zero money bags");
        stock.resetRound();
        require(stock.f[0].bags==BagCapacity && stock.f[1].bags==BagCapacity,"Round reset replenishes both stocks");

        Match summon(0,3);summon.intro=0;summon.f[0].hp=50;
        summon.f[0].x=260;summon.f[1].x=308;
        int assistHits=0,oldHp=summon.f[1].hp;float previousHit=-10.f;
        for(int n=0;n<660;n++) {
            summon.update(step,{},{});
            if(summon.f[1].hp<oldHp) {
                require(oldHp-summon.f[1].hp<=2,"Summoned Vorcaro never exceeds two damage per hit");
                require(n*step-previousHit>=1.09f,"Summoned attacks are at least 1.1 seconds apart");
                previousHit=n*step;oldHp=summon.f[1].hp;++assistHits;
            }
            require(!summon.assists[0].actor.bossBody,"Summoned ally keeps normal height and no boss bonuses");
        }
        require(assistHits>0 && assistHits<=5 && summon.assists[0].remaining==0,"Weak assist still attacks and expires after five seconds");

        Match boss(3,BossCharacter,true);boss.intro=0;
        require(boss.f[1].bossBody && bodyScale(boss.f[1])==1.25f && boss.f[1].hp==180,"Tournament boss has enlarged body and 180 HP");
        Input approach;approach.left=true;float initial=boss.f[1].x;boss.update(.1f,{},approach);
        require(std::abs(initial-boss.f[1].x-28.f)<.001f,"Final boss moves at 280 pixels per second");
        boss.damage(1,0,7,false);require(boss.f[0].hp==87,"Boss normal attacks gain six damage");
        boss.damage(1,0,10,true);require(boss.f[0].hp==73,"Boss ultimate hits gain four damage");
        boss.resetRound();require(boss.f[1].bossBody && boss.f[1].bags==BagCapacity,"Boss body and stock survive round reset");
        Match normal(3,8);normal.intro=0;boss.intro=0;
        normal.f[1].x=boss.f[1].x=370;
        normal.waves.push_back({365,FLOOR-162,1,0});boss.waves.push_back({365,FLOOR-162,1,0});
        normal.update(step,{},{});boss.update(step,{},{});
        require(normal.f[1].hp==100 && boss.f[1].hp==172,"Larger boss has a correspondingly taller projectile hitbox");

        Match easy(3,8),medium(3,8),hard(3,8);
        easy.difficulty=Difficulty::Easy;hard.difficulty=Difficulty::Hard;
        easy.ai(.001f);medium.ai(.001f);hard.ai(.001f);
        require(easy.aiTimer>medium.aiTimer && medium.aiTimer>hard.aiTimer,"CPU difficulty changes reaction delay");
        int defenses[3]{},attacks[3]{};
        Match *levels[]={&easy,&medium,&hard};
        for(int level=0;level<3;level++) {
            auto &cpu=*levels[level];cpu.f[0].x=280;cpu.f[1].x=328;
            for(int n=0;n<1500;n++) {
                cpu.aiTimer=0;cpu.f[0].move=Punch;
                defenses[level]+=cpu.ai(.01f).guard?1:0;
                cpu.aiTimer=0;cpu.f[0].move=Idle;
                auto input=cpu.ai(.01f);attacks[level]+=(input.punch || input.kick)?1:0;
            }
            cpu.resetRound();require(cpu.difficulty==static_cast<Difficulty>(level),"Difficulty persists across rounds");
        }
        require(defenses[0]<defenses[1] && defenses[1]<defenses[2],"Harder CPUs defend more consistently");
        require(attacks[0]<attacks[1] && attacks[1]<attacks[2],"Harder CPUs attack more often");
        hard.f[1].bags=0;hard.f[0].x=100;hard.f[1].x=500;
        for(int n=0;n<300;n++) {hard.aiTimer=0;require(!hard.ai(.01f).magic,"CPU never requests money after stock is empty");}

        for(int loser:{0,1}) {
            Match ko(3,8);ko.intro=0;ko.f[0].x=260;ko.f[1].x=308;
            ko.f[loser].hp=0;ko.update(step,{},{});
            require(ko.winner==1-loser && ko.koActive && ko.f[loser].knockedOut && ko.f[loser].thrown,"Lethal hit records winner and begins KO fall on either side");
            float x=ko.f[loser].x,timer=ko.remaining;
            Input mash;mash.punch=mash.magic=mash.jump=true;
            ko.update(.1f,mash,mash);
            require(ko.f[loser].y<FLOOR && std::abs(ko.f[loser].x-x-(-ko.f[loser].facing*180.f*.032f))<.01f,"KO falls backward with 32 percent simulation speed");
            require(ko.remaining==timer && ko.f[loser].bags==BagCapacity,"KO blocks inputs and freezes round timer");
            for(int n=0;n<600 && ko.f[loser].thrown;n++)ko.update(step,{},{});
            require(ko.f[loser].grounded() && ko.f[loser].knockdown>0 && ko.round==1,"KO reaches the floor before round advances");
            ko.update(.5f,{},{});require(ko.round==1,"Defeated character remains on the ground for the ending hold");
            for(int n=0;n<360 && ko.round==1;n++)ko.update(step,{},{});
            require(ko.round==2 && !ko.f[loser].knockedOut && !ko.koActive,"Next round restores both characters after fall and hold");
        }
        Match airborne(3,8);airborne.intro=0;airborne.f[1].hp=0;airborne.f[1].y=FLOOR-160;
        airborne.update(step,{},{});
        for(int n=0;n<320;n++)airborne.update(step,{},{});
        require(airborne.round==1 && airborne.koActive,"A high airborne KO cannot skip landing and ground hold at the old round timeout");
        for(int n=0;n<600 && airborne.round==1;n++)airborne.update(step,{},{});
        require(airborne.round==2,"Airborne KO eventually completes");

        Match aerialWinner(3,8);aerialWinner.intro=0;aerialWinner.f[0].y=FLOOR-80;aerialWinner.f[1].hp=0;
        aerialWinner.update(step,{},{});float winnerY=aerialWinner.f[0].y;
        Input jumpDuringKo;jumpDuringKo.jump=true;
        for(int n=0;n<230;n++)aerialWinner.update(step,jumpDuringKo,{});
        require(aerialWinner.f[0].y>winnerY && aerialWinner.f[0].grounded() && aerialWinner.round==1,"Airborne survivor lands during KO without accepting jump input");

        Match doubleKo(3,8);doubleKo.intro=0;doubleKo.f[0].x=260;doubleKo.f[1].x=308;doubleKo.f[0].hp=doubleKo.f[1].hp=7;
        Input punch;punch.punch=true;doubleKo.update(step,punch,punch);
        for(int n=0;n<30 && doubleKo.winner==-2;n++)doubleKo.update(step,{},{});
        require(doubleKo.winner==-1 && doubleKo.f[0].knockedOut && doubleKo.f[1].knockedOut,"Simultaneous lethal punches produce two KO falls and a draw");
        require(doubleKo.f[0].wins==0 && doubleKo.f[1].wins==0,"Double KO awards no round win");

        Match timeout(3,8);timeout.intro=0;timeout.remaining=0;timeout.f[1].hp=80;timeout.update(step,{},{});
        require(timeout.winner==0 && !timeout.koActive && !timeout.f[1].knockedOut,"Timeout preserves ordinary score result without faking a KO");
        Match throwKo(3,8);throwKo.intro=0;throwKo.f[0].x=260;throwKo.f[1].x=308;throwKo.f[1].hp=4;
        throwKo.grab(0,1);throwKo.update(.53f,{},{});
        require(throwKo.winner==-2 && throwKo.f[1].thrown,"Lethal grab retains its original airborne throw");
        for(int n=0;n<300 && throwKo.winner==-2;n++)throwKo.update(step,{},{});
        require(throwKo.winner==0 && throwKo.f[1].knockedOut && !throwKo.f[1].thrown && throwKo.f[1].grounded(),"Lethal grab ends on the floor without a second launch");
        std::cout<<"PASS: v12 difficulty, limited bags, weak summon, strong taller boss, slow-motion KO, double KO and timeout\n";
        return 0;
    } catch(const std::exception &e) {
        std::cerr<<"FAIL v12 combat: "<<e.what()<<'\n';return 1;
    }
}
