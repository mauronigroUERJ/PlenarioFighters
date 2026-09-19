#pragma once
// Code-native scenery: fixed 640x360 pixel canvas, SDL2 primitives only.
namespace art {
inline bool detailedStage(int host){return host==0||host==1||host==2||host==3||host==5||host==7;}
inline void skyBands(Color sky){
    box(0,0,640,284,sky);
    for(int y=80;y<199;y+=8)box(0,y,640,8,Color{Uint8(std::min(255,int(sky.r)+(y-80)/4)),Uint8(std::min(255,int(sky.g)+(y-80)/5)),Uint8(std::min(255,int(sky.b)+(y-80)/6)),255});
}
inline void brazilFlag(int x,int y,float t){
    box(x,y-3,2,77,white);int wave=int(std::sin(t*4)*2);box(x+2,y+wave,34,22,green);
    for(int n=0;n<17;n++){int width=24-std::abs(n-8)*3;box(x+19-width/2,y+3+n+wave,std::max(1,width),1,gold);}
    box(x+14,y+7+wave,11,8,Color{34,74,139,255});line(x+14,y+9+wave,x+24,y+11+wave,white);
}
inline void banker(int x,int feet,float t,int variant){
    Color suit=variant%2?Color{62,72,86,255}:Color{29,49,70,255},skin{211,157,117,255};int bob=int(std::sin(t*5+variant)*1.2f);
    box(x-7,feet-12,5,12,ink);box(x+2,feet-12,5,12,ink);box(x-10,feet-34,20,23,suit);
    box(x-2,feet-33,4,15,white);box(x-1,feet-31,2,16,variant%2?red:gold);
    box(x-7,feet-47+bob,14,14,skin);box(x-7,feet-48+bob,14,5,variant%3?ink:muted);
    box(x-6,feet-42+bob,12,2,ink);box(x-2,feet-37+bob,5,2,ink);
    box(x-14,feet-32,5,18,suit);box(x-14,feet-16,5,5,skin);
    // One arm holds a phone; the other swings a briefcase. Exactly two arms.
    int swing=int(std::sin(t*4+variant)*4);box(x+9,feet-34,5,11,suit);box(x+10+swing/2,feet-40,5,10,skin);
    box(x+9+swing/2,feet-45,7,13,ink);box(x+10+swing/2,feet-43,5,7,Color{88,188,201,255});
    border(x-15,feet-14,7,4,gold);box(x-21,feet-10,18,12,Color{84,57,40,255});box(x-20,feet-8,16,1,gold);
}
inline void farmer(int x,int feet,float t,int variant){
    Color shirt=variant%2?Color{167,66,45,255}:Color{53,108,74,255},skin{190,133,90,255};int swing=int(std::sin(t*6+variant)*4);
    box(x-7,feet-12,5,12,Color{48,71,92,255});box(x+2,feet-12,5,12,Color{48,71,92,255});box(x-8,feet-4,7,4,ink);box(x+1,feet-4,9,4,ink);
    box(x-10,feet-32,20,22,shirt);for(int n=0;n<3;n++){box(x-9+n*6,feet-31,2,19,Color{213,177,118,130});box(x-9,feet-28+n*6,18,1,Color{213,177,118,130});}
    box(x-7,feet-45,14,13,skin);box(x-10,feet-48,20,6,Color{137,100,55,255});box(x-16,feet-44,32,3,gold);box(x-6,feet-49,12,2,gold);
    box(x-4,feet-39,2,2,ink);box(x+3,feet-39,2,2,ink);box(x-3,feet-35,7,2,ink);
    box(x-14,feet-31,5,16,shirt);box(x-14,feet-17,5,5,skin);
    box(x+9,feet-33-swing,5,14,shirt);box(x+8,feet-37-swing,7,6,skin);
}
inline void tractor(int x,int y,float t){
    Color paint{66,128,54,255};box(x-28,y-24,48,14,paint);box(x-25,y-26,30,4,gold);box(x+7,y-47,27,28,ink);box(x+10,y-44,21,20,Color{104,169,177,255});
    box(x+5,y-50,32,4,paint);box(x-17,y-39,4,14,ink);box(x+17,y-16,21,6,paint);
    for(int wheel=0;wheel<2;wheel++){int cx=x+(wheel?23:-21),size=wheel?13:9;box(cx-size,y-size,size*2,size*2,ink);box(cx-size+3,y-size+3,size*2-6,size*2-6,Color{75,79,69,255});box(cx-3,y-3,6,6,gold);float a=t*4;line(cx-int(std::cos(a)*size*.7f),y-int(std::sin(a)*size*.7f),cx+int(std::cos(a)*size*.7f),y+int(std::sin(a)*size*.7f),muted);}
}
inline void cow(int x,int feet,float t){
    box(x-17,feet-24,35,18,Color{223,215,188,255});box(x-12,feet-23,11,9,ink);box(x+7,feet-20,8,12,ink);
    for(int n=0;n<4;n++)box(x-14+n*9,feet-8,3,8,Color{174,158,130,255});
    box(x+14,feet-28,15,17,Color{223,215,188,255});box(x+24,feet-18,8,7,Color{174,133,114,255});box(x+22,feet-25,2,2,ink);
    box(x+15,feet-32,2,6,gold);box(x+26,feet-32,2,6,gold);line(x-17,feet-21,x-25,feet-11+int(std::sin(t*5)*4),ink);
}
inline void guest(int id,int x,int feet,float t){
    // Arthur do Val / Kim Kataguiri, background caricatures identified by nameplates.
    bool kim=id==1;Color shirt=kim?Color{53,79,117,255}:Color{35,39,48,255},skin{218,167,132,255};int bob=int(std::sin(t*3+id)*1.2f);
    box(x-10,feet-17,7,17,ink);box(x+4,feet-17,7,17,ink);box(x-15,feet-42,30,27,shirt);box(x-14,feet-41,5,25,Color{74,88,105,255});
    box(x-11,feet-65+bob,23,24,skin);box(x-12,feet-68+bob,25,8,Color{32,28,30,255});box(x-12,feet-61+bob,4,10,ink);
    if(kim){box(x-10,feet-59+bob,22,2,ink);border(x-9,feet-60+bob,8,6,ink);border(x+3,feet-60+bob,8,6,ink);box(x-4,feet-48+bob,12,2,ink);}
    else{box(x-10,feet-49+bob,21,9,Color{56,37,32,255});box(x-7,feet-57+bob,4,2,ink);box(x+5,feet-57+bob,4,2,ink);box(x-4,feet-45+bob,10,2,white);}
    int swing=int(std::sin(t*4+id)*4);box(x-19,feet-39,6,19,shirt);box(x-19,feet-22,6,7,skin);
    box(x+13,feet-41,6,15,shirt);box(x+15,feet-45-swing,7,12,skin);
    if(kim){box(x+12,feet-34-swing,17,13,white);box(x+14,feet-32-swing,12,1,muted);box(x+14,feet-28-swing,10,1,muted);}
    else{box(x+16,feet-49-swing,8,15,ink);box(x+18,feet-47-swing,4,9,Color{69,168,206,255});}
    box(x-(kim?42:39),feet+3,kim?84:78,12,ink);center(kim?"KIM KATAGUIRI":"ARTHUR DO VAL",x,feet+6,gold);
}
inline void arcadeCabinet(int x,int y,float t){
    box(x,y,25,42,ink);box(x+2,y+2,21,6,Color{159,58,155,255});box(x+3,y+11,19,16,Color{33,76,113,255});
    box(x+5+int(t*8)%11,y+16,4,5,gold);box(x+14,y+20,4,3,white);box(x+1,y+29,24,5,red);box(x+6,y+28,2,4,white);box(x+16,y+30,3,2,gold);box(x+7,y+37,12,3,muted);
}
inline void atibaiaGuest(bool federal,int x,int feet,float t){
    Color skin{214,166,127,255},suit=federal?Color{39,48,48,255}:Color{42,47,61,255};
    Color hair=federal?Color{158,157,146,255}:Color{38,34,35,255};int bob=int(std::sin(t*2.8f+federal)*1.2f);
    box(x-9,feet-18,7,18,ink);box(x+3,feet-18,7,18,ink);box(x-11,feet-3,10,3,ink);box(x+2,feet-3,11,3,ink);
    box(x-15,feet-43,30,27,suit);box(x-14,feet-42,4,24,Color{64,74,81,255});
    box(x-10,feet-66+bob,21,24,skin);box(x-11,feet-68+bob,23,7,hair);box(x-11,feet-62+bob,3,10,hair);
    box(x-5,feet-58+bob,3,2,ink);box(x+5,feet-58+bob,3,2,ink);box(x-3,feet-48+bob,9,2,ink);
    if(federal){
        border(x-9,feet-60+bob,8,6,ink);border(x+2,feet-60+bob,8,6,ink);line(x-1,feet-58+bob,x+2,feet-58+bob,ink);
        box(x-12,feet-42,24,23,Color{23,30,32,255});text("PF",x-6,feet-37,gold);box(x-10,feet-24,8,4,Color{55,66,60,255});box(x+3,feet-24,8,4,Color{55,66,60,255});
    }else{box(x-5,feet-43,11,14,white);box(x-1,feet-41,3,19,Color{137,48,56,255});box(x+9,feet-61+bob,3,9,muted);}
    box(x-19,feet-40,6,21,suit);box(x-19,feet-22,6,7,skin);
    int gesture=int(std::sin(t*3.3f+federal)*3);box(x+13,feet-41,6,15,suit);
    if(federal){box(x+13,feet-51-gesture,7,14,skin);box(x+14,feet-57-gesture,7,14,ink);box(x+16,feet-62-gesture,2,7,ink);box(x+16,feet-53-gesture,3,4,gold);}
    else{box(x+13,feet-31+gesture,7,7,skin);box(x+9,feet-32+gesture,20,17,Color{200,188,148,255});box(x+11,feet-30+gesture,16,13,white);for(int n=0;n<3;n++)box(x+13,feet-27+gesture+n*3,11,1,muted);}
    int width=federal?108:72;box(x-width/2,feet+4,width,12,ink);center(federal?"JAPONES DA FEDERAL":"SERGIO MORO",x,feet+7,gold);
}
inline void atibaia(float t){
    skyBands(Color{105,144,164,255});
    for(int x=0;x<640;x+=6){int top=112-int(std::sin(x*.017f)*9+std::cos(x*.027f)*8);box(x,top,6,101,Color{72,113,79,255});}
    for(int side=0;side<2;side++)for(int n=0;n<3;n++){
        int x=side?535+n*42:15+n*41,y=151+(n%2)*11;box(x,y,7,70,Color{108,77,48,255});
        box(x-16,y-25,41,34,Color{43,105,65,255});box(x-11,y-32,30,10,Color{64,126,66,255});
        for(int fruit=0;fruit<4;fruit++)box(x-10+fruit*8,y-18+(fruit%2)*13,4,4,Color{224,151,59,255});
        box(x-18+int(std::sin(t*2+n)*2),y-18,12,3,Color{93,143,75,255});
    }
    // Fictional countryside composition: tiled roof, veranda, shutters and garden.
    box(126,149,392,69,Color{222,204,163,255});box(137,158,370,54,Color{201,181,140,255});
    for(int row=0;row<29;row++){int x=105+row*2,w=435-row*4;box(x,151-row,w,1,row%5?Color{158,75,46,255}:Color{191,106,64,255});}
    for(int x=137;x<495;x+=27)line(x,151,x+26,123,Color{112,59,42,255});
    box(117,151,413,6,Color{103,65,47,255});
    for(int x:{140,229,409,498}){box(x,158,6,56,Color{237,224,190,255});box(x-3,211,12,7,Color{167,156,125,255});}
    for(int x:{159,440}){box(x,167,39,31,Color{72,103,106,255});box(x-7,165,7,36,Color{79,94,61,255});box(x+39,165,7,36,Color{79,94,61,255});box(x+18,167,3,31,white);box(x,181,39,2,white);}
    box(303,162,36,51,Color{91,65,45,255});box(307,166,28,43,Color{132,92,55,255});box(327,187,3,3,gold);
    box(0,217,640,61,Color{139,133,97,255});box(128,216,394,5,Color{185,178,149,255});box(115,222,417,4,Color{174,167,137,255});
    // A reflecting pool and a stone path make the country-house silhouette readable.
    box(20,226,144,35,Color{197,192,158,255});box(24,230,136,26,Color{52,130,152,255});
    for(int n=0;n<13;n++)box(27+(n*23+int(t*9))%126,234+n%4*5,10,1,Color{132,191,197,255});
    for(int n=0;n<4;n++){int x=296-n*5,y=225+n*14;box(x,y,42+n*11,10,Color{186,179,153,255});box(x+2,y+2,38+n*11,1,Color{211,200,173,255});}
    for(int x:{29,61,544,579,610}){box(x,208,12,14,Color{143,76,51,255});box(x-3,200,18,12,green);box(x+1,200,4,4,red);box(x+8,204,3,3,gold);}
    box(202,105,183,16,Color{46,69,56,255});border(202,105,183,16,Color{147,163,118,255});center("SITIO EM ATIBAIA",293,110,white);
    // Recognizable named figures remain between the main fighters, clear of the HUD.
    atibaiaGuest(false,270,251,t);atibaiaGuest(true,361,251,t);
    box(540,244,67,20,Color{46,51,52,255});box(547,236,43,13,Color{64,81,87,255});box(552,239,15,9,Color{117,156,166,255});box(573,239,13,9,Color{117,156,166,255});box(546,262,13,9,ink);box(587,262,13,9,ink);text("PF",573,250,gold);
    box(570,232,15,4,int(t*4)%2?red:Color{66,98,173,255});
}
inline void detailedBackdrop(int host,float t){
    if(host==1){atibaia(t);return;}
    if(host==0){
        skyBands(Color{66,99,131,255});
        // The old Three Powers scene returns with the Planalto facade as its focal point.
        box(20,112,14,69,Color{88,119,137,255});box(39,112,14,69,Color{96,129,145,255});
        for(int y=115;y<176;y+=6){box(22,y,10,2,Color{46,77,99,255});box(41,y,10,2,Color{46,77,99,255});}
        box(135,128,405,63,Color{53,83,106,255});box(122,119,430,8,white);box(130,190,418,7,white);
        for(int x=149;x<535;x+=45){box(x,126,5,63,white);for(int y=0;y<54;y++){int bend=int(std::sin(y/54.f*3.14159f)*12);box(x+4+bend,132+y,3,1,Color{205,218,216,255});}box(x+5,180,20,7,white);}
        box(265,147,136,18,ink);center("PALACIO DO PLANALTO",333,153,white);
        for(int n=0;n<8;n++)box(127-n*3,198+n*4,419+n*6,3,Color{176,180,169,255});
        line(383,167,470,227,white);line(383,170,470,230,white);line(393,167,480,227,white);
        box(0,228,640,17,Color{46,101,117,255});for(int n=0;n<34;n++)box((n*29+int(t*4))%640,232+n%3*4,16,1,Color{118,155,163,255});
        box(0,246,640,38,Color{147,139,118,255});brazilFlag(77,115,t);
        for(int side=0;side<2;side++)for(int n=0;n<7;n++){float u=std::fmod(t*.075f+n/7.f,1.f);int x=side?644-int(u*183):-4+int(u*176),hop=int(std::max(0.f,1-std::abs(u-.6f)/.13f)*12);plazaRunner(x,221-int(u*26)-hop,t*10+n,side?-1:1,n+side);}
        for(int n=0;n<13;n++){int x=int(std::fmod(n*51.f+t*27,700.f))-30;plazaRunner(x,272,t*10+n,n%2?-1:1,n);}
        for(int x:{109,482}){line(x,257,x+34,268,muted);line(x+3,255,x,270,muted);line(x+30,265,x+27,276,muted);}
    }else if(host==3){
        skyBands(Color{77,108,126,255});
        const char* banks[]={"ITAU","BRADESCO","BTG","SANTANDER"};int xs[]={8,139,270,503},widths[]={112,112,129,129};
        for(int b=0;b<4;b++){int x=xs[b],w=widths[b],top=66+(b%2)*19;box(x,top,w,142,Color{36,58,76,255});
            for(int col=0;col<w-8;col+=13)for(int y=top+5;y<179;y+=12){box(x+5+col,y,9,8,Color{Uint8(60+b*11),Uint8(101+(col%3)*13),Uint8(130+(y%4)*7),255});if((y+col+b)%5==0)box(x+5+col,y,9,2,Color{163,189,189,255});}
            int signY=b==1?113:142;box(x+5,signY,w-10,17,Color{15,25,41,255});center(banks[b],x+w/2,signY+6,b==0?gold:white);
            box(x+9,173,w-18,28,Color{25,39,50,255});for(int n=0;n<3;n++)box(x+17+n*27,177,20,20,Color{101,155,164,255});}
        box(250,162,155,13,ink);center("FARIA LIMA / MERCADO",328,166,gold);
        box(0,210,640,74,Color{126,132,135,255});for(int y=220;y<277;y+=15)box(0,y,640,1,Color{164,166,157,255});
        box(270,187,127,27,ink);text(int(t)%2?"COMBO +12%":"GUARDA +8%",278,192,green);text("BOLSA DO COMBO",278,203,white);
        for(int i=0;i<8;i++)banker(37+i*81,263,t,i);
        for(int n=0;n<5;n++){int x=274+n*24;box(x,233,20,13,Color{79,58,41,255});border(x+6,229,7,5,gold);}
        box(18,201,130,12,ink);center("CAFE / REUNIAO / BONUS",83,205,white);
        for(int n=0;n<3;n++){int x=420+n*15;box(x,230,5,15,Color{240,238,216,255});box(x-2,228,9,3,gold);}
    }else if(host==5){
        skyBands(Color{175,143,97,255});box(569,93,24,24,gold);
        for(int x=0;x<640;x+=5){int y=146-int(std::sin(x*.017f)*12);box(x,y,5,64,Color{135,155,78,255});}
        for(int row=0;row<4;row++)for(int x=0;x<640;x+=13){int y=175+row*8;line(x,y,x+3,y-10,Color{220,190,76,255});box(x,y-9,4,3,gold);}
        box(32,126,55,76,Color{147,157,148,255});for(int n=0;n<5;n++)box(34,131+n*13,51,2,white);for(int n=0;n<12;n++)box(32+n*2,126-n,55-n*4,1,muted);
        box(482,143,117,60,Color{145,52,43,255});for(int n=0;n<28;n++)box(473+n*2,143-n,135-n*4,1,Color{87,57,48,255});box(520,161,41,42,Color{54,49,39,255});line(520,162,561,202,white);line(561,162,520,202,white);
        box(271,115,134,21,Color{81,76,42,255});center("EXPO AGRO",338,121,gold,2);
        box(437,124,3,70,muted);for(int n=0;n<4;n++){float angle=t*.8f+n*1.5708f;line(439,125,439+int(std::cos(angle)*22),125+int(std::sin(angle)*22),white);}
        box(0,207,640,77,Color{140,107,67,255});tractor(307+int(std::sin(t*.45f)*24),193,t);cow(314,252,t);
        for(int x=0;x<640;x+=34){box(x,209,4,27,Color{83,60,39,255});}box(0,216,640,3,Color{185,150,97,255});box(0,228,640,3,Color{185,150,97,255});
        for(int i=0;i<6;i++)farmer(40+i*108,272,t,i);
        for(int n=0;n<3;n++){box(481+n*24,235,22,15,gold);border(481+n*24,235,22,15,Color{152,115,53,255});}
    }else if(host==2){
        skyBands(Color{60,88,108,255});box(0,137,640,82,Color{45,55,67,255});
        for(int x=8;x<640;x+=47){box(x,142,39,69,Color{67,77,87,255});box(x+3,149,33,2,muted);}
        box(199,108,188,44,ink);border(199,108,188,44,gold);center("CLUBE DO DEBATE",293,118,gold,2);center("MISSAO / MBL",293,138,white);
        box(23,151,93,18,ink);center("AO VIVO",70,156,red,2);box(506,149,114,28,ink);center("CORTES",563,154,white,2);center("DO PLENARIO",563,172,gold);
        box(0,219,640,65,Color{85,90,96,255});box(226,236,189,6,Color{25,32,44,255});box(240,242,8,25,ink);box(393,242,8,25,ink);
        guest(0,271,223,t);guest(1,365,223,t);
        int turn=int(t/3)%2;box(205,134,176,15,ink);center(turn?"KIM: VAI, RENAN!":"ARTHUR: CORTA PRA MIM!",293,138,white);
        for(int side=0;side<2;side++){int x=side?561:78;box(x-11,201,22,13,ink);box(x+(side?-17:11),203,8,7,muted);line(x,214,x-13,251,ink);line(x,214,x+13,251,ink);box(x+4,199,3,3,red);}
        for(int n=0;n<4;n++){int x=126+n*131;box(x,106,3,30,muted);box(x-11,134,25,9,gold);}
        for(int n=0;n<4;n++){int x=n<2?37+n*102:499+(n-2)*102;civilian(x,274,t,n%2?-1:1,n%3,n%2==int(t*2)%2,false,false);}
        factionFlag(2,188,147,t);factionFlag(2,440,147,t);
    }else if(host==7){
        skyBands(Color{68,125,147,255});box(0,141,640,91,Color{36,99,131,255});
        for(int n=0;n<80;n++)box((n*47+int(t*10))%640,147+n%11*7,12+n%13,1,Color{99,157,175,255});
        int bob=int(std::sin(t*1.6f)*2);for(int y=0;y<20;y++)box(220+y*2,181+y+bob,275-y*4,1,Color{225,226,214,255});
        box(263,155+bob,153,26,white);box(293,139+bob,89,16,white);box(321,121+bob,3,18,muted);line(322,123+bob,352,134+bob,white);
        for(int n=0;n<6;n++)box(274+n*22,160+bob,14,10,Color{43,81,114,255});
        for(int n=0;n<3;n++)box(302+n*23,143+bob,14,7,Color{43,81,114,255});
        center("IATE DO COMBO",356,185+bob,Color{49,75,106,255});factionFlag(7,443,147+bob,t);
        box(10,127,131,80,Color{52,58,88,255});box(15,132,121,19,ink);center("GAME OVER TV",76,138,gold);box(19,157,49,30,ink);box(23,161,41,22,Color{75,122,151,255});
        box(26+int(t*9)%30,174,5,6,gold);box(48,174,5,6,red);box(77,160,51,25,ink);text("REC",81,165,red);text("GAMES",81,177,white);
        box(55,111,4,17,muted);line(56,112,38,101,white);line(57,112,77,98,white);
        box(0,232,640,52,Color{111,83,61,255});for(int y=237;y<284;y+=8)box(0,y,640,1,Color{73,58,46,255});for(int x=0;x<640;x+=34)box(x,233,1,48,Color{158,123,81,255});
        for(int x=163;x<630;x+=68){box(x,212,5,35,Color{96,64,41,255});box(x-2,211,9,4,gold);}line(164,219,639,219,Color{202,181,139,255});
        arcadeCabinet(22,226,t);arcadeCabinet(54,226,t+2);arcadeCabinet(526,226,t+4);arcadeCabinet(558,226,t+5);
        for(int i=0;i<4;i++){int x=170+i*107;civilian(x,275,t,i%2?-1:1,2+i%2,int(t*2+i)%2,false,false);box(x-9,238,18,4,white);box(x-5,234,10,5,white);}
        for(int side=0;side<2;side++){int x=side?604:184;box(x,144,6,61,Color{123,90,48,255});for(int n=0;n<5;n++)line(x+3,145,x-25+n*13,128+int(std::sin(t+n)*3),green);}
    }
}
inline void detailedFloor(int host){
    Color ground=host==1?Color{75,77,61,255}:host==5?Color{80,61,43,255}:host==7?Color{70,54,45,255}:host==0?Color{62,72,78,255}:Color{34,45,59,255};
    Color seams=host==1?Color{116,117,91,255}:host==5?Color{119,91,55,255}:host==7?Color{110,83,57,255}:Color{92,104,109,255};
    box(0,278,640,82,ground);box(0,278,640,3,seams);
    for(int y=295;y<360;y+=host==7?11:19)box(0,y,640,1,seams);
    for(int x=-160;x<850;x+=64)line(320+(x-320)/2,281,x,360,seams);
}
}
