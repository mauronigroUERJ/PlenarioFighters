#pragma once
#include "pixel.h"
#include "portraits_data.h"
#include "portraits_expansion.h"
#include "portraits_roster.h"
#include "portraits_historical.h"

// Cinematics use the same logical 640 x 360 pixel canvas as the fights.
namespace art {
inline SDL_Texture *faceAtlas=nullptr,*expansionAtlas=nullptr,*rosterAtlas=nullptr,*historicalAtlas=nullptr;
inline SDL_Texture* portraitTexture(const unsigned char *data,int size){
    SDL_Surface* surface=SDL_LoadBMP_RW(SDL_RWFromConstMem(data,size),1);
    if(!surface)return nullptr;
    SDL_Texture* texture=SDL_CreateTextureFromSurface(r,surface);SDL_FreeSurface(surface);return texture;
}
inline void initPortraits(){faceAtlas=portraitTexture(PortraitBMP,sizeof(PortraitBMP));expansionAtlas=portraitTexture(ExpansionBMP,sizeof(ExpansionBMP));rosterAtlas=portraitTexture(RosterBMP,sizeof(RosterBMP));historicalAtlas=portraitTexture(HistoricalBMP,sizeof(HistoricalBMP));}
inline void closePortraits(){if(faceAtlas)SDL_DestroyTexture(faceAtlas);if(expansionAtlas)SDL_DestroyTexture(expansionAtlas);if(rosterAtlas)SDL_DestroyTexture(rosterAtlas);if(historicalAtlas)SDL_DestroyTexture(historicalAtlas);faceAtlas=expansionAtlas=rosterAtlas=historicalAtlas=nullptr;}
inline bool hasPortrait(int id){return id>=0 && id<=pf::BossCharacter && (id<9?faceAtlas!=nullptr:id>=12 && id<=26?rosterAtlas!=nullptr:id==pf::Dilma || id==pf::Temer?historicalAtlas!=nullptr:expansionAtlas!=nullptr);}
inline void drawPortrait(int id,SDL_Rect dst,bool flip=false){
    if(hasPortrait(id)){
        int index=id,cols=3;SDL_Texture *tex=faceAtlas;
        if(id==pf::Dilma || id==pf::Temer){tex=historicalAtlas;cols=2;index=id-pf::Dilma;}
        else if(id>=12 && id<=26){tex=rosterAtlas;cols=3;index=id-12;}
        else if(id>=9){tex=expansionAtlas;cols=2;index=id==pf::Jair?0:id==pf::BossCharacter?1:id==pf::Kim?2:3;}
        SDL_Rect from{(index%cols)*128+3,(index/cols)*128+2,122,122};
        SDL_RenderCopyEx(r,tex,&from,&dst,0,nullptr,flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
    } else {
        box(dst.x,dst.y,dst.w,dst.h,ink);
        int scale=std::max(1,dst.h/65);pf::Fighter f;f.character=id;f.facing=flip?-1:1;
        f.x=float(dst.x+dst.w/2);f.y=float(dst.y+dst.h+scale*10);fighter(f,0,scale,true);
    }
}
namespace cinema {
constexpr float OpeningDuration=29.f, EndingDuration=10.f;
struct Blend {
    SDL_BlendMode previous=SDL_BLENDMODE_NONE;
    Blend(){SDL_GetRenderDrawBlendMode(r,&previous);SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);}
    ~Blend(){SDL_SetRenderDrawBlendMode(r,previous);}
};
inline float unit(float v){return std::clamp(v,0.f,1.f);}
inline float ease(float v){v=unit(v);return v*v*(3.f-2.f*v);}
inline const char* shortName(int id) {
    return id==pf::CastBackground?"C. CASTRO":pf::characterInfo(id).shortName;
}
inline void rays(float t,Color color,int cx=320,int cy=180) {
    for(int n=0;n<36;n++) {
        float a=n*.174533f+t*.08f;
        int near=65+(n*29)%145;
        int far=near+75+int(std::fmod(t*140+n*61,170.f));
        int x=cx+int(std::cos(a)*near),y=cy+int(std::sin(a)*near*.64f);
        int X=cx+int(std::cos(a)*far),Y=cy+int(std::sin(a)*far*.64f);
        line(x,y,X,Y,color);
        if(n%3==0)line(x,y+2,X,Y+2,color);
    }
}
inline void bands(const std::string& title,const std::string& caption) {
    box(0,0,640,35,ink);center(title,320,11,gold,2);
    box(0,320,640,40,ink);center(caption,320,326,white);
}
inline void officer(int x,int feet,float t,int facing=1,bool walking=true) {
    Color uniform{32,46,53,255}, vest{20,30,33,255}, skin{194,145,109,255};
    int stride=walking?int(std::sin(t*10)*4):0;
    box(x-12,feet-2,26,3,Color{10,13,20,160});
    box(x-8+stride,feet-13,6,13,uniform);box(x+2-stride,feet-13,6,13,uniform);
    box(x-10+stride,feet-3,9,4,ink);box(x+1-stride,feet-3,9,4,ink);
    box(x-11,feet-37,23,26,uniform);box(x-9,feet-36,19,22,vest);
    box(x-8,feet-50,17,14,skin);box(x-10,feet-53,21,7,ink);
    box(x+(facing>0?2:-13),feet-48,12,3,ink);box(x-1,feet-43,4,2,ink);
    box(x-15,feet-34,6,20,uniform);box(x+10,feet-34,6,20,uniform);
    box(x-15,feet-17,6,6,skin);box(x+10,feet-17,6,6,skin);
    text("PF",x-5,feet-31,gold);box(x-10,feet-14,22,4,ink);
}
inline void cuffs(int x,int y,bool broken=false,float t=0) {
    int spread=broken?int(ease(t)*15):0;
    border(x-12-spread,y,7,6,white);border(x+5+spread,y,7,6,white);
    if(!broken){line(x-5,y+3,x+5,y+3,muted);box(x-2,y+2,3,3,white);}
    else {line(x-5-spread,y+3,x-2-spread,y+1,muted);line(x+2+spread,y+1,x+5+spread,y+3,muted);}
}
inline void castro(int x,int feet,float t,bool free=false,bool cuffed=false) {
    int bob=int(std::sin(t*3)*1.2f);
    Color suit{54,70,89,255},skin{210,163,131,255};
    box(x-10,feet-16,8,16,ink);box(x+3,feet-16,8,16,ink);
    box(x-14,feet-47,28,33,suit);box(x-3,feet-46,5,22,gold);
    box(x-11,feet-70+bob,23,24,skin);box(x-11,feet-71+bob,23,6,Color{60,45,36,255});
    box(x-6,feet-60+bob,3,2,ink);box(x+5,feet-60+bob,3,2,ink);box(x-3,feet-51+bob,10,2,white);
    if(cuffed){box(x-18,feet-45,6,17,suit);box(x+12,feet-45,6,17,suit);box(x-15,feet-33,30,7,suit);box(x-11,feet-33,7,7,skin);box(x+4,feet-33,7,7,skin);}
    else {
        box(x-18,feet-45,6,22,suit);box(x-18,feet-27,6,7,skin);
        if(free){box(x+15,feet-58,5,29,suit);box(x+13,feet-62,8,8,skin);}
        else{box(x+12,feet-45,6,22,suit);box(x+12,feet-27,6,7,skin);}
    }
}
inline void patron(int id,int x,int feet,float t,bool cuffed,bool released=false) {
    if(id==pf::CastBackground)castro(x,feet,t,released,cuffed);
    else {
        pf::Fighter p;p.character=id;p.x=float(x);p.y=float(feet);p.facing=x<320?1:-1;
        p.guard=released;p.cinematicCuffed=cuffed;
        fighter(p,t,1,true);
    }
    if(cuffed) {
        cuffs(x,feet-32);
    }
}
inline void van(int x,int y,float t,bool doorOpen) {
    Color steel{35,48,58,255};
    box(x,y,90,39,ink);box(x+2,y+2,85,33,steel);
    box(x+6,y+6,29,15,Color{78,111,128,255});box(x+37,y+6,20,15,Color{78,111,128,255});
    box(x+4,y+29,81,4,gold);box(x+64,y+5,20,26,doorOpen?ink:steel);
    border(x+62,y+3,24,30,muted);if(doorOpen)box(x+86,y+3,5,30,muted);
    box(x+11,y+35,15,8,ink);box(x+64,y+35,15,8,ink);
    text("PF",x+40,y+24,gold);box(x+32,y-4,26,5,ink);
    box(x+34,y-5,10,4,int(t*9)%2?red:Color{77,32,45,255});
    box(x+46,y-5,10,4,int(t*9)%2?Color{28,58,93,255}:Color{103,179,255,255});
}
inline void confetti(float t,Color c) {
    for(int n=0;n<35;n++) {
        int x=(n*89+int(std::sin(t*2+n)*11)+640)%640;
        int y=int(std::fmod(n*37+t*24,282.f))+36;
        box(x,y,n%2?4:2,n%2?2:4,n%3?c:white);
    }
}
} // namespace cinema

inline void opening(float t) {
    cinema::Blend blend;
    t=std::max(0.f,t);
    if(t>=5.5f && t<20.5f){
        float cut=t-5.5f;const int featured[]={0,1,2,3,4,5,6,7,8,pf::Jair,pf::Kim,pf::Arthur,pf::Dilma,pf::Temer,pf::BossCharacter};int shot=std::min(14,int(cut)),id=featured[shot];float phase=cut-shot;Color color=accent(id);
        box(0,0,640,360,ink);cinema::rays(t*4,color,210,160);
        int pan=int(cinema::ease(phase)*12);
        SDL_Rect dest{15-pan,30,320,320};
        drawPortrait(id,dest);
        for(int n=0;n<9;n++)box(362+n*28,100+n*4,150,2,Color{color.r,color.g,color.b,70});
        text("0"+std::to_string(id+1),365,67,color,4);
        text(cinema::shortName(id),347,146,white,3);
        text(ultimateName(id),347,185,color);
        for(int n=0;n<15;n++)box(349+n*18,238,13,4,n==shot?gold:muted);
        cinema::bands("MUITOS RIVAIS / UM PLENARIO","APERTE QUALQUER BOTAO PARA CONTINUAR");return;
    }
    if(t>=20.5f)t-=15.f;
    box(0,0,640,360,ink);
    if(t<2.f) {
        arena(t);
        box(0,0,640,360,Color{7,11,22,185});cinema::rays(t,Color{235,185,91,130});
        int shift=int((1-cinema::ease(t/.8f))*70);
        center("MUITOS RIVAIS",320,116+shift,gold,3);
        if(t>.55f)center("UMA DISPUTA IMPOSSIVEL",320,159,white,2);
        if(t>1.15f)center("E UM ULTIMO CONTRATO...",320,199,red,2);
        box(0,0,640,40,ink);center("UMA SATIRA EM PIXEL ART",320,17,muted);
    } else if(t<5.5f) {
        cinema::rays(t,Color{66,101,125,150});
        for(int id=0;id<9;id++) {
            float progress=cinema::ease((t-2.f-id*.18f)/.5f);
            int col=id%3,row=id/3;
            int x=9+col*210+int((1-progress)*(row%2?-650:650)),y=43+row*94;
            if(x>640 || x+202<0)continue;
            Color c=accent(id);
            box(x,y,202,89,Color{Uint8(c.r/7+8),Uint8(c.g/7+9),Uint8(c.b/7+13),255});
            border(x,y,202,89,c);box(x+1,y+1,4,87,c);
            for(int n=0;n<4;n++)line(x+71+n*28,y+5,x+45+n*28,y+83,Color{c.r,c.g,c.b,60});
            pf::Fighter p;p.character=id;p.x=float(x+43);p.y=float(y+84);p.facing=1;
            p.move=id%2?pf::Kick:pf::Punch;p.clock=id%2?.23f:.13f;p.comboStage=id%3+1;
            SDL_Rect clip{x+5,y+2,196,84};SDL_RenderSetClipRect(r,&clip);fighter(p,t,1,true);SDL_RenderSetClipRect(r,nullptr);
            center(cinema::shortName(id),x+137,y+29,c);
            center(id%2?"CHUTE PRONTO":"PUNHOS PRONTOS",x+137,y+45,white);
            text("0"+std::to_string(id+1),x+174,y+73,muted);
        }
        box(0,0,640,36,ink);center("29 NOMES. NENHUM CONSENSO.",320,12,gold,2);
    } else if(t<9.f) {
        bossArena(t);box(0,132,640,180,Color{39,21,42,245});
        for(int n=0;n<7;n++) {
            int id=n+2,x=51+n*89;
            cinema::patron(id,x,207,t,false,true);
            box(x+18,171,6,10,gold);box(x+18,169,6,3,white);
        }
        float entry=cinema::ease((t-5.5f)/.6f);
        pf::Fighter a,b,boss;
        a.character=0;a.x=170-(1-entry)*130;a.y=308;a.facing=1;
        b.character=1;b.x=470+(1-entry)*130;b.y=308;b.facing=-1;
        boss.character=pf::BossCharacter;boss.x=320;boss.y=306;boss.facing=1;
        bool left=int((t-5.5f)*1.4f)%2==0;
        a.move=left?pf::Punch:pf::Idle;b.move=left?pf::Idle:pf::Punch;a.clock=b.clock=.13f;a.comboStage=b.comboStage=1;
        boss.facing=left?-1:1;boss.move=pf::Punch;boss.clock=.13f;boss.comboStage=1;
        fighter(a,t,2,true);fighter(b,t,2,true);fighter(boss,t,2,true);
        int toast=213-int(std::sin((t-5.5f)*2.5f)*5);
        box(349,toast,13,17,gold);box(349,toast-2,13,5,white);border(362,toast+3,5,8,white);
        cinema::bands("AMIGO DE TODOS","APERTOS DE MAO. BRINDES. ALIANCAS.");
        center("VORCARO",320,46,gold,2);
    } else if(t<12.f) {
        for(int y=0;y<360;y+=5)box(0,y,640,5,Color{Uint8(31+y/17),8,23,255});
        cinema::rays(t*2,Color{238,73,86,160},320,173);
        pf::Fighter boss;boss.character=pf::BossCharacter;boss.x=320;boss.y=328;boss.facing=1;
        fighter(boss,t,3,true);box(235,92,178,235,Color{4,5,14,175});
        box(312,142,8,4,red);box(345,142,8,4,red);
        for(int id=0;id<9;id++)cinema::patron(id,40+id*70,312,t,false);
        int cut=int(cinema::ease((t-9.f)/.7f)*640);
        for(int n=0;n<cut;n+=8){int y=180+int(std::sin(n*.035f)*12);box(n,y,10,3,red);}
        cinema::bands("INIMIGO DE TODOS","NO FIM, SO UM SAI LIVRE.");
        if(t<9.13f)box(0,35,640,285,Color{255,244,212,Uint8(200*(1-(t-9.f)/.13f))});
    } else {
        for(int y=0;y<360;y+=4)box(0,y,640,4,Color{Uint8(10+y/36),Uint8(17+y/27),Uint8(31+y/20),255});
        cinema::rays(t,Color{215,159,69,125});
        for(int id=0;id<9;id++) {
            pf::Fighter p;p.character=id;p.x=float(40+id*70);p.y=310;p.facing=id<5?1:-1;
            p.move=id%2?pf::Kick:pf::Punch;p.comboStage=3;p.clock=id%2?.22f:.12f;
            fighter(p,t,1,true);
        }
        box(69,63,502,148,Color{8,14,26,230});border(69,63,502,148,gold);
        center("PLENARIO",320,84,gold,5);center("FIGHTER",320,130,white,5);
        center("O ULTIMO CONTRATO",320,184,red,2);
        cinema::confetti(t,gold);
        box(0,0,640,30,ink);center("UMA SATIRA. UM TORNEIO. NENHUM INOCENTE NO COMBO.",320,11,muted);
    }
    box(0,339,640,21,ink);center("ENTER / START - PULAR ABERTURA",320,346,muted);
}

// Draw after bossArena() and before the fighting sprites and HUD.
inline void policeRaid(float t,int player,bool resolved=false,bool playerWon=false) {
    cinema::Blend blend;
    const float arrival=cinema::ease((t-3.f)/2.4f);
    if(arrival<=0 && !resolved)return;
    const bool release=resolved && !playerWon;
    const float releaseProgress=release?cinema::ease((t-9.f)/2.4f):0.f;
    float visibility=resolved?1.f:arrival;
    if(int(t*7)%2==0)box(0,70,640,164,Color{209,37,59,Uint8(18*visibility)});
    else box(0,70,640,164,Color{42,122,240,Uint8(22*visibility)});
    if(t>=5.7f || resolved) {
        // Replace the drinking row with its cuffed or newly released poses.
        box(4,140,632,96,Color{69,24,47,255});box(4,231,632,4,Color{134,47,79,255});
        for(int id=0;id<9;id++) {
            if(resolved && playerWon && id==player)continue;
            int x=32+id*59;
            if(release)x+=int(std::sin(t*3+id)*releaseProgress*4);
            cinema::patron(id,x,220,t,!release || releaseProgress<.48f,releaseProgress>=.48f);
            if(release && releaseProgress>.2f && releaseProgress<.86f)
                cinema::cuffs(x,191-int(releaseProgress*25),true,releaseProgress);
        }
        cinema::patron(pf::CastBackground,600,220,t,!release || releaseProgress<.48f,releaseProgress>=.48f);
        center("CLAUDIO",600,224,white);
    }
    float position=(resolved?1.f:arrival)*(1-releaseProgress);
    for(int n=0;n<4;n++) {
        int target=52+n*178;
        int origin=n<2?-45-n*45:700+(n-2)*45;
        int x=origin+int((target-origin)*position);
        cinema::officer(x,239,t+n,n<2?1:-1,position<.97f);
    }
    if(!release || releaseProgress<.6f) {
        box(226,225,188,17,ink);border(226,225,188,17,gold);
        center(t<5.7f&&!resolved?"POLICIA FEDERAL - PARADOS!":"PF - FIM DE FESTA",320,230,gold);
    } else {
        box(244,225,152,17,ink);center("TODOS LIBERADOS",320,230,green);
    }
}

inline void ending(float t,bool playerWon,int player) {
    cinema::Blend blend;
    player=pf::validCharacter(player);t=std::max(0.f,t);
    bossArena(t);
    policeRaid(8.f+t,player,true,playerWon);
    if(playerWon && t<4.f) {
        float escort=cinema::ease((t-.6f)/3.4f);
        pf::Fighter champion;champion.character=player;champion.x=132;champion.y=300;champion.guard=true;
        fighter(champion,t,2,true);
        cinema::van(478,249,t,true);
        int bx=338+int(escort*126);
        cinema::patron(pf::BossCharacter,bx,298,t,true);
        cinema::officer(bx-43,302,t,1);cinema::officer(bx+42,302,t+.9f,1);
        for(int n=0;n<5;n++)line(422+n*8,277-n%2*6,438+n*8,277-n%2*6,gold);
        cinema::bands("OPERACAO FIM DE FESTA","VORCARO CAIU. A CASA TAMBEM.");
    } else if(playerWon) {
        box(0,35,640,285,Color{13,24,34,248});
        for(int x=0;x<640;x+=20)line(x,36,x,319,Color{24,40,53,255});
        float slide=cinema::ease((t-4.f)/1.3f);
        int shift=int((1-slide)*450);
        box(12,88,211,215,Color{21,49,50,255});border(12,88,211,215,green);
        center("VOCE ESTA LIVRE",117,103,green,2);
        pf::Fighter champion;champion.character=player;champion.x=113;champion.y=288;champion.guard=true;
        fighter(champion,t,2,true);
        center(cinema::shortName(player),117,291,gold);
        int slot=0;
        for(int member=0;member<=10;member++) {
            int id=member==9?pf::BossCharacter:member==10?pf::CastBackground:member;
            if(id==player)continue;
            int col=slot%5,row=slot/5,x=238+col*78+shift,y=94+row*103;
            box(x,y,74,99,ink);border(x,y,74,99,muted);
            SDL_Rect clip{x+1,y+1,72,86};SDL_RenderSetClipRect(r,&clip);
            cinema::patron(id,x+37,y+80,t,true);
            for(int rail=0;rail<5;rail++){box(x+7+rail*14,y+1,3,86,Color{116,137,148,255});box(x+7+rail*14,y+1,1,86,white);}
            box(x+1,y+42,72,3,muted);SDL_RenderSetClipRect(r,nullptr);
            center(cinema::shortName(id),x+37,y+90,white);
            ++slot;
        }
        center("GERAL PRESO",432+shift,74,gold,2);
        cinema::bands("TORNEIO CONCLUIDO","O CAMPEAO SAI PELA PORTA DA FRENTE.");
        box(24,53,180,18,green);center("ULTIMO CONTRATO RASGADO",114,59,ink);
    } else {
        float freedom=cinema::ease((t-1.f)/3.f);
        box(0,244,640,76,Color{42,25,46,255});
        cinema::van(501+int(freedom*160),260,t,t<2.5f);
        for(int n=0;n<2;n++)cinema::officer(168+n*304+int(freedom*(n?270:-270)),307,t,n?1:-1);
        pf::Fighter boss;boss.character=pf::BossCharacter;boss.x=320;boss.y=314;boss.guard=t>2;
        fighter(boss,t,2,true);
        if(t>3.6f) {
            box(0,140,640,94,Color{69,24,47,255});
            for(int id=0;id<9;id++)cinema::patron(id,32+id*59,223,t,false,true);
            cinema::patron(pf::CastBackground,600,223,t,false,true);
            // Keep Vorcaro in front when the entire company is released.
            fighter(boss,t,2,true);cinema::confetti(t,gold);
        }
        if(t>1.f && t<4.f)for(int n=0;n<4;n++)cinema::cuffs(93+n*150,265-int(freedom*44),true,freedom);
        cinema::bands(t<3.6f?"VORCARO VENCEU":"TODOS SOLTOS",t<3.6f?"AS ALGEMAS SE ABREM...":"O CONTRATO CONTINUA. A FESTA RECOMECA.");
        if(t>=4.f){box(174,54,292,26,ink);border(174,54,292,26,gold);center("A CASA VENCEU ESTA VEZ",320,63,gold,2);}
    }
    box(0,340,640,20,ink);
    if(t>=9.f)center("ENTER / START - VOLTAR AO MENU",320,347,gold);
    else center("EPILOGO - "+std::to_string(std::max(1,9-int(t))),320,347,muted);
}
} // namespace art
