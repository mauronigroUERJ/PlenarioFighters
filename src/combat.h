#pragma once
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include "roster.h"

namespace pf {
constexpr float FLOOR = 292.f;
constexpr float WalkSpeed=195.f, BackSpeed=115.f, BossSpeed=280.f, BagSpeed=430.f;
constexpr int UltimateDamage=10;
constexpr int BagCapacity=6;
inline int validCharacter(int id){return std::clamp(id,0,CharacterCount-1);}

enum class Difficulty { Easy, Medium, Hard };
enum class HitLevel { Mid, Low, Overhead };
enum Move { Idle, Punch, Kick, Ultimate, Magic, Grab };
enum class HurtStyle { Auto, Jab, Hook, Uppercut, BodyKick, LowKick, AirKick, Bag, Super };

inline ProjectileKind ultimateKind(int character,int shot=0) {
    return character==0 && shot%2?ProjectileKind::Banknotes:characterInfo(character).kind;
}
struct Input { bool left=false, right=false, jump=false, guard=false, punch=false, kick=false, ultimate=false, crouch=false, magic=false, grab=false; };
struct Fighter {
    int character=0, facing=1, hp=100, maxHp=100, meter=0, wins=0, bags=BagCapacity;
    int bagLimit=BagCapacity;
    bool bossBody=false, knockedOut=false, cinematicCuffed=false;
    bool evolved=false;
    float hurtTime=0,hurtDuration=.32f,blockImpact=0;
    HurtStyle hurtStyle=HurtStyle::Jab;
    HitLevel hurtLevel=HitLevel::Mid;
    int hurtDirection=1;
    float evolutionFlash=0;
    float x=180, y=FLOOR, vy=0, stun=0, clock=0, magicCooldown=0;
    float walkPhase=0,walkVelocity=0;
    int emitted=0;
    float knockdown=0, vx=0, tumble=0;
    bool thrown=false;
    float pullPose=0; // Positive: pulling; negative: held victim.
    Move move=Idle;
    int comboStage=0;
    bool comboQueued=false;
    bool punchHeld=false,kickHeld=false;
    bool guard=false, connected=false, crouch=false;
    bool grounded() const { return y>=FLOOR-.1f; }
};
inline HurtStyle meleeReaction(const Fighter &p){
    if(p.move==Kick)return p.crouch?HurtStyle::LowKick:!p.grounded()?HurtStyle::AirKick:HurtStyle::BodyKick;
    if(p.move==Punch)return p.comboStage==3?HurtStyle::Uppercut:p.comboStage==2?HurtStyle::Hook:HurtStyle::Jab;
    return HurtStyle::Bag;
}
inline float bodyScale(const Fighter&p) { return p.bossBody?1.25f:1.f; }
inline float projectileHeight(const Fighter&p) { return p.grounded()?p.y-(p.crouch?35.f:87.f)*bodyScale(p):p.y-25.f; }
struct Hit { float x,y; int character; bool blocked, ultimate, grab; int attacker=-1; int defender=-1,points=0; HurtStyle style=HurtStyle::Jab; HitLevel level=HitLevel::Mid; };
struct Wave {
    float x,y; int direction,owner; float life=2.2f;
    ProjectileKind kind=ProjectileKind::MoneyBag;
    float age=0;
    bool ultimate=false;
    HitLevel level=HitLevel::Mid;
    float vx=0,vy=0; // Used by steerable drones.
    float launchY=0;
    int character=-1;
};
inline float duration(Move m) { return m==Punch?.32f:m==Kick?.52f:m==Ultimate?2.05f:m==Magic?.36f:m==Grab?.55f:0.f; }
inline float startup(Move m) { return m==Punch?.08f:m==Kick?.16f:m==Ultimate?.55f:m==Grab?.16f:.12f; }
inline float reach(Move m) { return m==Punch?58.f:m==Grab?54.f:83.f; }
inline int meleeDamage(const Fighter &p) {
    static constexpr int punches[]={7,9,16}, kicks[]={12,15,24};
    const int stage=std::clamp(p.comboStage-1,0,2);
    if(p.move==Punch)return punches[stage];
    return kicks[stage];
}
struct Match {
    struct Assist {Fighter actor;float remaining=0,cooldown=0;};
    Fighter f[2];
    int extraBags[2]={0,0}; // Award lasts for this match, including its later rounds.
    Assist assists[2];
    bool bossFight=false;
    Difficulty difficulty=Difficulty::Medium;
    bool koActive=false;
    float koElapsed=0,koHold=0;
    float bossCharge=0;
    int puller=-1, healed=0;
    float pullTime=0, pullStart=0;
    bool pullImpact=false;
    std::vector<Wave> waves;
    std::vector<Hit> hits;
    float remaining=60, intro=1.7f, finish=0;
    int round=1, winner=-2;
    bool over=false;
    std::mt19937 random{42};
    Input aiHeld,aiLeftHeld;
    float aiTimer=0,aiLeftTimer=0;
    explicit Match(int selected=0,int opponent=-1,bool boss=false) { bossFight=boss && opponent==BossCharacter;f[0].character=validCharacter(selected);f[1].character=bossFight?BossCharacter:opponent<0?(f[0].character+1)%CharacterCount:validCharacter(opponent);resetRound(); }
    void resetRound() {
        for(int i=0;i<2;i++) {
            int character=f[i].character, wins=f[i].wins;
            f[i]=Fighter{}; f[i].character=character; f[i].wins=wins;
            f[i].bagLimit=BagCapacity+std::clamp(extraBags[i],0,2);f[i].bags=f[i].bagLimit;
            if(bossFight && i==1){f[i].maxHp=f[i].hp=180;f[i].meter=100;f[i].bossBody=true;}
            f[i].x=i?455:185; f[i].facing=i?-1:1;
        }
        assists[0]=Assist{};assists[1]=Assist{};bossCharge=0;
        puller=-1; pullTime=0; pullImpact=false;
        waves.clear(); hits.clear(); remaining=60; intro=1.7f; finish=0; winner=-2;
        koActive=false;koElapsed=koHold=0;
        aiHeld=aiLeftHeld={}; aiTimer=aiLeftTimer=0;
    }
    Input ai(float dt,int player=1) {
        Input &held=player==0?aiLeftHeld:aiHeld;float &timer=player==0?aiLeftTimer:aiTimer;
        if(over || winner!=-2)return {};
        timer-=dt;
        if(timer>0) return held;
        const bool easy=difficulty==Difficulty::Easy,hard=difficulty==Difficulty::Hard;
        timer=(easy?.32f:hard?.055f:.13f)+float(random()%10)*(easy?.02f:hard?.005f:.01f);
        held={};
        const Fighter &a=f[player], &b=f[1-player];
        float d=std::abs(a.x-b.x);
        bool incoming=b.move!=Idle && d<110;
        held.guard=incoming && random()%100<(easy?22u:hard?88u:62u);
        held.crouch=held.guard && b.crouch;
        const Wave *threat=nullptr;
        for(const auto&w:waves)if(w.owner==1-player && (a.x-w.x)*w.direction>0 && std::abs(w.x-a.x)<(easy?100:hard?190:150)) {
            if(!threat || std::abs(w.x-a.x)<std::abs(threat->x-a.x))threat=&w;
        }
        if(threat && a.grounded() && a.move==Idle && random()%100<(easy?30u:hard?94u:75u)) {
            held.jump=threat->level==HitLevel::Low || random()%100<(easy?65u:25u);
            held.crouch=!held.jump;
            held.guard=false;
            return held;
        }
        if(!held.guard) {
            if(d>62 && random()%100<(easy?70u:hard?100u:93u)) { held.left=a.x>b.x; held.right=a.x<b.x; }
            else if(d<38 && random()%100<30) { held.left=a.x<b.x; held.right=a.x>b.x; }
            if(random()%100>=(easy?48u:hard?98u:78u))return held;
            unsigned roll=random()%100;
            held.ultimate=a.meter>=100 && d<400 && roll<(easy?25u:hard?85u:bossFight?75u:45u);
            held.punch=d<60 && roll<60;
            held.kick=d<86 && roll>=40;
            held.magic=a.bags>0 && a.magicCooldown<=0 && d>125 && roll<(easy?22u:hard?48u:35u);
            held.jump=d>120 && roll>=35 && roll<40;
            if(d<54 && b.guard && random()%100<(hard?38u:easy?0u:12u)) {
                held.grab=true;held.punch=held.kick=held.ultimate=false;
            }
            if(held.kick && b.guard && !b.crouch && random()%100<(hard?70u:easy?0u:30u))held.crouch=true;
        }
        return held;
    }
    bool start(Fighter &a, Move m) {
        if(a.move!=Idle || a.stun>0 || a.guard || a.hp<=0 || a.thrown || a.knockdown>0) return false;
        if(a.crouch && m!=Kick && m!=Magic) return false;
        if(m==Ultimate && (!a.grounded() || a.meter<100)) return false;
        if(m==Magic && (a.magicCooldown>0 || a.bags<=0)) return false;
        if(m==Grab && (!a.grounded() || a.vy<0)) return false;
        if(m==Ultimate) a.meter=0;
        if(m==Magic) {a.magicCooldown=.50f;--a.bags;}
        a.comboStage=(m==Punch || m==Kick)?1:0;a.comboQueued=false;
        a.move=m; a.clock=0; a.connected=false; a.emitted=0; return true;
    }
    void damage(int attacker, int defender, int amount, bool ult, int direction=0, HitLevel level=HitLevel::Mid, HurtStyle style=HurtStyle::Auto) {
        Fighter &a=f[attacker], &b=f[defender];
        if(b.thrown || b.knockdown>0 || b.hp<=0)return;
        if(direction==0)direction=a.facing;
        bool correctHeight=level==HitLevel::Mid || (level==HitLevel::Low?b.crouch:!b.crouch);
        bool blocked=b.guard && b.grounded() && b.facing==-direction && correctHeight;
        if(a.evolved && a.character==1)amount+=ult?2:3;
        if(a.bossBody)amount+=ult?4:6;
        int actual=std::min(b.hp,blocked?1:amount);
        b.hp=std::max(0,b.hp-actual);
        b.stun=blocked?.08f:(ult?.12f:.22f);
        if(!blocked) { b.move=Idle; b.clock=0; b.guard=false;b.comboStage=0;b.comboQueued=false; }
        if(style==HurtStyle::Auto)style=ult?HurtStyle::Super:meleeReaction(a);
        if(blocked){b.blockImpact=.28f;b.hurtTime=0;}
        else {b.blockImpact=0;b.hurtTime=b.hurtDuration=ult?.38f:style==HurtStyle::Uppercut?.36f:.32f;b.hurtDirection=direction;b.hurtLevel=level;b.hurtStyle=style;}
        b.x=std::clamp(b.x+direction*(blocked?3.f:ult?4.f:12.f),35.f,605.f);
        a.meter=std::min(100,a.meter+(ult?0:blocked?6:15));
        b.meter=std::min(100,b.meter+(blocked?5:10));
        hits.push_back({b.x+(blocked?b.facing*36:0),b.y-(b.crouch?40:level==HitLevel::Low?38:style==HurtStyle::Jab || style==HurtStyle::Hook || style==HurtStyle::Uppercut?118:87)*bodyScale(b),b.character,blocked,ult,false,attacker,defender,actual*10,style,level});
    }
    void grab(int attacker, int defender) {
        Fighter &a=f[attacker], &b=f[defender];
        if(!b.grounded() || b.crouch || b.thrown || b.knockdown>0) return;
        if(puller>=0 || a.vy<0 || b.vy<0) return;
        puller=attacker;pullTime=0;pullStart=b.x;pullImpact=false;healed=0;
        a.guard=b.guard=false;a.crouch=b.crouch=false;
        a.vy=b.vy=0;b.move=Idle;b.clock=0;b.comboStage=0;b.comboQueued=false;
        a.pullPose=.01f;b.pullPose=-.01f;
    }
    void updatePull(float dt) {
        Fighter &a=f[puller], &b=f[1-puller];
        pullTime+=dt;
        // Grip, sudden pull, impact hold, then release. Both inputs stay locked.
        float t=std::clamp((pullTime-.18f)/.16f,0.f,1.f);
        float ease=t*t*(3-2*t);
        b.x=pullStart+(a.x+a.facing*22.f-pullStart)*ease;
        b.y=FLOOR-10.f*std::sin(ease*1.570796f);
        a.pullPose=.01f+ease;b.pullPose=-a.pullPose;
        if(pullTime>=.34f && !pullImpact) {
            pullImpact=true;b.hp=std::max(0,b.hp-4);
            healed=std::min(18,a.maxHp-a.hp);a.hp+=healed;a.meter=std::min(100,a.meter+10);
            hits.push_back({b.x,b.y-86,b.character,false,false,true,puller,1-puller,40});
        }
        if(pullTime>=.52f) {
            // The throw releases the attacker. The victim follows a real ballistic arc.
            a.move=b.move=Idle;a.clock=b.clock=0;a.pullPose=b.pullPose=0;
            b.thrown=true;b.vx=a.facing*350.f;b.vy=-330.f;b.tumble=0;b.stun=0;
            puller=-1;
        }
    }

    void updateEvolution(float dt) {
        for(int i=0;i<2;i++) {
            auto &p=f[i];p.evolutionFlash=std::max(0.f,p.evolutionFlash-dt);
            if(!p.evolved && p.hp>0 && p.hp<=50 && (p.character==0 || p.character==1)) {
                p.evolved=true;p.evolutionFlash=1.2f;
                if(p.character==0) {
                    auto &assist=assists[i];assist=Assist{};assist.remaining=5;
                    assist.actor.character=BossCharacter;assist.actor.x=std::clamp(p.x-p.facing*65,35.f,605.f);assist.actor.facing=p.facing;
                }
            }
        }
    }
    void updateAssists(float dt) {
        for(int i=0;i<2;i++) {
            auto &a=assists[i];if(a.remaining<=0)continue;
            float activeDt=std::min(dt,a.remaining);a.remaining=std::max(0.f,a.remaining-dt);
            if(a.remaining<.0001f){a.remaining=0;continue;}
            auto &p=a.actor;auto &target=f[1-i];a.cooldown=std::max(0.f,a.cooldown-activeDt);
            p.facing=target.x>=p.x?1:-1;
            if(p.move==Idle) {
                float distance=std::abs(target.x-p.x);
                if(distance>48)p.x=std::clamp(p.x+p.facing*std::min(145.f*activeDt,distance-48),35.f,605.f);
                if(distance<62 && a.cooldown<=0 && !target.thrown && target.knockdown<=0 && target.hp>0) {
                    p.move=Punch;p.clock=0;p.connected=false;a.cooldown=1.1f;
                }
            } else {
                p.clock+=activeDt;
                if(!p.connected && p.clock>=.08f) {
                    if(std::abs(target.x-p.x)<65 && std::abs(target.y-p.y)<65 && !target.crouch)damage(i,1-i,2,false,p.facing);
                    p.connected=true;
                }
                if(p.clock>=.32f){p.move=Idle;p.clock=0;}
            }
        }
    }
    void beginKnockout() {
        koActive=false;koElapsed=koHold=0;
        waves.clear();
        for(auto &p:f) {
            p.move=Idle;p.clock=0;p.guard=p.crouch=false;p.comboStage=0;p.comboQueued=false;
            p.pullPose=0;
            if(p.hp>0)continue;
            koActive=true;p.knockedOut=true;p.stun=0;
            // A lethal grab has already brought the victim to the floor; do not throw them twice.
            if(p.knockdown>0) {p.thrown=false;p.knockdown=1.f;p.vx=p.vy=0;continue;}
            p.thrown=true;p.knockdown=0;p.vx=-p.facing*180.f;p.vy=-180.f;p.tumble=0;
        }
    }
    void updateKnockout(float dt) {
        koElapsed+=dt;
        const float slowDt=dt*.32f;
        bool falling=false;
        for(auto &p:f)if(!p.knockedOut && (!p.grounded() || p.vy<0)) {
            p.y+=p.vy*slowDt;p.vy+=900.f*slowDt;
            if(p.y>=FLOOR){p.y=FLOOR;p.vy=0;}
        }
        for(auto &p:f)if(p.knockedOut && p.thrown) {
            p.x=std::clamp(p.x+p.vx*slowDt,35.f,605.f);
            p.y+=p.vy*slowDt;p.vy+=900.f*slowDt;
            p.tumble=std::min(1.570796f,p.tumble+slowDt*4.4f);
            if(p.y>=FLOOR && p.vy>0) {
                p.y=FLOOR;p.thrown=false;p.knockdown=1.f;p.vx=p.vy=0;
                hits.push_back({p.x,FLOOR-7,p.character,false,false,false});
            } else falling=true;
        }
        if(!falling) {koHold+=dt;if(koHold>=1.15f)koActive=false;}
    }
    void update(float dt, Input a, Input b) {
        hits.clear();
        for(auto &p:f)p.walkVelocity=0;
        if(over) return;
        if(winner!=-2) { finish+=dt;if(koActive)updateKnockout(dt); if(finish>2.6f && !koActive) { if(f[0].wins==2 || f[1].wins==2) over=true; else { round++; resetRound(); } } return; }
        if(intro>0) { intro=std::max(0.f,intro-dt); return; }
        if(puller>=0) {updatePull(dt);return;}
        updateEvolution(dt);
        if(bossFight){bossCharge+=dt*(f[1].hp<=90?16.f:10.f);while(bossCharge>=1){f[1].meter=std::min(100,f[1].meter+1);bossCharge-=1;}}
        remaining=std::max(0.f,remaining-dt);
        float beforeX[2]={f[0].x,f[1].x};
        Input in[2]={a,b};
        for(int i=0;i<2;i++) {
            Fighter &p=f[i];
            p.blockImpact=std::max(0.f,p.blockImpact-dt);p.hurtTime=std::max(0.f,p.hurtTime-dt);
            // Buffer the next strike after startup; holding is an accessible alternative to tapping.
            bool repeat=p.move==Punch?(in[i].punch && !p.punchHeld):(in[i].kick && !p.kickHeld);
            if((p.move==Punch || p.move==Kick) && p.comboStage<3 && (repeat || p.clock>=startup(p.move)) &&
               (p.move==Punch?in[i].punch:in[i].kick))p.comboQueued=true;
            p.punchHeld=in[i].punch;p.kickHeld=in[i].kick;
            if(p.thrown) {
                p.x=std::clamp(p.x+p.vx*dt,35.f,605.f);p.y+=p.vy*dt;p.vy+=900.f*dt;p.tumble+=dt*10;
                if(p.y>=FLOOR) {
                    p.y=FLOOR;p.vx=p.vy=0;p.thrown=false;p.knockdown=1.f;p.guard=p.crouch=false;
                    hits.push_back({p.x,FLOOR-7,p.character,false,false,false});
                }
                continue;
            }
            if(p.knockdown>0) {
                p.knockdown=std::max(0.f,p.knockdown-dt);p.guard=p.crouch=false;p.move=Idle;
                continue;
            }
            if(p.move==Idle && p.stun<=0) p.facing=f[1-i].x>=p.x?1:-1;
            p.stun=std::max(0.f,p.stun-dt);
            p.magicCooldown=std::max(0.f,p.magicCooldown-dt);
            bool back=p.facing==1?(in[i].left && !in[i].right):(in[i].right && !in[i].left);
            bool attacking=in[i].punch || in[i].kick || in[i].magic || in[i].ultimate || in[i].grab;
            bool wasGuard=p.guard;
            p.guard=(back || in[i].guard) && !attacking && !in[i].jump && p.grounded() && p.move==Idle && (p.stun<=0 || wasGuard);
            if(p.move==Idle && (p.stun<=0 || p.guard))p.crouch=in[i].crouch && p.grounded();
            if(p.stun<=0 && p.move==Idle) {
                if(!p.crouch && (!p.guard || back)){
                    float before=p.x;
                    p.x=std::clamp(p.x+((in[i].right?1.f:0.f)-(in[i].left?1.f:0.f))*(p.guard?BackSpeed:(p.bossBody?BossSpeed:WalkSpeed))*dt,35.f,605.f);
                    p.walkVelocity=dt>0?(p.x-before)/dt:0;
                }
                if(in[i].jump && p.grounded()) {p.vy=-455;p.crouch=false;}
                if(in[i].ultimate) start(p,Ultimate);
                else if(in[i].grab) start(p,Grab);
                else if(in[i].kick) start(p,Kick);
                else if(in[i].punch && !p.crouch) start(p,Punch);
                else if(in[i].magic) start(p,Magic);
            }
            p.y+=p.vy*dt; p.vy+=900.f*dt;
            if(p.y>FLOOR) { p.y=FLOOR; p.vy=0; }
            p.x=std::clamp(p.x,35.f,605.f);
            if(p.move!=Idle) p.clock+=dt;
        }
        float distance=f[1].x-f[0].x;
        if(!f[0].thrown && !f[1].thrown && f[0].knockdown<=0 && f[1].knockdown<=0 && std::abs(distance)<38 && std::abs(f[0].y-f[1].y)<70) {
            float push=(38-std::abs(distance))*.5f, sign=distance>=0?1.f:-1.f;
            f[0].x=std::clamp(f[0].x-sign*push,35.f,605.f);
            f[1].x=std::clamp(f[1].x+sign*push,35.f,605.f);
        }
        for(int i=0;i<2;i++)if(f[i].walkVelocity!=0){
            f[i].walkVelocity=dt>0?(f[i].x-beforeX[i])/dt:0;
            if(f[i].grounded())f[i].walkPhase=std::fmod(f[i].walkPhase+(f[i].x-beforeX[i])*f[i].facing*.075f,6.283185f);
        }
        // Collect simultaneous melee hits before applying damage (trades are possible).
        struct Pending { int a,b,damage; HitLevel level; HurtStyle style; }; std::vector<Pending> pending;
        for(int i=0;i<2;i++) {
            Fighter &p=f[i], &q=f[1-i];
            if(p.move==Idle) continue;
            float t=startup(p.move);
            if(p.move==Ultimate) {
                while(p.emitted<6 && p.clock>=t+p.emitted*.18f) {
                    ProjectileKind kind=ultimateKind(p.character,p.emitted);
                    waves.push_back({p.x+p.facing*42,p.y-87*bodyScale(p),p.facing,i,2.2f,kind,0,true});
                    waves.back().character=p.character;
                    p.emitted++;
                }
            } else if(!p.connected && p.clock>=t && p.clock<t+.12f) {
                if(p.move==Magic) {waves.push_back({p.x+p.facing*42,projectileHeight(p),p.facing,i});waves.back().level=p.crouch?HitLevel::Low:HitLevel::Mid;p.connected=true;}
                else if(p.move==Grab) {
                    if((q.x-p.x)*p.facing>0 && std::abs(q.x-p.x)<reach(p.move) && std::abs(q.y-p.y)<56) {grab(i,1-i);p.connected=true;if(puller>=0)return;}
                } else if(!q.thrown && q.knockdown<=0 && (q.x-p.x)*p.facing>0 && std::abs(q.x-p.x)<reach(p.move)+18.f*std::max(0,p.comboStage-1) &&
                          (std::abs(q.y-p.y)<56 || (p.move==Kick && !p.grounded() && q.y-p.y<120)) && (!q.crouch || p.move==Kick)) {
                    pending.push_back({i,1-i,meleeDamage(p),p.move==Kick?(p.crouch?HitLevel::Low:(!p.grounded()?HitLevel::Overhead:HitLevel::Mid)):HitLevel::Mid,meleeReaction(p)}); p.connected=true;
                }
            }
            if(p.clock>=duration(p.move)+(p.comboStage==3?.15f:0.f)) {
                if(p.comboQueued && p.comboStage<3) {
                    ++p.comboStage;p.clock=0;p.connected=false;p.comboQueued=false;
                } else {p.move=Idle;p.clock=0;p.comboStage=0;p.comboQueued=false;}
            }
        }
        for(auto h:pending) damage(h.a,h.b,h.damage,false,0,h.level,h.style);
        updateAssists(dt);
        for(auto &w:waves) {
            if(w.kind==ProjectileKind::Drone) {
                const auto &target=f[1-w.owner];
                float aimY=target.y-(target.crouch?38.f:80.f),dx=target.x-w.x,dy=aimY-w.y;
                if(w.age==0){w.vx=w.direction*350.f;w.launchY=w.y;}
                // Lock-on while approaching, with bounded turning. A passed drone never loops back.
                if(dx*w.direction>0 && w.age<1.05f) {
                    float distance=std::max(1.f,std::sqrt(dx*dx+dy*dy));
                    float wantedY=dy/distance*350.f;
                    w.vy+=std::clamp(wantedY-w.vy,-650.f*dt,650.f*dt);
                }
                w.vx=w.direction*std::sqrt(std::max(120.f*120.f,350.f*350.f-w.vy*w.vy));
                w.x+=w.vx*dt;w.y+=w.vy*dt;
                w.level=target.crouch?HitLevel::Low:HitLevel::Mid;
            } else w.x+=w.direction*(w.ultimate?370.f:BagSpeed)*dt;
            w.life-=dt;w.age+=dt;
            Fighter &q=f[1-w.owner];
            float height=(q.crouch?72.f:144.f)*bodyScale(q);
            if(!q.thrown && q.knockdown<=0 && w.life>0 && std::abs(w.x-q.x)<29 && w.y+9>q.y-height && w.y-9<q.y) {
                damage(w.owner,1-w.owner,w.ultimate?UltimateDamage:8,w.ultimate,w.direction,w.level,w.ultimate?HurtStyle::Super:HurtStyle::Bag); w.life=0;
            }
        }
        waves.erase(std::remove_if(waves.begin(),waves.end(),[](const Wave&w){return w.life<=0 || w.x<-40 || w.x>680;}),waves.end());
        if((f[0].hp<=0 || f[1].hp<=0 || remaining<=0) && !f[0].thrown && !f[1].thrown) {
            winner=f[0].hp==f[1].hp?-1:(f[0].hp>f[1].hp?0:1);
            if(winner>=0) f[winner].wins++;
            finish=0;
            if(f[0].hp<=0 || f[1].hp<=0)beginKnockout();
        }
    }
};
}
