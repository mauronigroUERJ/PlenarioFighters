# Plenário Fighter — versão 18

Jogo de luta satírico em C++17, usando exclusivamente SDL2. Execute `PlenarioFighter.exe`. A SDL2, os retratos, a música e os efeitos sintetizados estão incorporados ao executável; não é necessária SDL2.dll ao lado dele.

## Elenco e seleção

São **27 personagens selecionáveis**, distribuídos em três páginas de nove. Cima/baixo percorrem todo o elenco; a página acompanha o personagem selecionado. Esquerda/direita mudam a opção inferior. Tab ou quadrado/X alternam o lado que está sendo escolhido.

As seis opções inferiores são **1 jogador**, **2 jogadores**, **Controles**, **Torneio**, **CPU** e **História**. O filtro CRT é parte permanente da apresentação: não há opção TV nem atalho F4 para desligá-lo. Configurações antigas de filtro desligado são ignoradas.

O elenco anterior foi preservado: Flávio Bolsonaro, Lula, Renan Santos, Romeu Zema, Augusto Cury, Ronaldo Caiado, Alexandre de Moraes, Lulinha e Fernando Haddad.

Novos convidados: **Jair Bolsonaro, Kim Kataguiri e Arthur do Val**. Jair lança uma sequência animada de frascos e cápsulas no especial **Chuva de Cloroquina**. Kim usa **Debate sem Fim**, com microfones e ondas; Arthur usa **Áudio Vazado**, com celulares e formas de onda.

Outros nomes da lista presidencial consultada no TSE: **Rui Costa Pimenta, Samara Martins, Hertz Dias, Edmilson Costa, Wilson Grassi e Clariana Barão**. Os especiais remetem a pimenta, urna, microfone, livros e causa animal.

Completam o grupo do STF: **Edson Fachin, Gilmar Mendes, Cármen Lúcia, Dias Toffoli, Luiz Fux, Nunes Marques, André Mendonça, Cristiano Zanin e Flávio Dino**. Seus especiais incluem Relatoria Infinita, Habeas Corpus, Sessão sem Silêncio, Vista Interminável, Liminar Turbo, Vista Relâmpago, Voto Solene, Defesa Técnica e Dinossauro do Plenário.

Todos os **28 personagens, contando o chefe**, possuem retratos pixelizados na seleção/apresentação correspondente. Vorcaro permanece exclusivo da final do torneio, além da invocação temporária de Flávio. O ator jogável não precisa ser candidato: o menu distingue Presidência 2026, figuras políticas e STF. A base e a data da consulta estão em `FONTES-HISTORIA.md`.

## Reações e defesa

O tipo do golpe determina a reação: jab, gancho/direto, uppercut, chute no corpo, chute baixo, chute aéreo, malote ou ultimate. Mudam o movimento da cabeça e do tronco, a posição dos braços e, no chute baixo, a perna atingida. A reação desaparece gradualmente.

Uma defesa bem-sucedida gera um estouro local de charge com pontas, partículas, a palavra **BLOQ!** e recuo dos braços. O efeito vale para defesa alta e baixa; defender na altura errada produz reação de dor. Receber dano não faz o cenário piscar. Sons de esforço, impacto, dor e ultimate continuam misturados em canais independentes.

## História — Arquivo Brasil, volume 1

Escolha **História** para entrar em uma cronologia inicial de dez capítulos:

1. 28/10/2018 — eleição de Jair Bolsonaro contra Haddad.
2. 20/05/2020 — protocolo e controvérsia da cloroquina.
3. 08/03/2021 — anulação de decisões contra Lula por incompetência do juízo de Curitiba.
4. 26/10/2021 — aprovação do relatório da CPI da Pandemia.
5. 17/05/2022 — aprovação da cassação de Arthur do Val.
6. 30/10/2022 — eleição de Lula contra Jair Bolsonaro.
7. 08/01/2023 — ataques às sedes dos Três Poderes.
8. 30/06/2023 — decisão de inelegibilidade de Bolsonaro.
9. 22/02/2024 — posse de Flávio Dino no STF.
10. 11/09/2026 — candidaturas presidenciais validadas pelo TSE.

Cada notícia tem data, ilustração animada, resumo e identificação da fonte. **F6** abre a página da fonte no navegador. **Enter/espaço/confirmar** inicia o duelo com o personagem indicado. Ao vencer, confirmar abre a próxima notícia; ao perder, confirmar repete o capítulo. Esc/voltar retorna ao menu. O progresso concluído fica em `story.cfg`, permitindo retomar mais tarde. Há uma tela final do volume; entrar novamente após concluir reinicia a cronologia.

As notícias são resumos de registros públicos; os diálogos, poderes e lutas são ficção satírica. Ganhar uma luta não reescreve o resultado histórico. Trata-se do primeiro volume, com acontecimentos selecionados, não de uma cobertura exaustiva de todas as polêmicas.

## Modos e recursos mantidos

- **Dois jogadores:** escolha a opção no menu, atribua dispositivos no lobby e confirme Iniciar luta. Teclado + controle ou dois controles independentes.
- **Torneio:** rivais do grupo dos 12 candidatos, sem repetição e sem enfrentar a si mesmo. Um candidato enfrenta 11 rivais; um convidado enfrenta 12. Depois vem Vorcaro. Há bônus de conversão a cada dois rivais derrotados; o vencedor ganha dois malotes adicionais somente na próxima partida.
- **CPU:** fácil/médio/difícil; alterar pela opção CPU ou F3 no menu.
- **Combate:** três socos ou chutes encadeados; terceiro golpe mais forte; ataques agachados e aéreos; malotes limitados e exibidos no HUD; defesa segurando para trás ou baixo+trás; agarrão com puxão, cura, arremesso e um segundo de imobilização após cair.
- **Chefe:** Vorcaro possui 180 de vida, corpo 25% maior, velocidade de 280 px/s e dano elevado. A invocação de Flávio continua mais fraca e dura cinco segundos. Lula mantém a evolução Antigo Testamento.
- **Cenários:** nove ambientes temáticos, incluindo Atibaia, Planalto, Faria Lima, agro, clube do debate e marina. Os novos personagens compartilham cenários adequados à sua categoria. Helicóptero, repórter e figurantes animados continuam presentes.
- **Arcade:** selo, patrocínios fictícios, abertura de 27 segundos, recordes, menu e demonstrações CPU contra CPU após 15 segundos sem entrada. Um comando encerra a demonstração. Música chiptune e efeito CRT são nativos.

## Teclado padrão e controles

| Ação | P1 | P2 |
|---|---|---|
| Andar | A/D | Esquerda/direita |
| Pular/agachar | W/S | Cima/baixo |
| Soco/chute | J/K | Num 1/Num 2 |
| Malote | I | Num 3 |
| Agarrar | O | Decimal numérico |
| Ultimate | U | Num 0 |
| Defesa auxiliar | L | Shift direito |
| Pausa | P | Enter numérico |

F1 abre o remapeamento; F2 redetecta dispositivos; F3 nas configurações inicia a calibração PS2. F5 no menu abre recordes, F11 alterna tela cheia e M alterna som. Consulte `CONTROLES.md` para DualShock, genéricos, PS2 e Parsec. Os arquivos de configuração anteriores continuam compatíveis.

## Código e construção

`build.ps1 -Test` compila com MinGW-w64 e roda as verificações. Há também CMake/CTest. A única biblioteca de jogo é a SDL2 estática em `vendor`.

- `src/roster.h`: elenco, especiais, cores, aparência, cenário e participantes do torneio.
- `src/combat.h`: combate, tipos de reação, bloqueios e projéteis.
- `src/pixel.h`: desenho SDL2 dos corpos, reações, fonte e especiais.
- `src/story.h`: notícias, datas, fontes, progresso e apresentação da história.
- `src/main.cpp`: menu, input, cenas, efeitos e integração dos modos.
- `src/cinematics.h` e `src/presentation.h`: retratos, abertura, epílogos e apresentação antes das lutas.
- `src/expansion_tests.h`: cobertura das novidades e capturas `--capture-v18`.

`ARTE-V18.md` registra os prompts e a ferramenta dos novos retratos. `VALIDACAO.md` descreve exatamente o que foi executado e a limitação imposta pelo Smart App Control na última compilação.
