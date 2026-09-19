#pragma once
#include "presentation.h"
#include <fstream>
#include <array>
namespace pf {
struct NewsChapter {int chronology;const char *date,*title,*news;int player,opponent;const char *source,*url;int scene;};
inline constexpr NewsChapter NewsChapters[]={
{20141026,"26/10/2014","DILMA REELEITA","Dilma Rousseff vence Aecio Neves no segundo turno. A chapa com Michel Temer e reeleita. No arcade, a dupla da chapa se enfrenta em um duelo ficticio.",27,28,"TSE / ELEICAO DE 2014","https://www.tse.jus.br/comunicacao/noticias/2014/Outubro/presidente-do-tse-anuncia-que-dilma-rousseff-foi-reeleita-presidente-da-republica",0},
{20150623,"23/06/2015","SAUDACAO A MANDIOCA","No lancamento dos Jogos Mundiais dos Povos Indigenas, Dilma destaca a mandioca como alimento importante para a civilizacao. A fala repercute e inspira o especial ficticio da personagem.",27,1,"ESTADAO / UOL / FALA DE DILMA","https://noticias.uol.com.br/ultimas-noticias/agencia-estado/2015/06/23/dilma-sauda-mandioca-em-lancamento-de-jogos-indigenas.htm",4},
{20151202,"02/12/2015","O PROCESSO E ABERTO","Eduardo Cunha aceita um pedido de impeachment de Dilma na Camara. A abertura inicia um processo parlamentar; nessa data, a presidente ainda permanece no cargo.",27,28,"CAMARA / ABERTURA DO PROCESSO","https://www.camara.leg.br/noticias/477179-decisao-de-cunha-sobre-pedido-de-impeachment-de-dilma-deve-ser-lida-nesta-quinta/",2},
{20160417,"17/04/2016","A VOTACAO NA CAMARA","Por 367 votos a 137, a Camara autoriza a instauracao do processo de impeachment de Dilma. A decisao envia o caso ao Senado; ainda nao e a destituicao da presidente.",27,9,"CAMARA / VOTACAO DE 17 DE ABRIL","https://www.camara.leg.br/noticias/485947-camara-autoriza-instauracao-de-processo-de-impeachment-de-dilma-com-367-votos-a-favor-e-137-contra/",6},
{20160831,"31/08/2016","DILMA SAI, TEMER ASSUME","O Senado aprova o impeachment por 61 votos a 20, retirando o mandato de Dilma. Uma votacao separada preserva seus direitos politicos. Temer toma posse como presidente efetivo.",28,27,"SENADO / JULGAMENTO E POSSE","https://www12.senado.leg.br/noticias/materias/2016/08/31/dilma-rousseff-perde-o-mandato-e-temer-e-confirmado-presidente",6},
{20161215,"15/12/2016","O TETO DE GASTOS","O Congresso promulga a Emenda Constitucional 95, que estabelece limites para despesas primarias federais. A medida e um eixo do ajuste fiscal defendido pelo governo Temer.",28,1,"SENADO / EMENDA CONSTITUCIONAL 95","https://www12.senado.leg.br/noticias/materias/2016/12/15/promulgada-emenda-constitucional-do-teto-de-gastos",7},
{20170713,"13/07/2017","REFORMA TRABALHISTA","Temer sanciona a reforma trabalhista em cerimonia no Planalto. A lei altera mais de cem pontos da CLT e se torna um marco controverso de seu governo.",28,1,"FOLHA / SANCAO DA REFORMA","https://www1.folha.uol.com.br/mercado/2017/07/1900990-temer-sanciona-reforma-trabalhista-em-cerimonia-no-planalto.shtml",7},
{20180212,"12/02/2018","O VAMPIRO DA SAPUCAI","O desfile da Paraiso do Tuiuti representa Temer como um vampiro neoliberal, em critica a reformas de seu governo. A fantasia de carnaval inspira sua versao vampiro no arcade.",28,27,"CONGRESSO EM FOCO / CARNAVAL","https://www.congressoemfoco.com.br/noticia/33861/temer-e-retratado-como-vampiro-neoliberal-na-sapucai-imagem-constrange-apresentadores-da-globo",5},
{20181028,"28/10/2018","ASCENSAO ELEITORAL","Jair Bolsonaro vence o segundo turno contra Fernando Haddad e e eleito presidente. O resultado encerra a disputa eleitoral entre os dois candidatos.",9,8,"TSE / RESULTADO DE 2018","https://www.tse.jus.br/comunicacao/noticias/2018/Outubro/presidente-do-tse-anuncia-eleicao-de-jair-bolsonaro-para-presidente-da-republica",0},
{20200520,"20/05/2020","A POLEMICA DA CLOROQUINA","O Ministerio da Saude amplia o protocolo de cloroquina para casos leves de covid-19. A decisao do governo Bolsonaro gera contestacao no Senado.",9,1,"SENADO / PROTOCOLO E REACOES","https://www12.senado.leg.br/noticias/videos/2020/05/liberacao-de-cloroquina-para-tratamento-de-casos-leves-da-covid-19-repercute-entre-senadores",1},
{20210308,"08/03/2021","CONDENACOES ANULADAS","Edson Fachin anula decisoes da vara de Curitiba nas acoes contra Lula por incompetencia do juizo. A decisao nao julga o merito das acusacoes.",1,18,"STF / DECISAO DE FACHIN","https://portal.stf.jus.br/noticias/verNoticiaDetalhe.asp?idConteudo=461870&tip=UN",2},
{20211026,"26/10/2021","CPI DA PANDEMIA","A CPI aprova seu relatorio final, com pedidos de indiciamento, incluindo Bolsonaro. O documento e encaminhado a autoridades; nao e uma sentenca judicial.",9,6,"SENADO / RELATORIO DA CPI","https://www12.senado.leg.br/noticias/noticias/materias/2021/10/27/cpi-da-pandemia-entrega-relatorio-final-ao-presidente-rodrigo-pacheco",2},
{20220517,"17/05/2022","CASSACAO NA ALESP","A Assembleia de Sao Paulo aprova a perda do mandato de Arthur do Val por quebra de decoro parlamentar. A resolucao e publicada em 20 de maio.",11,10,"ALESP / RESOLUCAO 933","https://www.al.sp.gov.br/repositorio/legislacao/resolucao.alesp/2022/resolucao.alesp-933-20.05.2022.html",2},
{20221030,"30/10/2022","LULA RETORNA A PRESIDENCIA","Lula vence Jair Bolsonaro no segundo turno e e eleito para um terceiro mandato. O resultado eleitoral pertence a historia, independentemente de quem venca o duelo ficticio.",1,9,"TSE / RESULTADO DE 2022","https://www.tse.jus.br/comunicacao/noticias/2022/Outubro/lula-e-eleito-novamente-presidente-da-republica-do-brasil",0},
{20230108,"08/01/2023","ATAQUES AOS TRES PODERES","As sedes do Congresso, do Planalto e do STF sao invadidas e depredadas. O STF abre investigacoes sobre os atos e a eventual responsabilidade de autoridades.",6,9,"STF / INVESTIGACOES DO 8 DE JANEIRO","https://portal.stf.jus.br/noticias/verNoticiaDetalhe.asp?idConteudo=500529&ori=1",3},
{20230630,"30/06/2023","INELEGIBILIDADE","O TSE declara Bolsonaro inelegivel por oito anos por abuso de poder politico e uso indevido dos meios de comunicacao na reuniao com embaixadores de 2022.",9,6,"TSE / JULGAMENTO DOS EMBAIXADORES","https://www.tse.jus.br/comunicacao/noticias/2023/Junho/por-maioria-de-votos-tse-declara-bolsonaro-inelegivel-por-8-anos",2},
{20240222,"22/02/2024","DINO CHEGA AO SUPREMO","Flavio Dino toma posse no STF. A cronologia passa do Executivo ao Judiciario e apresenta o novo personagem do plenario.",26,22,"STF / COMPOSICAO E POSSES","https://portal.stf.jus.br/ostf/",2},
{20260101,"01/01/2026","NOVA REDUCAO DO IR","Passa a valer a reducao mensal do IR que zera o imposto para rendimentos tributaveis de ate R$ 5 mil. O beneficio diminui ate R$ 7.350. A mudanca marca a agenda economica do governo Lula.",8,1,"RECEITA FEDERAL / TABELAS DE 2026","https://www.gov.br/receitafederal/pt-br/assuntos/meu-imposto-de-renda/tabelas/2026",7},
{20260302,"02/03/2026","ELEICOES E INTELIGENCIA ARTIFICIAL","O TSE aprova o calendario eleitoral e atualiza as regras de inteligencia artificial na campanha. Nunes Marques relata as resolucoes. A disputa nas redes entra no arquivo de 2026.",23,10,"TSE / RESOLUCOES E IA","https://www.tse.jus.br/comunicacao/noticias/2026/Marco/tse-aprova-calendario-eleitoral-e-regulamenta-uso-de-ia-nas-eleicoes-2026",8},
{20260527,"27/05/2026","A DISPUTA DA ESCALA 6X1","A Camara aprova em primeiro turno a PEC para reduzir a jornada a 40 horas em cinco dias. A noticia registra uma etapa da tramitacao; nao significa que a mudanca ja esteja em vigor.",1,3,"CAMARA / VOTACAO EM PRIMEIRO TURNO","https://www.camara.leg.br/noticias/1277067-camara-aprova-fim-da-escala-6x1-com-jornada-maxima-de-40-horas-semanais",7},
{20260816,"16/08/2026","A CAMPANHA VAI AS RUAS","Comeca o periodo permitido para campanha eleitoral nas ruas e na internet. Comicios, material de campanha e propaganda virtual entram na disputa. O horario gratuito no radio e na TV comeca em 28 de agosto.",0,1,"AGENCIA BRASIL / INICIO DA CAMPANHA","https://agenciabrasil.ebc.com.br/politica/noticia/2026-08/campanha-eleitoral-nas-ruas-e-propaganda-virtual-comecam-neste-domingo",3},
{20260911,"11/09/2026","O NOVO ELENCO ELEITORAL","O TSE informa doze candidaturas validadas para a Presidencia. O jogo reune esses nomes e convidados da politica; ser jogavel nao significa ser candidato.",0,2,"TSE / CANDIDATURAS DE 2026","https://www.tse.jus.br/comunicacao/noticias/2026/Setembro/eleicoes-2026-tem-12-candidaturas-na-disputa-pela-presidencia-da-republica",0},
};
constexpr int StoryChapterCount=sizeof(NewsChapters)/sizeof(NewsChapters[0]);
struct Story {
    bool active=false,briefing=false,finished=false;int chapter=0,checkpoint=0;float time=0;std::string path;
    std::array<bool,StoryChapterCount> completed{};
    const NewsChapter &news()const{return NewsChapters[std::clamp(chapter,0,StoryChapterCount-1)];}
    static int indexOf(int date){for(int i=0;i<StoryChapterCount;i++)if(NewsChapters[i].chronology==date)return i;return -1;}
    int firstUnfinished()const{for(int i=0;i<StoryChapterCount;i++)if(!completed[i])return i;return StoryChapterCount;}
    int completedCount()const{return int(std::count(completed.begin(),completed.end(),true));}
    void load(){
        std::ifstream in(path);std::string tag;int version,n;
        if(!(in>>tag>>version>>n) || tag!="PF_STORY")return;
        if(version==1 && n>=0 && n<=10){
            // v18 stored an array offset: map it to its original date before inserting chapters.
            constexpr int legacy[]={20181028,20200520,20210308,20211026,20220517,20221030,20230108,20230630,20240222,20260911};
            completed.fill(false);for(int i=0;i<n;i++){int at=indexOf(legacy[i]);if(at>=0)completed[at]=true;}
            checkpoint=n<10?indexOf(legacy[n]):firstUnfinished();if(checkpoint<0)checkpoint=firstUnfinished();
        } else if(version==2){
            completed.fill(false);int date;while(in>>date){int at=indexOf(date);if(at>=0)completed[at]=true;}
            int at=indexOf(n);checkpoint=at>=0?at:firstUnfinished();
        }
    }
    void save(){if(!path.empty()){std::ofstream out(path);out<<"PF_STORY 2 "<<(checkpoint<StoryChapterCount?NewsChapters[checkpoint].chronology:0)<<"\n";for(int i=0;i<StoryChapterCount;i++)if(completed[i])out<<NewsChapters[i].chronology<<"\n";}}
    void start(){active=briefing=true;finished=false;chapter=checkpoint<StoryChapterCount?checkpoint:0;time=0;}
    void leave(){active=briefing=finished=false;time=0;}
    void browse(int direction){if(!active || !briefing || finished)return;chapter=(chapter+direction+StoryChapterCount)%StoryChapterCount;time=0;}
    void era(int direction){
        if(!active || !briefing || finished)return;
        int years[]={2014,2018,2026};int current=news().chronology<20180101?0:news().chronology<20260101?1:2;
        int year=years[(current+direction+3)%3];for(int i=0;i<StoryChapterCount;i++)if(NewsChapters[i].chronology/10000>=year){chapter=i;time=0;break;}
    }
    void victory(){if(!active || briefing || chapter>=StoryChapterCount)return;completed[chapter]=true;++chapter;checkpoint=chapter;save();time=0;briefing=true;finished=chapter>=StoryChapterCount;}
};
}
namespace art {
inline void storyScreen(const pf::Story &story){
    const auto &n=story.news();float t=story.time;
    box(0,0,640,360,ink);for(int y=40;y<190;y+=8)box(0,y,640,1,Color{34,46,59,255});
    text("ARQUIVO BRASIL",18,13,gold,2);text("HISTORIA / 2014 - 2026",288,18,muted);
    if(story.finished){
        cinema::rays(t,gold);center("FIM DO PERCURSO",320,92,white,3);
        center("2014 - 2026",320,139,gold,3);
        center(std::to_string(story.completedCount())+" / "+std::to_string(pf::StoryChapterCount)+" CAPITULOS CONCLUIDOS",320,184,gold);
        wrapped("VOCE CHEGOU A ULTIMA NOTICIA. VOLTE AO ARQUIVO PARA EXPLORAR OS OUTROS CAPITULOS. OS DUELOS SAO FICTICIOS; OS RESULTADOS HISTORICOS CONTINUAM OS MESMOS.",96,208,448,white);
        center("AS FONTES ESTAO EM FONTES-HISTORIA.MD",320,275,muted);
        center("ENTER / CONFIRMAR: MENU",320,329,gold);return;
    }
    text(n.date,554,18,white);
    SDL_Rect clip{18,48,130,130};SDL_RenderSetClipRect(r,&clip);drawPortrait(n.player,clip);SDL_RenderSetClipRect(r,nullptr);
    clip={492,48,130,130};SDL_RenderSetClipRect(r,&clip);drawPortrait(n.opponent,clip,true);SDL_RenderSetClipRect(r,nullptr);
    border(17,47,132,132,accent(n.player));border(491,47,132,132,accent(n.opponent));
    box(162,48,316,130,Color{23,34,51,255});
    for(int k=0;k<8;k++)box(183+k*35,105,15,53,muted);
    box(174,96,292,8,white);box(168,158,304,6,white);
    center(std::to_string(n.chronology/10000),320,57,gold,3);
    if(n.scene==0){box(276,108,88,44,ink);box(280,112,80,37,white);box(287,118,43,23,Color{51,93,76,255});for(int i=0;i<9;i++)box(335+(i%3)*7,117+(i/3)*8,4,4,ink);box(337,142,13,4,green);}
    else if(n.scene==1){pf::Wave w;w.x=320;w.y=129;w.kind=pf::ProjectileKind::Chloroquine;w.age=t;projectile(w);}
    else if(n.scene==2){pf::Wave w;w.x=320;w.y=127;w.kind=pf::ProjectileKind::Book;w.character=n.opponent;w.age=t;projectile(w);}
    else if(n.scene==4){
        box(164,96,312,71,Color{34,61,46,255});box(164,147,312,20,Color{113,73,43,255});
        for(int i=0;i<8;i++){int x=179+i*39;line(x,148,x+int(std::sin(t*2+i)*3),119,green);for(int leaf=-1;leaf<=1;leaf++)line(x,132,x+leaf*13,117+std::abs(leaf)*6,green);}
        pf::Wave w;w.x=320;w.y=126;w.age=t;w.kind=pf::ProjectileKind::Cassava;projectile(w);
    } else if(n.scene==5){
        box(164,96,312,71,Color{32,20,53,255});box(413,101,24,24,gold);box(424,98,18,22,Color{32,20,53,255});
        for(int i=0;i<20;i++){int x=173+i*15,y=143+int(std::sin(t*5+i)*3);box(x,y,8,17,i%2?red:gold);box(x+1,y-6,6,6,white);}
        for(int i=0;i<3;i++){pf::Wave w;w.x=233+i*86;w.y=119+std::sin(t*3+i)*6;w.age=t+i;w.kind=pf::ProjectileKind::Bat;projectile(w);}
    } else if(n.scene==6){
        box(250,107,140,43,ink);center(n.chronology==20160417?"367 X 137":"61 X 20",320,117,gold,2);
        center("VOTACAO",320,139,white);for(int i=0;i<12;i++)box(181+i*24,155,12,7,i<8?green:red);
    } else if(n.scene==7){
        box(259,106,122,46,ink);center(n.chronology==20260527?"6 X 1":n.chronology==20260101?"IR 2026":"CLT / EC 95",320,111,gold);
        for(int i=0;i<7;i++){int h=9+(i*13)%24;box(272+i*13,147-h,8,h,i%2?green:red);}
    } else if(n.scene==8){
        box(272,104,96,49,ink);box(277,109,86,37,Color{39,72,85,255});center("IA / ELEICOES",320,116,white);
        for(int i=0;i<5;i++){box(285+i*14,134,8,3,int(t*5+i)%2?gold:green);}
        box(315,153,10,8,ink);box(296,161,48,3,ink);
    } else {for(int i=0;i<10;i++){int x=194+i*26,y=158+int(std::sin(t*5+i)*2);box(x,y-13,9,14,i%2?gold:green);box(x+1,y-20,7,7,Color{198,146,112,255});}for(int i=0;i<5;i++)line(204+i*47,98,222+i*47,151,ink);}
    box(162,168,316,10,ink);center(int(t*2)%2?"ARQUIVO / RECONSTITUICAO ILUSTRADA":"ARQUIVO / NOTICIA DOCUMENTADA",320,170,muted);
    center("CAPITULO "+std::to_string(story.chapter+1)+" / "+std::to_string(pf::StoryChapterCount)+(story.completed[story.chapter]?" / CONCLUIDO":""),320,190,gold);
    center(n.title,320,207,white,2);
    wrapped(n.news,24,235,592,white);
    text(n.source,24,283,muted);text("F6: ABRIR FONTE",494,283,gold);
    box(18,298,604,20,Color{48,39,37,255});center(std::string("DUELO FICTICIO / VOCE JOGA COMO ")+name(n.player),320,304,gold);
    center("CIMA/BAIXO: CAPITULO   ESQ/DIR: EPOCA   ENTER: LUTAR",320,326,white);
    center("ESC: MENU   /   PROGRESSO SALVO APOS CADA VITORIA",320,344,muted);
}
}
