#pragma once
#include <SDL.h>
#include <array>
#include <string>
#include <cmath>
#include "combat.h"

namespace art {
using Color=SDL_Color;
constexpr Color ink{12,19,34,255}, white{237,235,217,255}, muted{137,160,172,255}, gold{255,203,100,255}, green{100,223,174,255}, red{250,110,112,255};
inline SDL_Renderer *r=nullptr;
inline void box(int x,int y,int w,int h,Color c) { SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a); SDL_Rect a{x,y,w,h}; SDL_RenderFillRect(r,&a); }
inline void line(int x,int y,int X,int Y,Color c) { SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a); SDL_RenderDrawLine(r,x,y,X,Y); }
inline void border(int x,int y,int w,int h,Color c) {box(x,y,w,1,c);box(x,y+h-1,w,1,c);box(x,y,1,h,c);box(x+w-1,y,1,h,c);}
inline std::array<unsigned char,7> glyph(char c) {
    if(c>='a' && c<='z')c=char(c-'a'+'A');
    switch(c) {
    case 'A':return {14,17,17,31,17,17,17}; case 'B':return {30,17,17,30,17,17,30};
    case 'C':return {14,17,16,16,16,17,14}; case 'D':return {30,17,17,17,17,17,30};
    case 'E':return {31,16,16,30,16,16,31}; case 'F':return {31,16,16,30,16,16,16};
    case 'G':return {14,17,16,23,17,17,15}; case 'H':return {17,17,17,31,17,17,17};
    case 'I':return {31,4,4,4,4,4,31}; case 'J':return {7,2,2,2,18,18,12};
    case 'K':return {17,18,20,24,20,18,17}; case 'L':return {16,16,16,16,16,16,31};
    case 'M':return {17,27,21,21,17,17,17}; case 'N':return {17,25,25,21,19,19,17};
    case 'O':return {14,17,17,17,17,17,14}; case 'P':return {30,17,17,30,16,16,16};
    case 'Q':return {14,17,17,17,21,18,13}; case 'R':return {30,17,17,30,20,18,17};
    case 'S':return {15,16,16,14,1,1,30}; case 'T':return {31,4,4,4,4,4,4};
    case 'U':return {17,17,17,17,17,17,14}; case 'V':return {17,17,17,17,17,10,4};
    case 'W':return {17,17,17,21,21,21,10}; case 'X':return {17,17,10,4,10,17,17};
    case 'Y':return {17,17,10,4,4,4,4}; case 'Z':return {31,1,2,4,8,16,31};
    case '0':return {14,17,19,21,25,17,14}; case '1':return {4,12,4,4,4,4,14};
    case '2':return {14,17,1,2,4,8,31}; case '3':return {30,1,1,14,1,1,30};
    case '4':return {2,6,10,18,31,2,2}; case '5':return {31,16,16,30,1,1,30};
    case '6':return {14,16,16,30,17,17,14}; case '7':return {31,1,2,4,8,8,8};
    case '8':return {14,17,17,14,17,17,14}; case '9':return {14,17,17,15,1,1,14};
    case '\'':return {4,4,8,0,0,0,0};
    case ':':return {0,4,4,0,4,4,0}; case '-':return {0,0,0,31,0,0,0};
    case '/':return {1,2,2,4,8,8,16}; case '.':return {0,0,0,0,0,12,12};
    case '+':return {0,4,4,31,4,4,0}; case '!':return {4,4,4,4,4,0,4};
    case '$':return {4,15,20,14,5,30,4};
    case '>':return {16,8,4,2,4,8,16}; case '<':return {1,2,4,8,4,2,1};
    default:return {0,0,0,0,0,0,0};
    }
}
inline int width(const std::string &s,int scale=1) {return int(s.size())*6*scale-scale;}
inline void text(const std::string&s,int x,int y,Color c=white,int scale=1) {
    for(char ch:s) {auto g=glyph(ch);for(int j=0;j<7;j++)for(int i=0;i<5;i++)if(g[j]&(1<<(4-i)))box(x+i*scale,y+j*scale,scale,scale,c);x+=6*scale;}
}
inline void center(const std::string&s,int x,int y,Color c=white,int scale=1) {text(s,x-width(s,scale)/2,y,c,scale);}
inline Color rgb(uint32_t c){return {Uint8(c>>16),Uint8(c>>8),Uint8(c),255};}
inline Color accent(int who){return rgb(pf::characterInfo(who).accent);}
inline const char* name(int who){return pf::characterInfo(who).name;}
inline const char* ultimateName(int who){return pf::characterInfo(who).ultimate;}
inline void banknote(int x,int y,int scale=1) {
    box(x-10*scale,y-5*scale,20*scale,10*scale,ink);
    box(x-9*scale,y-4*scale,18*scale,8*scale,green);
    border(x-7*scale,y-3*scale,14*scale,6*scale,Color{35,122,91,255});
    box(x-2*scale,y-3*scale,4*scale,6*scale,white);
}
inline void star(int x,int y,float spin,int radius=17) {
    float px[10],py[10];
    for(int n=0;n<10;n++){float a=spin-1.5708f+n*.6283185f;float rr=n%2?radius*.46f:float(radius);px[n]=std::cos(a)*rr;py[n]=std::sin(a)*rr;}
    // Rasterize the rotating five-point star on the logical pixel grid.
    for(int yy=-radius;yy<=radius;yy++)for(int xx=-radius;xx<=radius;xx++) {
        bool inside=false;
        for(int i=0,j=9;i<10;j=i++)if((py[i]>yy)!=(py[j]>yy) && xx<(px[j]-px[i])*(yy-py[i])/(py[j]-py[i])+px[i])inside=!inside;
        if(inside)box(x+xx,y+yy,1,1,Color{236,49,74,255});
    }
    if(radius>=15)center("PT",x,y-3,white);
}
inline void projectile(const pf::Wave&w) {
    int x=int(w.x),y=int(w.y);
    Color c=w.kind==pf::ProjectileKind::PTStar?red:w.kind==pf::ProjectileKind::Mushroom?accent(2):w.kind==pf::ProjectileKind::Drone?accent(4):gold;
    if(w.character>=0)c=accent(w.character);
    if(w.ultimate) {
        for(int n=0;n<10;n++) {
            float phase=w.age*14+n*.75f;
            int tx=x-w.direction*(12+n*5),ty=y+int(std::sin(phase)*(3+n));
            box(tx,ty,3,3,Color{c.r,c.g,c.b,Uint8(180-n*14)});
        }
    }
    for(int n=1;n<5;n++)box(x-w.direction*(n*10+10),y-2+(n%2)*4,7,2,Color{c.r,c.g,c.b,Uint8(140-n*25)});
    switch(w.kind) {
    case pf::ProjectileKind::Cassava:
        for(int root=0;root<3;root++){
            int dx=(root-1)*10,dy=int(std::sin(w.age*14+root)*3);
            box(x+dx-5,y-10+dy,10,25,ink);box(x+dx-4,y-10+dy,8,22,Color{155,93,46,255});
            box(x+dx-2,y-8+dy,3,18,Color{230,190,120,255});box(x+dx-2,y+12+dy,4,8,Color{155,93,46,255});
            box(x+dx-3,y-12+dy,6,4,white);
        }
        box(x-1,y-23,3,16,green);
        for(int leaf=0;leaf<5;leaf++){float a=leaf*.65f;line(x,y-15,x-18+leaf*9,y-27-int(std::sin(a)*8),green);box(x-18+leaf*9,y-29-int(std::sin(a)*8),5,6,green);}break;
    case pf::ProjectileKind::Bat: {
        int flap=int(std::sin(w.age*24)*12);
        for(int side:{-1,1})for(int k=0;k<6;k++){
            int xx=x+side*(5+k*4)-(side<0?4:0),yy=y-6+(flap*k)/5;
            box(xx,yy,5,17-k*2,ink);box(xx,yy+2,4,7-k/2,Color{117,39,77,255});
        }
        box(x-5,y-10,10,21,ink);box(x-6,y-14,4,7,ink);box(x+2,y-14,4,7,ink);
        box(x-3,y-7,2,2,red);box(x+2,y-7,2,2,red);box(x-2,y,1,3,white);box(x+2,y,1,3,white);break;
    }
    case pf::ProjectileKind::Chloroquine:
        box(x-13,y-23,26,8,ink);box(x-11,y-22,22,5,white);
        box(x-15,y-16,30,37,ink);box(x-12,y-14,24,31,Color{72,189,132,255});
        box(x-12,y-5,24,14,white);center("CLQ",x,y-2,ink);box(x-8,y-12,4,7,Color{172,255,201,255});
        for(int n=0;n<3;n++){int px=x-w.direction*(23+n*13),py=y+int(std::sin(w.age*13+n)*16);box(px,py,10,5,white);box(px,py,5,5,green);}break;
    case pf::ProjectileKind::Microphone:
        box(x-4,y-1,8,22,ink);box(x-2,y,4,19,muted);box(x-10,y-17,20,18,ink);box(x-8,y-15,16,13,c);
        for(int j=-13;j<-2;j+=3)line(x-7,y+j,x+6,y+j,white);
        for(int n=0;n<3;n++){border(x+12+n*5,y-14-n*4,3,17+n*8,c);}break;
    case pf::ProjectileKind::Audio:
        box(x-11,y-20,22,40,ink);box(x-9,y-18,18,33,Color{45,77,91,255});
        for(int n=0;n<6;n++){int h=3+int(std::abs(std::sin(w.age*16+n))*16);box(x-8+n*3,y-h/2,2,h,c);}box(x-3,y+16,6,2,white);break;
    case pf::ProjectileKind::Pepper:
        box(x-8,y-12,17,18,red);box(x-5,y+6,10,7,red);box(x-2,y+13,5,5,red);box(x+1,y+18,5,3,red);
        box(x-2,y-18,4,7,green);box(x,y-20,9,4,green);box(x-6,y-9,3,10,white);break;
    case pf::ProjectileKind::Ballot:
        box(x-18,y-10,36,27,ink);box(x-16,y-8,32,23,white);box(x-10,y-7,20,3,ink);
        box(x-8,y-23,16,18,c);text("UP",x-5,y-19,white);break;
    case pf::ProjectileKind::Paw:
        box(x-10,y,20,14,c);box(x-5,y-3,10,6,c);
        for(int n=0;n<4;n++){box(x-17+n*9,y-16+std::abs(n-1)*2,7,10,white);}break;
    case pf::ProjectileKind::Book:
        box(x-19,y-18,38,36,ink);box(x-16,y-16,15,30,white);box(x+1,y-16,15,30,white);box(x-1,y-17,2,34,c);
        for(int n=0;n<5;n++){box(x-13,y-11+n*5,10,2,muted);box(x+4,y-11+n*5,9,2,muted);}break;
    case pf::ProjectileKind::Scroll:
        box(x-15,y-20,30,40,white);box(x-19,y-22,38,6,c);box(x-18,y+17,36,6,c);
        for(int n=0;n<4;n++){box(x-10,y-10+n*5,20-n*2,2,ink);}break;
    case pf::ProjectileKind::Clock:
        for(int n=-20;n<=20;n++){int half=int(std::sqrt(float(400-n*n)));box(x-half,y+n,half*2+1,1,c);}
        border(x-13,y-13,27,27,ink);line(x,y,x+int(std::sin(w.age*15)*12),y-int(std::cos(w.age*15)*12),ink);line(x,y,x-9,y+3,ink);box(x-2,y-2,4,4,white);break;
    case pf::ProjectileKind::Scales:
        box(x-2,y-20,4,37,c);box(x-14,y+17,28,4,c);box(x-23,y-13,46,4,c);
        for(int side:{-1,1}){int k=x+side*17;line(k,y-11,k-7,y+4,white);line(k,y-11,k+7,y+4,white);box(k-9,y+4,18,5,c);}break;
    case pf::ProjectileKind::Dinosaur:
        box(x-14,y-11,24,25,c);box(x+2,y-24,26,18,c);box(x+22,y-13,8,5,c);box(x+17,y-21,4,4,ink);
        box(x-11,y+12,6,10,c);box(x+4,y+12,6,10,c);box(x-27,y+1,18,8,c);box(x-33,y-5,10,9,c);
        box(x+14,y-7,10,3,white);box(x+9,y+1,11,4,c);break;
    case pf::ProjectileKind::MoneyBag:
        box(x-7,y-11,14,4,Color{159,103,56,255});box(x-5,y-7,10,4,gold);
        box(x-10,y-3,20,14,ink);box(x-8,y-4,16,14,Color{197,146,75,255});
        box(x-5,y-2,4,10,Color{230,187,108,255});text("$",x-2,y,ink);
        banknote(x+7,y-8);break;
    case pf::ProjectileKind::Chocolate: {
        int bob=int(std::sin(w.age*20)*2);
        box(x-13,y-10+bob,26,20,ink);box(x-12,y-9+bob,24,18,Color{92,46,32,255});
        for(int yy=0;yy<2;yy++)for(int xx=0;xx<3;xx++){box(x-10+xx*7,y-7+yy*7+bob,6,6,Color{148,82,47,255});box(x-10+xx*7,y-7+yy*7+bob,5,1,Color{201,132,74,255});}
        box(x-13,y+4+bob,26,6,gold);box(x-13,y+3+bob,26,2,white);
        banknote(x-15*w.direction,y-16+int(std::sin(w.age*18)*5));
        for(int n=0;n<5;n++)box(x-12+n*5,y-15+(n%2)*2,4,3,n%2?white:ink);
        break;
    }
    case pf::ProjectileKind::Banknotes:
        banknote(x-6,y-6);banknote(x+5,y+3);banknote(x-12*w.direction,y+15);break;
    case pf::ProjectileKind::PTStar:
        star(x,y,w.age*7);box(x-12,y+14,24,9,white);center("FILA",x,y+15,ink);break;
    case pf::ProjectileKind::Mushroom: {
        int pulse=int(std::sin(w.age*18)*3);
        box(x-5,y,10,17,white);box(x-3,y+3,3,11,gold);
        box(x-17-pulse,y-8,34+pulse*2,10,Color{207,86,239,255});
        box(x-12-pulse,y-16,24+pulse*2,9,Color{157,72,204,255});box(x-7,y-20,14,5,Color{207,86,239,255});
        box(x-11,y-9,6,4,white);box(x+4,y-12,5,5,white);box(x-2,y-17,4,4,gold);
        for(int n=0;n<5;n++){float a=w.age*6+n*1.2566f;box(x+int(std::cos(a)*24),y+int(std::sin(a)*24),3,3,n%2?green:gold);}break;
    }
    case pf::ProjectileKind::Sale:
        box(x-16,y-11,32,22,ink);box(x-14,y-9,28,18,accent(3));
        center("6X1",x,y-3,ink);box(x+11,y-6,2,2,white);break;
    case pf::ProjectileKind::Drone: {
        for(int dx:{-17,17})for(int dy:{-9,9}) {
            line(x,y,x+dx,y+dy,muted);
            float a=w.age*85+(dx+dy)*.2f;
            line(x+dx-int(std::cos(a)*11),y+dy-int(std::sin(a)*4),x+dx+int(std::cos(a)*11),y+dy+int(std::sin(a)*4),white);
            box(x+dx-2,y+dy-2,4,4,gold);
        }
        box(x-9,y-5,18,12,ink);box(x-7,y-4,14,8,accent(4));
        box(x-3,y+7,6,5,white);box(x-1,y+8,2,2,ink);
        box(x-5,y-8,10,3,int(w.age*14)%2?red:accent(2));
        line(x-2,y+13,x-14,y+26,Color{120,180,210,120});line(x+2,y+13,x+14,y+26,Color{120,180,210,120});break;
    }
    case pf::ProjectileKind::Lasso:
        for(int n=0;n<40;n++){float a=n*6.28318f/40;box(x+int(std::cos(a)*18),y+int(std::sin(a)*(10+4*std::sin(w.age*12))),3,3,gold);}
        line(x-18*w.direction,y,x-40*w.direction,y+10,gold);box(x-20,y+14,40,9,white);center("ANISTIA",x,y+15,ink);break;
    case pf::ProjectileKind::Gavel: {
        int swing=int(std::sin(w.age*20)*7);
        box(x-21,y-3+swing,38,7,ink);box(x-20,y-2+swing,31,5,gold);
        box(x+12,y-2+swing,6,5,white);box(x+18,y+swing,4,2,ink);
        for(int n=0;n<3;n++)line(x-18-n*8,y+12+n*3,x+6-n*8,y+10+n*3,white);
        break;
    }
    case pf::ProjectileKind::Gamepad:
        box(x-17,y-10,34,20,ink);box(x-16,y-9,32,18,accent(7));
        box(x-13,y+7,8,7,accent(7));box(x+5,y+7,8,7,accent(7));
        box(x-11,y-4,3,9,ink);box(x-14,y-1,9,3,ink);
        box(x+8,y-5,3,3,red);box(x+12,y,3,3,gold);border(x-4,y-17,8,8,gold);box(x-6,y-11,12,9,gold);break;
    case pf::ProjectileKind::BossCard:
        box(x-18,y-12,36,24,ink);box(x-16,y-10,32,20,gold);border(x-13,y-7,26,14,white);
        center("MASTER",x,y-3,ink);banknote(x-23*w.direction,y+13);
        break;
    case pf::ProjectileKind::Tax:
        box(x-13,y-17,26,34,ink);box(x-11,y-15,22,30,white);
        center("TAXA",x,y-11,red);for(int n=0;n<10;n++)box(x-9+n*2,y+6,1,7,ink);
        center("$",x,y-1,ink);box(x-20,y-8,7,12,red);box(x-24,y-8,4,5,red);box(x-13,y-8,4,5,red);break;
    }
}
inline void supporter(int id,int x,int ground,float t) {
    const char* top[]={"RENAN","ZEMA","AUGUSTO","CAIADO","ALEXANDRE","LULINHA","HADDAD"};
    const char* bottom[]={"SANTOS","","CURY","","DE MORAES","",""};
    Color hair[]={Color{40,32,32,255},Color{144,146,145,255},Color{124,113,104,255},Color{224,223,208,255},Color{219,170,140,255},Color{57,40,32,255},Color{74,65,61,255}};
    Color shirt[]={Color{36,54,68,255},Color{120,148,151,255},Color{66,64,79,255},Color{156,175,180,255},Color{29,32,43,255},Color{150,51,60,255},Color{61,77,103,255}};
    Color skin{216,166,135,255};Color team=id<4?green:red;
    int phase=int(t*4+id*1.7f)%4;
    int y=ground-(phase==2?3:0);
    center(top[id],x,114,muted);center(bottom[id],x,123,muted);
    box(x-11,y-28,22,28,ink);box(x-9,y-27,18,26,shirt[id]);
    box(x-7,y-3,5,7,ink);box(x+3,y-3,5,7,ink);
    box(x-9,y-50,18,22,ink);box(x-8,y-49,16,20,skin);box(x-10,y-42,3,7,skin);box(x+8,y-42,3,7,skin);
    if(id!=4) {box(x-8,y-50,16,5,hair[id]);box(x-9,y-46,3,8,hair[id]);box(x+7,y-46,2,8,hair[id]);}
    else {box(x-6,y-51,12,3,skin);box(x-4,y-49,7,2,Color{242,203,168,255});}
    box(x-5,y-40,2,2,ink);box(x+4,y-40,2,2,ink);box(x,y-38,2,4,Color{173,121,99,255});
    box(x-3,y-31,7,2,white);
    if(id==0){box(x-7,y-33,14,4,hair[id]);box(x-3,y-32,7,1,white);}
    if(id==2){border(x-7,y-42,6,5,ink);border(x+2,y-42,6,5,ink);line(x-1,y-40,x+2,y-40,ink);box(x-4,y-34,9,2,hair[id]);}
    if(id==5){box(x-7,y-35,14,6,hair[id]);box(x-3,y-32,7,1,white);}
    if(id==6){box(x-8,y-49,4,4,muted);box(x-5,y-43,5,1,ink);box(x+3,y-43,5,1,ink);}
    box(x-2,y-27,4,12,team);
    // Four looping poses: raised fists, clap, jump and flag waving.
    int raised=phase==0 || phase==2?12:phase==3?7:0;
    if(phase==1){box(x-15,y-27,6,11,shirt[id]);box(x+10,y-27,6,11,shirt[id]);box(x-10,y-22,20,5,shirt[id]);box(x-6,y-23,6,6,skin);box(x+1,y-23,6,6,skin);}
    else{box(x-15,y-27-raised,6,16,shirt[id]);box(x+10,y-27-raised,6,16,shirt[id]);box(x-15,y-31-raised,6,6,skin);box(x+10,y-31-raised,6,6,skin);}
    if(phase==3){box(x+14,y-54,2,25,white);box(x+16,y-54,15,9,team);if(id>=4)star(x+23,y-49,t,4);}
}
inline void crowd(float t,int left=-1,int right=-1) {
    box(8,97,263,10,Color{18,55,51,255});center(left<0?"TORCIDA FLAVIO":std::string("TORCIDA ")+name(left),139,99,left<0?green:accent(left));
    box(379,97,253,10,Color{67,31,44,255});center(right<0?"TORCIDA LULA":std::string("TORCIDA ")+name(right),505,99,right<0?red:accent(right));
    box(6,204,267,29,Color{24,43,48,255});box(377,204,257,29,Color{43,36,50,255});
    for(int i=0;i<4;i++)if(i+2!=left && i+2!=right)supporter(i,38+i*66,198,t);
    for(int i=4;i<7;i++)if(i+2!=left && i+2!=right)supporter(i,411+(i-4)*85,198,t);
    box(6,204,267,3,green);box(377,204,257,3,red);
    center(left<0?"FLAVIO! FLAVIO!":std::string(name(left))+"!",139,214,left<0?green:accent(left));center(right<0?"LULA! LULA!":std::string(name(right))+"!",505,214,right<0?red:accent(right));
}
// Hand-authored pixel shapes on a two-pixel grid; no fonts or image libraries.
inline void fighter(const pf::Fighter &f,float time,int scale=2,bool portrait=false) {
    int dir=f.facing;
    float stature=pf::bodyScale(f);
    float pain=f.hurtTime>0 && !f.guard && !f.thrown && !f.knockedOut && f.move==pf::Idle?std::sqrt(f.hurtTime/f.hurtDuration):0.f;
    bool walking=!portrait && std::abs(f.walkVelocity)>1 && f.grounded() && f.move==pf::Idle && !f.crouch && !f.thrown && f.knockdown<=0 && f.stun<=0 && pain<=0;
    int walkDir=f.walkVelocity*f.facing>=0?1:-1;
    int bob=walking?int(std::abs(std::sin(f.walkPhase))*1.5f):!f.knockedOut && f.grounded() && f.move==pf::Idle && f.stun<=0?int(std::sin(time*6)*1.2f):0;
    int ox=int(f.x), oy=int(f.y)-bob*scale;
    if(f.move==pf::Ultimate)ox+=dir*int(std::sin(f.clock*34)*2);
    auto b=[&](int x,int y,int w,int h,Color c){
        auto crouchY=[](float v){return v<-47?(v+47)*.76f-16:v<-23?(v+47)*.375f-16:v*7.f/23.f;};
        x+=int((f.pullPose>0?-7.f:-13.f)*std::abs(f.pullPose)*std::clamp(-float(y)/60.f,0.f,1.f));
        // Head snaps back on high hits; low hits fold the torso around the abdomen.
        float upper=std::clamp((-float(y)-24.f)/48.f,0.f,1.f);
        if(walking)x+=int(upper*2*walkDir);
        float recoil=0,fold=0;
        using H=pf::HurtStyle;
        switch(f.hurtStyle){
        case H::Hook:recoil=16;fold=2;break;
        case H::Uppercut:recoil=8;fold=-13;break;
        case H::BodyKick:recoil=-9;fold=10;break;
        case H::LowKick:recoil=-6;fold=6;break;
        case H::AirKick:recoil=14;fold=9;break;
        case H::Bag:recoil=-7;fold=5;break;
        case H::Super:recoil=18;fold=-6;break;
        default:recoil=7;fold=-2;break;
        }
        x+=int(pain*upper*f.hurtDirection*dir*recoil);
        y+=int(pain*upper*fold);
        if(f.hurtStyle==H::LowKick && y>-24){x+=int(pain*(x>0?7:-3));y-=int(pain*(x>0?7:0));}
        if(f.hurtStyle==H::Hook && y<-50)x+=int(pain*(y+62)*.32f);
        if(f.blockImpact>0)x-=int(upper*std::sin(f.blockImpact/.28f*3.14159f)*4);
        int sy=int((f.crouch?crouchY(float(y)):float(y))*scale*stature);
        int sh=std::max(1,int((f.crouch?crouchY(float(y+h)):float(y+h))*scale*stature)-sy);
        if(f.thrown || f.knockdown>0) {
            float angle=f.knockdown>0?dir*1.570796f:f.tumble*dir;
            if(f.knockedOut)angle=-dir*(f.knockdown>0?1.570796f:std::min(f.tumble,1.570796f));
            float ca=std::cos(angle),sa=std::sin(angle);
            float margin=78.f*stature;
            float cx=f.knockdown>0?std::clamp(f.x,margin,640.f-margin):f.x;
            if(f.knockedOut && f.thrown){float tilt=std::min(std::abs(angle)/1.570796f,1.f);cx=f.x+(std::clamp(f.x,margin,640.f-margin)-f.x)*tilt;}
            float cy=f.knockdown>0?pf::FLOOR-50.f:f.y-72.f*stature;
            if(f.knockedOut)cy=f.y-(72.f*stature+(50-72.f*stature)*std::min(std::abs(angle)/1.570796f,1.f));
            float lx=float((dir==1?x:-x-w)*scale),ly=float(sy+36*scale*stature);
            float xs[]={lx,lx+w*scale,lx+w*scale,lx},ys[]={ly,ly,ly+sh,ly+sh};
            SDL_Vertex vertices[4]{};
            for(int i=0;i<4;i++){vertices[i].position={cx+xs[i]*ca-ys[i]*sa,cy+xs[i]*sa+ys[i]*ca};vertices[i].color=c;}
            int indices[]={0,1,2,0,2,3};SDL_RenderGeometry(r,nullptr,vertices,4,indices,6);
        } else {
            if(f.move==pf::Ultimate && f.clock>=.55f && f.clock<1.7f)x+=int(std::sin((f.clock-.55f)*34)*3*std::clamp(-float(y)/60.f,0.f,1.f));
            box(ox+(dir==1?x:-x-w)*scale,oy+sy,w*scale,sh,c);
        }
    };
    const auto &look=pf::characterInfo(f.character);
    Color skin=rgb(look.skin),shade{Uint8(skin.r*.77f),Uint8(skin.g*.72f),Uint8(skin.b*.74f),255};
    Color hair=rgb(look.hair),suit=rgb(look.suit);
    Color light{Uint8(std::min(255,int(suit.r)+24)),Uint8(std::min(255,int(suit.g)+24)),Uint8(std::min(255,int(suit.b)+24)),255};
    if(f.character==1 && f.evolved){hair={40,31,27,255};suit={160,39,51,255};light={199,59,61,255};}
    Color tie=accent(f.character);
    bool kick=f.move==pf::Kick && f.clock>.12f && f.clock<.35f;
    bool punch=f.move==pf::Punch && f.clock>.05f && f.clock<.20f;
    bool ult=f.move==pf::Ultimate;
    bool grab=f.move==pf::Grab && f.clock>.08f;
    if(f.character==pf::Temer){
        for(int yy=-48;yy<-3;yy+=3){int width=19+(-yy<26?7:3)+int(std::sin(time*5+yy*.1f)*2);b(-width,yy,width*2,3,ink);b(-width+2,yy,5,3,Color{115,31,54,255});}
        b(-22,-58,8,19,ink);b(14,-58,8,19,ink);b(-20,-55,5,12,red);b(15,-55,5,12,red);
    }
    // Shoes, trousers, jacket with rim highlights.
    if(walking){
        for(int leg=0;leg<2;leg++){
            float cycle=std::sin(f.walkPhase+leg*3.141593f);int swing=int(cycle*6),lift=int(std::max(0.f,std::cos(f.walkPhase+leg*3.141593f))*4);
            int hip=leg?4:-13;
            b(hip,-24,10,11,suit);b(hip+swing/2,-17,10,9,light);
            b(hip+swing,-12-lift,9,9,suit);b(hip+swing-2,-5-lift,14,5,ink);b(hip+swing-1,-5-lift,11,1,muted);
        }
    } else {
    b(-15,-5,13,5,ink);if(!kick)b(4,-5,16,5,ink);
    b(-13,-23,10,19,suit); b(-11,-23,3,17,light);
    if(kick && f.comboStage==3) {
        b(0,-34,14,15,ink);b(2,-34,10,13,light);
        b(9,-46,13,18,suit);b(18,-55,13,18,light);b(25,-61,16,10,ink);
        b(31,-64,16,2,gold);b(38,-56,12,2,white);b(42,-48,10,2,gold);
    } else if(kick && f.comboStage==2) {
        b(-8,-30,24,11,ink);b(-6,-29,22,8,light);
        b(12,-37,20,10,suit);b(29,-38,15,10,ink);b(34,-38,8,2,muted);
        b(35,-43,12,2,white);b(42,-35,9,2,gold);
    } else if(kick) { b(0,-27,24,10,ink);b(1,-26,22,7,suit);b(21,-27,13,9,ink);b(23,-27,10,2,muted); }
    else {b(4,-23,10,19,suit);b(5,-23,3,17,light);}
    }
    b(-18,-48,36,27,ink);b(-16,-47,32,24,suit);b(-15,-46,5,23,light);
    b(-5,-48,15,14,white);b(0,-46,4,17,tie);b(-8,-46,5,12,light);b(8,-46,5,12,light);
    b(-1,-26,2,2,gold);b(10,-41,3,2,accent(f.character));
    // Longer hair sits behind the head; robes are part of the torso, never extra arms.
    if(look.look&8){b(-15,-67,7,30,hair);b(11,-66,7,29,hair);}
    if(f.character==6 || (f.character>=18 && f.character<=26)){b(-16,-26,32,10,suit);b(-3,-45,7,15,white);}
    // Big caricatural heads, distinct hair, eyebrows and beard.
    b(-12,-72,27,25,ink);b(-13,-66,29,16,shade);b(-11,-71,25,21,skin);b(-9,-68,21,16,skin);
    b(-13,-68,4,9,skin);b(12,-62,5,5,skin);
    if(f.character==pf::BossCharacter) {
        b(-12,-73,25,7,hair);b(-13,-68,5,12,hair);b(11,-68,4,10,hair);
        b(-10,-57,5,8,hair);b(-6,-53,18,7,hair);b(11,-57,4,9,hair);
        b(-4,-64,6,2,ink);b(6,-64,6,2,ink);b(-2,-61,2,2,ink);b(9,-61,2,2,ink);
        b(3,-60,3,6,shade);b(-2,-53,11,2,white);b(12,-33,6,5,gold);
    } else if(f.character>=2) {
        if(!(look.look&4)){b(-11,-73,23,5,hair);b(-13,-69,5,10,hair);b(10,-69,4,7,hair);}
        else {b(-8,-73,17,3,skin);b(-4,-72,10,2,Color{242,203,168,255});}
        b(-5,-64,6,2,hair);b(6,-64,6,2,hair);b(-3,-61,2,2,ink);b(9,-61,2,2,ink);b(3,-60,3,6,shade);
        b(-3,-52,11,2,white);
        if(look.look&1){b(-10,-55,7,6,hair);b(-6,-52,18,5,hair);b(-2,-52,10,2,white);}
        if(look.look&2){b(-9,-64,10,1,ink);b(-9,-59,10,1,ink);b(-9,-64,1,6,ink);b(0,-64,1,6,ink);b(4,-64,10,1,ink);b(4,-59,10,1,ink);b(4,-64,1,6,ink);b(13,-64,1,6,ink);b(1,-62,3,1,ink);b(-2,-54,9,2,hair);}
        if(f.character==8){b(-11,-71,4,9,muted);b(-6,-65,7,2,ink);b(6,-65,7,2,ink);}
    } else if(f.character==0) {
        b(-11,-73,23,6,hair);b(-13,-69,6,9,hair);b(-8,-74,14,2,hair);b(7,-69,7,3,hair);
        b(-7,-71,13,2,Color{97,77,65,255});
        b(-4,-64,6,2,hair);b(7,-64,6,2,hair);b(-2,-61,2,2,ink);b(9,-61,2,2,ink);
        b(3,-61,3,6,shade);b(1,-53,9,1,hair);b(-5,-51,14,2,shade);
    } else {
        b(-12,-71,5,12,hair);b(-9,-73,19,4,hair);b(10,-70,4,7,hair);
        b(-7,-72,14,2,f.evolved?hair:white);b(-5,-65,7,2,hair);b(7,-65,6,2,hair);
        b(-2,-62,2,2,ink);b(9,-62,2,2,ink);b(4,-61,3,5,shade);
        b(-11,-57,6,8,hair);b(-7,-54,21,7,hair);b(-4,-48,15,3,hair);b(11,-58,4,9,hair);
        b(-3,-55,13,2,white);b(1,-53,7,2,shade);b(-4,-49,13,2,white);
    }
    if(f.character==pf::Dilma){b(-14,-71,8,17,hair);b(-8,-76,20,5,hair);b(6,-73,10,7,hair);b(-9,-73,17,3,Color{151,80,42,255});b(-14,-57,2,3,gold);b(13,-56,2,3,gold);b(-2,-52,9,2,red);b(-4,-44,11,3,white);}
    if(f.character==pf::Temer){b(-8,-75,16,3,hair);b(-5,-62,3,2,red);b(8,-62,3,2,red);b(-3,-53,11,3,ink);b(-2,-53,2,5,white);b(5,-53,2,5,white);b(-1,-42,5,5,red);}
    if(pain>0) {
        b(-6,-65,19,8,skin);
        b(-5,-63,6,2,ink);b(-4,-64,2,1,ink);b(6,-63,6,2,ink);b(9,-64,2,1,ink);
        b(-1,-55,10,6,ink);b(1,-51,6,2,Color{179,75,74,255});
    }
    if(ult && f.clock<.55f && !portrait) {
        pf::Wave prop;prop.x=f.x+f.facing*38;prop.y=f.y-157-std::sin(f.clock*8)*10;
        prop.direction=f.facing;prop.character=f.character;prop.kind=pf::ultimateKind(f.character);prop.age=f.clock;
        projectile(prop);
    }
    // Arms are drawn last, making guarded and extended poses readable.
    auto rearIdle=[&](){b(-22,-43,8,18,ink);b(-21,-42,6,15,light);b(-21,-29,7,6,skin);};
    if(f.cinematicCuffed) {
        b(-19,-43,8,15,light);b(12,-43,8,15,light);
        b(-16,-33,32,7,light);b(-11,-33,7,7,skin);b(4,-33,7,7,skin);
    } else if(pain>0) {
        using H=pf::HurtStyle;
        if(f.hurtStyle==H::LowKick){b(-22,-41,7,23,light);b(-21,-21,9,8,skin);b(10,-34,9,20,suit);b(8,-18,12,8,skin);}
        else if(f.hurtStyle==H::Bag || f.hurtStyle==H::BodyKick){b(-16,-36,25,8,light);b(3,-35,12,7,skin);b(13,-42,8,16,suit);b(4,-29,16,7,skin);}
        else if(f.hurtStyle==H::Uppercut || f.hurtStyle==H::Super){b(-27,-46,12,10,light);b(-30,-50,10,9,skin);b(15,-45,17,9,light);b(29,-42,10,9,skin);}
        else if(f.hurtStyle==H::Hook){b(-23,-55,8,19,light);b(-20,-60,13,9,skin);b(12,-38,14,9,light);b(23,-34,9,9,skin);}
        else if(f.hurtStyle==H::AirKick){b(-22,-42,10,16,light);b(-25,-31,11,9,skin);b(12,-54,9,18,light);b(8,-59,13,10,skin);}
        else {b(-26,-51,8,18,light);b(-27,-59,10,11,skin);b(14,-35,13,9,light);b(23,-37,10,10,skin);}
    } else if(f.guard && f.crouch) {
        b(8,-37,17,10,ink);b(9,-36,15,8,light);b(21,-38,9,9,skin);
        b(-8,-31,28,8,light);b(17,-31,10,8,skin);
    } else if(f.pullPose>0) {
        int extension=int(19-13*std::min(f.pullPose,1.f));
        b(9,-48,extension+8,9,ink);b(10,-47,extension+6,7,light);
        b(15+extension,-49,9,10,skin);b(16+extension,-48,7,2,white);
        b(-12,-40,extension+26,8,light);b(12+extension,-42,10,9,skin);
    } else if(f.pullPose<0) {
        b(10,-37,19,8,light);b(25,-35,9,9,skin);
        b(-25,-50,8,16,light);b(-26,-55,9,8,skin);
    } else if(f.guard) {
        b(12,-48,9,19,ink);b(13,-47,7,17,light);b(12,-56,9,10,skin);b(13,-55,7,2,white);
        b(-6,-43,20,8,light);b(7,-43,9,7,skin);
    } else if(ult && f.clock<.2f) {
        b(-12,-43,33,10,light);b(3,-47,10,10,skin);b(-7,-47,9,10,skin);
    } else if(ult && f.clock<.55f) {
        b(11,-57,9,23,light);b(10,-63,11,9,skin);
        b(-20,-58,8,16,light);b(-21,-64,10,8,skin);
    } else if(ult && f.clock<1.7f && std::fmod(f.clock-.55f,.18f)>.095f) {
        b(6,-47,12,8,light);b(6,-53,9,9,skin);b(-21,-43,8,14,light);b(-20,-46,10,8,skin);
    } else if(punch && f.comboStage==3) {
        b(12,-47,12,18,ink);b(14,-46,9,15,light);
        b(22,-64,10,27,light);b(21,-73,13,13,skin);b(23,-72,9,3,white);
        b(36,-72,2,20,gold);b(40,-66,2,11,white);b(19,-78,17,2,gold);
        b(-10,-42,20,8,light);b(5,-45,9,10,skin);
    } else if(punch && f.comboStage==2) {
        b(-14,-49,42,10,ink);b(-12,-48,39,7,light);
        b(25,-51,17,11,skin);b(28,-50,11,2,white);
        b(42,-49,13,2,white);b(37,-54,10,2,gold);
        b(13,-36,8,13,suit);b(15,-32,9,8,skin);
    } else if(punch || grab || ult || (f.move==pf::Magic && f.clock>.14f)) {
        rearIdle();
        b(12,-44,19,10,ink);b(13,-43,18,7,light);
        b(29,-44,10,10,skin);b(30,-43,8,2,white);
        if(grab) {b(36,-47,5,5,skin);b(37,-37,5,4,skin);}
        if(ult && f.clock<1.7f) {b(35,-46,3,14,gold);b(32,-41,10,3,white);}
    } else if(walking) {
        int swing=int(std::sin(f.walkPhase)*5);
        b(-22,-43,8,11,light);b(-22-swing/2,-35,8,11,suit);b(-23-swing/2,-26,9,7,skin);
        b(13,-43,9,11,light);b(13+swing/2,-35,9,11,light);b(14+swing/2,-26,9,7,skin);
    } else {
        rearIdle();
        b(13,-43,10,17,ink);b(14,-42,8,14,light);b(16,-31,9,8,skin);b(17,-30,7,2,white);
    }
}
inline void bossArena(float t) {
    Color velvet{77,21,48,255},pink{238,99,180,255};
    box(0,0,640,360,Color{22,12,31,255});
    for(int x=0;x<640;x+=24){box(x,65,15,167,velvet);box(x+2,65,3,150,Color{113,31,65,255});}
    box(158,90,324,30,ink);border(158,90,324,30,pink);center("CABARE MASTER",320,96,gold,2);
    center("CENARIO FICTICIO / SATIRA / ADULTOS",320,123,muted);
    // Fully clothed adult stage performers, separate from the public-figure caricatures.
    for(int side=0;side<2;side++) {
        int px=82+side*476,py=125+int(std::sin(t*3+side)*2);
        box(px+13,85,2,59,gold);
        box(px-6,py-29,12,14,Color{49,28,38,255});box(px-4,py-27,8,10,Color{207,157,128,255});
        box(px-5,py-16,10,12,pink);box(px-9,py-5,18,7,pink);
        box(px-6,py+2,4,12,ink);box(px+2,py+2,4,12,ink);
        line(px+5,py-13,px+13,py-24,Color{207,157,128,255});
        box(px-18,py+14,36,3,gold);
    }
    for(int n=0;n<6;n++) {
        int x=45+n*110;box(x,66,10,7,gold);
        int dx=int(std::sin(t*1.5f+n)*35);
        line(x+5,74,x+dx-25,216,Color{139,62,107,130});line(x+5,74,x+dx+25,216,Color{139,62,107,130});
    }
    box(9,187,622,47,velvet);box(12,190,616,5,pink);
    // Nine adult caricatures share tables, raise beer mugs and exchange fictional envelopes.
    for(int id=0;id<9;id++) {
        int x=32+id*59;bool raised=int(t*1.6f+id)%3==0;
        pf::Fighter patron;patron.character=id;patron.x=float(x);patron.y=220;patron.facing=id%2?-1:1;patron.guard=raised;
        SDL_Rect clip{x-28,135,56,82};SDL_RenderSetClipRect(r,&clip);fighter(patron,t,1,true);SDL_RenderSetClipRect(r,nullptr);
        int mx=x+patron.facing*(raised?17:22),my=raised?165:188;
        box(mx-4,my,8,12,gold);box(mx-4,my-2,8,4,white);border(mx+4,my+2,4,6,white);
        if(id%2==0){box(x-20,213,54,4,Color{121,74,63,255});box(x+4,217,4,15,ink);banknote(x+int(std::sin(t*2+id)*13),209,1);}
    }
    // Claudio Castro, labelled, at the far-right VIP booth.
    int x=599,y=204,bob=int(std::sin(t*2)*2);
    box(x-12,y-30,24,32,Color{54,70,89,255});box(x-3,y-29,5,20,gold);
    box(x-11,y-54+bob,23,24,Color{210,163,131,255});box(x-11,y-55+bob,23,6,Color{60,45,36,255});
    box(x-6,y-44+bob,3,2,ink);box(x+5,y-44+bob,3,2,ink);box(x-3,y-35+bob,10,2,white);
    box(x-19,y-28,8,15,gold);box(x-19,y-30,8,4,white);
    center("CLAUDIO",599,219,white);center("CASTRO",599,228,white);
    box(190,229,252,17,ink);center("CAMAROTE / CONTRATOS E MALOTES",316,234,gold);
    for(int n=0;n<4;n++) {int bx=212+n*58;box(bx,245,23,14,Color{87,59,47,255});border(bx+7,240,9,6,gold);banknote(bx+int(std::sin(t*2+n)*10),246);}
    box(0,280,640,80,Color{42,25,46,255});box(0,283,640,4,gold);
    for(int y=301;y<360;y+=18)line(0,y,640,y,Color{81,43,78,255});
    for(int x=0;x<700;x+=60)line(320+(x-320)/2,287,x,360,Color{91,48,83,255});
}
inline void plazaRunner(int x,int ground,float phase,int direction,int variant) {
    Color jersey{220,187,48,255},trim{35,112,66,255},shorts{37,70,124,255};
    Color skin=variant%3==0?Color{125,85,62,255}:variant%3==1?Color{192,138,96,255}:Color{223,173,127,255};
    int bob=int(std::abs(std::sin(phase))*2),y=ground-bob;
    int stride=int(std::sin(phase)*4);
    box(x-5,ground-1,15,2,Color{85,83,75,255});
    box(x-4,y-9,4,8,skin);box(x+3,y-9,4,8,skin);
    box(x-5+stride,y-3,6,3,ink);box(x+2-stride,y-3,6,3,ink);
    box(x-5,y-13,13,6,shorts);box(x-6,y-25,15,13,jersey);
    box(x-6,y-25,15,2,trim);box(x-7,y-23,3,6,trim);box(x+8,y-23,3,6,trim);
    box(x-2,y-34,8,9,skin);box(x-2,y-35,8,3,Color{64,49,38,255});
    box(x+(direction>0?5:-2),y-30,3,3,skin);
    box(x-1,y-21,2,6,trim);box(x+3,y-21,2,6,trim);
    int swing=int(std::sin(phase+1)*4);
    box(x-9,y-21+swing,3,8,skin);box(x+10,y-21-swing,3,8,skin);
    if(variant%4==0) {
        int pole=x+direction*13,flutter=int(std::sin(phase*.6f)*2);
        box(pole,y-49,2,31,muted);box(pole+2,y-49+flutter,21,13,trim);
        for(int row=0;row<7;row++){int span=7-std::abs(row-3)*2;box(pole+12-span/2,y-46+row+flutter,span,1,jersey);}
        box(pole+10,y-44+flutter,5,4,shorts);
    }
}
inline void arena(float t,int left=-1,int right=-1) {
    box(0,0,640,360,ink);
    for(int y=0;y<245;y+=5) box(0,y,640,5,Color{Uint8(65+y/5),Uint8(93+y/9),Uint8(122+y/20),255});
    box(483,70,28,25,Color{245,194,126,255});
    for(int n=0;n<6;n++){int x=int(std::fmod(n*127.f+t*2,760.f))-80;box(x,70+n%3*8,70,3,Color{173,160,155,255});}
    // Stylized Three Powers square: STF and Planalto frame the Congress silhouette.
    box(0,156,640,103,Color{139,134,112,255});
    box(8,126,155,28,Color{67,91,102,255});box(1,121,174,5,white);box(0,152,182,5,muted);
    box(475,119,156,35,Color{62,89,102,255});box(470,113,170,6,white);box(467,151,173,5,muted);
    for(int n=0;n<7;n++) {
        int x=13+n*22;box(x,125,4,26,white);line(x,125,x+9,148,white);box(x+5,147,7,4,white);
        x=480+n*23;box(x,119,4,32,white);line(x,120,x+10,146,white);
    }
    center("STF",84,132,white);center("PLANALTO",555,128,white);
    for(int n=0;n<5;n++)box(9-n*2,157+n*3,154+n*4,2,Color{181,178,154,255});
    for(int n=0;n<5;n++)box(477-n*2,157+n*3,154+n*4,2,Color{181,178,154,255});
    // Brasilia-inspired silhouette: twin towers, domes and reflecting pool.
    box(278,113,24,106,Color{60,89,109,255});box(310,113,24,106,Color{72,104,122,255});
    box(278,111,24,3,muted);box(310,111,24,3,muted);
    for(int y=120;y<210;y+=7) {box(281,y,18,2,Color{31,52,73,255});box(313,y,18,2,Color{37,61,80,255});}
    box(177,205,290,17,Color{84,114,130,255});box(168,221,310,7,Color{42,70,89,255});
    for(int y=0;y<23;y++) {int w=int(std::sqrt(std::max(0.,1.-std::pow((23.-y)/23.,2)))*42);box(221-w,182+y,w*2,1,Color{121,151,162,255});}
    for(int y=0;y<18;y++) box(365+y,184+y,86-2*y,1,Color{143,165,168,255});
    box(357,181,101,4,Color{183,197,189,255});
    box(0,235,640,18,Color{28,62,79,255});
    for(int i=0;i<28;i++)box((i*39)%640,238+(i%4)*3,15+i%10,1,Color{69,108,116,255});
    // Background-only invasion loop: running, flags and people vaulting displaced rails.
    for(int side=0;side<2;side++)for(int n=0;n<9;n++) {
        float u=std::fmod(t*.065f+n/9.f,1.f);
        int x=side?650-int(u*198):-10+int(u*198);
        int y=190-int(u*32);
        int hop=int(std::max(0.f,1.f-std::abs(u-.55f)/.12f)*13);
        plazaRunner(x,y-hop,t*10+n*2,side?-1:1,n+side);
    }
    crowd(t,left,right);
    // Rails and audience silhouettes.
    box(0,235,640,31,Color{116,113,96,255});
    for(int n=0;n<22;n++) {
        int direction=n%2?1:-1;
        int x=int(std::fmod(n*41.f+t*(direction>0?31.f:-27.f)+64000,700.f))-30;
        plazaRunner(x,262+(n%2)*3,t*11+n,direction,n);
    }
    for(int x:{112,488}){line(x,259,x+27,271,muted);line(x,261,x+27,273,muted);line(x+4,257,x+1,268,muted);}
    box(0,268,640,3,Color{106,135,141,255});box(0,277,640,3,Color{29,42,56,255});
    for(int x=12;x<640;x+=48)box(x,268,3,18,Color{83,107,120,255});
    box(0,284,640,76,Color{34,44,59,255});box(0,284,640,3,Color{153,156,139,255});
    for(int y=298;y<360;y+=19)box(0,y,640,1,Color{52,64,77,255});
    for(int x=-160;x<850;x+=64)line(320+(x-320)/2,287,x,360,Color{52,64,77,255});
    box(243,304,154,2,Color{81,91,91,255});box(273,314,94,2,Color{81,91,91,255});
    (void)t;
}
}
