#pragma once
inline int expansionTests(Game &game){
    try {
        check(CharacterCount==29 && BossCharacter==29,"29 selectable fighters plus exclusive boss");
        for(char c='a';c<='z';c++)check(glyph(c)==glyph(char(c-'a'+'A')),"News prose maps lowercase to the readable arcade font");
        int candidates=0,judges=0;
        for(int id=0;id<=BossCharacter;id++){
            check(hasPortrait(id),"Every fighter and boss have a loaded portrait");
            check(std::strlen(name(id))>0 && std::strlen(ultimateName(id))>0,"Roster names and specials exist");
            check(characterInfo(id).stage>=0 && characterInfo(id).stage<9,"Arena aliases are bounded");
            candidates+=presidential(id);judges+=std::string(category(id))=="STF";
        }
        check(candidates==12 && judges==10,"Verified presidential and STF lists are complete");
        check(!hasPortrait(-1) && !hasPortrait(BossCharacter+1),"Invalid portraits cannot read outside an atlas");
        game.returnMenu();game.selected=8;game.menuNav(controls::DownNav);check(game.selected==Jair,"Navigation reaches the second page");
        game.selected=Temer;game.chooseCharacter(1);check(game.selected==0,"Selection wraps without exposing boss");
        game.chooseCharacter(-1);check(game.selected==Temer,"Reverse selection reaches Temer");
        check(std::string(category(Dilma))!="STF" && std::string(category(Temer))!="STF","Historical presidents do not inherit STF robes or category");
        check(ultimateKind(Dilma)==ProjectileKind::Cassava && ultimateKind(Temer)==ProjectileKind::Bat,"Historical ultimates have distinct projectiles");
        for(int id:{Dilma,Temer}){
            Match m(id,id==Dilma?Temer:Dilma);m.intro=0;m.f[0].x=150;m.f[1].x=430;m.f[0].meter=100;
            check(m.start(m.f[0],Ultimate),"New fighter can activate ultimate");
            bool seen=false;for(int frame=0;frame<180;frame++){
                m.update(1.f/60,{},{});for(const auto &w:m.waves)if(w.ultimate){check(w.kind==ultimateKind(id) && w.character==id,"Every ultimate wave retains its source and kind");seen=true;}
            }
            check(seen && m.f[1].hp<100,"Cassava and bats actually inflict damage");
        }
        for(int attack=0;attack<8;attack++){
            Match m(Jair,Kim);m.intro=0;m.f[0].x=260;m.f[1].x=310;m.f[1].facing=-1;
            m.f[0].move=attack<3?Punch:attack<6?Kick:Magic;m.f[0].comboStage=attack+1;
            if(attack==4)m.f[0].crouch=true;
            if(attack==5)m.f[0].y=FLOOR-30;
            HurtStyle expected[]={HurtStyle::Jab,HurtStyle::Hook,HurtStyle::Uppercut,HurtStyle::BodyKick,HurtStyle::LowKick,HurtStyle::AirKick,HurtStyle::Bag,HurtStyle::Super};
            m.damage(0,1,8,attack==7);
            check(m.f[1].hurtStyle==expected[attack] && m.f[1].hurtTime>0,"Distinct attack classification survives hit application");
        }
        for(bool low:{false,true}){
            Match m(Jair,Kim);m.intro=0;m.f[1].guard=true;m.f[1].facing=-1;m.f[1].crouch=low;
            m.damage(0,1,20,false,1,low?HitLevel::Low:HitLevel::Overhead);
            check(m.f[1].hp==99 && m.f[1].blockImpact>0 && m.f[1].hurtTime==0,"Guard impact with chip damage at matching height");
            game.match=m;game.hitEffects();check(!game.bursts.empty() && game.bursts.back().blocked,"Blocked hit creates a localized comic burst");
            game.ageEffects(.4f);check(game.bursts.empty(),"Comic impacts expire");
            m.f[1].crouch=!low;m.damage(0,1,20,false,1,low?HitLevel::Low:HitLevel::Overhead);
            check(m.f[1].hp==79 && m.f[1].blockImpact==0 && m.f[1].hurtTime>0,"Wrong-height guard reacts with pain, not a block burst");
        }
        for(int n=0;n<StoryChapterCount;n++){
            auto &c=NewsChapters[n];check(n==0 || c.chronology>NewsChapters[n-1].chronology,"News remains strictly chronological");
            check(c.player<CharacterCount && c.opponent<CharacterCount && std::string(c.url).find("https://")==0,"Every chapter has valid fighters and a source");
        }
        struct Temp {std::string path="pf-story-test-"+std::to_string(SDL_GetPerformanceCounter())+".cfg";~Temp(){std::remove(path.c_str());}} file;
        for(int legacy=0;legacy<=10;legacy++){
            {std::ofstream out(file.path);out<<"PF_STORY 1 "<<legacy<<"\n";}
            Story old;old.path=file.path;old.load();check(old.completedCount()==legacy,"Migration preserves every previously completed chapter");
            if(legacy==8)check(NewsChapters[old.checkpoint].chronology==20240222,"v18 Dino checkpoint keeps its date after insertion");
            if(legacy==10)check(old.checkpoint==0,"Completed v18 resumes at first newly added chapter");
            old.save();Story again;again.path=file.path;again.load();check(again.completed==old.completed && again.checkpoint==old.checkpoint,"Stable date saves round-trip all migrated progress");
        }
        game.story.path=file.path;game.story.checkpoint=0;game.startStory();
        check(game.story.briefing && !game.menu && !game.tournament.active,"History starts in news briefing");
        game.step(2);check(game.story.time==2 && !game.versusActive,"News waits for explicit input");
        game.story.era(1);check(game.story.news().chronology/10000==2018,"Epoch selection reaches 2018");
        game.story.era(1);check(game.story.news().chronology/10000==2026,"Epoch selection reaches expanded 2026");
        game.story.era(1);check(game.story.chapter==0,"Epoch selection wraps to 2014");
        game.story.browse(-1);check(game.story.chapter==StoryChapterCount-1,"Chapter browse wraps backwards");game.story.browse(1);
        check(game.story.checkpoint==0,"Browsing does not overwrite the saved checkpoint");
        game.storyAccept();check(game.versusActive && game.match.f[0].character==Dilma && game.match.f[1].character==Temer,"Chapter launches its own cast through versus scene");
        game.finishVersus();game.match.over=true;game.match.f[1].wins=2;game.continueMatch();
        check(game.story.chapter==0 && game.versusActive,"Losing retries the same historical chapter");
        game.match.over=true;game.match.f[0].wins=2;game.continueMatch();
        check(game.story.briefing && game.story.chapter==1,"Victory opens the next chronological news item");
        Story saved;saved.path=file.path;saved.load();check(saved.checkpoint==1,"History progress persists");
        game.returnMenu();game.startStory();check(game.story.chapter==1,"Re-entering history resumes its checkpoint");
        while(!game.story.finished){game.storyAccept();game.match.over=true;game.match.f[0].wins=2;game.continueMatch();}
        check(game.story.chapter==StoryChapterCount && game.story.checkpoint==StoryChapterCount,"All chapters reach the volume ending");
        game.storyAccept();check(game.menu && !game.story.active,"Ending returns to menu");
        game.startStory();check(game.story.chapter==0,"Completed story can be replayed");
        game.story.path.clear();game.returnMenu();
        std::cout<<"PASS: 29 fighters, 30 portraits, 4 pages, 12 candidates, 10 judges, cassava/bat damage, pain/block, 22 chapters, browsing, v18 migration, stable saves, retry, resume and ending\n";return 0;
    }catch(const std::exception &e){std::cerr<<"EXPANSION FAIL: "<<e.what()<<"\n";return 2;}
}
inline int captureExpansion(Game &g){
    SDL_SetWindowSize(g.window,640,360);g.returnMenu();g.crtEnabled=true;
    for(int page=0;page<(CharacterCount+8)/9;page++){g.selected=page*9;g.menuChoice=5;g.draw();if(!g.capture("v19-menu-"+std::to_string(page)+".bmp"))return 2;}
    for(int id=0;id<=BossCharacter;id++){g.returnMenu();g.match=Match(Jair,id,id==BossCharacter);g.menu=false;g.versusActive=true;g.versusTime=4.5f;g.draw();if(!g.capture("v18-versus-"+std::to_string(id)+".bmp"))return 2;}
    for(int n=0;n<StoryChapterCount;n++){g.startStory();g.story.chapter=n;g.story.time=1.2f;g.draw();if(!g.capture("v18-news-"+std::to_string(n)+".bmp"))return 2;}
    g.story.finished=true;g.draw();if(!g.capture("v18-story-ending.bmp"))return 2;
    for(int style=1;style<=8;style++){
        g.returnMenu();g.menu=false;g.local=true;g.match=Match(Jair,Kim);g.match.intro=0;g.match.f[0].x=240;g.match.f[1].x=350;
        g.match.damage(0,1,8,style==8,1,HitLevel::Mid,HurtStyle(style));g.match.f[1].hurtTime*=.9f;
        g.time=1;g.reporter.reset(g.match);g.draw();if(!g.capture("v18-pain-"+std::to_string(style)+".bmp"))return 2;
    }
    for(bool low:{false,true})for(int frame=0;frame<10;frame++){
        g.returnMenu();g.menu=false;g.local=true;g.match=Match(Jair,Kim);g.match.intro=0;g.match.f[0].x=255;g.match.f[1].x=333;
        g.match.f[0].move=low?Kick:Punch;g.match.f[0].clock=.16f;g.match.f[0].comboStage=1;g.match.f[0].crouch=low;
        g.match.f[1].guard=true;g.match.f[1].crouch=low;g.match.f[1].facing=-1;
        g.match.damage(0,1,10,false,1,low?HitLevel::Low:HitLevel::Mid);g.hitEffects();g.ageEffects(frame*.035f);g.reporter.reset(g.match);g.draw();if(!g.capture("v18-block-"+std::to_string(low)+"-"+std::to_string(frame)+".bmp"))return 2;
    }
    g.returnMenu();g.menu=false;g.match=Match(Jair,1);g.match.intro=0;g.match.f[0].meter=100;g.match.f[0].x=150;g.match.f[1].x=580;
    g.match.start(g.match.f[0],Ultimate);g.reporter.reset(g.match);
    for(int frame=0;frame<20;frame++){for(int k=0;k<4;k++)g.match.update(1.f/60,{},{});g.time=frame/15.f;g.draw();if(!g.capture("v18-cloroquina-"+std::to_string(frame)+".bmp"))return 2;}
    g.returnMenu();g.menu=false;g.openingActive=true;g.openingTime=15;g.draw();if(!g.capture("v18-opening-jair.bmp"))return 2;
    for(int id:{Dilma,Temer}){
        g.returnMenu();g.menu=false;g.match=Match(id,id==Dilma?Temer:Dilma);g.match.intro=0;g.match.f[0].meter=100;g.match.f[0].x=150;g.match.f[1].x=530;g.match.start(g.match.f[0],Ultimate);g.reporter.reset(g.match);
        for(int frame=0;frame<30;frame++){for(int k=0;k<3;k++)g.match.update(1.f/60,{},{});g.time=frame*.05f;g.draw();if(!g.capture("v19-ultimate-"+std::to_string(id)+"-"+std::to_string(frame)+".bmp"))return 2;}
        g.returnMenu();g.menu=false;g.openingActive=true;g.openingTime=17.8f+(id-Dilma);g.draw();if(!g.capture("v19-opening-"+std::to_string(id)+".bmp"))return 2;
    }
    return 0;
}
