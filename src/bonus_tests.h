#pragma once
inline int bonusTests(Game &game){
    auto check=[](bool ok,const char*msg){if(!ok)throw std::runtime_error(msg);};
    try{
        ConversionBonus b(0,7);check(b.people.size()==24 && b.remaining==10,"24 gray targets, ten seconds");
        b.humanP2=true;b.people={{300,1,-1}};b.shots={{270,1,0}};b.tick(.1f,{},{});
        check(b.score[0]==1 && b.people[0].owner==0,"Swept money shot converts target");
        b.shots={{330,1,1}};b.tick(.1f,{},{});check(b.score[1]==0,"Converted target cannot be stolen");
        ConversionBonus tie;tie.humanP2=true;tie.people={{320,1,-1}};tie.shots={{290,1,0},{350,1,1}};tie.tick(.1f,{},{});
        check(tie.score[0]==0 && tie.score[1]==0,"Simultaneous opposing hits leave neutral target");
        tie.remaining=.01f;tie.tick(.1f,{},{});check(tie.over() && tie.winner==-1,"Exactly ten seconds, tied score gets no reward");
        tie.shots={{300,1,0}};tie.tick(.1f,{},{});check(tie.score[0]==0,"No conversions after expiry");
        ConversionBonus join;Input command;command.magic=true;join.tick(.01f,{},command);check(join.humanP2,"P2 can replace CPU with their own input");
        ConversionBonus aim;aim.humanP2=true;command={};command.jump=true;aim.tick(.01f,command,{});aim.tick(.01f,command,{});check(aim.lane[0]==0,"Held direction does not skip rows");
        ConversionBonus solo;for(int n=0;n<601;n++)solo.tick(1.f/60,{},{});check(solo.over() && solo.score[1]>0,"CPU independently aims and converts");
        game.returnMenu();game.tournament.start(0);game.begin();int bonuses=0;
        for(int stage=1;stage<=int(game.tournament.rivals.size());stage++){
            game.match.over=true;game.match.f[0].wins=2;game.continueMatch();
            check(game.tournament.stage==stage,"Tournament advances once per victory");
            check(game.bonusActive==(stage%2==0),"Bonus after each pair of defeated rivals");
            if(game.bonusActive){bonuses++;game.bonus.score[0]=5;game.bonus.finish();game.finishBonus();check(game.match.f[0].bags==8 && game.match.f[1].bags==6,"Bonus adds two bags to winner only");game.match.resetRound();check(game.match.f[0].bags==8,"Award persists through match rounds");}
            else check(game.match.f[0].bags==6,"Award expires after next match");
        }
        check(bonuses==int(game.tournament.rivals.size())/2 && game.match.bossFight,"Bonus cadence follows bracket size and still reaches final boss");
        game.returnMenu();game.tournament.start(3);game.begin();game.match.over=true;game.match.f[1].wins=2;game.continueMatch();check(!game.bonusActive && game.tournament.stage==0,"Loss does not trigger a bonus");
        game.bonus=ConversionBonus(3,7);game.bonus.score[1]=2;game.bonus.finish();game.finishBonus();check(game.match.f[1].bags==8 && game.match.f[0].bags==6,"P2 or CPU can win the award");
        game.finishVersus();game.bonus=ConversionBonus(3,7);game.bonusActive=true;game.bonus.score[0]=4;game.bonus.finish();game.step(2.6f);check(!game.bonusActive && game.match.f[0].bags==8,"Result automatically starts the next fight with the award");
        Voices sound;sound.prepareMusic();sound.setMusic(2);std::vector<float> samples(44100);sound.mix(samples.data(),samples.size());float energy=0;for(float v:samples){check(std::isfinite(v)&&std::abs(v)<=.95f,"Music output is finite and bounded");energy+=v*v;}check(energy>1,"Anthem contains audible PCM");
        size_t cursor=sound.musicCursor;sound.clear();sound.setMusic(2);check(sound.musicCursor==cursor,"SFX clear and repeated scene selection preserve music");
        sound.setMusic(0);sound.mix(samples.data(),samples.size());check(std::all_of(samples.begin(),samples.end(),[](float v){return v==0;}),"Mute stops all music");
        sound.setMusic(1);sound.musicCursor=sound.openingMusic.size()-2;sound.mix(samples.data(),5);check(sound.musicCursor==3,"Theme loops across buffer boundary");
        check(faceAtlas!=nullptr,"Embedded pixel portrait atlas loads with SDL2");
        game.returnMenu();game.startDemo();SDL_Event mute{};mute.type=SDL_KEYDOWN;mute.key.keysym.sym=SDLK_m;SDL_PushEvent(&mute);game.events();check(!game.attractActive && game.menu,"Mute hotkey also exits an idle demonstration");
        std::cout<<"PASS: bonus collisions, timer, two players, CPU, tournament cadence, one-match reward, music and embedded portraits\n";return 0;
    }catch(const std::exception &e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 2;}
}
