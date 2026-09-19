#pragma once
#include "combat.h"
namespace pf {
// Presidential rivals only; guests and judges remain playable without lengthening the bracket.
struct Tournament {
    bool active=false;
    int player=0,stage=0;
    std::vector<int> rivals;
    void start(int selected) {
        active=true;player=validCharacter(selected);stage=0;rivals.clear();
        for(int candidate:PresidentialRoster)if(candidate!=player)rivals.push_back(candidate);
    }
    bool boss()const{return active && stage==int(rivals.size());}
    int opponent()const{return boss()?BossCharacter:rivals.at(stage);}
    bool advance(){if(!active || boss())return false;stage++;return true;}
    Match match()const{return Match(player,opponent(),boss());}
};
}
