# Áudio e falas dos ultimates

A versão 13 já contém sons de golpes, esforço, dor e ultimates sintetizados pelo próprio jogo em SDL2. Não é preciso baixar arquivos para ouvir esses efeitos. São vocalizações originais de arcade, sem simular vozes reais. Os WAVs opcionais abaixo substituem apenas a vocalização inicial do ultimate.


Esta versão tem reprodução e mistura de falas em SDL2, mas **não inclui gravações dos políticos**. A pesquisa encontrou memes, porém as tentativas de baixar a página e o MP3 retornaram HTTP 403. Nenhuma voz artificial foi colocada no lugar.

Para adicionar um trecho, coloque um WAV de até 10 segundos nesta pasta e reinicie o jogo. Use preferencialmente PCM 16 bits, mono, 44.100 Hz; o carregador também converte formatos WAV aceitos por SDL_LoadWAV. MP3 não é lido diretamente. Arquivos ausentes ou inválidos preservam os efeitos sonoros normais.

| Personagem | Arquivo |
|---|---|
| Flávio Bolsonaro | flavio.wav |
| Lula | lula.wav |
| Renan Santos | renan.wav |
| Zema | zema.wav |
| Augusto Cury | cury.wav |
| Caiado | caiado.wav |
| Alexandre de Moraes | moraes.wav |
| Lulinha | lulinha.wav |
| Haddad | haddad.wav |
| Vorcaro | vorcaro.wav |

A fala toca uma vez na ativação do ultimate. P1 e P2 têm canais separados; golpes e rajadas não interrompem a gravação. M silencia e limpa os canais. Iniciar outra partida também limpa as falas. As falas já iniciadas terminam normalmente ao pausar a luta.

Pesquisa em 12/09/2026:
- [LULA diz: Chora não companheiro — Myinstants](https://www.myinstants.com/pt/instant/lula-diz-chora-nao-companheiro-92545/): página com botão de download; arquivo MP3 retornou 403. A atribuição é do usuário que publicou; o áudio não foi obtido nem autenticado.
- [Busca Haddad](https://www.myinstants.com/en/search/?name=Haddad) e [busca Alexandre de Moraes](https://www.myinstants.com/en/search/?name=alexandre+de+moraes): resultados de soundboard misturam falas e paródias; nenhum trecho foi incorporado.

O código do carregador e mixer está em `src/voices.h`. O jogo continua exclusivamente SDL2, sem SDL_mixer e sem DLL adicional.
