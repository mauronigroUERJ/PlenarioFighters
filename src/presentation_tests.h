#pragma once
inline int presentationTests(Game &game){
    auto require=[](bool value,const char*message){if(!value)throw std::runtime_error(message);};
    try{
        for(int a=0;a<=BossCharacter;a++)for(int b=0;b<=BossCharacter;b++)require(!duelLine(a,b).empty() && duelLine(a,b).size()<92,"Every matchup has readable dialogue, including mirrors and boss");
        require(duelLine(0,1)!=duelLine(0,3) && duelLine(1,0)!=duelLine(1,3),"Pair-specific replies replace fallback lines");
        game.returnMenu();game.local=true;game.begin();require(game.versusActive && !game.menu,"Every local duel begins with versus presentation");
        float remaining=game.match.remaining;int hp=game.match.f[0].hp;game.step(3);
        require(game.versusActive && game.match.remaining==remaining && game.match.f[0].hp==hp,"Presentation freezes combat and timer");
        game.step(4);require(!game.versusActive && game.match.intro>0,"Automatic presentation hands off to round countdown");
        game.begin();SDL_Event key{};key.type=SDL_KEYDOWN;key.key.keysym.sym=SDLK_RETURN;SDL_PushEvent(&key);game.events();require(!game.versusActive && !game.paused && !game.menu,"Enter skips once without pausing or restarting combat");
        game.tournament.start(2);game.tournament.stage=int(game.tournament.rivals.size());game.begin();require(game.versusActive && game.match.bossFight,"Final boss also receives presentation");
        key.key.keysym.sym=SDLK_ESCAPE;SDL_PushEvent(&key);game.events();require(game.menu&&!game.versusActive,"Escape exits presentation cleanly");
        game.tournament.start(1);game.begin();game.finishVersus();game.bonus=ConversionBonus(1,2);game.bonus.score[0]=2;game.bonus.finish();game.finishBonus();require(game.versusActive && game.match.f[0].bags==8,"Bonus award survives next versus scene");
        Match m;m.intro=0;Input forward;forward.right=true;float x=m.f[0].x;
        for(int n=0;n<12;n++)m.update(1.f/60,forward,{});
        require(std::abs(m.f[0].x-x-39)<.02f,"Forward movement is 195 pixels per second");
        require(m.f[0].walkVelocity>190 && m.f[0].walkPhase!=0,"Walking phase follows real displacement");
        float phase=m.f[0].walkPhase;m.update(.02f,{},{});require(m.f[0].walkVelocity==0 && phase==m.f[0].walkPhase,"Idle does not keep walking");
        Input back;back.left=true;x=m.f[0].x;m.update(.1f,back,{});require(std::abs(m.f[0].x-x+11.5f)<.02f && m.f[0].guard,"Backward footwork keeps directional guard");
        m.f[0].x=35;m.update(.1f,back,{});require(m.f[0].walkVelocity==0,"Feet stop at arena boundary");
        Match contact;contact.intro=0;contact.f[0].x=280;contact.f[1].x=318;Input fromRight;fromRight.left=true;contact.update(1.f/60,forward,fromRight);require(std::abs(contact.f[0].walkVelocity)<.01f && std::abs(contact.f[1].walkVelocity)<.01f,"Feet do not slide when opponents block each other");
        Reporter reporter;Match action;action.intro=0;reporter.reset(action);action.f[0].move=Ultimate;reporter.update(.01f,action);require(reporter.line.find("ESPECIAL")!=std::string::npos,"Reporter detects actual ultimate start");
        auto line=reporter.line;action.f[1].hp-=7;reporter.update(.01f,action);require(reporter.line==line,"Minor hit does not interrupt important special callout");
        action.winner=0;reporter.update(.01f,action);require(reporter.line.find("LEVA O ROUND")!=std::string::npos,"Round result has commentary priority");
        game.returnMenu();std::cout<<"PASS: versus dialogue, freezes, skips, boss/bonus integration, faster walking and event-driven reporter\n";return 0;
    }catch(const std::exception&e){std::cerr<<"PRESENTATION FAIL: "<<e.what()<<"\n";return 2;}
}
