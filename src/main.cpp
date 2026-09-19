#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "pixel.h"
#include "settings.h"
#include "tournament.h"
#include "voices.h"
#include "cinematics.h"
#include "arcade.h"
#include "bonus.h"
#include "stages.h"
#include "story.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

using namespace pf;
using namespace art;
struct Burst {float x,y,age;int character;bool blocked=false;};
struct Particle {float x,y,vx,vy,life;Color color;};
class Game {
public:
    SDL_Window* window=nullptr;
    SDL_Renderer* renderer=nullptr;
    SDL_AudioDeviceID audio=0;
    Voices voices;
    Match match;
    Reporter reporter;
    bool versusActive=false;
    float versusTime=0;
    void finishVersus(){versusActive=false;versusTime=0;paused=false;voices.clear();}
    Tournament tournament;
    Story story;
    void startStory(){returnMenu();story.start();local=false;menu=false;paused=false;}
    void storyAccept(){if(story.finished){returnMenu();return;}story.briefing=false;begin();}

    ConversionBonus bonus;
    bool bonusActive=false;
    int pendingBags[2]={0,0};
    void finishBonus(){if(bonus.winner>=0)pendingBags[bonus.winner]=2;bonusActive=false;begin();}
    controls::System controls;
    Settings settings;
    bool running=true, menu=true, paused=false, local=false, mutedAudio=false;
    bool focused=true,lobby=false;
    bool openingActive=false,endingActive=false,endingWon=false,crtEnabled=true;
    float openingTime=0,endingTime=0,raidTime=0;
    bool bootActive=false,scoreboardActive=false,attractActive=false,recorded=false;
    int bootStage=0,demoIndex=0;
    float bootTime=0,scoreboardTime=0,idleTime=0,demoTime=0;
    Scoreboard scoreboard;
    std::map<std::pair<SDL_JoystickID,int>,int> lastRawAxes;
    int points[2]={0,0};
    void showScores(){openingActive=false;scoreboardActive=true;scoreboardTime=0;voices.clear();}
    void nextBoot(){bootTime=0;if(bootStage==0)bootStage=1;else{bootActive=false;openingActive=true;openingTime=0;}}
    void returnMenu(){story.leave();endingActive=false;versusActive=false;bonusActive=false;pendingBags[0]=pendingBags[1]=0;attractActive=false;scoreboardActive=false;openingActive=false;bootActive=false;menu=true;lobby=false;paused=false;tournament.active=false;idleTime=0;voices.clear();particles.clear();bursts.clear();}
    void startDemo(){story.leave();versusActive=false;attractActive=true;scoreboardActive=false;menu=false;local=false;tournament.active=false;paused=false;points[0]=points[1]=0;demoTime=0;int a=demoIndex%CharacterCount,b=(a+3+demoIndex/CharacterCount) % CharacterCount;if(a==b)b=(b+1)%CharacterCount;++demoIndex;match=Match(a,b);match.difficulty=Difficulty::Medium;particles.clear();bursts.clear();voices.clear();}
    bool passiveInput(){idleTime=0;if(attractActive || scoreboardActive){returnMenu();return true;}if(bootActive){nextBoot();return true;}if(openingActive){showScores();return true;}return false;}
    bool soundOn()const{return !mutedAudio && !attractActive;}
    void recordMatch(){if(recorded || attractActive || story.active || !match.over)return;recorded=true;for(int p=0;p<(local?2:1);p++){int value=points[p]+(match.f[p].wins>=2?3000:0);scoreboard.add(value,match.f[p].character,p,local?1:tournament.active?2:0,cpuDifficulty);}}
    int cpuDifficulty=1;
    std::string preferencesPath;
    static constexpr int MenuCount=6;
    const char* difficultyLabel()const{return cpuDifficulty==0?"FACIL":cpuDifficulty==2?"DIFICIL":"MEDIO";}
    void savePreferences(){if(!preferencesPath.empty()){std::ofstream out(preferencesPath);out<<"PF_GAME 1 "<<cpuDifficulty<<" "<<int(crtEnabled)<<"\n";}}
    void cycleDifficulty(){cpuDifficulty=(cpuDifficulty+1)%3;savePreferences();}
    void startEnding(){versusActive=false;endingActive=true;endingWon=match.f[0].wins>=2;endingTime=0;paused=false;voices.clear();}
    void leaveEnding(){versusActive=false;idleTime=0;endingActive=false;menu=true;paused=false;tournament.active=false;voices.clear();}
    int selected=0,opponent=1,selectSide=0,menuChoice=0,lobbyRow=2;
    void chooseCharacter(int direction) {int &id=selectSide?opponent:selected;id=(id+direction+CharacterCount)%CharacterCount;}
    float time=0;
    std::vector<Particle> particles;
    std::vector<Burst> bursts;
    std::mt19937 random{91};
    bool init(bool hidden=false) {
        SDL_SetMainReady();
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI,"1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4,"1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5,"1");
        SDL_SetHint(SDL_HINT_XINPUT_ENABLED,"1");
        SDL_SetHint(SDL_HINT_DIRECTINPUT_ENABLED,"1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_SWITCH,"1");
        if(hidden)SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
        if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER|SDL_INIT_GAMECONTROLLER|SDL_INIT_JOYSTICK)!=0) return false;
        char*base=SDL_GetBasePath();std::string dir=base?base:"";SDL_free(base);
        bootActive=!hidden;openingActive=false;
        if(!hidden){scoreboard.path=dir+"scores.cfg";scoreboard.load();story.path=dir+"story.cfg";story.load();}
        if(!hidden){
            preferencesPath=dir+"game.cfg";std::ifstream prefs(preferencesPath);std::string tag;int version,difficulty,crt;
            if(prefs>>tag>>version>>difficulty>>crt && tag=="PF_GAME" && version==1 && difficulty>=0 && difficulty<=2 && (crt==0 || crt==1)){cpuDifficulty=difficulty;crtEnabled=true;}
        }
        // Optional standard SDL2 mappings; raw joysticks work without this file.
        std::ifstream mappings(dir+"gamecontrollerdb.txt");
        if(mappings.good())controls.mappingsLoaded=SDL_GameControllerAddMappingsFromFile((dir+"gamecontrollerdb.txt").c_str());
        controls.init(hidden?"input-test-unused.cfg":dir+"controls.cfg",!hidden);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
        window=SDL_CreateWindow("Plenario Fighter | SDL2",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720,hidden?SDL_WINDOW_HIDDEN:SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
        if(!window) return false;
        SDL_SetWindowMinimumSize(window,640,360);
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        if(!renderer) renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
        if(!renderer) return false;
        r=renderer;initPortraits();
        SDL_RenderSetLogicalSize(r,640,360);SDL_RenderSetIntegerScale(r,SDL_TRUE);SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
        SDL_AudioSpec want{},have{};want.freq=44100;want.format=AUDIO_F32SYS;want.channels=1;want.samples=1024;
        for(int i=0;i<=BossCharacter;i++)voices.load(i,dir+"audio/"+Voices::names[i]+".wav");
        voices.prepareMusic();
        want.callback=Voices::callback;want.userdata=&voices;
        audio=SDL_OpenAudioDevice(nullptr,0,&want,&have,0);
        voices.device=audio;
        if(audio) SDL_PauseAudioDevice(audio,0);
        return true;
    }
    ~Game(){controls.close();if(audio)SDL_CloseAudioDevice(audio);closePortraits();if(renderer)SDL_DestroyRenderer(renderer);if(window)SDL_DestroyWindow(window);SDL_Quit();}
    void tone(float pitch,float seconds,bool noise=false) {
        if(!audio || !soundOn()) return;
        std::vector<float> samples(int(seconds*44100));
        float phase=0;
        for(size_t i=0;i<samples.size();i++) {float u=float(i)/float(samples.size());phase+=pitch*(1.f-u*.65f)/44100.f;float v=noise?(float(int(random()%200)-100)/100.f):(std::sin(phase*6.2831853f)>0?1.f:-1.f);samples[i]=v*.11f*(1-u)*(1-u);}
        voices.effect(std::move(samples));
    }
    void begin() {versusActive=true;versusTime=0;bonusActive=false;voices.clear();attractActive=false;scoreboardActive=false;idleTime=0;recorded=false;points[0]=points[1]=0;match=story.active?Match(story.news().player,story.news().opponent):tournament.active?tournament.match():Match(selected,opponent);match.difficulty=Difficulty(cpuDifficulty);for(int i=0;i<2;i++)match.extraBags[i]=pendingBags[i];match.resetRound();reporter.reset(match);pendingBags[0]=pendingBags[1]=0;menu=false;lobby=false;paused=false;endingActive=false;raidTime=0;particles.clear();bursts.clear();tone(550,.16f);}
    void continueMatch() {
        if(story.active){
            if(!match.over)return;
            if(match.f[0].wins>=2){story.victory();versusActive=false;voices.clear();particles.clear();bursts.clear();}
            else begin();
            return;
        }
        if(tournament.boss() && match.over){startEnding();return;}
        if(tournament.active && match.over && match.f[0].wins>=2) {
            if(!tournament.advance())tournament.start(tournament.player);
            else if(tournament.stage%2==0){versusActive=false;bonus=ConversionBonus(tournament.player,tournament.opponent());bonusActive=true;voices.clear();return;}
        }
        begin();
    }
    void openSettings() {settings.open=true;settings.capturing=false;if(!menu)paused=true;}
    void menuAccept() {
        if(menuChoice==4){cycleDifficulty();return;}
        if(menuChoice==5){startStory();return;}
        if(menuChoice==2){openSettings();return;}
        if(menuChoice==3)tournament.start(selected);else tournament.active=false;
        local=menuChoice==1;
        if(local){lobby=true;lobbyRow=2;controls.prepareLocal();}
        else begin();
    }
    void menuNav(controls::Nav n) {
        if(n==controls::LeftNav)menuChoice=(menuChoice+MenuCount-1)%MenuCount;
        if(n==controls::RightNav)menuChoice=(menuChoice+1)%MenuCount;
        if(n==controls::UpNav)chooseCharacter(-1);
        if(n==controls::DownNav)chooseCharacter(1);
        if(n==controls::ModeNav)selectSide=1-selectSide;
        if(n==controls::AcceptNav || n==controls::PauseNav)menuAccept();
    }
    void lobbyNav(controls::Nav n) {
        if(n==controls::BackNav){lobby=false;return;}
        if(n==controls::UpNav || n==controls::DownNav) {
            const int order[]={0,1,4,5,2,3};int index=0;while(order[index]!=lobbyRow)index++;
            lobbyRow=order[(index+(n==controls::UpNav?5:1))%6];
        }
        if((n==controls::LeftNav || n==controls::RightNav) && lobbyRow>=4) {
            int &id=lobbyRow==4?selected:opponent;id=(id+(n==controls::LeftNav?CharacterCount-1:1))%CharacterCount;
        }
        if((n==controls::LeftNav || n==controls::RightNav) && lobbyRow<2)controls.cycleDevice(lobbyRow,n==controls::LeftNav?-1:1);
        if(n==controls::AcceptNav) {
            if(lobbyRow<2)controls.cycleDevice(lobbyRow,1);
            else if(lobbyRow>=4){int &id=lobbyRow==4?selected:opponent;id=(id+1)%CharacterCount;}
            else if(lobbyRow==2)begin();
            else openSettings();
        }
    }
    void events() {
        SDL_Event e;
        bool suppressNavigation=settings.capturing;
        while(SDL_PollEvent(&e)) {
            controls.event(e);
            if(e.type==SDL_QUIT) running=false;
            if(suppressNavigation && !settings.capturing && !settings.open)continue;
            bool rawAxisActivity=false;
            if(e.type==SDL_JOYAXISMOTION){auto id=std::make_pair(e.jaxis.which,int(e.jaxis.axis));auto old=lastRawAxes.find(id);rawAxisActivity=old!=lastRawAxes.end() && std::abs(int(e.jaxis.value)-old->second)>8000;lastRawAxes[id]=e.jaxis.value;}
            bool activity=(e.type==SDL_KEYDOWN && !e.key.repeat) || e.type==SDL_MOUSEBUTTONDOWN || e.type==SDL_MOUSEWHEEL || e.type==SDL_JOYBUTTONDOWN || e.type==SDL_CONTROLLERBUTTONDOWN || (e.type==SDL_JOYHATMOTION && e.jhat.value!=SDL_HAT_CENTERED) || (e.type==SDL_CONTROLLERAXISMOTION && e.caxis.axis<=SDL_CONTROLLER_AXIS_RIGHTY && std::abs(int(e.caxis.value))>18000) || (e.type==SDL_JOYAXISMOTION && e.jaxis.axis<2 && std::abs(int(e.jaxis.value))>18000);
            activity|=rawAxisActivity || (e.type==SDL_CONTROLLERAXISMOTION && e.caxis.axis>SDL_CONTROLLER_AXIS_RIGHTY && e.caxis.value>18000);
            if(e.type==SDL_KEYDOWN && !e.key.repeat && e.key.keysym.sym==SDLK_m && !settings.capturing){idleTime=0;mutedAudio=!mutedAudio;if(mutedAudio){voices.clear();voices.setMusic(0);}if(attractActive)returnMenu();continue;}
            if(activity){bool handled=passiveInput();if(handled){suppressNavigation=true;continue;}}
            if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_FOCUS_LOST){focused=false;if(!menu && !versusActive)paused=true;}
            if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_FOCUS_GAINED)focused=true;
            if(settings.open && settings.capturing){settings.captureEvent(controls,e);suppressNavigation=true;continue;}
            if(e.type!=SDL_KEYDOWN || e.key.repeat) continue;
            auto key=e.key.keysym.sym;
            if(openingActive || bootActive || scoreboardActive)continue;
            if(endingActive){if(endingTime>1 && (key==SDLK_RETURN || key==SDLK_SPACE || key==SDLK_ESCAPE))leaveEnding();continue;}
            if(settings.open){settings.key(controls,key);suppressNavigation=true;continue;}
            if(key==SDLK_F1){openSettings();continue;}
            if(key==SDLK_F2){controls.scan();continue;}
            if(key==SDLK_F11) {bool full=SDL_GetWindowFlags(window)&SDL_WINDOW_FULLSCREEN_DESKTOP;SDL_SetWindowFullscreen(window,full?0:SDL_WINDOW_FULLSCREEN_DESKTOP);}

            if(story.active && story.briefing){
                if(key==SDLK_ESCAPE)returnMenu();
                else if(key==SDLK_F6 && !story.finished)SDL_OpenURL(story.news().url);
                else if(key==SDLK_UP)story.browse(-1);
                else if(key==SDLK_DOWN)story.browse(1);
                else if(key==SDLK_LEFT)story.era(-1);
                else if(key==SDLK_RIGHT)story.era(1);
                else if(key==SDLK_RETURN || key==SDLK_SPACE){storyAccept();suppressNavigation=true;}
                continue;
            }
            if(versusActive){if(SDL_GetScancodeFromKey(key)==controls.keys[0][controls::Pause]){paused=!paused;continue;}if(key==SDLK_RETURN || key==SDLK_SPACE){finishVersus();suppressNavigation=true;}else if(key==SDLK_ESCAPE)returnMenu();continue;}
            if(bonusActive){if(key==SDLK_ESCAPE){returnMenu();}else if(SDL_GetScancodeFromKey(key)==controls.keys[0][controls::Pause] || SDL_GetScancodeFromKey(key)==controls.keys[1][controls::Pause])paused=!paused;continue;}
            if(lobby) {
                if(key==SDLK_UP)lobbyNav(controls::UpNav);
                if(key==SDLK_DOWN)lobbyNav(controls::DownNav);
                if(key==SDLK_LEFT)lobbyNav(controls::LeftNav);
                if(key==SDLK_RIGHT)lobbyNav(controls::RightNav);
                if(key==SDLK_RETURN || key==SDLK_SPACE)lobbyNav(controls::AcceptNav);
                if(key==SDLK_ESCAPE)lobbyNav(controls::BackNav);
                continue;
            }
            if(menu) {
                if(key==SDLK_F5){showScores();continue;}
                if(key==SDLK_LEFT || key==SDLK_a)menuNav(controls::LeftNav);
                if(key==SDLK_RIGHT || key==SDLK_d)menuNav(controls::RightNav);
                if(key==SDLK_UP || key==SDLK_w)menuNav(controls::UpNav);
                if(key==SDLK_DOWN || key==SDLK_s)menuNav(controls::DownNav);
                if(key==SDLK_TAB)menuNav(controls::ModeNav);
                if(key==SDLK_F3)cycleDifficulty();
                if(key==SDLK_RETURN || key==SDLK_SPACE)menuAccept();
                if(key==SDLK_ESCAPE) running=false;
            } else {
                if(key==SDLK_ESCAPE) {returnMenu();continue;}
                auto scan=SDL_GetScancodeFromKey(key);
                if((scan==controls.keys[0][controls::Pause] || (local && scan==controls.keys[1][controls::Pause])) && !match.over)paused=!paused;
                if(key==SDLK_r || (key==SDLK_RETURN && match.over)) continueMatch();
            }
        }
        auto nav=controls.navigation();
        if(controls.disconnected){if(!menu)paused=true;controls.disconnected=false;}
        if(!focused || suppressNavigation)return;
        bool navInput=false;for(bool value:nav)navInput|=value;
        if(navInput && passiveInput())return;
        if(openingActive || bootActive || scoreboardActive)return;
        if(endingActive){if(endingTime>1 && (nav[controls::AcceptNav] || nav[controls::PauseNav] || nav[controls::BackNav]))leaveEnding();return;}
        if(settings.open){if(!settings.capturing)for(int n=0;n<controls::NavCount;n++)if(nav[n])settings.nav(controls,controls::Nav(n));return;}
        if(nav[controls::SettingsNav]){openSettings();return;}
        if(story.active && story.briefing){if(nav[controls::BackNav])returnMenu();else if(nav[controls::AcceptNav] || nav[controls::PauseNav])storyAccept();else if(nav[controls::UpNav])story.browse(-1);else if(nav[controls::DownNav])story.browse(1);else if(nav[controls::LeftNav])story.era(-1);else if(nav[controls::RightNav])story.era(1);return;}
        if(versusActive){if(nav[controls::AcceptNav] || nav[controls::PauseNav])finishVersus();else if(nav[controls::BackNav])returnMenu();return;}
        if(bonusActive){if(nav[controls::PauseNav])paused=!paused;if(nav[controls::BackNav] && paused)returnMenu();return;}
        if(lobby){for(int n=0;n<controls::NavCount;n++)if(nav[n])lobbyNav(controls::Nav(n));return;}
        if(menu) {
            for(int n=0;n<controls::NavCount;n++)if(nav[n])menuNav(controls::Nav(n));
        } else {
            if(nav[controls::PauseNav] && !match.over){paused=!paused;return;}
            if(nav[controls::AcceptNav] && match.over)continueMatch();
            else if(nav[controls::BackNav] && (paused || match.over)){returnMenu();}
        }
    }
    Input keyboard(int player) {
        return controls.input(player);
    }
    void hitEffects() {
        for(const Hit&h:match.hits) {
            if(h.attacker>=0 && !attractActive)points[h.attacker]=std::min(2000000,points[h.attacker]+h.points);
            if(soundOn()) {
                int stage=h.attacker>=0?std::max(1,match.f[h.attacker].comboStage):1;
                bool kick=h.attacker>=0 && match.f[h.attacker].move==Kick;
                voices.layer(Voices::strike(kick,stage,true));
                if(!h.blocked && h.defender>=0)voices.layer(Voices::vocal(h.character,true));
                if(h.blocked)tone(620,.045f);
            }
            if(h.blocked)bursts.push_back({h.x,h.y,0,h.character,true});
            // Local comic bursts; the background never flashes on damage.
            for(int i=0;i<(h.blocked?3:5);i++)particles.push_back({h.x,h.y,float(int(random()%120)-60),float(int(random()%100)-80),.10f,h.blocked?muted:gold});
        }
    }
    void ageEffects(float dt) {
        for(auto&b:bursts)b.age+=dt;
        bursts.erase(std::remove_if(bursts.begin(),bursts.end(),[](const Burst&b){return b.age>.35f;}),bursts.end());
        for(auto&p:particles){p.life-=dt;p.x+=p.vx*dt;p.y+=p.vy*dt;p.vy+=330*dt;}
        particles.erase(std::remove_if(particles.begin(),particles.end(),[](const Particle&p){return p.life<=0;}),particles.end());
    }
    void step(float dt, bool scripted=false) {
        voices.setMusic(mutedAudio || attractActive?0:bootActive || openingActive || versusActive || bonusActive || endingActive || (story.active && story.briefing)?1:menu || scoreboardActive?2:0);
        if(bootActive){bootTime+=dt;if(bootTime>=(bootStage==0?4.f:9.f))nextBoot();return;}
        if(openingActive){openingTime+=dt;if(openingTime>=cinema::OpeningDuration)showScores();return;}
        if(scoreboardActive){scoreboardTime+=dt;if(scoreboardTime>=8)returnMenu();return;}
        if(endingActive){endingTime=std::min(11.f,endingTime+dt);return;}
        if(paused || settings.open || !focused)return;
        if(story.active && story.briefing){story.time+=dt;return;}
        if(versusActive){versusTime+=dt;if(versusTime>=VersusDuration)finishVersus();return;}
        if(bonusActive){bonus.tick(dt,keyboard(0),keyboard(1));if(bonus.over() && bonus.resultTime>=2.5f)finishBonus();return;}
        if(menu && !lobby){idleTime+=dt;if(idleTime>=15){startDemo();return;}}
        time+=dt;
        if(attractActive){demoTime+=dt;if(match.over || demoTime>=35){startDemo();return;}}
        if(!menu && !match.over) {
            if(match.bossFight)raidTime+=dt;
            int previousWinner=match.winner;
            Input p=attractActive?match.ai(dt,0):keyboard(0),q=local?keyboard(1):match.ai(dt);
            if(scripted) {p={};p.right=match.f[0].x<match.f[1].x-50;p.left=match.f[0].x>match.f[1].x+50;p.punch=true;p.ultimate=match.f[0].meter==100;}
            Move before[2]={match.f[0].move,match.f[1].move};
            int stages[2]={match.f[0].comboStage,match.f[1].comboStage};
            match.update(dt,p,q);reporter.update(dt,match);
            for(int i=0;i<2;i++)if(soundOn()) {
                auto &f=match.f[i];
                if(f.move==Ultimate && before[i]!=Ultimate){if(!voices.clips[f.character].empty())voices.play(i,f.character);else voices.layer(Voices::vocal(f.character,false,true));tone(190+f.character*27,.22f);}
                if((f.move==Punch || f.move==Kick) && (f.move!=before[i] || f.comboStage!=stages[i])){voices.layer(Voices::strike(f.move==Kick,f.comboStage,false));voices.layer(Voices::vocal(f.character,false));}
                if(f.move==Magic && before[i]!=Magic)tone(260,.08f);
            }
            hitEffects();
            if(previousWinner==-2 && match.winner!=-2){tone(180,.3f);if(match.winner>=0 && !attractActive)points[match.winner]=std::min(2000000,points[match.winner]+1000+int(match.remaining)*10);}
            if(match.over){recordMatch();if(tournament.boss() && !attractActive)startEnding();}
        }
        ageEffects(dt);
    }
    void drawMenu() {
        arena(time);
        box(0,0,640,360,Color{8,15,26,165});
        center("PLENARIO FIGHTER",320,16,white,3);
        center(std::string("ESCOLHENDO ")+(selectSide?"P2 / CPU":"P1")+" - TAB / QUADRADO-X TROCA O LADO",320,46,gold);
        const int page=(selectSide?opponent:selected)/9;
        for(int slot=0;slot<9;slot++) {
            int i=page*9+slot;if(i>=CharacterCount)break;
            int x=25+(slot%3)*198,y=62+(slot/3)*74;
            bool active=(selectSide?opponent:selected)==i;
            box(x,y,194,70,Color{24,38,52,245});border(x,y,194,70,active?gold:accent(i));
            SDL_Rect clip{x+135,y+1,58,68};SDL_RenderSetClipRect(r,&clip);drawPortrait(i,SDL_Rect{x+135,y+1,68,68},true);SDL_RenderSetClipRect(r,nullptr);
            text(characterInfo(i).shortName,x+7,y+7,white);
            wrapped(ultimateName(i),x+7,y+22,126,accent(i));
            text(category(i),x+7,y+46,muted);
            if(selected==i)text("P1",x+8,y+58,green);
            if(opponent==i)text("P2/CPU",x+47,y+58,red);
        }
        box(470,18,160,18,ink);center("ELENCO "+std::to_string(page+1)+"/"+std::to_string((CharacterCount+8)/9)+" - "+std::to_string(CharacterCount),550,23,gold);
        center("CIMA-BAIXO: LUTADOR  ESQ-DIR: MENU ABAIXO  TAB: P1/P2",320,287,muted);
        std::string labels[]={"1 JOGADOR","2 JOGADORES","CONTROLES","TORNEIO",std::string("CPU ")+difficultyLabel(),"HISTORIA"};
        for(int i=0;i<MenuCount;i++){int x=14+i*103;box(x,301,98,28,menuChoice==i?gold:Color{42,57,73,255});center(labels[i],x+49,311,menuChoice==i?ink:white);}
        center(menuChoice==4?"ENTER: ALTERAR DIFICULDADE / VALE PARA CPU E TORNEIO":menuChoice==5?"ENTER: CRONOLOGIA JOGAVEL / NOTICIAS E SATIRA":"ENTER CONFIRMA / TORNEIO: PRESIDENCIAVEIS + CHEFE",320,334,muted);
        center("F3 CPU   F5 RECORDES   F11 TELA CHEIA   M SOM",320,348,muted);
    }
    void drawLobby() {
        box(0,0,640,360,ink);box(24,22,6,24,gold);text("2 JOGADORES",40,22,white,3);
        center("P1 VS P2 - SEM ADVERSARIO CONTROLADO PELA CPU",320,59,gold);
        center("COM UM CONTROLE: P1 NO TECLADO / P2 NO CONTROLE OU PARSEC",320,79,muted);
        for(int p=0;p<2;p++) {
            int y=87+p*67;box(25,y,590,65,Color{24,38,52,255});border(25,y,590,65,lobbyRow==p?gold:Color{62,76,89,255});
            text("P"+std::to_string(p+1),38,y+9,p?red:green,2);
            auto*d=controls.device(p);text(d?d->name.substr(0,74):"TECLADO",90,y+10,white);
            text(controls.deviceDetails(p),90,y+25,muted);
            text(controls.diagnostic(p).substr(0,82),90,y+43,gold);
        }
        for(int p=0;p<2;p++) {
            int y=224+p*13;bool active=lobbyRow==4+p;
            if(active)box(25,y-2,590,12,Color{42,57,73,255});
            center(std::string(p?"P2: < ":"P1: < ")+name(p?opponent:selected)+" >",320,y,active?gold:white);
        }
        box(120,254,400,25,lobbyRow==2?gold:Color{42,57,73,255});center("INICIAR LUTA - P1 VS P2",320,260,lobbyRow==2?ink:white,2);
        box(120,287,400,20,lobbyRow==3?gold:Color{42,57,73,255});center("CONFIGURAR TECLAS E CONTROLES",320,293,lobbyRow==3?ink:white);
        center("CIMA-BAIXO ESCOLHE LINHA / ESQ-DIR TROCA CONTROLE OU LUTADOR",320,319,muted);
        center("F2 REDETECTAR  F1 CONFIGURAR  ESC VOLTAR  /  TESTE OS BOTOES ACIMA",320,336,muted);
        if(controls.devices.empty())center("PARSEC: PRESSIONE UM BOTAO NO CONTROLE DO CONVIDADO",320,349,gold);
    }
    void hud() {
        box(16,15,270,63,Color{10,19,30,235});box(354,15,270,63,Color{10,19,30,235});
        for(int i=0;i<2;i++) {
            const Fighter &f=match.f[i];int x=i?367:28;
            text(f.evolved && f.character==1?"LULA ANTIGO TESTAMENTO":name(f.character),x,24,white);text(i?(attractActive?"CPU2":local?"P2":"CPU"):attractActive?"CPU1":"P1",i?598:262,24,accent(f.character));
            box(x,38,245,12,Color{64,50,61,255});
            int health=int(float(f.hp)/f.maxHp*245);box(i?x+245-health:x,38,health,12,accent(f.character));
            box(i?x+245-health:x,38,health,3,Color{233,241,205,100});
            for(int n=0;n<2;n++){box(x+n*10,56,6,4,n<f.wins?gold:Color{63,76,88,255});}
            text(std::to_string(f.hp)+" / "+std::to_string(f.maxHp),x+29,55,muted);
            text("MALOTES "+std::to_string(f.bags)+"/"+std::to_string(f.bagLimit),x,67,f.bags?gold:red);
            for(int n=0;n<f.bagLimit;n++){int bx=x+90+n*10;box(bx,67,7,6,n<f.bags?gold:Color{52,62,73,255});}
            if(f.bags==0)text("VAZIO",x+180,67,red);
        }
        box(293,13,54,51,ink);border(293,13,54,51,gold);
        center(std::to_string(int(std::ceil(match.remaining))),320,23,gold,3);center("TEMPO",320,50,muted);
        center(tournament.active?(tournament.boss()?"FINAL BOSS / VORCARO":("TORNEIO / RIVAL "+std::to_string(tournament.stage+1)+" DE "+std::to_string(tournament.rivals.size()))):("ROUND "+std::to_string(match.round)+" / MELHOR DE 3"),320,83,muted);
        for(int i=0;i<2;i++) {
            const Fighter&f=match.f[i];int x=i?433:28;
            box(x,313,179,8,ink);box(x+1,314,int(f.meter*1.77f),6,f.meter==100?gold:accent(f.character));
            for(int n=1;n<4;n++)box(x+n*45,314,1,6,ink);
            text(f.meter==100?"ULT PRONTO":"ULT "+std::to_string(f.meter)+" / 100",x,325,f.meter==100?gold:muted);
        }
        if(attractActive)return;
        auto hint=[&](int p){std::string s="P"+std::to_string(p+1)+" ";for(int a:{controls::Punch,controls::Kick,controls::Magic,controls::Grab,controls::Ultimate})s+=std::string(controls::actionName(a))+":"+(controls.device(p)?controls.bindingLabel(p,a):controls.keyLabel(p,a))+"  ";return s.substr(0,103);};
        center(hint(0),320,341,muted);
        if(local)center(hint(1),320,352,muted);
        else center("F1 / SHARE CONFIGURAR   "+controls.keyLabel(0,controls::Pause)+" / START PAUSA   ESC MENU",320,352,muted);
    }
    void overlay(const std::string &title,const std::string &sub,Color color=gold) {
        box(80,130,480,94,Color{8,15,27,238});border(80,130,480,94,color);center(title,320,150,color,3);center(sub,320,195,white);
    }
    void drawFight() {
        if(tournament.active && match.bossFight){bossArena(time);policeRaid(raidTime,match.f[0].character);}else battleArena(time,match);
        for(const Fighter&f:match.f)if(f.move==Ultimate) {
            Color c=accent(f.character);
            if(f.clock<.55f) {
                for(int n=0;n<12;n++) {
                    float a=n*.5236f+f.clock*8;float radius=65-f.clock*45;
                    box(int(f.x+std::cos(a)*radius),int(f.y-65+std::sin(a)*radius),3,3,c);
                }
                box(int(f.x)-36,int(f.y)-2,72,3,gold);
            }
        }
        for(int owner=0;owner<2;owner++) {
            const auto &a=match.assists[owner];if(a.remaining<=0)continue;
            for(int n=0;n<10;n++){float phase=time*5+n*.628f;box(int(a.actor.x+std::cos(phase)*35),int(a.actor.y-60+std::sin(phase)*65),2,2,gold);}
            fighter(a.actor,time);
            center("ALIADO P"+std::to_string(owner+1)+" / "+std::to_string(a.remaining).substr(0,3)+"S",int(std::clamp(a.actor.x,70.f,570.f)),int(a.actor.y)-159,gold);
        }
        for(int n=0;n<2;n++) {
            int index=match.puller<0?n:(n==0?1-match.puller:match.puller);
            const auto &f=match.f[index];
            box(int(f.x)-33,289,66,5,Color{7,15,25,145});fighter(f,time);
            if(f.comboStage>0 && (f.move==Punch || f.move==Kick)) {
                std::string label=(f.move==Punch?"SOCO ":"CHUTE ")+std::to_string(f.comboStage)+"/3";
                if(f.comboStage==3)label+=" FINAL!";
                center(label,int(std::clamp(f.x,80.f,560.f)),int(f.y)-(f.crouch?87:158),f.comboStage==3?gold:white);
            }
            if(f.evolutionFlash>0) {
                for(int k=0;k<16;k++){float angle=time*8+k*.3927f;float radius=35+(1.2f-f.evolutionFlash)*32;box(int(f.x+std::cos(angle)*radius),int(f.y-65+std::sin(angle)*radius),4,4,gold);}
                center(f.character==1?"ANTIGO TESTAMENTO!":"VORCARO INVOCADO!",194,118,gold,2);
            }
            if(f.guard) {
                int y=int(f.y)-(f.crouch?64:130);
                center(f.crouch?"DEFESA BAIXA":"DEFESA ALTA",int(f.x),y-15,accent(f.character));
                for(int k=0;k<5;k++)box(int(f.x)+f.facing*(35+k%2*3),y+k*8,2,5,accent(f.character));
            }
            if(f.thrown)for(int k=0;k<4;k++)line(int(f.x)-f.facing*(28+k*8),int(f.y)-65+k*11,int(f.x)-f.facing*(48+k*8),int(f.y)-65+k*11,white);
            if(f.knockdown>0 && !f.knockedOut) {
                int x=int(std::clamp(f.x,90.f,550.f));
                center("IMOBILIZADO "+std::to_string(f.knockdown).substr(0,3)+"S",x,198,gold);
                for(int k=0;k<3;k++){float a=time*7+k*2.094f;box(x+int(std::cos(a)*27),218+int(std::sin(a)*7),5,5,gold);}
                box(x-30,236,60,3,ink);box(x-30,236,int(f.knockdown*60),3,gold);
            }
        }
        if(match.puller>=0) {
            const auto &a=match.f[match.puller], &b=match.f[1-match.puller];
            if(match.pullTime>.18f && match.pullTime<.34f)for(int n=0;n<7;n++) {
                int y=int(b.y)-115+n*15;
                line(int(b.x)+a.facing*38,y,int(b.x)+a.facing*(65+n*3),y,white);
            }
            if(match.pullTime>=.34f && match.pullTime<.52f) {
                int x=int(b.x),y=int(b.y)-87;
                for(int n=0;n<12;n++){float angle=n*6.28318f/12;
                    line(x+int(std::cos(angle)*16),y+int(std::sin(angle)*16),x+int(std::cos(angle)*40),y+int(std::sin(angle)*40),n%2?gold:white);}
                center("PUXAO!",320,112,gold,2);
            }
            if(match.pullImpact)center("+"+std::to_string(match.healed)+" VIDA",int(a.x),int(a.y)-164,green,2);
        }
        for(const Wave&w:match.waves) {
            projectile(w);
        }
        for(const auto&b:bursts) {
            Color c=accent(b.character);float progress=b.age/.35f;int radius=int(10+progress*48);
            if(b.blocked){
                int cx=int(b.x),cy=int(b.y),rad=int(18+std::sin(progress*3.14159f)*22);
                auto starburst=[&](int radius,Color color){
                    SDL_Vertex v[25]{};v[0].position={float(cx),float(cy)};v[0].color=color;
                    for(int n=0;n<24;n++){float angle=n*6.283185f/24;float len=n%2?radius*.46f:float(radius);v[n+1].position={cx+std::cos(angle)*len,cy+std::sin(angle)*len};v[n+1].color=color;}
                    int idx[72];for(int n=0;n<24;n++){idx[n*3]=0;idx[n*3+1]=n+1;idx[n*3+2]=(n+1)%24+1;}SDL_RenderGeometry(r,nullptr,v,25,idx,72);
                };
                starburst(rad+3,ink);starburst(rad,Color{140,222,255,Uint8(255*(1-progress*.6f))});starburst(rad-7,white);
                box(cx-19,cy-6,38,13,ink);center("BLOQ!",cx,cy-3,Color{130,215,255,255});
                for(int n=0;n<8;n++){float a=n*.785398f;int rr=rad+8+int(progress*14);box(cx+int(std::cos(a)*rr),cy+int(std::sin(a)*rr),4,4,c);}
                continue;
            }
            for(int n=0;n<16;n++) {
                float angle=n*6.28318f/16+progress;
                int x=int(b.x+std::cos(angle)*radius),y=int(b.y+std::sin(angle)*radius);
                line(x,y,x+int(std::cos(angle)*10*(1-progress)),y+int(std::sin(angle)*10*(1-progress)),n%2?white:c);
                if(b.character==2){box(x-3,y-3,7,4,c);box(x-1,y+1,3,4,white);}
                else if(b.character==4){line(x,y,x+7,y-5,white);line(x+7,y-5,x+3,y-10,c);}
                else if(n%3==0)box(x-2,y-2,5,5,c);
            }
        }
        for(const Particle&p:particles)box(int(p.x),int(p.y),3,3,p.color);
        hud();
        if(!attractActive){box(8,83,374,19,Color{7,13,24,215});text(stageName(match.f[1].character),16,89,muted);}
        if(!attractActive && match.intro<=0 && match.winner==-2)reporterPanel(reporter,time,match.f[1].character);
        for(int i=0;i<2;i++)if(match.f[i].move==Ultimate) {
            box(8,83,374,20,Color{8,15,27,235});center(std::string("ULT / ")+ultimateName(match.f[i].character),194,89,accent(match.f[i].character));
        }
        if(match.intro>0) {
            if(match.intro>.65f)center("ROUND "+std::to_string(match.round),320,149,white,4);
            else center("LUTEM!",320,149,gold,5);
        }
        if(match.winner!=-2) {
            bool finished=match.over;
            std::string title=match.winner==-1?"EMPATE":finished?"VITORIA!":match.remaining<=0?"TEMPO ESGOTADO":"K.O.";
            std::string sub=match.winner==-1?"NOVO ROUND":std::string(name(match.f[match.winner].character))+" VENCEU";
            if(match.koActive){box(207,104,226,43,Color{8,15,27,220});center("K.O.",320,108,gold,3);center("CAMERA LENTA",320,136,white);}
            else overlay(title,sub);
            if(finished) {
                box(95,231,450,31,ink);
                bool won=match.f[0].wins>=2;
                std::string action=story.active?(won?"ENTER: PROXIMA NOTICIA / ESC MENU":"ENTER: REPETIR CAPITULO / ESC MENU"):tournament.active?(won?(tournament.boss()?"TORNEIO CONCLUIDO! ENTER REINICIA":"ENTER PROXIMO ADVERSARIO / ESC MENU"):"ENTER TENTAR ESTA ETAPA / ESC MENU"):"ENTER REVANCHE / ESC MENU";
                center(action,320,242,gold);
            }
        }
        if(paused)overlay("PAUSADO",controls.keyLabel(0,controls::Pause)+" / START CONTINUAR   F1 / SHARE CONFIGURAR");
    }
    void draw() {
        SDL_SetRenderDrawColor(r,ink.r,ink.g,ink.b,255);SDL_RenderClear(r);
        
        if(bootActive){if(bootStage==0)arcadeSeal(bootTime);else arcadeSponsors(bootTime);}else if(scoreboardActive)arcadeScores(scoreboard);
        else if(openingActive)opening(openingTime);else if(endingActive)ending(endingTime,endingWon,match.f[0].character);
        else if(settings.open)settings.draw(controls);else if(story.active && story.briefing)storyScreen(story);else if(versusActive){versusScreen(match,versusTime);if(paused)overlay("PAUSADO","P CONTINUA / CONFIRMAR PULA A APRESENTACAO");}else if(bonusActive){conversionBonus(bonus);if(paused)overlay("PAUSADO","START PARA CONTINUAR");}else if(lobby)drawLobby();else if(menu)drawMenu();else drawFight();
        if(attractActive){box(128,92,384,25,ink);center("DEMONSTRACAO / CPU VS CPU",320,98,gold,2);box(91,333,458,22,ink);center("APERTE QUALQUER TECLA, BOTAO OU CLIQUE PARA JOGAR",320,341,white);}
        SDL_RenderSetViewport(r,nullptr);
        if(crtEnabled) {
            // Subtle scanlines, phosphor grille and dark glass edges, all native SDL2.
            for(int y=0;y<360;y+=2)box(0,y,640,1,Color{0,0,0,31});
            for(int x=0;x<640;x+=3){box(x,0,1,360,Color{100,36,22,9});box(x+2,0,1,360,Color{18,58,112,9});}
            for(int n=0;n<7;n++){box(n,0,1,360,Color{0,0,0,Uint8(70-n*8)});box(639-n,0,1,360,Color{0,0,0,Uint8(70-n*8)});}
            box(0,0,640,2,Color{0,0,0,100});box(0,358,640,2,Color{0,0,0,100});
        }
    }
    bool capture(const std::string &path) {
        int w,h;SDL_GetRendererOutputSize(r,&w,&h);
        SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
        if(!s)return false;
        bool ok=SDL_RenderReadPixels(r,nullptr,s->format->format,s->pixels,s->pitch)==0 && SDL_SaveBMP(s,path.c_str())==0;
        SDL_FreeSurface(s);return ok;
    }
};

void check(bool value,const char*message){if(!value)throw std::runtime_error(message);}
int selfTest() {
    try {
        for(int selected=0;selected<CharacterCount;selected++) {
            Tournament tour;tour.start(selected);bool visited[CharacterCount]{};
            for(int stage=0;stage<int(tour.rivals.size());stage++) {
                auto duel=tour.match();int rival=tour.opponent();
                check(rival!=selected && presidential(rival) && !visited[rival] && !duel.bossFight,"Tournament visits every other contender once");
                visited[rival]=true;check(tour.advance(),"Tournament advances after an opponent");
            }
            auto boss=tour.match();check(tour.boss() && boss.f[1].character==BossCharacter && boss.f[1].hp==180 && !tour.advance(),"Boss is the final exclusive stage");
            boss.resetRound();check(boss.f[1].maxHp==180 && boss.f[1].meter==100,"Boss stats survive round reset");
        }
        Match forbidden(0,BossCharacter);check(!forbidden.bossFight && forbidden.f[1].character!=BossCharacter,"Normal modes cannot select boss");
        Match evolve(1,3);evolve.intro=0;evolve.f[0].hp=50;evolve.update(1.f/60,{},{});
        check(evolve.f[0].evolved && evolve.f[0].evolutionFlash>0,"Lula evolves at half health");
        evolve.damage(0,1,7,false);check(evolve.f[1].hp==90,"Old Testament Lula gains melee damage");
        evolve.f[0].hp=85;evolve.update(1.f/60,{},{});check(evolve.f[0].evolved,"Healing does not undo evolution");
        evolve.resetRound();check(!evolve.f[0].evolved,"Evolution resets between rounds");
        Match summon(0,3);summon.intro=0;summon.f[0].hp=50;summon.f[0].x=250;summon.f[1].x=340;
        summon.update(1.f/60,{},{});check(summon.assists[0].remaining>4.9f && summon.assists[0].actor.character==BossCharacter,"Flavio summons Vorcaro");
        for(int i=0;i<299;i++)summon.update(1.f/60,{},{});
        check(summon.f[1].hp<100 && summon.assists[0].remaining<.001f,"Assist attacks autonomously and expires at five combat seconds");
        for(int i=0;i<5;i++)summon.update(1.f/60,{},{});
        int after=summon.f[1].hp;for(int i=0;i<90;i++)summon.update(1.f/60,{},{});
        check(summon.assists[0].remaining==0 && summon.f[1].hp==after,"Expired assist stops attacking and cannot respawn from low health");
        summon.resetRound();check(summon.assists[0].remaining==0 && !summon.f[0].evolved,"Reset removes summoned ally");
        Match guided(4,3);guided.intro=0;guided.f[0].meter=100;guided.f[0].x=150;guided.f[1].x=480;guided.start(guided.f[0],Ultimate);
        Input crouchingTarget;crouchingTarget.crouch=true;
        bool steered=false;for(int i=0;i<100;i++){guided.update(1.f/60,{},crouchingTarget);for(auto&w:guided.waves)steered|=w.kind==ProjectileKind::Drone && w.y>FLOOR-83;}
        check(steered && guided.f[1].hp<100,"Guided drones curve toward a crouched target");
        Match fast;fast.intro=0;Input move;move.right=true;float initial=fast.f[0].x;fast.update(.1f,move,{});
        check(std::abs(fast.f[0].x-initial-19.5f)<.01f,"Movement speed is 195 pixels per second");
        Input fastMagic;fastMagic.magic=true;fast.update(1.f/60,fastMagic,{});for(int i=0;i<8;i++)fast.update(1.f/60,{},{});
        check(!fast.waves.empty(),"Money launches after 0.12 second startup");float waveX=fast.waves.front().x;fast.update(1.f/60,{},{});
        check(std::abs(fast.waves.front().x-waveX-BagSpeed/60)<.01f,"Money travels at 430 pixels per second");
        Match m; m.intro=0;m.f[0].x=250;m.f[1].x=298;
        Input punch; punch.punch=true;
        m.update(1.f/60,punch,{});for(int i=0;i<10;i++)m.update(1.f/60,{},{});
        check(m.f[1].hp==93,"Punch should hit once for 7");check(m.f[0].meter==15,"Hit gains meter");
        for(int i=0;i<35;i++)m.update(1.f/60,{},{});
        check(m.f[1].hp==93,"No repeat damage during recovery");
        for(int direction:{-1,1}) {
            for(bool lowAttack:{false,true})for(bool lowGuard:{false,true}) {
                Match block;block.intro=0;block.f[0].x=320;block.f[1].x=320+direction*48;
                Input attack,defense;attack.kick=true;attack.crouch=lowAttack;defense.crouch=lowGuard;
                defense.left=direction<0;defense.right=direction>0;
                block.update(1.f/60,attack,defense);for(int frame=0;frame<15;frame++)block.update(1.f/60,{},defense);
                check(block.f[1].hp==(lowAttack && !lowGuard?88:99),"Back blocks mid attacks; down-back required for lows, both sides");
            }
            for(bool lowGuard:{false,true}) {
                Match aerial;aerial.intro=0;aerial.f[0].x=320;aerial.f[1].x=320+direction*48;
                Input attack,defense;attack.jump=true;attack.kick=true;defense.crouch=lowGuard;defense.left=direction<0;defense.right=direction>0;
                aerial.update(1.f/60,attack,defense);for(int frame=0;frame<14;frame++)aerial.update(1.f/60,{},defense);
                check(aerial.f[1].hp==(lowGuard?88:99),"Air kick defeats low guard and is blocked standing on either side");
            }
            Match facing;facing.intro=0;facing.f[0].x=320;facing.f[1].x=320+direction*48;
            Input back;back.left=direction<0;back.right=direction>0;facing.update(1.f/60,{},back);
            check(facing.f[1].guard,"Directional guard activates without auxiliary button");
            facing.f[0].x=facing.f[1].x+direction*48;facing.update(1.f/60,{},back);
            check(!facing.f[1].guard,"Guard direction reverses after sides switch");
            Input attackBack=back;attackBack.kick=true;facing.update(1.f/60,{},attackBack);
            check(facing.f[1].move==Kick,"Holding back does not prevent an intentional attack");
        }
        for(bool lowGuard:{false,true}) {
            Match lowBag;lowBag.intro=0;lowBag.f[0].x=200;lowBag.f[1].x=380;
            Input fire,defense;fire.magic=true;fire.crouch=true;defense.guard=true;defense.crouch=lowGuard;
            lowBag.update(1.f/60,fire,defense);for(int i=0;i<95;i++)lowBag.update(1.f/60,{},defense);
            check(lowBag.f[1].hp==(lowGuard?99:92),"Crouched money projectile respects low guard");
        }
        for(int direction:{-1,1})for(bool corner:{false,true}) {
            Match toss;toss.intro=0;toss.f[0].x=corner?(direction==1?550.f:90.f):320.f;
            toss.f[0].facing=direction;toss.f[1].x=toss.f[0].x+direction*48;toss.f[0].hp=70;
            toss.grab(0,1);toss.update(.53f,{},{});
            check(toss.puller==-1 && toss.f[1].thrown && toss.f[1].vy<0,"Grab releases into an airborne throw");
            int frames=0;while(toss.f[1].thrown && frames++<120)toss.update(1.f/60,{},{});
            check(toss.f[1].knockdown==1.f && toss.f[1].grounded(),"One-second knockdown begins exactly on landing");
            float lockedX=toss.f[1].x;int hp=toss.f[1].hp;Input mash;mash.left=mash.right=mash.jump=mash.kick=mash.magic=mash.grab=true;
            for(int i=0;i<59;i++){toss.update(1.f/60,{},mash);toss.damage(0,1,12,false);toss.grab(0,1);}
            check(toss.f[1].knockdown>0 && toss.f[1].x==lockedX && toss.f[1].move==Idle && toss.f[1].hp==hp && toss.puller==-1,"Knockdown locks inputs and cannot be extended by hits or grabs");
            for(int i=0;i<3;i++)toss.update(1.f/60,{},{});
            Input jump;jump.jump=true;toss.update(1.f/60,{},jump);
            check(toss.f[1].knockdown==0 && toss.f[1].y<FLOOR && toss.f[0].hp==88,"Victim regains control after one second; healing occurs once");
        }
        Match freeAttacker;freeAttacker.intro=0;freeAttacker.f[0].x=250;freeAttacker.f[1].x=300;freeAttacker.f[1].knockdown=1;
        freeAttacker.start(freeAttacker.f[0],Punch);for(int i=0;i<24;i++)freeAttacker.update(1.f/60,{},{});
        check(freeAttacker.f[0].move==Idle && freeAttacker.f[1].knockdown>0,"Attacker recovers normally while victim is down");
        freeAttacker.f[0].meter=100;freeAttacker.start(freeAttacker.f[0],Ultimate);freeAttacker.f[1].knockdown=1;
        for(int i=0;i<40;i++)freeAttacker.update(1.f/60,{},{});
        check(freeAttacker.f[0].emitted==1,"Ultimate emission timing is not delayed by victim knockdown");
        Match fatalThrow;fatalThrow.intro=0;fatalThrow.f[0].x=250;fatalThrow.f[1].x=298;fatalThrow.f[1].hp=4;
        fatalThrow.grab(0,1);fatalThrow.update(.53f,{},{});fatalThrow.update(1.f/60,{},{});
        check(fatalThrow.winner==-2 && fatalThrow.f[1].thrown,"KO waits for throw landing");
        for(int i=0;i<100 && fatalThrow.winner==-2;i++)fatalThrow.update(1.f/60,{},{});
        check(fatalThrow.winner==0 && fatalThrow.f[1].grounded(),"Lethal throw finishes round on the ground");
        Match guard;guard.intro=0;guard.f[0].x=250;guard.f[1].x=298;Input defend;defend.guard=true;
        guard.update(1.f/60,punch,defend);for(int i=0;i<10;i++)guard.update(1.f/60,{},defend);
        check(guard.f[1].hp==99,"Guard reduces damage");
        Match u;u.intro=0;u.f[0].x=220;u.f[1].x=350;
        check(!u.start(u.f[0],Ultimate),"Empty ultimate must fail");u.f[0].meter=100;
        check(u.start(u.f[0],Ultimate),"Full ultimate should start");check(u.f[0].meter==0,"Ultimate consumes meter");
        for(int i=0;i<170;i++)u.update(1.f/60,{},{});
        check(u.f[1].hp==40,"Six-shot ultimate deals 60 total");
        Match blocked;blocked.intro=0;blocked.f[0].x=220;blocked.f[1].x=350;blocked.f[0].meter=100;blocked.start(blocked.f[0],Ultimate);
        for(int i=0;i<170;i++)blocked.update(1.f/60,{},defend);
        check(blocked.f[1].hp==94,"Guard reduces ultimate to 6");
        Match miss;miss.intro=0;miss.update(1.f/60,punch,{});for(int i=0;i<30;i++)miss.update(1.f/60,{},{});
        check(miss.f[1].hp==100 && miss.f[0].meter==0,"Out-of-range punches cannot hit or charge meter");
        Match dodge;dodge.intro=0;dodge.f[0].x=220;dodge.f[1].x=300;dodge.waves.push_back({290,FLOOR-87,1,0});dodge.f[1].y=FLOOR-120;
        dodge.update(1.f/60,{},{});check(dodge.f[1].hp==100,"Jump clears projectile hitbox");
        Match jump;jump.intro=0;Input up;up.jump=true;jump.update(1.f/60,up,{});
        check(jump.f[0].y<FLOOR,"Jump leaves ground");for(int i=0;i<90;i++)jump.update(1.f/60,{},{});
        check(jump.f[0].grounded(),"Jump lands");
        Input magic;magic.magic=true;Input duck;duck.crouch=true;
        Input crouchKick; crouchKick.crouch=true; crouchKick.kick=true;
        Match crouchKickMatch;crouchKickMatch.intro=0;crouchKickMatch.f[0].x=250;crouchKickMatch.f[1].x=298;
        crouchKickMatch.update(1.f/60,crouchKick,{});for(int i=0;i<14;i++)crouchKickMatch.update(1.f/60,{},{});
        check(crouchKickMatch.f[1].hp==88,"Crouched kick hits");
        Input airKick;airKick.jump=true;airKick.kick=true;
        Match airKickMatch;airKickMatch.intro=0;airKickMatch.f[0].x=250;airKickMatch.f[1].x=320;
        airKickMatch.update(1.f/60,airKick,{});for(int i=0;i<14;i++)airKickMatch.update(1.f/60,{},{});
        check(airKickMatch.f[0].y<FLOOR && airKickMatch.f[1].hp==88,"Flying kick hits in the air");
        Input airMagic;airMagic.jump=true;airMagic.magic=true;
        Match airMagicMatch;airMagicMatch.intro=0;airMagicMatch.f[0].x=180;airMagicMatch.f[1].x=370;
        airMagicMatch.update(1.f/60,airMagic,{});for(int i=0;i<90;i++)airMagicMatch.update(1.f/60,{},{});
        check(airMagicMatch.f[1].hp==92,"Flying money launch hits");
        Match bag;bag.intro=0;bag.f[0].x=180;bag.f[1].x=370;bag.update(1.f/60,magic,{});
        for(int i=0;i<90;i++)bag.update(1.f/60,{},{});
        check(bag.f[1].hp==92,"Single money bag deals 8 once");
        Match crouching;crouching.intro=0;crouching.f[0].x=180;crouching.f[1].x=370;crouching.update(1.f/60,magic,duck);
        for(int i=0;i<150;i++)crouching.update(1.f/60,{},duck);
        check(crouching.f[1].hp==100 && crouching.f[1].crouch,"Crouch avoids money bag");
        crouching.update(1.f/60,{},{});check(!crouching.f[1].crouch,"Releasing crouch restores standing");
        Match crouchMoney;crouchMoney.intro=0;crouchMoney.f[0].x=180;crouchMoney.f[1].x=370;Input crouchMagic; crouchMagic.crouch=true;crouchMagic.magic=true;
        crouchMoney.update(1.f/60,crouchMagic,{});for(int i=0;i<90;i++)crouchMoney.update(1.f/60,{},{});
        check(crouchMoney.f[1].hp==92,"Crouched money launch hits");
        Match grabMatch;grabMatch.intro=0;grabMatch.f[0].x=250;grabMatch.f[1].x=298;grabMatch.f[0].hp=70;Input grabInput;grabInput.grab=true;
        grabMatch.update(1.f/60,grabInput,{});for(int i=0;i<70;i++)grabMatch.update(1.f/60,{},{});
        check(grabMatch.f[1].hp==96 && grabMatch.f[0].hp==88 && grabMatch.f[0].meter==10,"Grab damages and heals attacker");
        for(int dir:{-1,1}) {
            Match pull;pull.intro=0;pull.f[0].x=dir==1?35.f:605.f;pull.f[0].facing=dir;
            pull.f[1].x=pull.f[0].x+dir*50;pull.f[0].hp=95;
            float original=pull.f[1].x;pull.grab(0,1);
            pull.update(.2f,{},{});check(pull.f[0].hp==95 && pull.f[1].hp==100,"Pull grip does not heal before impact");
            pull.update(.15f,{},{});check(std::abs(pull.f[1].x-pull.f[0].x)<std::abs(original-pull.f[0].x),"Pull moves victim toward attacker in both directions");
            check(pull.f[0].hp==100 && pull.healed==5 && pull.f[1].hp==96,"Pull impact heals once and caps health");
            for(int n=0;n<90;n++)pull.update(1.f/60,{},{});
            check(pull.puller==-1 && pull.f[1].grounded() && pull.f[1].x>=35 && pull.f[1].x<=605 && pull.f[1].hp==96,"Pull releases at both arena edges without repeated damage");
        }
        for(int id=0;id<CharacterCount;id++) {
            Match roster(id,(id+3)%CharacterCount);roster.intro=0;roster.f[1].evolved=true;roster.f[0].meter=100;
            check(roster.start(roster.f[0],Ultimate),"Every roster member starts an ultimate");
            bool seen=false;
            for(int frame=0;frame<150;frame++) {
                roster.update(1.f/60,{},{});
                for(const auto&w:roster.waves){seen=true;check(w.kind==ultimateKind(id) || (id==0 && w.kind==ProjectileKind::Banknotes),"Roster-specific projectile");}
            }
            check(seen && roster.f[1].hp==40,"Every roster ultimate emits and deals six hits");
            roster.resetRound();check(roster.f[0].character==id && roster.f[1].character==(id+3)%CharacterCount,"Roster survives round reset");
        }
        for(int left=0;left<CharacterCount;left++)for(int right=0;right<CharacterCount;right++) {
            Match pair(left,right);check(pair.f[0].character==left && pair.f[1].character==right,"All roster matchups including mirror matches");
        }
        Match grabGuard;grabGuard.intro=0;grabGuard.f[0].x=250;grabGuard.f[1].x=298;grabGuard.f[0].hp=70;
        grabGuard.update(1.f/60,grabInput,defend);for(int i=0;i<70;i++)grabGuard.update(1.f/60,{},defend);
        check(grabGuard.f[1].hp==96 && grabGuard.f[0].hp==88,"Grab bypasses guard");
        Match grabDuck;grabDuck.intro=0;grabDuck.f[0].x=250;grabDuck.f[1].x=298;Input grabDuckInput;grabDuckInput.crouch=true;
        grabDuck.update(1.f/60,grabInput,grabDuckInput);for(int i=0;i<14;i++)grabDuck.update(1.f/60,{},grabDuckInput);
        check(grabDuck.f[1].hp==100,"Grab misses crouching target");
        Match grabAir;grabAir.intro=0;grabAir.f[0].x=250;grabAir.f[1].x=298;Input grabJump;grabJump.jump=true;grabJump.grab=true;
        grabAir.update(1.f/60,grabJump,{});for(int i=0;i<14;i++)grabAir.update(1.f/60,{},{});
        check(grabAir.f[1].hp==100,"Grab requires grounded attacker");
        Match cooldown;cooldown.intro=0;cooldown.update(1.f/60,magic,{});for(int i=0;i<24;i++)cooldown.update(1.f/60,{},{});
        check(!cooldown.start(cooldown.f[0],Magic),"Money bag cannot bypass cooldown");
        for(int i=0;i<30;i++)cooldown.update(1.f/60,{},{});
        check(cooldown.start(cooldown.f[0],Magic),"Cooldown permits next bag");
        Match duckUlt;duckUlt.intro=0;duckUlt.f[0].meter=100;duckUlt.start(duckUlt.f[0],Ultimate);
        for(int i=0;i<200;i++)duckUlt.update(1.f/60,{},duck);
        check(duckUlt.f[1].hp==100,"Crouch avoids ultimate salvo");
        Match leap;leap.intro=0;leap.f[0].x=180;leap.f[1].x=370;leap.update(1.f/60,magic,{});
        for(int i=0;i<5;i++)leap.update(1.f/60,{},{});
        leap.update(1.f/60,{},up);for(int i=0;i<100;i++)leap.update(1.f/60,{},{});
        check(leap.f[1].hp==100,"Timed real jump avoids money bag");
        for(int character=0;character<2;character++) {
            Match volley(character);volley.intro=0;volley.f[0].meter=100;volley.start(volley.f[0],Ultimate);
            for(int i=0;i<80;i++)volley.update(1.f/60,{},duck);
            check(volley.f[0].emitted>=4,"Ultimate emits over time");
            for(const auto&w:volley.waves)check(character?w.kind==ProjectileKind::PTStar:(w.kind==ProjectileKind::Chocolate || w.kind==ProjectileKind::Banknotes),"Character projectile identity");
        }
        Match timed;timed.intro=0;timed.remaining=.001f;timed.f[1].hp=80;timed.update(1.f/60,{},{});
        check(timed.winner==0 && timed.f[0].wins==1,"Timeout awards higher HP");
        for(int i=0;i<160;i++)timed.update(1.f/60,{},{});
        check(timed.round==2 && timed.f[0].wins==1 && timed.f[1].hp==100,"Round resets and preserves score");
        timed.intro=0;timed.f[1].hp=0;timed.update(1.f/60,{},{});for(int i=0;i<160;i++)timed.update(1.f/60,{},{});
        check(timed.over && timed.f[0].wins==2,"Best of three ends");
        Match tied;tied.intro=0;tied.remaining=0;tied.update(1.f/60,{},{});check(tied.winner==-1 && tied.f[0].wins==0,"Draw awards no points");
        Match lock;lock.update(1.f/60,punch,{});check(lock.f[0].move==Idle,"Intro locks attacks");
        Match sim;sim.intro=0;sim.f[0].x=250;sim.f[1].x=298;sim.update(1.f/60,punch,punch);for(int i=0;i<10;i++)sim.update(1.f/60,{},{});
        check(sim.f[0].hp==93 && sim.f[1].hp==93,"Simultaneous punches trade");
        Match endurance;endurance.intro=0;Input attack;attack.punch=true;
        for(int i=0;i<15000 && !endurance.over;i++) {
            attack.right=endurance.f[0].x<endurance.f[1].x-49;attack.left=endurance.f[0].x>endurance.f[1].x+49;attack.ultimate=endurance.f[0].meter==100;
            endurance.update(1.f/60,attack,endurance.ai(1.f/60));
            for(auto &f:endurance.f)check(f.hp>=0 && f.hp<=100 && f.meter>=0 && f.meter<=100 && f.x>=35 && f.x<=605 && f.y<=FLOOR,"Simulation invariants");
        }
        check(endurance.over,"CPU match finishes");
        std::cout<<"PASS: combat, guard, animated salvos, money bags, crouch, jump dodge, cooldown, projectile identity, rounds, CPU simulation\n";return 0;
    } catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}
}

#include "input_tests.h"
#include "combo_tests.h"
#include "combat_v12_tests.h"

int flowTests(Game &game) {
    try {
        auto key=[&](SDL_Keycode k){SDL_Event e{};e.type=SDL_KEYDOWN;e.key.keysym.sym=k;SDL_PushEvent(&e);game.events();};
        game.openingActive=true;game.step(.5f);check(game.openingActive && game.menu,"Opening precedes menu");
        key(SDLK_RETURN);check(!game.openingActive && game.scoreboardActive,"Opening leads to scoreboard");key(SDLK_RETURN);check(game.menu && !game.scoreboardActive,"Scoreboard leads to menu");
        key(SDLK_RIGHT);check(game.menuChoice==1 && game.selected==0,"Right changes lower menu only");
        key(SDLK_DOWN);check(game.selected==1 && game.menuChoice==1,"Down changes fighter only");
        key(SDLK_UP);key(SDLK_LEFT);check(game.selected==0 && game.menuChoice==0,"Reverse navigation");
        game.menuChoice=4;game.menuAccept();check(game.cpuDifficulty==2 && game.menu,"Difficulty cycles to hard");
        game.menuAccept();check(game.cpuDifficulty==0,"Difficulty wraps easy");
        game.menuChoice=5;game.menuAccept();check(game.story.active && game.story.briefing && game.crtEnabled,"History replaces TV setting");key(SDLK_F4);check(game.crtEnabled,"F4 leaves native CRT enabled");game.returnMenu();
        game.menuChoice=0;game.menuAccept();check(game.match.difficulty==Difficulty::Easy,"Selected CPU difficulty applied");
        for(bool won:{true,false}) {
            game.tournament.start(3);game.tournament.stage=int(game.tournament.rivals.size());game.begin();game.finishVersus();game.match.intro=0;
            int victor=won?0:1;game.match.f[victor].wins=1;
            game.finishVersus();game.match.damage(victor,1-victor,500,false);game.match.update(.01f,{},{});
            for(int n=0;n<600 && !game.endingActive;n++)game.step(1.f/60);
            check(game.endingActive && game.endingWon==won,"Boss match automatically starts correct ending");
            game.step(11);game.draw();check(game.endingTime>=10,"Ending reaches stable final scene");
            key(SDLK_RETURN);check(game.menu && !game.endingActive && !game.tournament.active,"Ending returns to menu");
        }
        std::cout<<"PASS: opening skip, menu axes, difficulty, CRT, boss win/loss ending transitions\n";return 0;
    }catch(const std::exception&e){std::cerr<<"FLOW FAIL: "<<e.what()<<"\n";return 1;}
}

int arcadeTests(Game &game) {
    try {
        auto key=[&](SDL_Keycode k){SDL_Event e{};e.type=SDL_KEYDOWN;e.key.keysym.sym=k;SDL_PushEvent(&e);game.events();};
        game.bootActive=true;game.bootStage=0;game.step(4.1f);check(game.bootStage==1 && !game.openingActive,"Seal precedes sponsors");
        game.step(9.1f);check(game.openingActive && !game.bootActive,"Sponsors precede opening");
        game.step(cinema::OpeningDuration+.1f);check(game.scoreboardActive,"Opening automatically reaches records");
        game.step(8.1f);check(game.menu && !game.scoreboardActive,"Records automatically reach menu");
        game.step(15.1f);check(game.attractActive && !game.menu,"Idle menu enters CPU demonstration");
        int oldIndex=game.demoIndex;game.match.over=true;game.step(.01f);check(game.demoIndex==oldIndex+1,"Demonstration cycles matches");
        game.match.intro=0;for(int n=0;n<600;n++)game.step(1.f/60);
        check(game.scoreboard.rows.empty() && game.points[0]==0 && game.points[1]==0,"CPU demo never writes records");
        key(SDLK_RETURN);check(game.menu && !game.attractActive && !game.lobby,"Demo input is consumed, not forwarded to menu");
        game.startDemo();SDL_Event click{};click.type=SDL_MOUSEBUTTONDOWN;click.button.button=SDL_BUTTON_LEFT;SDL_PushEvent(&click);game.events();check(game.menu && !game.attractActive,"Mouse click exits demonstration");
        game.startDemo();SDL_Event button{};button.type=SDL_JOYBUTTONDOWN;button.jbutton.button=3;SDL_PushEvent(&button);game.events();check(game.menu && !game.attractActive,"Generic gamepad button exits demonstration");
        game.menuChoice=0;game.begin();game.match.over=true;game.match.f[0].wins=2;game.points[0]=2300;game.recordMatch();game.recordMatch();
        check(game.scoreboard.rows.size()==1 && game.scoreboard.rows[0].score==5300,"Completed match records once with win bonus");
        struct TempScore {std::string path="pf-records-test-"+std::to_string(SDL_GetPerformanceCounter())+".cfg";~TempScore(){std::remove(path.c_str());}} file;
        Scoreboard saved;saved.path=file.path;for(int n=1;n<15;n++)saved.add(n*100,n%9,0,0,1);
        Scoreboard loaded;loaded.path=file.path;check(loaded.load() && loaded.rows.size()==10 && loaded.rows.front().score==1400 && loaded.rows.back().score==500,"Top ten scores persist in descending order");
        {std::ofstream invalid(file.path);invalid<<"PF_SCORES 1 1\n999 -9 0 0 0\n";}
        check(!loaded.load() && loaded.rows.front().score==1400,"Malformed record file cannot partially overwrite scores");
        Match pain(3,8);pain.intro=0;pain.damage(0,1,7,false);check(pain.f[1].hurtTime>0,"Damage starts a body reaction");
        for(int n=0;n<60;n++)pain.update(1.f/60,{},{});
        check(pain.f[1].hurtTime==0,"Pain reaction recovers");
        pain.f[1].guard=true;pain.f[1].facing=-1;pain.damage(0,1,7,false);check(pain.f[1].hurtTime==0,"Guard does not play pain pose");
        Voices audio;auto effort=Voices::vocal(0,false),hurt=Voices::vocal(0,true),ult=Voices::vocal(0,false,true);
        check(effort.size()<hurt.size() && hurt.size()<ult.size(),"Distinct effort, pain and ultimate envelopes");
        audio.layer(Voices::strike(false,1,false));audio.layer(hurt);audio.layer(Voices::strike(true,3,true));
        std::vector<float> mixed(12000);audio.mix(mixed.data(),mixed.size());float energy=0;for(float v:mixed){check(std::isfinite(v) && std::abs(v)<=.951f,"Layered sounds finite and limited");energy+=v*v;}check(energy>1,"Sound mixer produces audible energy");
        std::cout<<"PASS: boot order, scoreboard, AFK demo, click/key/pad exit, scoring, pain and layered audio\n";return 0;
    }catch(const std::exception&e){std::cerr<<"ARCADE FAIL: "<<e.what()<<"\n";return 1;}
}

#include "bonus_tests.h"
#include "presentation_tests.h"
#include "expansion_tests.h"

int main(int argc,char**argv) {
    bool expansionTesting=argc>1 && std::string(argv[1])=="--expansion-test";
    bool captureV18=argc>1 && (std::string(argv[1])=="--capture-v18" || std::string(argv[1])=="--capture-v19");
    bool captureV17=argc>1 && std::string(argv[1])=="--capture-v17";
    bool captureV16=argc>1 && std::string(argv[1])=="--capture-v16";
    bool presentationTesting=false,captureV15=false;
    for(int i=1;i<argc;i++){std::string a=argv[i];if(a=="--presentation-test")presentationTesting=true;if(a=="--capture-v15")captureV15=true;}
    bool bonusTesting=false,captureV14=false;
    for(int i=1;i<argc;i++){std::string a=argv[i];if(a=="--bonus-test")bonusTesting=true;if(a=="--capture-v14")captureV14=true;}
    bool arcadeTesting=false,captureV13=false;
    for(int i=1;i<argc;i++){std::string a=argv[i];if(a=="--arcade-test")arcadeTesting=true;if(a=="--capture-v13")captureV13=true;}
    bool captureV12=false,flowTesting=false;
    for(int i=1;i<argc;i++){std::string a=argv[i];if(a=="--combat-v12-test")return combatV12Tests();if(a=="--capture-v12")captureV12=true;if(a=="--flow-test")flowTesting=true;}
    bool plazaCapture=argc>1 && std::string(argv[1])=="--capture-plaza";
    bool comboCapture=false;
    for(int i=1;i<argc;i++){std::string a=argv[i];if(a=="--combo-test")return comboTests();if(a=="--capture-combos")comboCapture=true;}
    bool v9Capture=false,rosterCapture=false,pullCapture=false;
    bool smoke=false,capture=false,animation=false,inputTesting=false,settingsCapture=false,report=false;
    for(int i=1;i<argc;i++){std::string a=argv[i];if(a=="--capture-v9")v9Capture=true;if(a=="--capture-roster")rosterCapture=true;if(a=="--capture-grab")pullCapture=true;if(a=="--self-test")return selfTest();if(a=="--smoke-test")smoke=true;if(a=="--capture")capture=true;if(a=="--capture-animation")animation=true;if(a=="--input-test")inputTesting=true;if(a=="--capture-settings")settingsCapture=true;if(a=="--controller-report")report=true;}
    Game game;
    if(!game.init(expansionTesting || captureV18 || captureV17 || captureV16 || presentationTesting || captureV15 || bonusTesting || captureV14 || arcadeTesting || captureV13 || captureV12 || flowTesting || plazaCapture || comboCapture || v9Capture || rosterCapture || pullCapture || smoke || capture || animation || inputTesting || settingsCapture || report)){std::cerr<<"SDL2: "<<SDL_GetError()<<"\n";return 1;}
    if(expansionTesting)return expansionTests(game);
    if(captureV18)return captureExpansion(game);
    if(captureV17){
        SDL_SetWindowSize(game.window,640,360);game.crtEnabled=false;game.local=true;game.menu=false;
        game.match=Match(0,1);game.match.intro=0;game.reporter.reset(game.match);
        for(int frame=0;frame<40;frame++){game.time=frame/10.f;game.draw();if(!game.capture("v17-atibaia-"+std::to_string(frame)+".bmp"))return 2;}
        detailedBackdrop(1,2.5f);detailedFloor(1);if(!game.capture("v17-background.bmp"))return 2;
        return 0;
    }
    if(captureV16){
        SDL_SetWindowSize(game.window,640,360);game.crtEnabled=false;game.local=true;game.menu=false;
        for(int host:{0,2,3,5,7}){
            game.match=Match(host==0?1:0,host);game.match.intro=0;game.reporter.reset(game.match);
            for(int frame=0;frame<40;frame++){
                game.time=frame/10.f;game.draw();
                if(!game.capture("v16-stage-"+std::to_string(host)+"-"+std::to_string(frame)+".bmp"))return 2;
            }
            detailedBackdrop(host,2.5f);detailedFloor(host);
            if(!game.capture("v16-background-"+std::to_string(host)+".bmp"))return 2;
        }
        return 0;
    }
    if(presentationTesting)return presentationTests(game);
    if(captureV15){
        SDL_SetWindowSize(game.window,640,360);game.crtEnabled=false;game.local=true;
        for(int id=0;id<10;id++){
            game.match=Match(id==0?1:0,id,id==9);game.versusActive=true;game.menu=false;game.versusTime=4.5f;game.draw();if(!game.capture("v15-versus-"+std::to_string(id)+".bmp"))return 2;
            game.finishVersus();game.match.intro=0;game.tournament.active=id==9;game.reporter.reset(game.match);game.time=4;game.raidTime=4;game.draw();if(!game.capture("v15-stage-"+std::to_string(id)+".bmp"))return 2;
        }
        game.tournament.active=false;game.match=Match(0,1);game.match.intro=0;game.match.f[0].x=85;
        game.match=Match(1,0);game.match.intro=0;game.time=4;game.draw();if(!game.capture("v15-helicopter.bmp"))return 2;
        game.match=Match(0,1);game.match.intro=0;game.match.f[0].x=85;
        for(int n=0;n<48;n++){Input in;in.right=n<24;in.left=n>=24;game.match.update(1.f/60,in,{});game.time+=1.f/60;game.draw();if(!game.capture("v15-walk-"+std::to_string(n)+".bmp"))return 2;}
        return 0;
    }
    if(bonusTesting)return bonusTests(game);
    if(captureV14){
        SDL_SetWindowSize(game.window,640,360);game.crtEnabled=false;
        game.openingActive=true;
        for(int id=0;id<9;id++){game.openingTime=6.f+id;game.draw();if(!game.capture("v14-portrait-"+std::to_string(id)+".bmp"))return 2;}
        game.returnMenu();game.bonusActive=true;game.bonus=ConversionBonus(0,7);game.draw();if(!game.capture("v14-bonus-start.bmp"))return 2;
        for(int n=0;n<350;n++){Input p;p.magic=true;if(n%110==0)p.jump=true;if(n==220)p.crouch=true;game.bonus.tick(1.f/60,p,{});}
        game.draw();if(!game.capture("v14-bonus-play.bmp"))return 2;game.bonus.finish();game.draw();if(!game.capture("v14-bonus-end.bmp"))return 2;
        game.returnMenu();game.pendingBags[0]=2;game.begin();game.draw();if(!game.capture("v14-ammo-eight.bmp"))return 2;
        game.openingActive=true;game.openingTime=16.5f;game.draw();if(!game.capture("v14-handshake.bmp"))return 2;game.openingActive=false;
        for(int side=0;side<2;side++){
            box(0,0,640,360,ink);
            for(int n=0;n<12;n++){
                int x=80+(n%6)*105,y=155+(n/6)*170;Fighter f;f.character=n%9;f.x=float(x);f.y=float(y);f.facing=side?-1:1;
                if(n<3){f.move=Punch;f.comboStage=n+1;f.clock=.13f;}
                if(n>=3&&n<=5){f.move=Ultimate;f.clock=n==3?.1f:n==4?.4f:.69f;}
                if(n==6)f.guard=true;
                if(n==7){f.guard=true;f.crouch=true;}
                if(n==8)f.pullPose=.5f;
                if(n==9)f.pullPose=-.5f;
                if(n==10)f.cinematicCuffed=true;
                if(n==11)f.hurtTime=.2f;
                fighter(f,0,1,true);center(std::to_string(n+1),x,y+12,gold);
            }
            if(!game.capture("v14-arms-"+std::to_string(side)+".bmp"))return 2;
        }
        return 0;
    }
    if(arcadeTesting)return arcadeTests(game);
    if(captureV13) {
        SDL_SetWindowSize(game.window,640,360);
        game.bootActive=true;game.draw();if(!game.capture("v13-seal.bmp"))return 2;
        game.bootStage=1;game.bootTime=2;game.draw();if(!game.capture("v13-bank.bmp"))return 2;
        game.bootTime=6;game.draw();if(!game.capture("v13-parties.bmp"))return 2;
        game.bootActive=false;game.showScores();game.draw();if(!game.capture("v13-scores.bmp"))return 2;
        game.returnMenu();game.startDemo();for(int n=0;n<300;n++)game.step(1.f/60);game.draw();if(!game.capture("v13-demo.bmp"))return 2;
        game.returnMenu();game.begin();game.match=Match(3,8);game.match.intro=0;game.match.f[0].x=240;game.match.f[1].x=315;game.match.damage(0,1,7,false);
        for(int n=0;n<24;n++){game.draw();if(!game.capture("pain-"+std::to_string(n)+".bmp"))return 2;game.match.update(1.f/60,{},{});}
        return 0;
    }
    if(flowTesting)return flowTests(game);
    if(captureV12) {
        SDL_SetWindowSize(game.window,640,360);game.menuChoice=4;game.draw();if(!game.capture("v12-menu.bmp"))return 2;
        game.openingActive=true;
        for(int n=0;n<90;n++){game.openingTime=n*14.f/90;game.draw();if(!game.capture("intro-"+std::to_string(n)+".bmp"))return 2;}
        game.openingActive=false;game.tournament.start(3);game.tournament.stage=int(game.tournament.rivals.size());game.begin();game.finishVersus();game.match.intro=0;
        for(int n=0;n<72;n++){game.time=n/8.f;game.raidTime=n/8.f;game.draw();if(!game.capture("boss-"+std::to_string(n)+".bmp"))return 2;}
        for(bool won:{true,false}) {
            game.endingActive=true;game.endingWon=won;
            for(int n=0;n<80;n++){game.endingTime=n*11.f/80;game.draw();if(!game.capture(std::string(won?"victory-":"defeat-")+std::to_string(n)+".bmp"))return 2;}
        }
        game.endingActive=false;game.tournament.active=false;game.match=Match(3,8);game.match.intro=0;game.match.f[0].x=260;game.match.f[1].x=320;game.match.damage(0,1,500,false);
        for(int n=0;n<36;n++){game.match.update(1.f/15,{},{});game.draw();if(!game.capture("ko-"+std::to_string(n)+".bmp"))return 2;}
        return 0;
    }
    if(plazaCapture) {
        SDL_SetWindowSize(game.window,640,360);game.menu=false;game.match.intro=0;
        for(int frame=0;frame<60;frame++) {
            game.time=frame/15.f;game.draw();
            if(!game.capture("plaza-"+std::to_string(frame)+".bmp"))return 2;
        }
        return 0;
    }
    if(comboCapture) {
        SDL_SetWindowSize(game.window,640,360);game.menu=false;game.local=true;
        for(int kind=0;kind<2;kind++)for(int stage=1;stage<=3;stage++) {
            game.match=Match(0,1);game.match.intro=0;game.match.f[0].x=260;game.match.f[1].x=450;
            auto &p=game.match.f[0];p.move=kind?Kick:Punch;p.comboStage=stage;p.clock=kind?.2f:.12f;
            game.draw();center(std::string(kind?"CHUTE ":"SOCO ")+std::to_string(stage)+" / DANO "+std::to_string(meleeDamage(p)),320,105,gold,2);
            if(!game.capture("combo-"+std::to_string(kind)+"-"+std::to_string(stage)+".bmp"))return 2;
        }
        for(int kind=0;kind<2;kind++) {
            game.match=Match(3,8);game.match.intro=0;game.match.f[0].x=260;game.match.f[1].x=308;bool third=false;
            for(int frame=0;frame<144;frame++) {
                third|=game.match.f[0].comboStage==3;Input command;
                if(!third){command.punch=!kind;command.kick=kind;}
                game.match.update(1.f/60,command,{});game.time+=1.f/60;game.hitEffects();game.ageEffects(1.f/60);
                if(frame%2==0){game.draw();if(!game.capture("motion-"+std::to_string(kind)+"-"+std::to_string(frame/2)+".bmp"))return 2;}
            }
        }
        return 0;
    }
    if(inputTesting)return inputTests(game);
    if(report) {
        SDL_version v;SDL_GetVersion(&v);std::cout<<"SDL "<<int(v.major)<<"."<<int(v.minor)<<"."<<int(v.patch)<<"\nMappings loaded: "<<game.controls.mappingsLoaded<<"\nDevices: "<<game.controls.devices.size()<<"\n";
        for(auto&d:game.controls.devices)std::cout<<d.name<<" | GUID "<<d.guid<<" | "<<(d.controller?"GameController":"Raw joystick")<<" | buttons "<<SDL_JoystickNumButtons(d.joy)<<" axes "<<SDL_JoystickNumAxes(d.joy)<<" hats "<<SDL_JoystickNumHats(d.joy)<<"\n";
        return 0;
    }
    if(v9Capture) {
        SDL_SetWindowSize(game.window,640,360);game.menuChoice=3;game.draw();if(!game.capture("v9-menu.bmp"))return 2;
        game.menu=false;game.local=true;
        for(int scenario=0;scenario<4;scenario++) {
            game.tournament.active=scenario==3;game.tournament.stage=int(game.tournament.rivals.size());
            game.match=Match(scenario==0?1:scenario==1?0:4,scenario==3?BossCharacter:3,scenario==3);
            game.match.intro=0;game.match.f[0].x=180;game.match.f[1].x=480;
            if(scenario<2)game.match.f[0].hp=50;else {game.match.f[0].meter=100;game.match.start(game.match.f[0],Ultimate);}
            game.particles.clear();game.bursts.clear();
            for(int frame=0;frame<180;frame++) {
                Input enemy;if(scenario==2)enemy.crouch=frame<60; // Demonstrate tracking down, then back up.
                if(scenario==3)enemy=game.match.ai(1.f/30);
                game.match.update(1.f/30,{},enemy);game.hitEffects();game.ageEffects(1.f/30);game.time+=1.f/30;game.draw();
                if(!game.capture("v9-"+std::to_string(scenario)+"-"+std::to_string(frame)+".bmp"))return 2;
            }
        }
        return 0;
    }
    if(rosterCapture) {
        game.draw();if(!game.capture("roster.bmp"))return 2;
        game.menu=false;game.local=true;
        for(int id=0;id<CharacterCount;id++) {
            game.match=Match(id,(id+1)%CharacterCount);game.bursts.clear();game.particles.clear();game.match.intro=0;
            game.match.f[0].x=125;game.match.f[1].x=535;game.match.f[0].meter=100;
            game.match.start(game.match.f[0],Ultimate);
            for(int frame=0;frame<90;frame++) {
                game.match.update(1.f/30,{},{});game.hitEffects();game.ageEffects(1.f/30);game.time+=1.f/30;game.draw();
                if(!game.capture("roster-"+std::to_string(id)+"-"+std::to_string(frame)+".bmp"))return 2;
            }
        }
        return 0;
    }
    if(pullCapture) {
        game.menu=false;game.local=true;
        for(bool low:{false,true}) {
            game.match=Match(4,2);game.match.intro=0;game.match.f[0].x=240;game.match.f[1].x=400;
            Input a,b;a.left=true;b.right=true;a.crouch=b.crouch=low;game.match.update(1.f/60,a,b);
            game.draw();if(!game.capture(low?"defesa-baixa.bmp":"defesa-alta.bmp"))return 2;
        }
        game.match=Match();
    }
    if(pullCapture) {
        game.menu=false;game.local=true;game.match.intro=0;
        game.match.f[0].x=270;game.match.f[1].x=320;game.match.f[0].hp=65;
        Input input;input.grab=true;
        for(int i=0;i<105;i++) {
            game.match.update(.033333f,i==0?input:Input{},{});game.time+=.033333f;
            game.draw();if(!game.capture("pull-"+std::to_string(i)+".bmp"))return 2;
        }
        return 0;
    }
    if(settingsCapture){game.settings.open=true;game.settings.row=7;game.draw();if(!game.capture("settings.bmp"))return 2;game.settings.open=false;game.menuChoice=1;game.draw();if(!game.capture("menu.bmp"))return 2;game.menuAccept();game.draw();if(!game.capture("lobby.bmp"))return 2;return 0;}
    if(animation) {
        SDL_SetWindowSize(game.window,640,360);game.menu=false;game.local=true;
        for(int frame=0;frame<144;frame++) {
            if(frame%72==0){game.match=Match(frame/72);game.match.intro=0;game.match.f[0].x=95;game.match.f[1].x=540;game.match.f[0].meter=100;}
            if(frame%72==8)game.match.start(game.match.f[0],Ultimate);
            for(int n=0;n<3;n++){game.match.update(1.f/60,{},{});game.time+=1.f/60;}
            game.draw();std::string num=std::to_string(frame);num=std::string(3-num.size(),'0')+num;
            if(!game.capture("frame-"+num+".bmp"))return 2;
        }
        std::cout<<"PASS: captured 144 animation frames at 20 fps\n";return 0;
    }
    if(capture) {
        game.draw();if(!game.capture("menu.bmp"))return 2;
        game.begin();game.local=true;game.match.intro=0;game.match.f[0].x=243;game.match.f[1].x=397;game.match.f[0].meter=75;game.match.f[1].meter=100;
        game.draw();if(!game.capture("arena.bmp"))return 2;
        for(int who=0;who<2;who++) {
            game.match=Match(who);game.match.intro=0;game.match.f[0].x=110;game.match.f[1].x=535;game.match.f[0].meter=100;
            game.match.start(game.match.f[0],Ultimate);
            for(int i=0;i<70;i++){game.match.update(1.f/60,{},{});game.time+=1.f/60;}
            game.draw();if(!game.capture(who?"ultimate_lula.bmp":"ultimate_flavio.bmp"))return 2;
        }
        game.match=Match();game.match.intro=0;game.match.f[0].x=150;game.match.f[1].x=380;
        Input magic;magic.magic=true;Input duck;duck.crouch=true;game.match.update(1.f/60,magic,duck);
        for(int i=0;i<42;i++)game.match.update(1.f/60,{},duck);
        game.draw();if(!game.capture("crouch.bmp"))return 2;
        std::cout<<"PASS: captured menu, crowd, both ultimates and crouch\n";return 0;
    }
    if(smoke) {
        SDL_Event e{};e.type=SDL_KEYDOWN;e.key.keysym.sym=SDLK_RETURN;SDL_PushEvent(&e);game.events();
        check(!game.menu,"Enter starts game");
        for(int i=0;i<900;i++){game.step(1.f/60,true);if(i%30==0){game.draw();SDL_RenderPresent(game.renderer);}}
        if(game.match.over){game.begin();game.finishVersus();}
        e.key.keysym.sym=SDLK_p;SDL_PushEvent(&e);game.events();check(game.paused,"Pause event");
        e.key.keysym.sym=SDLK_ESCAPE;SDL_PushEvent(&e);game.events();check(game.menu,"Escape opens menu");
        e.key.keysym.sym=SDLK_TAB;SDL_PushEvent(&e);game.events();check(game.selectSide==1,"Tab selects opponent slot");
        e.key.keysym.sym=SDLK_DOWN;SDL_PushEvent(&e);game.events();check(game.opponent==2 && game.selected==0,"Independent opponent selection");
        e.key.keysym.sym=SDLK_TAB;SDL_PushEvent(&e);game.events();
        e.key.keysym.sym=SDLK_UP;SDL_PushEvent(&e);game.events();check(game.selected==CharacterCount-1,"Roster wraps to final selectable fighter");
        e.key.keysym.sym=SDLK_RIGHT;SDL_PushEvent(&e);game.events();
        e.key.keysym.sym=SDLK_RETURN;SDL_PushEvent(&e);game.events();check(game.lobby && game.local,"Two-player lobby still accessible");
        game.lobbyRow=4;game.lobbyNav(controls::RightNav);check(game.selected==0,"Lobby changes P1 fighter");
        game.lobbyRow=5;game.lobbyNav(controls::RightNav);check(game.opponent==3,"Lobby changes P2 fighter");
        game.lobbyRow=2;game.lobbyNav(controls::AcceptNav);
        check(game.match.f[0].character==0 && game.match.f[1].character==3,"Selected pair starts in local mode");
        game.menu=true;game.menuChoice=3;game.menuAccept();check(game.tournament.active && !game.local && !game.match.bossFight,"Menu enters tournament");
        game.match.over=true;game.match.f[1].wins=2;game.continueMatch();check(game.tournament.stage==0,"Defeat retries same stage");
        for(int stage=0;stage<int(game.tournament.rivals.size());stage++){game.match.over=true;game.match.f[0].wins=2;game.continueMatch();if(game.bonusActive){game.bonus.finish();game.finishBonus();}}
        check(game.tournament.boss() && game.match.bossFight,"Eight victories unlock final boss through UI continuation");
        game.draw();check(game.capture("v9-boss.bmp"),"Boss arena renders");
        game.match.over=true;game.match.f[0].wins=2;game.continueMatch();check(game.endingActive && game.endingWon,"Champion enters ending");game.leaveEnding();
        game.menuChoice=0;game.menuAccept();check(!game.tournament.active && !game.match.bossFight,"Normal mode clears boss context");
        game.draw();SDL_RenderPresent(game.renderer);std::cout<<"PASS: SDL2 video, audio, rendering, simulation, menu, pause, selection\n";return 0;
    }
    Uint64 previous=SDL_GetPerformanceCounter();double accumulator=0;double frequency=double(SDL_GetPerformanceFrequency());
    while(game.running) {
        Uint64 now=SDL_GetPerformanceCounter();double elapsed=std::min(.1,double(now-previous)/frequency);previous=now;
        game.events();accumulator+=elapsed;
        while(accumulator>=1.0/60.0) {game.step(1.f/60.f);accumulator-=1.0/60.0;}
        game.draw();SDL_RenderPresent(game.renderer);SDL_Delay(1);
    }
    return 0;
}
