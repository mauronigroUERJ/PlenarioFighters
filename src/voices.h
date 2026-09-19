#pragma once
#include <SDL.h>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cmath>
#include "music.h"
#include "roster.h"

// SDL2 alone: WAV decoding, resampling and a callback mixer. No SDL_mixer dependency.
struct Voices {
    std::array<std::vector<float>,pf::CharacterCount+1> clips;
    struct Channel {const std::vector<float>* clip=nullptr;size_t cursor=0;};
    Channel players[2];
    struct Effect {std::vector<float> samples;size_t cursor=0;};
    std::array<Effect,12> layers;
    size_t nextLayer=0;
    std::vector<float> effects;
    size_t effectCursor=0;
    SDL_AudioDeviceID device=0;
    std::vector<float> openingMusic,menuMusic;
    int musicTrack=0;
    size_t musicCursor=0;
    void prepareMusic(){openingMusic=chip::theme();menuMusic=chip::anthem();}
    void setMusic(int track){lock();if(musicTrack!=track){musicTrack=track;musicCursor=0;}unlock();}
    static constexpr const char* names[]={"flavio","lula","renan","zema","cury","caiado","moraes","lulinha","haddad","jair","kim","arthur","rui","samara","hertz","edmilson","grassi","clariana","fachin","gilmar","carmen","toffoli","fux","nunes","mendonca","zanin","dino","dilma","temer","vorcaro"};
    bool load(int character,const std::string &path) {
        return loadRW(character,SDL_RWFromFile(path.c_str(),"rb"));
    }
    bool loadRW(int character,SDL_RWops *source) {
        if(!source)return false;
        SDL_AudioSpec spec{};Uint8 *raw=nullptr;Uint32 bytes=0;
        if(!SDL_LoadWAV_RW(source,1,&spec,&raw,&bytes))return false;
        SDL_AudioCVT cvt{};
        int result=SDL_BuildAudioCVT(&cvt,spec.format,spec.channels,spec.freq,AUDIO_F32SYS,1,44100);
        // Keep clips short and conversion allocations bounded.
        if(result<0 || bytes>8*1024*1024 || spec.freq<=0 || spec.channels==0 ||
           bytes/double(spec.freq*spec.channels*(SDL_AUDIO_BITSIZE(spec.format)/8))>10 || cvt.len_mult<1) {SDL_FreeWAV(raw);return false;}
        std::vector<Uint8> converted(size_t(bytes)*cvt.len_mult);
        std::memcpy(converted.data(),raw,bytes);SDL_FreeWAV(raw);
        cvt.buf=converted.data();cvt.len=int(bytes);
        if(SDL_ConvertAudio(&cvt)<0)return false;
        auto &clip=clips.at(character);clip.resize(size_t(cvt.len_cvt)/sizeof(float));
        std::memcpy(clip.data(),converted.data(),clip.size()*sizeof(float));
        float peak=.01f;for(float &v:clip){if(!std::isfinite(v))v=0;peak=std::max(peak,std::abs(v));}
        for(float &v:clip)v*=.65f/peak;
        return !clip.empty();
    }
    void lock(){if(device)SDL_LockAudioDevice(device);}
    void unlock(){if(device)SDL_UnlockAudioDevice(device);}
    void play(int player,int character) {
        if(player<0 || player>1 || character<0 || character>pf::BossCharacter || clips[character].empty())return;
        lock();players[player]={&clips[character],0};unlock();
    }
    void effect(std::vector<float> samples) {
        lock();effects=std::move(samples);effectCursor=0;unlock();
    }
    void layer(std::vector<float> samples){lock();size_t slot=nextLayer;for(size_t n=0;n<layers.size();n++)if(layers[n].cursor>=layers[n].samples.size()){slot=n;break;}layers[slot]={std::move(samples),0};nextLayer=(slot+1)%layers.size();unlock();}
    // Original arcade vocalizations: harmonic/formant synthesis, not a real person's voice.
    static std::vector<float> vocal(int character,bool pain,bool ultimate=false) {
        float seconds=ultimate?.65f:pain?.28f:.16f,base=100.f+character*7;
        std::vector<float> data(size_t(seconds*44100));float phase=0;
        for(size_t n=0;n<data.size();n++) {
            float t=float(n)/44100,u=t/seconds;phase+=(base*(pain?1.35f-.55f*u:1.f+.3f*u))/44100;
            float v=std::sin(phase*6.283185f)+.5f*std::sin(phase*12.56637f)+.26f*std::sin(phase*18.84956f);
            float vowel=.6f+.4f*std::sin(t*(pain?720.f:980.f)*6.283185f);
            float env=std::min(1.f,t/.012f)*std::pow(1-u,1.2f);
            data[n]=v*vowel*env*(ultimate?.19f:.14f);
        }
        return data;
    }
    static std::vector<float> strike(bool kick,int stage,bool impact) {
        float seconds=impact?.13f:.11f;std::vector<float> data(size_t(seconds*44100));unsigned seed=unsigned(stage*91+kick*17+1);
        float phase=0;
        for(size_t n=0;n<data.size();n++) {float u=float(n)/float(data.size());seed=1664525u*seed+1013904223u;float noise=float((seed>>16)&65535)/32768.f-1;
            phase+=(impact?(kick?85.f:135.f)/(1+stage*.15f):550.f*(1-u))/44100;
            float v=impact?.65f*std::sin(phase*6.283185f)+noise*.35f:noise;
            data[n]=v*std::min(1.f,u*20)*std::pow(1-u,2.f)*(impact?.2f+.03f*stage:.085f);
        }return data;
    }
    void clear(){lock();players[0]={};players[1]={};effects.clear();effectCursor=0;for(auto &p:layers){p.samples.clear();p.cursor=0;}unlock();}
    void mix(float *out,size_t count) {
        for(size_t i=0;i<count;i++) {
            float value=effectCursor<effects.size()?effects[effectCursor++]:0;
            const auto &music=musicTrack==2?menuMusic:openingMusic;
            if(musicTrack && !music.empty()){value+=music[musicCursor];musicCursor=(musicCursor+1)%music.size();}
            for(auto &p:layers)if(p.cursor<p.samples.size())value+=p.samples[p.cursor++];
            for(auto &p:players)if(p.clip && p.cursor<p.clip->size())value+=(*p.clip)[p.cursor++];
            out[i]=std::clamp(value,-.95f,.95f);
        }
    }
    static void callback(void *userdata,Uint8 *stream,int length) {
        auto &self=*static_cast<Voices*>(userdata);
        self.mix(reinterpret_cast<float*>(stream),size_t(length)/sizeof(float));
    }
};
