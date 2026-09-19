#pragma once
#include "pixel.h"
#include <array>

namespace pf {
// A separate ruleset: no health, attacks, fighting AI or finite ammunition.
struct ConversionBonus {
    struct Person {float x;int lane,owner=-1;};
    struct Shot {float x;int lane,owner;};
    std::vector<Person> people;
    std::vector<Shot> shots;
    int character[2]={0,1},lane[2]={1,1},score[2]={0,0};
    float cooldown[2]={0,0},pose[2]={0,0},remaining=10,resultTime=0,clock=0;
    bool upHeld[2]={false,false},downHeld[2]={false,false},humanP2=false;
    int winner=-2;
    ConversionBonus(int a=0,int b=1){character[0]=a;character[1]=b;for(int row=0;row<3;row++)for(int col=0;col<8;col++)people.push_back({155.f+col*47.f,row,-1});}
    bool over()const{return remaining<=0;}
    void finish(){remaining=0;shots.clear();winner=score[0]==score[1]?-1:score[0]>score[1]?0:1;}
    Input cpu(float dt) {
        (void)dt;Input in;int counts[3]={0,0,0};for(auto&p:people)if(p.owner<0)counts[p.lane]++;
        int target=lane[1];if(!counts[target]){for(int n=0;n<3;n++)if(counts[n]>counts[target])target=n;}
        if(target!=lane[1]){lane[1]=target;}
        in.magic=counts[target]>0;return in;
    }
    void tick(float dt,Input p,Input q) {
        if(over()){resultTime+=dt;return;}
        dt=std::min(std::max(dt,0.f),remaining);clock+=dt;
        if(q.left||q.right||q.jump||q.crouch||q.magic||q.punch||q.kick)humanP2=true;
        if(!humanP2)q=cpu(dt);
        Input input[2]={p,q};
        for(int i=0;i<2;i++){
            if(input[i].jump && !upHeld[i])lane[i]=std::max(0,lane[i]-1);
            if(input[i].crouch && !downHeld[i])lane[i]=std::min(2,lane[i]+1);
            upHeld[i]=input[i].jump;downHeld[i]=input[i].crouch;
            cooldown[i]=std::max(0.f,cooldown[i]-dt);pose[i]=std::max(0.f,pose[i]-dt);
            if(input[i].magic && cooldown[i]<=0){shots.push_back({i?568.f:72.f,lane[i],i});cooldown[i]=i==1&&!humanP2?.32f:.24f;pose[i]=.22f;}
        }
        // Resolve swept collisions by arrival time, avoiding tunneling and P1 iteration bias.
        struct Arrival {float time;size_t shot,person;};std::vector<Arrival> arrivals;
        for(size_t s=0;s<shots.size();s++){
            const auto &b=shots[s];float end=b.x+(b.owner?-1:1)*580*dt;
            for(size_t n=0;n<people.size();n++){auto &v=people[n];if(v.owner>=0||v.lane!=b.lane)continue;
                float distance=(v.x-b.x)*(b.owner?-1:1);if(distance>=-9 && distance<=580*dt+9)arrivals.push_back({std::max(0.f,distance-9)/580,s,n});}
            shots[s].x=end;
        }
        std::sort(arrivals.begin(),arrivals.end(),[](auto&a,auto&b){return a.time<b.time;});
        std::vector<bool> used(shots.size(),false);
        for(size_t a=0;a<arrivals.size();a++){
            auto e=arrivals[a];if(used[e.shot]||people[e.person].owner>=0)continue;
            bool tie=false;for(size_t b=a+1;b<arrivals.size()&&std::abs(arrivals[b].time-e.time)<.00001f;b++)if(arrivals[b].person==e.person&&!used[arrivals[b].shot]&&shots[arrivals[b].shot].owner!=shots[e.shot].owner){used[arrivals[b].shot]=true;tie=true;}
            used[e.shot]=true;if(!tie){int owner=shots[e.shot].owner;people[e.person].owner=owner;score[owner]++;}
        }
        for(size_t s=shots.size();s-->0;)if(used[s]||shots[s].x<40||shots[s].x>600)shots.erase(shots.begin()+s);
        remaining=std::max(0.f,remaining-dt);if(remaining<=.00001f)finish();
    }
};
}
namespace art {
inline Color factionColor(int id){static const Color colors[]={{36,119,221,255},{214,40,56,255},{27,57,107,255},{243,119,25,255},{245,131,39,255},{46,162,85,255},{101,110,130,255},{51,145,195,255},{214,40,56,255},{191,146,53,255}};return id<9?colors[id]:accent(id);}
inline void factionFlag(int id,int x,int y,float t){
    Color c=factionColor(id);int wave=int(std::sin(t*7+x)*2);box(x,y,1,25,white);box(x+1,y+wave,29,16,c);
    const char* labels[]={"PL","PT","MIS","NOVO","AVA","PSD","JUS","","PT","$"};
    if(id==2){ // Tiny jaguar silhouette inspired by the supplied party-flag reference.
        box(x+1,y+wave,29,16,ink);box(x+10,y+wave+3,11,8,gold);box(x+9,y+wave+2,3,4,gold);box(x+20,y+wave+2,3,4,gold);
        box(x+14,y+wave+10,10,3,white);box(x+11,y+wave+5,2,2,ink);box(x+18,y+wave+5,2,2,ink);box(x+16,y+wave+8,3,2,ink);
    }
    else if(id==7){box(x+5,y+wave+10,19,3,white);box(x+8,y+wave+7,12,3,white);line(x+15,y+wave+2,x+15,y+wave+10,white);for(int n=0;n<6;n++)box(x+16,y+wave+3+n,n+1,1,gold);}
    else text(id<9?labels[id]:pf::characterInfo(id).faction,x+4,y+wave+5,white);
}
inline void conversionBonus(const pf::ConversionBonus &b){
    box(0,0,640,360,Color{17,29,48,255});
    for(int x=5;x<640;x+=46){box(x,78,32,29,Color{29,46,61,255});box(x+6,84,6,10,gold);}
    center("BONUS / DISPUTA PELA POPULACAO",320,10,gold,2);
    text(std::string("P1 ")+name(b.character[0]),14,37,factionColor(b.character[0]));
    text(std::string(b.humanP2?"P2 ":"CPU ")+name(b.character[1]),409,37,factionColor(b.character[1]));
    text("CONVERTIDOS "+std::to_string(b.score[0]),14,53,white);text("CONVERTIDOS "+std::to_string(b.score[1]),409,53,white);
    center(std::to_string(int(std::ceil(b.remaining))),320,36,gold,3);
    for(int row=0;row<3;row++){int y=134+row*62;box(27,y+25,585,25,Color{28,44,53,255});line(30,y+26,611,y+26,Color{58,84,88,255});}
    for(size_t n=0;n<b.people.size();n++){
        auto &p=b.people[n];int x=int(p.x),feet=160+p.lane*62,bob=p.owner<0?int(std::sin(b.clock*4+n)*1.5f):int(std::sin(b.clock*9+n)*2);
        Color c=p.owner<0?Color{131,135,141,255}:factionColor(b.character[p.owner]);
        box(x-6,feet-9,4,9,ink);box(x+2,feet-9,4,9,ink);box(x-8,feet-26+bob,16,18,c);
        box(x-6,feet-38+bob,12,12,Color{Uint8(173+n%4*17),Uint8(122+n%4*13),Uint8(89+n%4*12),255});
        box(x-6,feet-39+bob,12,4,ink);box(x-3,feet-33+bob,2,2,ink);box(x+3,feet-33+bob,2,2,ink);
        box(x-11,feet-25+bob,3,13,c);box(x+8,feet-25+bob,3,13,c);
        if(p.owner>=0)factionFlag(b.character[p.owner],x+10,feet-43+bob,b.clock);
    }
    for(int i=0;i<2;i++){pf::Fighter f;f.character=b.character[i];f.facing=i?-1:1;f.x=i?588.f:52.f;f.y=178.f+b.lane[i]*62;f.move=b.pose[i]>0?pf::Magic:pf::Idle;f.clock=.2f;fighter(f,b.clock,1,true);box(i?567:28,184+b.lane[i]*62,42,2,factionColor(f.character));}
    for(auto &s:b.shots){pf::Wave w{s.x,float(135+s.lane*62),s.owner?-1:1,s.owner};projectile(w);}
    box(0,315,640,45,ink);center("CIMA / BAIXO: MIRA   MALOTE: DISPARAR   MUNICAO LIVRE",320,322,white);
    center(b.humanP2?"MAIS CONVERSOES = +2 MALOTES NA PROXIMA PARTIDA":"P2: USE SEU CONTROLE OU TECLAS PARA ASSUMIR A CPU",320,339,gold);
    if(b.over()){
        box(55,113,530,128,Color{5,11,23,247});border(55,113,530,128,gold);
        center(b.winner<0?"EMPATE!":std::string(b.winner==0?"P1":"P2 / CPU")+" VENCEU!",320,133,gold,3);
        center(b.winner<0?"NINGUEM RECEBE MALOTES EXTRAS":"+2 MALOTES NA PROXIMA PARTIDA",320,173,white,2);
        center("A DISPUTA CONTINUA...",320,215,muted);
    }
}
}
