#pragma once
#include <SDL.h>
#include <array>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include "combat.h"

namespace controls {
enum Action { Left, Right, Jump, Guard, Punch, Kick, Ultimate, Crouch, Magic, Grab, Pause, Count };
inline const char* actionName(int a) {
    static const char* names[]={"ESQUERDA","DIREITA","PULAR","DEFENDER","SOCO","CHUTE","ULTIMATE","AGACHAR","MALOTE","AGARRAR","PAUSAR"};return names[a];
}
enum Kind { None, Button, Axis, Hat };
struct Binding {
    int kind=None,index=0,value=0;
    bool operator==(const Binding&b)const{return kind==b.kind && index==b.index && value==b.value;}
};
using Keys=std::array<SDL_Scancode,Count>;
struct Profile {std::array<Binding,Count> bind{};int deadzone=10000;bool ps2=false;Binding up{};};
struct Device {
    SDL_GameController* controller=nullptr;
    SDL_Joystick* joy=nullptr;
    SDL_JoystickID id=-1;
    std::string guid,name;
    bool playstation=false;
    std::array<int,256> neutral{};
};
enum Nav { UpNav, DownNav, LeftNav, RightNav, AcceptNav, BackNav, SettingsNav, ModeNav, PauseNav, NavCount };
using Navigation=std::array<bool,NavCount>;
inline std::string upper(std::string s) {for(char&c:s)c=char(std::toupper(static_cast<unsigned char>(c)));return s;}
inline Keys defaultKeys(int p) {
    if(!p)return {SDL_SCANCODE_A,SDL_SCANCODE_D,SDL_SCANCODE_W,SDL_SCANCODE_L,SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_U,SDL_SCANCODE_S,SDL_SCANCODE_I,SDL_SCANCODE_O,SDL_SCANCODE_P};
    return {SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT,SDL_SCANCODE_UP,SDL_SCANCODE_RSHIFT,SDL_SCANCODE_KP_1,SDL_SCANCODE_KP_2,SDL_SCANCODE_KP_0,SDL_SCANCODE_DOWN,SDL_SCANCODE_KP_3,SDL_SCANCODE_KP_PERIOD,SDL_SCANCODE_KP_ENTER};
}
inline bool reserved(SDL_Scancode k) {return k==SDL_SCANCODE_ESCAPE || k==SDL_SCANCODE_F1 || k==SDL_SCANCODE_F2 || k==SDL_SCANCODE_F3 || k==SDL_SCANCODE_F11 || k==SDL_SCANCODE_M || k==SDL_SCANCODE_R || k==SDL_SCANCODE_RETURN || k==SDL_SCANCODE_UNKNOWN;}
class System {
public:
    Keys keys[2]={defaultKeys(0),defaultKeys(1)};
    std::vector<Device> devices;
    SDL_JoystickID assigned[2]={-1,-1};
    std::string preferred[2];
    std::map<std::string,Profile> profiles;
    Navigation navPrevious{};
    bool pausePrevious[2]={false,false};
    std::string path,message="CONFIGURACOES PRONTAS";
    bool disconnected=false;
    bool testVirtualOnly=false;
    int mappingsLoaded=0;
    System()=default;
    System(const System&)=delete;
    System& operator=(const System&)=delete;
    void close() {for(auto&d:devices){if(d.controller)SDL_GameControllerClose(d.controller);else if(d.joy)SDL_JoystickClose(d.joy);}devices.clear();assigned[0]=assigned[1]=-1;}
    Device* device(int p) {for(auto&d:devices)if(d.id==assigned[p])return &d;return nullptr;}
    Device* byID(SDL_JoystickID id) {for(auto&d:devices)if(d.id==id)return &d;return nullptr;}
    static Profile defaults(const Device&d) {
        Profile p;
        if(d.controller) {
            int buttons[]={SDL_CONTROLLER_BUTTON_DPAD_LEFT,SDL_CONTROLLER_BUTTON_DPAD_RIGHT,SDL_CONTROLLER_BUTTON_A,SDL_CONTROLLER_BUTTON_LEFTSHOULDER,SDL_CONTROLLER_BUTTON_X,SDL_CONTROLLER_BUTTON_B,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,SDL_CONTROLLER_BUTTON_DPAD_DOWN,SDL_CONTROLLER_BUTTON_Y,SDL_CONTROLLER_BUTTON_LEFTSTICK,SDL_CONTROLLER_BUTTON_START};
            for(int a=0;a<Count;a++)p.bind[a]={Button,buttons[a],0};
        } else {
            int buttons[]={-1,-1,0,4,2,1,5,-1,3,6,9};
            for(int a=0;a<Count;a++)if(buttons[a]>=0 && buttons[a]<SDL_JoystickNumButtons(d.joy))p.bind[a]={Button,buttons[a],0};
            if(SDL_JoystickNumHats(d.joy)>0) {p.bind[Left]={Hat,0,SDL_HAT_LEFT};p.bind[Right]={Hat,0,SDL_HAT_RIGHT};p.bind[Crouch]={Hat,0,SDL_HAT_DOWN};}
            else {p.bind[Left]={Axis,0,-1};p.bind[Right]={Axis,0,1};p.bind[Crouch]={Axis,1,1};}
        }
        return p;
    }
    std::string profileKey(int p,const Device&d)const{return std::to_string(p)+":"+d.guid+(d.controller?":mapped":":raw");}
    Profile* profile(int p) {Device*d=device(p);if(!d)return nullptr;auto key=profileKey(p,*d);if(!profiles.count(key))profiles[key]=defaults(*d);return &profiles[key];}
    void add(int index) {
        if(testVirtualOnly && !SDL_JoystickIsVirtual(index))return;
        auto id=SDL_JoystickGetDeviceInstanceID(index);if(id<0 || byID(id))return;
        Device d;
        if(SDL_IsGameController(index))d.controller=SDL_GameControllerOpen(index);
        d.joy=d.controller?SDL_GameControllerGetJoystick(d.controller):SDL_JoystickOpen(index);
        if(!d.joy)return;
        d.id=SDL_JoystickInstanceID(d.joy);
        char guid[33];SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(d.joy),guid,sizeof(guid));d.guid=guid;
        const char*n=SDL_JoystickName(d.joy);d.name=upper(n?n:"CONTROLE GENERICO");
        if(d.controller) {auto t=SDL_GameControllerGetType(d.controller);d.playstation=t==SDL_CONTROLLER_TYPE_PS3 || t==SDL_CONTROLLER_TYPE_PS4 || t==SDL_CONTROLLER_TYPE_PS5;}
        SDL_JoystickUpdate();
        if(!d.controller)for(int a=0;a<std::min(256,SDL_JoystickNumAxes(d.joy));a++)d.neutral[a]=SDL_JoystickGetAxis(d.joy,a);
        devices.push_back(d);
        int slot=-1;
        for(int p=0;p<2;p++)if(assigned[p]<0 && preferred[p]==d.guid){slot=p;break;}
        if(slot<0)for(int p=0;p<2;p++)if(assigned[p]<0 && preferred[p]!="-"){slot=p;break;}
        if(slot>=0){assigned[slot]=d.id;preferred[slot]=d.guid;profile(slot);}
        message="CONTROLE CONECTADO: "+d.name.substr(0,35);
    }
    void remove(SDL_JoystickID id) {
        for(auto it=devices.begin();it!=devices.end();++it)if(it->id==id) {
            for(int p=0;p<2;p++)if(assigned[p]==id){assigned[p]=-1;disconnected=true;pausePrevious[p]=false;}
            if(it->controller)SDL_GameControllerClose(it->controller);else SDL_JoystickClose(it->joy);
            devices.erase(it);message="CONTROLE DESCONECTADO - TECLADO DISPONIVEL";navPrevious={};return;
        }
    }
    void init(const std::string&file,bool loadSettings=true) {
        path=file;if(loadSettings)load();
        for(int n=0;n<SDL_NumJoysticks();n++)add(n);
    }
    void event(const SDL_Event&e) {
        if(e.type==SDL_JOYDEVICEADDED)add(e.jdevice.which);
        if(e.type==SDL_JOYDEVICEREMOVED)remove(e.jdevice.which);
    }
    void assign(int player,SDL_JoystickID id) {
        if(id>=0 && !byID(id))return;
        auto old=assigned[player];assigned[player]=id;
        if(id>=0 && assigned[1-player]==id){assigned[1-player]=old;Device*other=device(1-player);preferred[1-player]=other?other->guid:"-";}
        Device*d=device(player);preferred[player]=d?d->guid:"-";profile(player);
    }
    void prepareLocal() {
        if(devices.size()<2) {
            assign(0,-1);
            if(devices.size()==1)assign(1,devices[0].id);
            else {assign(1,-1);preferred[1].clear();}
        } else {
            if(!device(0))assign(0,devices[0].id);
            if(!device(1))for(auto&d:devices)if(d.id!=assigned[0]){assign(1,d.id);break;}
        }
        message="2 JOGADORES: CONFIRA P1 E P2 ANTES DE LUTAR";
    }
    void scan() {
        SDL_PumpEvents();
        for(int n=0;n<SDL_NumJoysticks();n++)add(n);
        message="CONTROLES DETECTADOS: "+std::to_string(devices.size());
    }
    void cycleDevice(int player,int direction) {
        int current=-1;
        for(int i=0;i<int(devices.size());i++)if(devices[i].id==assigned[player])current=i;
        int next=(current+1+direction+int(devices.size())+1)%(int(devices.size())+1)-1;
        assign(player,next<0?-1:devices[next].id);message="ATRIBUICAO ALTERADA - SALVE AO TERMINAR";
    }
    bool button(const Device&d,int n)const {
        if(d.controller)return n>=0 && n<SDL_CONTROLLER_BUTTON_MAX && SDL_GameControllerGetButton(d.controller,SDL_GameControllerButton(n));
        return n>=0 && n<SDL_JoystickNumButtons(d.joy) && SDL_JoystickGetButton(d.joy,n);
    }
    int axis(const Device&d,int n)const {
        if(d.controller)return n>=0 && n<SDL_CONTROLLER_AXIS_MAX?SDL_GameControllerGetAxis(d.controller,SDL_GameControllerAxis(n)):0;
        return n>=0 && n<SDL_JoystickNumAxes(d.joy)?SDL_JoystickGetAxis(d.joy,n):0;
    }
    bool held(const Device&d,const Binding&b,int zone)const {
        if(b.kind==Button)return button(d,b.index);
        if(b.kind==Axis) {
            int v=axis(d,b.index);
            if(!d.controller && b.index>=0 && b.index<256 && std::abs(d.neutral[b.index])>24000)
                return (v-d.neutral[b.index])*b.value>zone;
            return v*b.value>zone;
        }
        if(b.kind==Hat)return !d.controller && b.index>=0 && b.index<SDL_JoystickNumHats(d.joy) && (SDL_JoystickGetHat(d.joy,b.index)&b.value)!=0;
        return false;
    }
    bool padAction(int player,int action) {
        auto*p=profile(player);auto*d=device(player);if(!d || !p)return false;
        bool result=held(*d,p->bind[action],p->deadzone);
        if(p->ps2) {
            if(action==Jump)result|=held(*d,p->up,p->deadzone);
            return result;
        }
        // The standardized left stick remains an alternative to the D-pad.
        if(d->controller) {
            if(action==Left)result|=axis(*d,SDL_CONTROLLER_AXIS_LEFTX)<-p->deadzone;
            if(action==Right)result|=axis(*d,SDL_CONTROLLER_AXIS_LEFTX)>p->deadzone;
            if(action==Crouch)result|=axis(*d,SDL_CONTROLLER_AXIS_LEFTY)>p->deadzone;
            if(action==Jump)result|=button(*d,SDL_CONTROLLER_BUTTON_DPAD_UP) || axis(*d,SDL_CONTROLLER_AXIS_LEFTY)<-p->deadzone;
        } else {
            // Raw pads may report a hat and a stick at the same time.
            if(SDL_JoystickNumHats(d->joy)>0 && action==Jump)result|=(SDL_JoystickGetHat(d->joy,0)&SDL_HAT_UP)!=0;
            if(SDL_JoystickNumAxes(d->joy)>=2 && std::abs(d->neutral[0])<8000 && std::abs(d->neutral[1])<8000) {
                if(action==Left)result|=axis(*d,0)<-p->deadzone;
                if(action==Right)result|=axis(*d,0)>p->deadzone;
                if(action==Jump)result|=axis(*d,1)<-p->deadzone;
                if(action==Crouch)result|=axis(*d,1)>p->deadzone;
            }
        }
        return result;
    }
    pf::Input input(int player,const Uint8*keyboard=nullptr) {
        if(!keyboard)keyboard=SDL_GetKeyboardState(nullptr);
        bool a[Count];for(int i=0;i<Count;i++)a[i]=keyboard[keys[player][i]] || padAction(player,i);
        return {a[Left],a[Right],a[Jump],a[Guard],a[Punch],a[Kick],a[Ultimate],a[Crouch],a[Magic],a[Grab]};
    }
    Navigation navigation() {
        Navigation heldNav{};
        for(const auto&d:devices) {
            Profile *custom=nullptr;
            for(int player=0;player<2;player++)if(assigned[player]==d.id)custom=profile(player);
            if(custom && custom->ps2) {
                auto down=[&](int a){return held(d,custom->bind[a],custom->deadzone);};
                heldNav[UpNav]|=held(d,custom->up,custom->deadzone);
                heldNav[DownNav]|=down(Crouch);heldNav[LeftNav]|=down(Left);heldNav[RightNav]|=down(Right);
                heldNav[AcceptNav]|=down(Jump);heldNav[BackNav]|=down(Kick);
                heldNav[SettingsNav]|=down(Magic);heldNav[ModeNav]|=down(Punch);
                continue;
            }
            bool mapped=d.controller!=nullptr;
            auto btn=[&](int gc,int raw){return button(d,mapped?gc:raw);};
            int hat=!mapped && SDL_JoystickNumHats(d.joy)>0?SDL_JoystickGetHat(d.joy,0):0;
            bool useX=mapped || std::abs(d.neutral[0])<8000,useY=mapped || std::abs(d.neutral[1])<8000;
            heldNav[UpNav]|=btn(SDL_CONTROLLER_BUTTON_DPAD_UP,-1) || (hat&SDL_HAT_UP) || (useY && axis(d,1)<-16000);
            heldNav[DownNav]|=btn(SDL_CONTROLLER_BUTTON_DPAD_DOWN,-1) || (hat&SDL_HAT_DOWN) || (useY && axis(d,1)>16000);
            heldNav[LeftNav]|=btn(SDL_CONTROLLER_BUTTON_DPAD_LEFT,-1) || (hat&SDL_HAT_LEFT) || (useX && axis(d,0)<-16000);
            heldNav[RightNav]|=btn(SDL_CONTROLLER_BUTTON_DPAD_RIGHT,-1) || (hat&SDL_HAT_RIGHT) || (useX && axis(d,0)>16000);
            heldNav[AcceptNav]|=btn(SDL_CONTROLLER_BUTTON_A,0);
            heldNav[BackNav]|=btn(SDL_CONTROLLER_BUTTON_B,1);
            heldNav[SettingsNav]|=btn(SDL_CONTROLLER_BUTTON_BACK,8);
            heldNav[ModeNav]|=btn(SDL_CONTROLLER_BUTTON_X,2);
        }
        Navigation edge{};for(int i=0;i<NavCount;i++){edge[i]=heldNav[i]&&!navPrevious[i];navPrevious[i]=heldNav[i];}
        for(int p=0;p<2;p++){bool down=padAction(p,Pause);edge[PauseNav]|=down&&!pausePrevious[p];pausePrevious[p]=down;}
        return edge;
    }
    bool setKey(int p,int action,SDL_Scancode key) {
        if(key<=SDL_SCANCODE_UNKNOWN || key>=SDL_NUM_SCANCODES || reserved(key)){message="TECLA RESERVADA - ESC CANCELA";return false;}
        for(int player=0;player<2;player++)for(int a=0;a<Count;a++)if((player!=p || a!=action) && keys[player][a]==key) {message="TECLA JA USADA POR P"+std::to_string(player+1)+" / "+actionName(a);return false;}
        keys[p][action]=key;message="TECLA ALTERADA - SALVE AO TERMINAR";return true;
    }
    bool setBinding(int p,int action,Binding binding) {
        Profile*pr=profile(p);if(!pr)return false;
        // Swapping preserves a complete usable mapping without double actions.
        for(int a=0;a<Count;a++)if(a!=action && pr->bind[a]==binding){pr->bind[a]=pr->bind[action];break;}
        pr->bind[action]=binding;message="BOTAO ALTERADO - SALVE AO TERMINAR";return true;
    }
    bool capture(int p,int action,const SDL_Event&e) {
        Device*d=device(p);if(!d)return false;
        if(d->controller) {
            if(e.type==SDL_CONTROLLERBUTTONDOWN && e.cbutton.which==d->id)return setBinding(p,action,{Button,e.cbutton.button,0});
            if(e.type==SDL_CONTROLLERAXISMOTION && e.caxis.which==d->id && std::abs(int(e.caxis.value))>24000) {
                if(e.caxis.axis>=SDL_CONTROLLER_AXIS_TRIGGERLEFT && e.caxis.value<0)return false;
                return setBinding(p,action,{Axis,e.caxis.axis,e.caxis.value>0?1:-1});
            }
        } else {
            if(e.type==SDL_JOYBUTTONDOWN && e.jbutton.which==d->id)return setBinding(p,action,{Button,e.jbutton.button,0});
            if(e.type==SDL_JOYHATMOTION && e.jhat.which==d->id && (e.jhat.value==1 || e.jhat.value==2 || e.jhat.value==4 || e.jhat.value==8))return setBinding(p,action,{Hat,e.jhat.hat,e.jhat.value});
            if(e.type==SDL_JOYAXISMOTION && e.jaxis.which==d->id) {
                int v=e.jaxis.value,rest=d->neutral[e.jaxis.axis];
                if(std::abs(rest)>24000 && std::abs(v-rest)>24000)return setBinding(p,action,{Axis,e.jaxis.axis,v>rest?1:-1});
                if(std::abs(rest)<=24000 && std::abs(v)>24000)return setBinding(p,action,{Axis,e.jaxis.axis,v>0?1:-1});
            }
        }
        return false;
    }
    std::string diagnostic(int player) {
        Device*d=device(player);if(!d)return "SEM CONTROLE - CONECTE E PRESSIONE UM BOTAO";
        std::string s="TESTE: ";
        for(int a=0;a<Count;a++)if(padAction(player,a))s+=std::string(actionName(a))+" ";
        if(s=="TESTE: ")s+="APERTE UM BOTAO OU MOVA O ANALOGICO";
        return s;
    }
    std::string deviceDetails(int player) {
        Device*d=device(player);if(!d)return "PARSEC: O CONTROLE DEVE APARECER NO PC HOST";
        return std::to_string(SDL_JoystickNumButtons(d->joy))+" BOTOES / "+std::to_string(SDL_JoystickNumAxes(d->joy))+" EIXOS / "+std::to_string(SDL_JoystickNumHats(d->joy))+" DIRECIONAIS";
    }
    std::string keyLabel(int p,int a)const {return upper(SDL_GetScancodeName(keys[p][a]));}
    std::string bindingLabel(int p,int a) {
        Device*d=device(p);Profile*pr=profile(p);if(!d || !pr)return "SEM CONTROLE";
        Binding b=pr->bind[a];
        if(b.kind==None)return "NAO DEFINIDO";
        if(b.kind==Axis)return "EIXO "+std::to_string(b.index)+(b.value>0?" +":" -");
        if(b.kind==Hat)return "D-PAD "+std::to_string(b.index)+" / "+std::to_string(b.value);
        if(!d->controller)return "BOTAO "+std::to_string(b.index+1);
        if(d->playstation) {
            switch(b.index){case SDL_CONTROLLER_BUTTON_A:return "X / CRUZ";case SDL_CONTROLLER_BUTTON_B:return "CIRCULO";case SDL_CONTROLLER_BUTTON_X:return "QUADRADO";case SDL_CONTROLLER_BUTTON_Y:return "TRIANGULO";case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:return "L1";case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:return "R1";case SDL_CONTROLLER_BUTTON_START:return "OPTIONS / START";case SDL_CONTROLLER_BUTTON_BACK:return "SHARE / SELECT";default:break;}
        }
        switch(b.index){
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:return "D-PAD ESQUERDA";
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:return "D-PAD DIREITA";
            case SDL_CONTROLLER_BUTTON_DPAD_UP:return "D-PAD CIMA";
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:return "D-PAD BAIXO";
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:return "LB";
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:return "RB";
            case SDL_CONTROLLER_BUTTON_LEFTSTICK:return "L3";
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK:return "R3";
            default:break;
        }
        const char*n=SDL_GameControllerGetStringForButton(SDL_GameControllerButton(b.index));return upper(n?n:"BOTAO");
    }
    bool save() {
        std::ofstream out(path,std::ios::trunc);if(!out){message="ERRO AO SALVAR - PASTA SEM PERMISSAO?";return false;}
        out<<"PF_CONTROLS 2\n";
        for(int p=0;p<2;p++){out<<"PLAYER "<<p<<" "<<(preferred[p].empty()?"auto":preferred[p])<<"\n";for(int a=0;a<Count;a++)out<<"KEY "<<p<<" "<<a<<" "<<int(keys[p][a])<<"\n";}
        for(const auto&entry:profiles){out<<"PROFILE "<<entry.first<<" "<<entry.second.deadzone<<"\n";if(entry.second.ps2){auto b=entry.second.up;out<<"PS2 "<<entry.first<<" "<<b.kind<<" "<<b.index<<" "<<b.value<<"\n";}for(int a=0;a<Count;a++){auto b=entry.second.bind[a];out<<"BIND "<<entry.first<<" "<<a<<" "<<b.kind<<" "<<b.index<<" "<<b.value<<"\n";}}
        out.flush();bool ok=bool(out);message=ok?"CONFIGURACOES SALVAS":"ERRO AO SALVAR CONFIGURACOES";return ok;
    }
    bool load() {
        std::ifstream in(path);if(!in)return false;
        std::string header;int version;if(!(in>>header>>version) || header!="PF_CONTROLS" || (version!=1 && version!=2)){message="CONFIGURACAO INVALIDA - USANDO PADROES";return false;}
        Keys temp[2]={defaultKeys(0),defaultKeys(1)};std::string pref[2];std::map<std::string,Profile> loaded;
        std::string line;std::getline(in,line);int lines=0;
        while(std::getline(in,line) && ++lines<4096) {
            std::istringstream s(line);std::string tag;s>>tag;
            int p,a,k;std::string id;
            if(tag=="KEY" && s>>p>>a>>k){if(p<0 || p>1 || a<0 || a>=Count || k<=0 || k>=SDL_NUM_SCANCODES || reserved(SDL_Scancode(k)))return false;temp[p][a]=SDL_Scancode(k);}
            else if(tag=="PLAYER" && s>>p>>id){if(p<0 || p>1 || id.size()>64)return false;pref[p]=id=="auto"?"":id;}
            else if(tag=="PROFILE" && s>>id>>k){if(id.size()>80 || k<4000 || k>24000)return false;loaded[id].deadzone=k;}
            else if(tag=="PS2" && version==2) {
                Binding b;if(!(s>>id>>b.kind>>b.index>>b.value) || !loaded.count(id) || b.kind<Button || b.kind>Hat || b.index<0 || b.index>255 || (b.kind==Axis && std::abs(b.value)!=1) || (b.kind==Hat && b.value!=1 && b.value!=2 && b.value!=4 && b.value!=8))return false;
                loaded[id].ps2=true;loaded[id].up=b;
            }
            else if(tag=="BIND") {Binding b;if(!(s>>id>>a>>b.kind>>b.index>>b.value) || !loaded.count(id) || a<0 || a>=Count || b.kind<None || b.kind>Hat || b.index<0 || b.index>255 || (b.kind==Axis && b.value!=1 && b.value!=-1) || (b.kind==Hat && b.value!=1 && b.value!=2 && b.value!=4 && b.value!=8))return false;loaded[id].bind[a]=b;}
            else if(!tag.empty())return false;
        }
        for(int p=0;p<2;p++)for(int a=0;a<Count;a++)for(int q=0;q<=p;q++)for(int b=0;b<Count;b++)if((q<p || b<a) && temp[p][a]==temp[q][b])return false;
        for(int p=0;p<2;p++){keys[p]=temp[p];preferred[p]=pref[p];}profiles=loaded;message="CONFIGURACOES CARREGADAS";return true;
    }
};
}
