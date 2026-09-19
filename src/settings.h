#pragma once
#include "controls.h"
#include "pixel.h"

class Settings {
public:
    static constexpr int firstActionRow=3;
    static constexpr int restoreRow=firstActionRow+controls::Count;
    static constexpr int ps2Row=restoreRow+1;
    static constexpr int saveRow=ps2Row+1;
    static constexpr int rowCount=saveRow+1;
    bool open=false,capturing=false,padColumn=false;
    int player=0,row=0;
    int ps2Step=-1;
    SDL_JoystickID ps2Device=-1;
    controls::Profile ps2Original{},ps2Pending{};
    controls::Binding previous{};
    bool awaitRelease=false;
    void beginPS2(controls::System&c) {
        auto*p=c.profile(player);auto*d=c.device(player);
        if(!p || !d){c.message="SELECIONE O CONTROLE NA LINHA CONTROLE ATRIBUIDO";return;}
        ps2Original=*p;ps2Pending=*p;ps2Pending.bind={};ps2Pending.ps2=true;
        ps2Step=0;ps2Device=d->id;awaitRelease=false;capturing=true;padColumn=true;
        c.message="PS2: SOLTE OS BOTOES ENTRE CADA ETAPA";
    }
    void capturePS2(controls::System&c,const SDL_Event&e) {
        auto*d=c.device(player);auto*p=c.profile(player);
        if((e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_ESCAPE) || !d || d->id!=ps2Device || !p) {
            ps2Step=-1;capturing=false;c.message="CALIBRACAO CANCELADA - PERFIL ANTERIOR PRESERVADO";return;
        }
        if(awaitRelease) {
            if(c.held(*d,previous,12000))return;
            awaitRelease=false;
        }
        // Use the normal event decoder on a temporary profile, committing only on completion.
        controls::Profile saved=*p;*p=controls::Profile{};
        bool accepted=c.capture(player,controls::Jump,e);
        controls::Binding candidate=p->bind[controls::Jump];*p=saved;
        if(!accepted)return;
        for(int n=0;n<ps2Step && n<controls::Count;n++)if(ps2Pending.bind[n]==candidate) {
            c.message="BOTAO REPETIDO - USE O BOTAO PEDIDO";return;
        }
        if(ps2Step<controls::Count)ps2Pending.bind[ps2Step]=candidate;
        else ps2Pending.up=candidate;
        previous=candidate;awaitRelease=true;
        if(++ps2Step==controls::Count+1) {
            *p=ps2Pending;ps2Step=-1;capturing=false;c.navPrevious.fill(true);c.pausePrevious[player]=true;
            c.save();c.message="PS2 CONFIGURADO - X CONFIRMA / CIRCULO VOLTA / R2 AGARRA";
        }
    }
    std::array<int,256> baseline{};
    void beginCapture(controls::System&c) {
        if(row<firstActionRow || row>=restoreRow)return;
        if(padColumn && !c.device(player)){c.message="CONECTE OU SELECIONE UM CONTROLE PRIMEIRO";return;}
        capturing=true;
        if(auto*d=c.device(player))for(int n=0;n<256;n++)baseline[n]=c.axis(*d,n);
        c.message=padColumn?"APERTE UM BOTAO OU MOVA UM EIXO - ESC CANCELA":"PRESSIONE UMA TECLA - ESC CANCELA";
    }
    void captureEvent(controls::System&c,const SDL_Event&e) {
        if(ps2Step>=0){capturePS2(c,e);return;}
        if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_ESCAPE){capturing=false;c.message="ALTERACAO CANCELADA";return;}
        if(!padColumn && e.type==SDL_KEYDOWN && !e.key.repeat) {
            SDL_Scancode code=e.key.keysym.scancode;
            if(code==SDL_SCANCODE_UNKNOWN)code=SDL_GetScancodeFromKey(e.key.keysym.sym);
            if(c.setKey(player,row-firstActionRow,code))capturing=false;
        }
        if(padColumn) {
            auto*d=c.device(player);
            if(!d){capturing=false;c.message="CONTROLE DESCONECTADO - CAPTURA CANCELADA";return;}
            int axis=-1,value=0;
            if(e.type==SDL_CONTROLLERAXISMOTION && d->controller && e.caxis.which==d->id){axis=e.caxis.axis;value=e.caxis.value;}
            if(e.type==SDL_JOYAXISMOTION && !d->controller && e.jaxis.which==d->id){axis=e.jaxis.axis;value=e.jaxis.value;}
            if(axis>=0 && std::abs(value)<8000 && (d->controller || std::abs(d->neutral[axis])<=24000)){baseline[axis]=value;return;}
            if(axis>=0 && std::abs(value-baseline[axis])<16000)return;
            if(c.capture(player,row-firstActionRow,e))capturing=false;
        }
    }
    void nav(controls::System&c,controls::Nav n) {
        if(capturing)return;
        if(n==controls::BackNav){if(c.save())open=false;return;}
        if(n==controls::UpNav)row=(row+rowCount-1)%rowCount;
        if(n==controls::DownNav)row=(row+1)%rowCount;
        if(n==controls::LeftNav || n==controls::RightNav) {
            int dir=n==controls::LeftNav?-1:1;
            if(row==0)player=1-player;
            else if(row==1)c.cycleDevice(player,dir);
            else if(row==2){if(auto*p=c.profile(player))p->deadzone=std::clamp(p->deadzone+dir*2000,4000,24000);}
            else padColumn=!padColumn;
        }
        if(n==controls::AcceptNav) {
            if(row==0)player=1-player;
            else if(row==1)c.cycleDevice(player,1);
            else if(row==2){if(auto*p=c.profile(player))p->deadzone=p->deadzone>=24000?4000:p->deadzone+2000;}
            else if(row<restoreRow)beginCapture(c);
            else if(row==restoreRow){c.keys[0]=controls::defaultKeys(0);c.keys[1]=controls::defaultKeys(1);c.profiles.clear();c.message="TECLAS E BOTOES RESTAURADOS PARA AMBOS";}
            else if(row==ps2Row)beginPS2(c);
            else if(row==saveRow && c.save())open=false;
        }
    }
    void key(controls::System&c,SDL_Keycode k) {
        if(k==SDLK_ESCAPE){nav(c,controls::BackNav);return;}
        if(k==SDLK_UP)nav(c,controls::UpNav);
        if(k==SDLK_DOWN)nav(c,controls::DownNav);
        if(k==SDLK_LEFT)nav(c,controls::LeftNav);
        if(k==SDLK_RIGHT)nav(c,controls::RightNav);
        if(k==SDLK_TAB)padColumn=!padColumn;
        if(k==SDLK_F2)c.scan();
        if(k==SDLK_F3)beginPS2(c);
        if(k==SDLK_RETURN || k==SDLK_KP_ENTER)nav(c,controls::AcceptNav);
    }
    void draw(controls::System&c) {
        using namespace art;
        box(0,0,640,360,ink);
        box(18,15,5,16,green);text("CONFIGURACOES",32,17,white,2);text("SDL2 / CONTROLES",522,21,muted);
        for(int p=0;p<2;p++) {
            auto*d=c.device(p);int x=p?330:24;
            text("P"+std::to_string(p+1)+"  "+(d?d->name.substr(0,42):"TECLADO / SEM CONTROLE"),x,45,d?green:muted);
            text(d?(d->controller?(d->playstation?"PLAYSTATION RECONHECIDO":"GAMEPAD RECONHECIDO"):"GENERICO - MAPEAMENTO MANUAL"):"CONECTE POR USB OU BLUETOOTH",x,57,muted);
        }
        text("ACAO",40,76,muted);text("TECLADO",276,76,padColumn?muted:gold);text("CONTROLE",441,76,padColumn?gold:muted);
        for(int n=0;n<rowCount;n++) {
            int y=85+n*11;
            if(n==row){box(24,y-3,592,13,Color{40,55,69,255});text(">",28,y,gold);}
            Color col=n==row?white:muted;
            if(n==0){text("JOGADOR",40,y,col);text("< P"+std::to_string(player+1)+" >",276,y,gold);}
            else if(n==1){text("CONTROLE ATRIBUIDO",40,y,col);auto*d=c.device(player);text(d?d->name.substr(0,53):"SOMENTE TECLADO",276,y,col);}
            else if(n==2){text("ZONA MORTA",40,y,col);auto*p=c.profile(player);text(p?"< "+std::to_string(int(p->deadzone*100.f/32767))+" POR CENTO >":"CONECTE UM CONTROLE",276,y,col);}
            else if(n<restoreRow) {
                text(controls::actionName(n-firstActionRow),40,y,col);
                text(c.keyLabel(player,n-firstActionRow).substr(0,23),276,y,n==row&&!padColumn?gold:col);
                text(c.bindingLabel(player,n-firstActionRow).substr(0,26),441,y,n==row&&padColumn?gold:col);
            } else text(n==restoreRow?"RESTAURAR TECLAS E BOTOES PADRAO":n==ps2Row?"CONFIGURAR CONTROLE PS2 (F3)":"SALVAR E VOLTAR",40,y,n==saveRow?green:col);
        }
        box(24,291,592,1,Color{59,77,91,255});
        center(c.message.substr(0,95),320,300,capturing?gold:green);
        center("SETAS / D-PAD NAVEGAR  ENTER / X-A CONFIRMAR  ESC / CIRCULO-B SALVAR E VOLTAR",320,320,muted);
        center("ESQ-DIR ALTERA  TAB COLUNA  F2 REDETECTAR  F3 CONFIGURAR PS2",320,333,muted);
        center(c.diagnostic(player).substr(0,98),320,346,gold);
        if(ps2Step>=0) {
            static const char* prompt[]={"DIRECIONAL ESQUERDA","DIRECIONAL DIREITA","X / CRUZ","L1","QUADRADO","CIRCULO","R1","DIRECIONAL BAIXO","TRIANGULO","R2","START","DIRECIONAL CIMA"};
            box(65,117,510,129,ink);border(65,117,510,129,gold);
            center("CALIBRAR PS2 - P"+std::to_string(player+1),320,128,gold,2);
            center("ETAPA "+std::to_string(ps2Step+1)+" / 12",320,155,muted);
            center(prompt[ps2Step],320,173,white,2);
            center("APERTE E SOLTE O BOTAO INDICADO",320,201,green);
            center("ESC CANCELA SEM ALTERAR O PERFIL",320,220,muted);
        } else if(capturing) {
            box(103,142,434,55,Color{8,15,27,250});border(103,142,434,55,gold);
            center(std::string("MAPEAR ")+controls::actionName(row-firstActionRow),320,151,gold,2);
            center(padColumn?"APERTE BOTAO / MOVA EIXO OU DIRECIONAL":"PRESSIONE A NOVA TECLA",320,177,white);
        }
    }
};
