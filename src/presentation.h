#pragma once
#include "cinematics.h"

namespace pf {
constexpr float VersusDuration=6.5f;
struct Banter {int a,b;const char *first,*second;};
// Original fictional dialogue, never recordings or purported quotations.
inline std::string duelLine(int speaker,int opponent){
    static const Banter pairs[]={
        {Dilma,Temer,"HOJE EU VOU SAUDAR A MANDIOCA!","EU PREFIRO UM COMBO DEPOIS DO POR DO SOL."},
        {Dilma,1,"COMPANHEIRO, A RAIZ DESTE COMBO E FORTE.","ENTAO VAMOS CAPRICHAR NA COLHEITA."},
        {Temer,9,"ESTA FAIXA COMBINA COM A MINHA CAPA.","QUERO VER ESSA CAPA SEGURAR MEU ULTIMATE."},
        {9,1,"TROUXE UM ESTOQUE PARA O ULTIMATE.","ENTAO PREPARE O ESTOQUE DE DEFESA."},
        {9,0,"HOJE O RECHEIO DO COMBO E DIFERENTE.","SO NAO CONFUNDA MEUS CHOCOLATES."},
        {10,11,"O DEBATE TEM TEMPO. O COMBO TAMBEM.","ESPERA SO EU TERMINAR ESTE AUDIO."},
        {10,2,"A MISSAO HOJE E TESTAR SUA GUARDA.","CUIDADO COM O REINO DOS COGUMELOS."},
        {18,1,"ESTA RELATORIA TEM TRES SOCOS.","E MINHA RESPOSTA TEM TRES CHUTES."},
        {26,22,"MEU DINOSSAURO NAO ACEITA LIMINAR.","ENTAO VOU PEDIR PRIORIDADE NO COMBO."},
        {0,1,"TROUXE CHOCOLATE. O RECHEIO E SURPRESA.","PODE GUARDAR. HOJE EU VIM DE ESTRELA."},
        {0,2,"ESSE COGUMELO PASSOU NA ALFANDEGA?","PRIMEIRO PASSE PELA MINHA GUARDA."},
        {0,4,"SEU DRONE ENTREGA CHOCOLATE?","ENTREGA. MAS HOJE O FRETE E UM ULTIMATE."},
        {0,6,"QUERO REVISAR AS REGRAS DESTE ROUND.","PEDIDO NEGADO. LEVANTE A GUARDA."},
        {1,2,"ESSE COGUMELO VAI PARA O CHURRASCO?","SO SE VOCE AGUENTAR O TEMPERAMENTO."},
        {1,3,"VOCE VAI PRIVATIZAR A BARRA DE VIDA?","PRIMEIRO VOU CORTAR SEU TEMPO DE REACAO."},
        {1,7,"MENINO, ESTACIONOU O IATE ONDE?","NA FINAL. SO FALTA VOCE CHEGAR LA."},
        {1,8,"HOJE NAO TEM TAXA SOBRE O CHURRASCO.","MAS O COMBO VEM COM ACRESCIMO."},
        {2,4,"COGUMELO NAO PRECISA DE CONTROLE REMOTO.","E DRONE NAO PRECISA DE ADUBO."},
        {2,3,"MINHA MISSAO E CHEGAR NA FINAL.","ENTAO VAI PRECISAR DE UM PLANO DE METAS."},
        {3,5,"VAMOS RESOLVER ISSO SEM DESPERDICIO.","OTIMO. VOU USAR UM LACO SO."},
        {3,8,"SE TAXAR MEU SOCO, EU CORTO O CUSTO.","PODE CORTAR. O CHUTE TEM OUTRA ALIQUOTA."},
        {4,5,"MEUS DRONES NAO PEGAM FILA.","QUERO VER PASSAREM POR ESTE LACO."},
        {4,7,"SEU IATE TEM PISTA PARA DRONES?","TEM. MAS A TAXA DE POUSO E UM COMBO."},
        {5,6,"AQUI O LACO E CURTO.","E O PRAZO PARA RECORRER TAMBEM."},
        {6,8,"O ROUND NAO ACEITA RECURSO INFINITO.","MAS ACEITA PARCELAR A DERROTA?"},
        {7,8,"NEM PENSE EM TAXAR MEU IATE.","JA SEPAREI UMA GUIA PARA CADA DEQUE."},
    };
    if(speaker==opponent)return "ATE MEU ESPELHO QUER A MINHA VAGA!";
    for(auto &p:pairs){if(speaker==p.a&&opponent==p.b)return p.first;if(speaker==p.b&&opponent==p.a)return p.second;}
    static const char* againstBoss[]={"O REFORCO VIROU ADVERSARIO?","HOJE A CONTA CHEGOU AO PLENARIO.","MINHA MISSAO TERMINA NESTA FINAL.","VOU AUDITAR ESSA BARRA DE VIDA.","MEUS DRONES JA LOCALIZARAM O CHEFE.","ESSE CONTRATO NAO ESCAPA DO LACO.","A SESSAO FINAL ESTA ABERTA.","SO CABE UM CAPITAO NESTE IATE.","VIM COBRAR O ULTIMO ROUND."};
    if(opponent==BossCharacter)return speaker<9?againstBoss[std::clamp(speaker,0,8)]:"HOJE SEU CONTRATO NAO PASSA PELA MINHA GUARDA.";
    if(speaker==BossCharacter)return "AMIZADE E AMIZADE. A FINAL E OUTRO CONTRATO.";
    static const char* defaults[]={"MINHA CAMPANHA COMECA COM UM COMBO.","COMPANHEIRO, HOJE O DEBATE E NO SOCO.","MISSAO DO DIA: VENCER ESTE ROUND.","MENOS DISCURSO. MAIS RESULTADO.","RESPIRE FUNDO. O DRONE JA ESTA NO AR.","SEGURE FIRME. O LACO VAI APERTAR.","DECLARO ABERTA A SESSAO DE COMBATE.","O IATE ESPERA. PRIMEIRO EU VOU VENCER.","SE PREPARE PARA O AJUSTE DO COMBO."};
    return speaker<9?defaults[std::clamp(speaker,0,8)]:std::string("PREPARE-SE PARA ")+characterInfo(speaker).ultimate+"!";
}
}
namespace art {
inline void wrapped(const std::string &message,int x,int y,int width,Color color,int scale=1){
    std::string lineText,word;int row=0;const int maxChars=width/(6*scale);
    auto flush=[&](){text(lineText,x,y+row*11*scale,color,scale);lineText.clear();row++;};
    for(size_t n=0;n<=message.size();n++){
        if(n==message.size() || message[n]==' '){if(int(lineText.size()+word.size()+(!lineText.empty()))>maxChars)flush();if(!lineText.empty())lineText+=' ';lineText+=word;word.clear();}
        else word+=message[n];
    }
    if(!lineText.empty())flush();
}
inline void versusScreen(const pf::Match&m,float t){
    box(0,0,640,360,ink);Color a=accent(m.f[0].character),b=accent(m.f[1].character);
    box(0,0,320,360,Color{Uint8(a.r/7),Uint8(a.g/7),Uint8(a.b/7+12),255});
    box(320,0,320,360,Color{Uint8(b.r/7+10),Uint8(b.g/7),Uint8(b.b/7),255});
    cinema::rays(t*3,a,175,138);cinema::rays(-t*3,b,470,138);
    float ease=cinema::ease(t/.65f);
    for(int i=0;i<2;i++){
        int id=m.f[i].character,x=(i?365:35)+(i?1:-1)*int((1-ease)*330);Color c=i?b:a;
        box(x-3,46,246,198,ink);border(x-3,46,246,198,c);
        SDL_Rect clip{x,49,240,191};SDL_RenderSetClipRect(r,&clip);
        drawPortrait(id,SDL_Rect{x,49,240,240},i!=0);
        SDL_RenderSetClipRect(r,nullptr);
        box(x,218,240,22,ink);center(name(id),x+120,225,white,1);
        bool active=i==0?t>=.65f:t>=3.f;
        box(i?330:12,251,298,72,Color{8,13,22,240});border(i?330:12,251,298,72,active?c:muted);
        text(i?"P2 / RESPOSTA":"P1 / DESAFIO",i?340:22,259,c);
        if(active)wrapped(pf::duelLine(id,m.f[1-i].character),i?340:22,277,276,white);
        else text("...",i?340:22,280,muted);
    }
    box(292,121,56,60,ink);center("VS",320,140,gold,3);
    center(m.bossFight?"O ULTIMO CONTRATO":"ANTES DO PRIMEIRO ROUND",320,15,gold,2);
    center("FALAS FICTICIAS / SATIRA    ENTER OU CONFIRMAR: PULAR",320,341,muted);
    box(14,330,int(std::clamp(t/pf::VersusDuration,0.f,1.f)*612),2,gold);
}
}
