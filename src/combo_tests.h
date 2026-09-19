#pragma once
inline int comboTests() {
    try {
        auto check=[](bool ok,const char*why){if(!ok)throw std::runtime_error(why);};
        for(bool kick:{false,true})for(int side:{0,1})for(bool crouch:{false,true}) {
            if(crouch && !kick)continue;
            Match m(3,8);m.intro=0;m.f[0].x=260;m.f[1].x=308;
            Input attack;attack.punch=!kick;attack.kick=kick;attack.crouch=crouch;
            int hitCount=0,previous=m.f[1-side].hp;bool third=false;
            for(int n=0;n<260;n++) {
                third|=m.f[side].comboStage==3;
                Input command=third?Input{}:attack;
                m.update(1.f/120,side?Input{}:command,side?command:Input{});
                if(m.f[1-side].hp<previous){++hitCount;previous=m.f[1-side].hp;}
            }
            check(third && hitCount==3,"three strikes on both player sides, standing/crouching");
            check(m.f[1-side].hp==100-(kick?51:32),"stage damage totals");
            check(m.f[side].comboStage==0 && m.f[side].move==Idle,"finisher returns to idle");
        }
        Match tap(3,8);tap.intro=0;tap.f[0].x=260;tap.f[1].x=308;
        Input punch;punch.punch=true;tap.update(.01f,punch,{});
        for(int n=0;n<100;n++)tap.update(.01f,{},{});
        check(tap.f[1].hp==93,"single tap does not automatically queue three hits");
        tap.start(tap.f[0],Punch);tap.f[0].comboStage=2;tap.f[0].comboQueued=true;
        tap.damage(1,0,7,false);
        check(tap.f[0].comboStage==0 && !tap.f[0].comboQueued,"taking damage cancels chain");
        Match block(3,8);block.intro=0;block.f[0].x=260;block.f[1].x=308;
        Input guard;guard.guard=true;bool third=false;
        for(int n=0;n<150;n++){third|=block.f[0].comboStage==3;block.update(.01f,third?Input{}:punch,guard);}
        check(block.f[1].hp==97,"guard protects against all three punches");
        Match air(3,8);air.intro=0;air.f[0].x=260;air.f[1].x=308;
        air.f[0].y=FLOOR-25;air.f[0].move=Kick;air.f[0].comboStage=3;air.f[0].clock=.16f;
        air.update(.01f,{},{});check(air.f[1].hp==76,"aerial finisher damage");
        Voices mixer;mixer.clips[0]={.2f,.3f,.4f};mixer.clips[1]={.1f,.1f};
        mixer.play(0,0);mixer.play(1,1);mixer.effect({.05f});float out[4]{};mixer.mix(out,1);
        check(std::abs(out[0]-.35f)<.001f,"two ultimate voices mix with impact");
        mixer.effect({.02f});mixer.mix(out,1);
        check(std::abs(out[0]-.42f)<.001f,"new impact does not restart/cut voice");
        mixer.clear();mixer.mix(out,4);check(out[0]==0 && out[3]==0,"mute/reset clears channels");
        // A real PCM stereo WAV fixture verifies decoding, mono conversion and resampling.
        std::vector<Uint8> wav(44+22050*4,0);
        auto u16=[&](int at,int value){wav[at]=Uint8(value);wav[at+1]=Uint8(value>>8);};
        auto u32=[&](int at,int value){u16(at,value);u16(at+2,value>>16);};
        std::memcpy(wav.data(),"RIFF",4);u32(4,int(wav.size())-8);std::memcpy(wav.data()+8,"WAVEfmt ",8);
        u32(16,16);u16(20,1);u16(22,2);u32(24,22050);u32(28,88200);u16(32,4);u16(34,16);
        std::memcpy(wav.data()+36,"data",4);u32(40,int(wav.size())-44);
        for(int i=0;i<22050;i++){int sample=int(std::sin(i*.1)*10000);u16(44+i*4,sample);u16(46+i*4,sample);}
        check(mixer.loadRW(2,SDL_RWFromConstMem(wav.data(),int(wav.size()))),"SDL WAV decoding");
        check(mixer.clips[2].size()==44100,"stereo 22050 WAV converts to mono 44100");
        check(!mixer.loadRW(3,SDL_RWFromConstMem("invalid",7)),"invalid audio rejected");
        mixer.play(0,2);mixer.mix(out,4);check(std::abs(out[2])>.01f,"decoded WAV reaches mixer");
        std::cout<<"PASS: three-stage punches/kicks, P1/P2, crouch/air, damage, cancellation, defense, voice mixing\n";
        return 0;
    }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}
}
