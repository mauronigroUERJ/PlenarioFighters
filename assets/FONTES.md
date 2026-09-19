# Assets da versão 14

## Retratos

Gerados e editados com a ferramenta integrada `image_gen.imagegen`, em 13/09/2026. Arquivo usado: `retratos-pixel.png`; atlas técnico convertido para BMP e incorporado em `src/portraits_data.h`. A conversão de formato e escala usa nearest-neighbor; o jogo carrega com SDL_LoadBMP_RW. Não usa SDL_image.

Orientação da edição: preservar as nove caricaturas na ordem Flávio, Lula, Renan / Zema, Cury, Caiado / Moraes, Lulinha, Haddad; redesenhar como pixel art de arcade dos anos 1990, com pixels grandes visíveis, cores chapadas, contornos escuros, feições exageradas, sem textura de pintura ou realismo fotográfico. A edição substituiu a primeira proposta mais realista, conforme a correção do usuário.

O vídeo sugerido (https://www.youtube.com/watch?v=lEv31wa9TVo) não pôde ser carregado pelo navegador de pesquisa. Os cortes e close-ups foram implementados a partir da orientação textual, preservando a abertura existente; não foram copiados frames nem áudio do vídeo.

## Música

Hino Nacional Brasileiro: melodia de Francisco Manuel da Silva, composição em domínio público. Referência de partitura: https://pt.cantorion.org/music/4811/Hino-Nacional-Brasileiro-Brazilian-national-anthem .

A sequência monofônica de referência foi conferida no canal `Melody` do MIDI disponível em https://www.midi-karaoke.info/212c4d4d.html (créditos informados no arquivo: Jonas de Castro Deus; revisão Grillo). Apenas o trecho da melodia foi transcrito em notas, em `src/anthem_data.h`. Nenhuma gravação ou arranjo orquestral é distribuído. A harmonização simples e os timbres chiptune são implementados em `src/music.h` com ondas quadradas e triangulares. O tema das aberturas e do bônus é uma composição original sintetizada no mesmo arquivo.

A primeira referência MIDI encontrada tinha metadados de outro hino e foi descartada. Ela não é usada no jogo.

## Bandeiras do bônus

Desenhadas com retângulos e linhas SDL2 em `src/bonus.h`, em versões estilizadas para o tamanho de tela. A pequena onça usa como referência visual a imagem fornecida pelo usuário; o iate de Lulinha e os símbolos Justiça/Master são elementos ficcionais da sátira. Não representam patrocínio nem filiação real de pessoas sem partido.
