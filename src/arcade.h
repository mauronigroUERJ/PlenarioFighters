#pragma once
#include "pixel.h"
#include <fstream>
#include <vector>

struct Scoreboard {
    struct Entry {int score=0,character=0,player=0,mode=0,difficulty=1;};
    std::vector<Entry> rows;
    std::string path;
    bool saveFailed=false;
    bool load() {
        std::ifstream in(path);std::string tag;int version,count;
        if(!(in>>tag>>version>>count) || tag!="PF_SCORES" || version!=1 || count<0 || count>10)return false;
        std::vector<Entry> next;
        for(int n=0;n<count;n++){Entry e;if(!(in>>e.score>>e.character>>e.player>>e.mode>>e.difficulty) || e.score<0 || e.score>2000000 || e.character<0 || e.character>=pf::CharacterCount || e.player<0 || e.player>1 || e.mode<0 || e.mode>2 || e.difficulty<0 || e.difficulty>2)return false;next.push_back(e);}
        rows=std::move(next);sort();return true;
    }
    void sort(){std::stable_sort(rows.begin(),rows.end(),[](const Entry&a,const Entry&b){return a.score>b.score;});if(rows.size()>10)rows.resize(10);}
    void add(int score,int character,int player,int mode,int difficulty){if(score<=0)return;rows.push_back({std::clamp(score,0,2000000),pf::validCharacter(character),std::clamp(player,0,1),std::clamp(mode,0,2),std::clamp(difficulty,0,2)});sort();save();}
    void save(){if(path.empty())return;std::ofstream out(path);out<<"PF_SCORES 1 "<<rows.size()<<'\n';for(auto&e:rows)out<<e.score<<' '<<e.character<<' '<<e.player<<' '<<e.mode<<' '<<e.difficulty<<'\n';out.flush();saveFailed=!out.good();}
};
namespace art {
inline void arcadeSeal(float t) {
    box(0,0,640,360,Color{104,132,146,255});
    // A fictional arcade seal inspired by the supplied boot-screen reference.
    for(int n=0;n<72;n++){float a=n*6.283185f/72;int rad=n%2?88:96;line(320+int(std::cos(a)*70),131+int(std::sin(a)*70),320+int(std::cos(a)*rad),131+int(std::sin(a)*rad),gold);}
    for(int y=-82;y<=82;y++){int w=int(std::sqrt(float(82*82-y*y)));box(320-w,131+y,w*2,1,ink);}
    for(int n=0;n<240;n++){float a=n*6.283185f/240;box(320+int(std::cos(a)*81),131+int(std::sin(a)*81),2,2,gold);}
    center("FEDERACAO",320,64,white);center("DO PIXEL ARCADE",320,77,white);
    for(int n=0;n<8;n++){float a=n*6.283185f/8;int x=320+int(std::cos(a)*60),y=135+int(std::sin(a)*51);box(x-4,y,9,2,gold);box(x,y-4,2,9,gold);}
    box(300,100,40,63,gold);box(304,105,32,55,white);box(315,109,10,56,red);
    center("PF",320,119,ink,2);center("SELO FICTICIO",320,188,gold);
    center("WINNERS DON'T USE DRUGS",320,254,gold,2);
    center("UMA HOMENAGEM AS TELAS DE ABERTURA DOS ARCADES",320,284,white);
    center("APERTE UM BOTAO PARA CONTINUAR",320,335,ink);
    (void)t;
}
inline void arcadeSponsors(float t) {
    box(0,0,640,360,ink);center("PATROCINIOS DE MENTIRINHA",320,16,gold,2);
    if(t<4.5f) {
        box(150,65,340,90,Color{41,30,48,255});border(150,65,340,90,gold);
        center("BANCO MASTER",320,88,gold,3);center("SEU COMBO RENDE. A CONTA E DE OUTRO.",320,132,white);
        center("APOIOS INVOLUNTARIOS - PARODIA",320,185,muted);
        center("BRB - BANCO DO RISCO BRAVO",320,216,white,2);
        center("RIOPREVIDENCIA - FUNDO DO FUTURO INCERTO",320,255,white);
    } else {
        const char* parties[]={"PT - PARTIDO DO TROCO","PL - PARTIDO DO LOTE","MDB - MOVIMENTO DO BALCAO","PSD - PARTIDO SO DEPOIS","NOVO RECIBO","UNIAO DO PIX"};
        for(int n=0;n<6;n++){int x=20+(n%2)*310,y=70+n/2*73;Color c=accent(n);box(x,y,290,60,Color{24,35,48,255});border(x,y,290,60,c);center(parties[n],x+145,y+24,c);}
        center("COLIGACAO TODO MUNDO NO MESMO FLIPERAMA",320,306,gold);
    }
    center("PATROCINIO FICTICIO / NENHUM VINCULO OU APOIO REAL",320,328,muted);
    center("APERTE UM BOTAO PARA CONTINUAR",320,347,white);
}
inline void arcadeScores(const Scoreboard &board) {
    box(0,0,640,360,ink);center("HALL DOS RECORDES",320,21,gold,3);
    center("PONTUACOES LOCAIS / PARTIDAS CONCLUIDAS",320,54,muted);
    text("POS",24,82,gold);text("LUTADOR",74,82,gold);text("MODO",289,82,gold);text("CPU",411,82,gold);text("PONTOS",527,82,gold);
    for(int n=0;n<10;n++){int y=104+n*19;box(17,y-3,606,18,n%2?Color{24,34,48,255}:Color{17,26,39,255});text(std::to_string(n+1),28,y,muted);
        if(n<int(board.rows.size())){auto&e=board.rows[n];text(std::string("P")+std::to_string(e.player+1)+" "+name(e.character),74,y,accent(e.character));text(e.mode==1?"LOCAL":e.mode==2?"TORNEIO":"CPU",289,y,white);text(e.mode==1?"---":e.difficulty==0?"FACIL":e.difficulty==2?"DIFICIL":"MEDIO",411,y,muted);text(std::to_string(e.score),527,y,gold);}
        else {text("---",74,y,muted);text("0",527,y,muted);}}
    center(board.rows.empty()?"SEJA O PRIMEIRO A REGISTRAR UMA PARTIDA":"DEMONSTRACOES NAO CONTAM PARA OS RECORDES",320,309,white);
    center(board.saveFailed?"NAO FOI POSSIVEL SALVAR O ARQUIVO DE RECORDES":"APERTE UM BOTAO PARA IR AO MENU",320,340,board.saveFailed?red:gold);
}
}
