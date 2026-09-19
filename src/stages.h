#pragma once
#include "presentation.h"
#include "bonus.h"
namespace art {
inline const char* stageName(int host){static const char* names[]={"PLANALTO / PRACA DOS TRES PODERES","SITIO EM ATIBAIA","CLUBE DO DEBATE / MISSAO","FARIA LIMA / BOLSA DO COMBO","PATIO DOS DRONES","EXPO AGRO / FEIRA DO INTERIOR","PRACA DA JUSTICA","MARINA / GAME OVER TV","ESPLANADA DO ORCAMENTO"};return host==pf::BossCharacter?"CAMAROTE MASTER":names[pf::characterInfo(host).stage];}
inline void civilian(int x,int feet,float t,int facing,int team,bool attacking,bool duck,bool hurt){
    Color c=team==0?Color{229,188,43,255}:team==1?Color{204,50,61,255}:team==2?Color{55,116,196,255}:team==3?Color{231,118,40,255}:Color{140,72,154,255};
    Color skin{Uint8(160+team*15),Uint8(109+team*12),Uint8(79+team*10),255};
    int y=feet-(duck?-7:0),lean=hurt?-facing*4:0,swing=int(std::sin(t*9)*2);
    box(x-6+swing,feet-9,4,9,ink);box(x+2-swing,feet-9,4,9,ink);
    box(x-8+lean,y-26,16,18,c);box(x-6+lean,y-38,12,12,skin);box(x-6+lean,y-39,12,4,ink);
    box(x+lean+(facing>0?3:-4),y-33,2,2,ink);box(x+lean-2,y-28,hurt?5:3,2,ink);
    if(team==0){box(x-8+lean,y-25,16,2,green);box(x-1+lean,y-24,3,9,green);}
    if(team==1)star(x+lean,y-19,0,4);
    int front=x+lean+facing*7,back=x+lean-facing*10;
    box(back-2,y-25,4,13,c);box(back-2,y-14,5,5,skin);
    if(attacking){box(std::min(front,front+facing*14),y-26,17,5,c);box(front+facing*17-2,y-27,6,6,skin);}
    else{box(front-2,y-27,4,13,c);box(front-3,y-30,6,6,skin);}
}
inline void helicopter(float t,int variant){
    float loop=std::fmod(t+variant*3.f,22.f);if(loop>14)return;
    int dir=variant%2?-1:1,x=dir>0?-80+int(loop*58):720-int(loop*58),y=120+int(std::sin(t*1.2f)*5);
    line(x,y+9,x-dir*22,y+61,Color{226,207,136,55});line(x,y+9,x+dir*28,y+61,Color{226,207,136,55});
    box(x-19,y-10,39,17,ink);box(x-16,y-8,33,12,Color{204,211,200,255});box(x+(dir>0?3:-15),y-7,12,9,Color{54,115,151,255});
    box(x-dir*31-8,y-7,22,4,muted);box(x-dir*42,y-14,4,17,white);
    box(x-2,y-18,4,9,muted);int span=int(t*40)%2?37:25;box(x-span,y-20,span*2,2,ink);
    box(x-14,y+6,2,5,ink);box(x+10,y+6,2,5,ink);box(x-20,y+11,40,2,ink);
    box(x-3,y+7,7,7,Color{59,67,76,255});box(x-1,y+11,3,3,red);
    text("TV",x-8,y-6,ink);if(int(t*3)%2)box(x+17,y-10,3,3,red);
}
} // namespace art
#include "themed_stages.h"
namespace art {
inline void battleArena(float t,const pf::Match &m){
    int host=pf::characterInfo(m.f[1].character).stage;Color c=factionColor(host),other=factionColor(m.f[0].character);
    if(detailedStage(host)){detailedBackdrop(host,t);helicopter(t,host);detailedFloor(host);return;}
    static const Color skies[]={{71,107,139,255},{147,89,92,255},{76,123,126,255},{109,136,151,255},{46,76,117,255},{182,143,101,255},{76,87,113,255},{57,109,152,255},{89,123,127,255}};
    box(0,0,640,284,skies[host]);box(512,91,22,22,gold);
    for(int n=0;n<4;n++)box((n*179+int(t*4))%750-50,101+n%2*9,58,3,Color{174,178,170,130});
    if(host==3||host==5||host==2){
        for(int x=0;x<640;x+=8){int top=116-int(std::sin(x*.014f)*15+std::cos(x*.035f)*9);box(x,top,8,95,Color{61,105,90,255});}
        if(host==2){for(int x:{45,140,510,590}){box(x,139,7,52,Color{95,68,44,255});box(x-19,119,46,33,green);}}
        else if(host==3){box(218,124,205,62,Color{227,214,184,255});box(245,108,28,74,white);box(371,108,28,74,white);for(int x=224;x<410;x+=23)box(x,143,9,17,Color{57,81,95,255});}
        else{for(int x=18;x<640;x+=124){box(x,145,103,13,c);box(x+7,158,4,34,white);box(x+92,158,4,34,white);box(x+13,178,78,14,Color{118,78,48,255});}}
    } else if(host==7){
        box(0,134,640,61,Color{34,93,124,255});for(int n=0;n<34;n++)box((n*39+int(t*16))%640,142+n%6*8,22,2,Color{101,157,171,255});
        for(int y=0;y<17;y++)box(205+y*2,161+y,237-y*4,1,white);
        box(259,137,105,24,white);box(280,128,60,10,muted);box(301,110,3,24,white);
        for(int x=269;x<351;x+=21)box(x,142,13,8,Color{42,66,91,255});
        for(int x=20;x<640;x+=76)box(x,170,7,38,Color{99,71,52,255});
    } else {
        Color walls=host==6?Color{193,196,195,255}:Color{127,146,148,255};
        for(int n=0;n<7;n++){int x=n*99,height=38+(n*19+host*13)%45;box(x,181-height,87,height,walls);for(int w=8;w<78;w+=17)box(x+w,155-height,8,10,Color{64,86,111,255});}
        if(host==6||host==8){box(179,126,282,51,white);box(169,119,302,7,muted);for(int x=191;x<447;x+=32)box(x,126,8,49,Color{127,152,156,255});box(275,139,90,17,ink);center(host==6?"JUSTICA":"ORCAMENTO",320,144,gold);}
        if(host==0||host==1){box(225,146,190,46,Color{46,55,68,255});box(225,140,190,9,c);center(host==0?"CARREATA":"PALANQUE",320,157,white,2);box(234,168,34,10,ink);box(372,168,34,10,ink);}
        if(host==4){box(224,143,190,41,Color{38,52,69,255});center("CENTRAL DOS DRONES",320,153,gold);for(int n=0;n<3;n++){int x=230+n*80,y=129+int(std::sin(t*3+n)*8);box(x,y,10,4,muted);line(x-8,y-3,x+18,y-3,white);}}
    }
    box(0,191,640,87,Color{116,111,98,255});
    for(int x=0;x<640;x+=85){box(x,197,74,3,muted);box(x+5,197,3,18,ink);}
    for(int side=0;side<2;side++){int x=side?615:23;box(x,109,2,86,white);box(side?x-42:x+2,109,42,22,side?c:other);center(side?"P2":"P1",side?x-21:x+23,116,white);}
    // Opposing spectators trade punches, block, duck and recoil independently of gameplay.
    int crowdIds[9],count=0;for(int id=0;id<9;id++)if(id!=m.f[0].character&&id!=m.f[1].character)crowdIds[count++]=id;
    for(int pair=0;pair<(count+1)/2;pair++){
        int mid=72+pair*166;float phase=std::fmod(t*1.4f+pair*.7f,2.f);
        for(int side=0;side<2;side++){
            if(pair*2+side>=count){civilian(mid+29,218,t,-1,2,phase>=1,false,false);continue;}
            pf::Fighter p;p.character=crowdIds[pair*2+side];p.x=float(mid+(side?29:-29));p.y=218;p.facing=side?-1:1;
            bool attack=(phase<1)==(side==0);p.guard=!attack;p.move=attack?pf::Punch:pf::Idle;p.comboStage=pair%3+1;p.clock=.06f+std::fmod(phase,.34f);fighter(p,t,1,true);}
    }
    for(int pair=0;pair<8;pair++){
        int x=34+pair*82;float phase=std::fmod(t*1.9f+pair*.41f,2.f);bool turn=phase<1;float strike=std::fmod(phase,1.f);
        civilian(x-16,265,t,1,pair%5,turn&&strike<.38f,!turn&&strike>.55f,!turn&&strike<.2f);
        civilian(x+16,265,t,-1,(pair+1)%5,!turn&&strike<.38f,turn&&strike>.55f,turn&&strike<.2f);
    }
    helicopter(t,host);
    box(0,276,640,8,Color{46,55,67,255});box(0,284,640,76,Color{29,41,57,255});
    for(int y=297;y<360;y+=19)box(0,y,640,1,Color{58,72,86,255});
    for(int x=-160;x<850;x+=64)line(320+(x-320)/2,285,x,360,Color{58,72,86,255});
}
}
namespace pf {
struct Reporter {
    std::string line="CAMERAS AO VIVO. OS DOIS ESTAO NO RINGUE.";
    float hold=0;int priority=0,lastWinner=-2;int previousHp[2]={100,100};Move previousMove[2]={Idle,Idle};int previousStage[2]={0,0};
    void say(const std::string &s,int level){if(hold>0 && level<=priority)return;line=s;priority=level;hold=level>=4?3.8f:2.8f;}
    void reset(const Match&m){*this=Reporter{};for(int i=0;i<2;i++)previousHp[i]=m.f[i].hp;}
    void update(float dt,const Match&m){
        hold=std::max(0.f,hold-dt);if(hold<=0)priority=0;
        if(m.winner!=-2 && lastWinner==-2)say(m.winner<0?"OS DOIS CAIRAM! QUE FINAL DE ROUND!":std::string(art::cinema::shortName(m.f[m.winner].character))+" LEVA O ROUND! A CAMERA PEGOU TUDO!",5);
        lastWinner=m.winner;
        for(int i=0;i<2;i++){
            const auto &f=m.f[i];std::string who=art::cinema::shortName(f.character);
            if(f.move==Ultimate && previousMove[i]!=Ultimate)say(who+" SOLTOU O ESPECIAL! OLHO NO RINGUE!",4);
            else if(f.move==Grab && previousMove[i]!=Grab)say(who+" TENTA O AGARRAO! VAI TER ARREMESSO?",3);
            else if(f.comboStage==3 && previousStage[i]!=3 && (f.move==Punch||f.move==Kick))say(who+" EMENDA O TERCEIRO GOLPE!",3);
            else if(f.hp<previousHp[i])say(who+" SENTIU O GOLPE! A DISPUTA APERTOU!",2);
            previousMove[i]=f.move;previousStage[i]=f.comboStage;previousHp[i]=f.hp;
        }
        if(hold<=0){
            int lead=m.f[0].hp==m.f[1].hp?-1:m.f[0].hp>m.f[1].hp?0:1;
            say(m.remaining<12?"RETA FINAL! O TEMPO ESTA ACABANDO!":lead<0?"TUDO IGUAL! NINGUEM CEDE NO RINGUE!":std::string(art::cinema::shortName(m.f[lead].character))+" ESTA NA FRENTE NESTE ROUND!",1);
        }
    }
};
}
namespace art {
inline void reporterPanel(const pf::Reporter &reporter,float t,int host){
    box(392,81,239,51,Color{7,13,24,232});border(392,81,239,51,Color{101,139,158,255});
    box(398,98,17,17,Color{217,165,127,255});box(398,96,17,5,ink);box(400,111,13,15,Color{75,97,127,255});
    box(401,103,2,2,ink);box(410,103,2,2,ink);box(405,108,int(t*9)%2?5:3,2,ink);box(413,111,3,12,muted);box(411,110,7,5,ink);
    text("TV PLENARIO",421,87,gold);text("AO VIVO",580,87,red);
    wrapped(reporter.line,421,99,203,white);(void)host;
}
}
