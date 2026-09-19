#pragma once
#include <algorithm>
#include <cstdint>
namespace pf {
constexpr int CharacterCount=29, BossCharacter=CharacterCount, CastBackground=BossCharacter+1;
constexpr int Jair=9, Kim=10, Arthur=11, Fachin=18, Dilma=27, Temer=28;
enum class ProjectileKind {MoneyBag,Chocolate,Banknotes,PTStar,Mushroom,Sale,Drone,Lasso,Gavel,Gamepad,Tax,BossCard,Chloroquine,Microphone,Audio,Pepper,Ballot,Book,Paw,Scales,Clock,Scroll,Dinosaur,Cassava,Bat};
struct CharacterInfo {
    const char *name,*shortName,*ultimate;ProjectileKind kind;const char *faction;int stage;
    uint32_t accent,hair,suit,skin;int look; // beard=1, glasses=2, bald=4, long hair=8
};
inline constexpr CharacterInfo Roster[]={
{"FLAVIO BOLSONARO","FLAVIO","CINEMA MASTER",ProjectileKind::Chocolate,"PL",0,0x64dfae,0x3a2c2b,0x224151,0xe3ae8e,0},
{"LULA","LULA","FILA SEM FIM",ProjectileKind::PTStar,"PT",1,0xfa6e70,0xd2d6cd,0x35435d,0xdea885,1},
{"RENAN SANTOS","RENAN","REINO DOS COGUMELOS",ProjectileKind::Mushroom,"MIS",2,0x6bb8ff,0x282020,0x243644,0xdea885,1},
{"ROMEU ZEMA","ZEMA","ESCALA SEM FIM",ProjectileKind::Sale,"NOVO",3,0xffb052,0x909291,0x789497,0xdea885,2},
{"AUGUSTO CURY","A. CURY","ESQUECE O DRONE",ProjectileKind::Drone,"AV",4,0xba8ef4,0x7c7168,0x42404f,0xdea885,2},
{"RONALDO CAIADO","CAIADO","ANISTIA NO LACO",ProjectileKind::Lasso,"PSD",5,0xbfd483,0xe0dfd0,0x9cafb4,0xdea885,0},
{"ALEXANDRE DE MORAES","A. MORAES","CANETADA SUPREMA",ProjectileKind::Gavel,"STF",6,0xdac997,0xdbab8c,0x1d202b,0xdea885,4},
{"LULINHA","LULINHA","SIGILO EM JOGO",ProjectileKind::Gamepad,"IATE",7,0x71dbdb,0x392820,0x96333c,0xdea885,1},
{"FERNANDO HADDAD","HADDAD","TAXA DAS BLUSINHAS",ProjectileKind::Tax,"PT",8,0xf28ba1,0x4a413d,0x3d4d67,0xdea885,0},
{"JAIR BOLSONARO","JAIR","CHUVA DE CLOROQUINA",ProjectileKind::Chloroquine,"PL",0,0x55df70,0x73634e,0x253b60,0xe3ae8e,0},
{"KIM KATAGUIRI","KIM","DEBATE SEM FIM",ProjectileKind::Microphone,"MIS",2,0x579fea,0x191e28,0x303c53,0xe3b18b,2},
{"ARTHUR DO VAL","ARTHUR","AUDIO VAZADO",ProjectileKind::Audio,"MIS",2,0xef9740,0x362d27,0x353638,0xd9a07c,1},
{"RUI COSTA PIMENTA","RUI PIMENTA","RAJADA DE PIMENTA",ProjectileKind::Pepper,"PCO",1,0xdb5155,0xdbd7ce,0x6a3444,0xd9a580,2},
{"SAMARA MARTINS","SAMARA","ONDA POPULAR",ProjectileKind::Ballot,"UP",1,0xe76066,0x262124,0xa8334a,0x9c6445,8},
{"HERTZ DIAS","HERTZ","BATIDA REVOLUCIONARIA",ProjectileKind::Microphone,"PSTU",1,0xf36b56,0x28211e,0x792e36,0x925b40,1},
{"EDMILSON COSTA","EDMILSON","CAPITAL EM COLAPSO",ProjectileKind::Book,"PCB",8,0xd94649,0xe3ded2,0x49435a,0xc58c65,2},
{"WILSON GRASSI","GRASSI","RESGATE ANIMAL",ProjectileKind::Paw,"DCM",3,0x73cc99,0x695b4c,0x4b776c,0xdca67d,0},
{"CLARIANA BARAO","CLARIANA","VOZ DA TRIBUNA",ProjectileKind::Microphone,"DC",6,0x9494e9,0x594030,0x735483,0xd8a480,8},
{"EDSON FACHIN","FACHIN","RELATORIA INFINITA",ProjectileKind::Book,"STF",6,0x92b6d2,0xdfd9c9,0x242631,0xe0b798,2},
{"GILMAR MENDES","GILMAR","HABEAS CORPUS",ProjectileKind::Scales,"STF",6,0xd3af67,0xa2a19a,0x22242e,0xe0b191,6},
{"CARMEN LUCIA","CARMEN","SESSAO SEM SILENCIO",ProjectileKind::Microphone,"STF",6,0xac83be,0x97877c,0x242432,0xdbac8c,8},
{"DIAS TOFFOLI","TOFFOLI","VISTA INTERMINAVEL",ProjectileKind::Clock,"STF",6,0x87c3b2,0x64574e,0x262736,0xe1b496,2},
{"LUIZ FUX","FUX","LIMINAR TURBO",ProjectileKind::Scroll,"STF",6,0xd29d77,0x4b443e,0x282731,0xd5a781,0},
{"NUNES MARQUES","NUNES","VISTA RELAMPAGO",ProjectileKind::Clock,"STF",6,0x7ebfc6,0x48413b,0x242830,0xd5a17e,0},
{"ANDRE MENDONCA","MENDONCA","VOTO SOLENE",ProjectileKind::Book,"STF",6,0x7b92cd,0x5b5149,0x252735,0xddab86,2},
{"CRISTIANO ZANIN","ZANIN","DEFESA TECNICA",ProjectileKind::Scales,"STF",6,0x88ba92,0x493c32,0x292836,0xdeab87,0},
{"FLAVIO DINO","DINO","DINOSSAURO DO PLENARIO",ProjectileKind::Dinosaur,"STF",6,0xc8a25f,0x3e3430,0x2a2732,0xcf9a73,2},
{"DILMA ROUSSEFF","DILMA","SAUDAR A MANDIOCA",ProjectileKind::Cassava,"PT",0,0xf06c65,0x743a24,0xa32c40,0xe0ab87,0},
{"MICHEL TEMER","TEMER","VAMPIRO DO PLANALTO",ProjectileKind::Bat,"MDB",0,0xbb88e7,0xc9c8c5,0x252335,0xd7c5b8,0},
{"DANIEL VORCARO","VORCARO","CONTRATO MASTER",ProjectileKind::BossCard,"BOSS",6,0xffcb64,0x251a19,0x1e1a24,0xdea885,1},
};
static_assert(sizeof(Roster)/sizeof(Roster[0])==CharacterCount+1);
inline const CharacterInfo &characterInfo(int id){return Roster[std::clamp(id,0,BossCharacter)];}
inline constexpr int PresidentialRoster[]={0,1,2,3,4,5,12,13,14,15,16,17};
inline bool presidential(int id){for(int c:PresidentialRoster)if(c==id)return true;return false;}
inline const char *category(int id){return id==6 || (id>=18 && id<=26)?"STF":presidential(id)?"PRESIDENCIA 2026":id==Dilma || id==Temer?"ARQUIVO 2014 - 2018":"FIGURA POLITICA";}
}
