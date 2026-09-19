<<<<<<< HEAD
# Controles e configuração — versão 18

## Menu e atalhos

No menu principal, **esquerda/direita** mudam a opção inferior; **cima/baixo** mudam o personagem. **Tab** ou quadrado/X no controle alternam P1 e P2/CPU. **Enter, espaço ou confirmar no controle** ativam a opção escolhida.

Há seis opções: 1 jogador, 2 jogadores, Controles, Torneio, CPU e História. Na opção CPU, confirmar alterna fácil/médio/difícil. O CRT é permanente; a seleção percorre três páginas de nove lutadores.

| Atalho | Ação |
|---|---|
| F1 | Abrir configurações de controles |
| F2 | Redetectar controles |
| F3 no menu | Alternar dificuldade da CPU |
| F3 nas configurações | Iniciar calibração PS2 |
| F6 na notícia da História | Abrir a fonte da notícia |
| F11 | Alternar tela cheia |
| M | Ligar/desligar som |
| Esc durante a luta | Voltar ao menu |

A abertura pode ser pulada com Enter, espaço, Esc ou confirmar/Start/voltar no controle. O ending da partida final volta ao menu com Enter/confirmar; é possível pulá-lo após o primeiro segundo.

## Teclado durante a luta

| Ação | P1 | P2 |
|---|---|---|
| Mover | A / D | Setas esquerda / direita |
| Pular | W | Seta para cima |
| Agachar | S | Seta para baixo |
| Soco | J | Num 1 |
| Chute | K | Num 2 |
| Malote | I | Num 3 |
| Agarrar | O | Decimal numérico |
| Ultimate | U | Num 0 |
| Defesa auxiliar | L | Shift direito |
| Pausa | P | Enter numérico |

Repita ou segure soco/chute para encadear três golpes. O terceiro tem animação e dano maiores. Segure baixo para chutar ou lançar um malote agachado; pule e ataque para usar as variantes aéreas. Cada personagem tem **6 malotes por round**, indicados em seu lado do HUD.

Segurar o sentido **contrário ao inimigo** defende em pé. **Baixo+trás** defende golpes baixos. A defesa auxiliar continua disponível. Teclas e botões são remapeáveis nas configurações.

## Dois jogadores

Escolha **2 jogadores** no menu. No lobby, cima/baixo percorrem as linhas; esquerda/direita mudam o dispositivo ou personagem da linha selecionada. Atribua os controles a P1 e P2 e confirme **Iniciar luta**. É possível combinar teclado e controle ou usar dois controles distintos.

## PS2, genéricos e Parsec

Conecte o adaptador PS2 com botões soltos. Abra Configurações, escolha o jogador e o dispositivo em **Controle atribuído**, e selecione **Configurar controle PS2 (F3)**. Aperte e solte os botões pedidos nas 12 etapas. Esc cancela; o perfil só é aplicado e salvo ao concluir. O assistente aceita botões, hats e eixos, sem presumir uma numeração universal do adaptador.

Depois da calibração: X pula/confirma, círculo chuta/volta, quadrado soca, triângulo lança malote, L1 é defesa auxiliar, R1 é ultimate, **R2 agarra**, Start pausa. O direcional permite defesa alta/baixa. Se mudar o modo ANALOG do adaptador, confira os comandos e recalibre se necessário. Sem calibração PS2, o padrão dos gamepads reconhecidos usa L3 para agarrar.

Pelo Parsec, calibre no jogo do host usando o dispositivo encaminhado pelo convidado. Ele pode aparecer como Xbox ou PlayStation em vez de 8BitDo/PS2. O host precisa receber dois dispositivos distintos para usar dois controles separadamente. Se o controle não aparece nem após F2, a calibração não pode criar um dispositivo que o sistema não está recebendo.

`controls.cfg` guarda os perfis dos jogadores ao lado do executável. Você pode copiar seu arquivo das versões anteriores para a pasta da versão 18; o formato permanece compatível. `game.cfg` guarda a dificuldade; o campo legado do filtro é ignorado e o CRT fica ligado. `story.cfg` guarda o próximo capítulo do modo História.

## Arcade na v13

F5 no menu abre os recordes. As telas iniciais avançam com qualquer tecla, clique ou botão. Quinze segundos sem interação no menu iniciam uma demonstração CPU contra CPU; um comando a interrompe e volta ao menu. Os comandos de combate permanecem iguais.


## Bônus do torneio (v14)

- P1 padrão: W/S mudam a fileira; I lança malote.
- P2 padrão: setas cima/baixo mudam a fileira; 3 do teclado numérico lança malote.
- No controle, use cima/baixo e o botão configurado como Malote. Os remapeamentos existentes continuam valendo.
- P2 começa como CPU e pode ser assumido com seus próprios comandos. Configure a atribuição dos dispositivos em F1 se necessário.
- P/Start pausa/continua; Esc encerra o torneio e volta ao menu. M silencia música e efeitos.
- O bônus dura 10 segundos e tem munição livre. Mais conversões rendem +2 malotes na partida seguinte; empate não premia.


## Apresentação de cada luta (v15)

- Enter/espaço ou confirmar/Start no controle: pular a tela VS e seguir à contagem do round.
- Esc/voltar: retornar ao menu.
- P (ou sua tecla configurada de pausa): pausar/continuar a apresentação.
- F1: configurações de controles. M: música e efeitos.
- O comando que confirma a apresentação não reinicia a luta nem aciona sua pausa.

## História (v18)

Enter/espaço/confirmar inicia a luta após a notícia. Vitória avança para a próxima notícia; derrota permite repetir o capítulo. Esc/voltar encerra o modo mantendo o progresso salvo. F6 abre a fonte no navegador.
=======
# Controles e configuração — versão 18

## Menu e atalhos

No menu principal, **esquerda/direita** mudam a opção inferior; **cima/baixo** mudam o personagem. **Tab** ou quadrado/X no controle alternam P1 e P2/CPU. **Enter, espaço ou confirmar no controle** ativam a opção escolhida.

Há seis opções: 1 jogador, 2 jogadores, Controles, Torneio, CPU e História. Na opção CPU, confirmar alterna fácil/médio/difícil. O CRT é permanente; a seleção percorre três páginas de nove lutadores.

| Atalho | Ação |
|---|---|
| F1 | Abrir configurações de controles |
| F2 | Redetectar controles |
| F3 no menu | Alternar dificuldade da CPU |
| F3 nas configurações | Iniciar calibração PS2 |
| F6 na notícia da História | Abrir a fonte da notícia |
| F11 | Alternar tela cheia |
| M | Ligar/desligar som |
| Esc durante a luta | Voltar ao menu |

A abertura pode ser pulada com Enter, espaço, Esc ou confirmar/Start/voltar no controle. O ending da partida final volta ao menu com Enter/confirmar; é possível pulá-lo após o primeiro segundo.

## Teclado durante a luta

| Ação | P1 | P2 |
|---|---|---|
| Mover | A / D | Setas esquerda / direita |
| Pular | W | Seta para cima |
| Agachar | S | Seta para baixo |
| Soco | J | Num 1 |
| Chute | K | Num 2 |
| Malote | I | Num 3 |
| Agarrar | O | Decimal numérico |
| Ultimate | U | Num 0 |
| Defesa auxiliar | L | Shift direito |
| Pausa | P | Enter numérico |

Repita ou segure soco/chute para encadear três golpes. O terceiro tem animação e dano maiores. Segure baixo para chutar ou lançar um malote agachado; pule e ataque para usar as variantes aéreas. Cada personagem tem **6 malotes por round**, indicados em seu lado do HUD.

Segurar o sentido **contrário ao inimigo** defende em pé. **Baixo+trás** defende golpes baixos. A defesa auxiliar continua disponível. Teclas e botões são remapeáveis nas configurações.

## Dois jogadores

Escolha **2 jogadores** no menu. No lobby, cima/baixo percorrem as linhas; esquerda/direita mudam o dispositivo ou personagem da linha selecionada. Atribua os controles a P1 e P2 e confirme **Iniciar luta**. É possível combinar teclado e controle ou usar dois controles distintos.

## PS2, genéricos e Parsec

Conecte o adaptador PS2 com botões soltos. Abra Configurações, escolha o jogador e o dispositivo em **Controle atribuído**, e selecione **Configurar controle PS2 (F3)**. Aperte e solte os botões pedidos nas 12 etapas. Esc cancela; o perfil só é aplicado e salvo ao concluir. O assistente aceita botões, hats e eixos, sem presumir uma numeração universal do adaptador.

Depois da calibração: X pula/confirma, círculo chuta/volta, quadrado soca, triângulo lança malote, L1 é defesa auxiliar, R1 é ultimate, **R2 agarra**, Start pausa. O direcional permite defesa alta/baixa. Se mudar o modo ANALOG do adaptador, confira os comandos e recalibre se necessário. Sem calibração PS2, o padrão dos gamepads reconhecidos usa L3 para agarrar.

Pelo Parsec, calibre no jogo do host usando o dispositivo encaminhado pelo convidado. Ele pode aparecer como Xbox ou PlayStation em vez de 8BitDo/PS2. O host precisa receber dois dispositivos distintos para usar dois controles separadamente. Se o controle não aparece nem após F2, a calibração não pode criar um dispositivo que o sistema não está recebendo.

`controls.cfg` guarda os perfis dos jogadores ao lado do executável. Você pode copiar seu arquivo das versões anteriores para a pasta da versão 18; o formato permanece compatível. `game.cfg` guarda a dificuldade; o campo legado do filtro é ignorado e o CRT fica ligado. `story.cfg` guarda o próximo capítulo do modo História.

## Arcade na v13

F5 no menu abre os recordes. As telas iniciais avançam com qualquer tecla, clique ou botão. Quinze segundos sem interação no menu iniciam uma demonstração CPU contra CPU; um comando a interrompe e volta ao menu. Os comandos de combate permanecem iguais.


## Bônus do torneio (v14)

- P1 padrão: W/S mudam a fileira; I lança malote.
- P2 padrão: setas cima/baixo mudam a fileira; 3 do teclado numérico lança malote.
- No controle, use cima/baixo e o botão configurado como Malote. Os remapeamentos existentes continuam valendo.
- P2 começa como CPU e pode ser assumido com seus próprios comandos. Configure a atribuição dos dispositivos em F1 se necessário.
- P/Start pausa/continua; Esc encerra o torneio e volta ao menu. M silencia música e efeitos.
- O bônus dura 10 segundos e tem munição livre. Mais conversões rendem +2 malotes na partida seguinte; empate não premia.


## Apresentação de cada luta (v15)

- Enter/espaço ou confirmar/Start no controle: pular a tela VS e seguir à contagem do round.
- Esc/voltar: retornar ao menu.
- P (ou sua tecla configurada de pausa): pausar/continuar a apresentação.
- F1: configurações de controles. M: música e efeitos.
- O comando que confirma a apresentação não reinicia a luta nem aciona sua pausa.

## História (v18)

Enter/espaço/confirmar inicia a luta após a notícia. Vitória avança para a próxima notícia; derrota permite repetir o capítulo. Esc/voltar encerra o modo mantendo o progresso salvo. F6 abre a fonte no navegador.
>>>>>>> d574e720ee2a70123b601a194c3360de9f62476b
