#pragma once
#include "anthem_data.h"
#include <vector>
#include <cmath>
#include <algorithm>
namespace chip {
inline void note(std::vector<float>&out,float start,float duration,int pitch,float volume,bool bass=false){
    float frequency=440.f*std::pow(2.f,(pitch-69)/12.f);size_t first=size_t(start*44100),count=size_t(duration*44100);
    for(size_t n=0;n<count&&first+n<out.size();n++){
        float t=float(n)/44100,phase=t*frequency;phase-=std::floor(phase);
        float wave=bass?1.f-4.f*std::abs(phase-.5f):(phase<.25f?.75f:-.25f);
        float env=std::min(1.f,t/.005f)*std::min(1.f,(duration-t)/.035f);
        out[first+n]+=wave*volume*env;
    }
}
inline std::vector<float> anthem(){
    float length=0;for(auto &n:Anthem)length=std::max(length,n.start+n.duration);
    std::vector<float> out(size_t((length+.35f)*44100));
    for(auto &n:Anthem)note(out,n.start,n.duration,n.pitch+12,n.velocity/127.f*.19f);
    // Original sparse arcade accompaniment under the public-domain anthem melody.
    for(int beat=0;beat<int(length*2);beat++){int root=(beat/8)%2?48:41;note(out,beat*.5f,.4f,root+(beat%2?7:0),.055f,true);}
    float peak=.001f;for(float v:out)peak=std::max(peak,std::abs(v));for(float &v:out)v*=.23f/peak;
    return out;
}
inline std::vector<float> theme(){
    constexpr int melody[]={64,67,71,76,74,71,67,69,64,67,71,79,76,74,71,67,62,65,69,74,72,69,65,67,62,65,69,77,74,72,69,65};
    std::vector<float> out(44100*8);
    for(int n=0;n<32;n++){note(out,n*.25f,.21f,melody[n],.17f);note(out,n*.25f,.22f,n<16?40:38,.12f,true);}
    unsigned rng=43;for(int n=0;n<32;n++)for(int i=0;i<2200;i++){rng=1664525*rng+1013904223;float env=1-i/2200.f;out[size_t(n*.25f*44100)+i]+=(float((rng>>16)&65535)/32768.f-1)*env*.024f;}
    return out;
}
}
