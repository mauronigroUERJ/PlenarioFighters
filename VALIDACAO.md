# Validação da versão 18

## Compilação final

13/09/2026, MinGW-w64 UCRT64, C++17, SDL2 estática, `-O2 -Wall -Wextra -Wpedantic`, sem avisos. Retratos, fonte, efeitos e música são incorporados. Não há dependência de SDL2.dll externa.

## Testes executados na compilação anterior da v18

Passaram: `--expansion-test`, `--self-test`, `--combo-test`, `--combat-v12-test`, `--flow-test`, `--arcade-test`, `--presentation-test` e `--input-test`.

A verificação da expansão percorreu os 27 selecionáveis, confirmou 28 texturas de retrato, 12 presidenciáveis e dez ministros, páginas, oito reações de dano, bloqueio alto/baixo, dez capítulos em ordem cronológica, derrota/repetição, vitória/avanço, arquivo de progresso, retomada e conclusão do volume. Os testes de controles usam dispositivos virtuais; não houve teste físico novo de DualShock, PS2, 8BitDo ou Parsec.

`--bonus-test` e `--smoke-test` encontraram expectativas antigas de oito rivais e seleção terminando em Haddad. Foram ajustadas para o tamanho real do novo elenco/torneio; a nova execução ainda está pendente pelo bloqueio descrito abaixo.

## Inspeção visual

Capturadas com SDL2: três páginas de menu, 28 apresentações de duelo, dez notícias e conclusão, oito poses de dor, 20 quadros de defesa alta/baixa, 20 quadros de cloroquina e abertura de Jair. Inspecionados os menus, o retrato do boss, novos rostos, reações, explosão local de defesa e a animação do especial.

A inspeção detectou que o corpo das notícias usava minúsculas não suportadas pela fonte antiga. A compilação final corrige a fonte para mapear a–z aos glifos A–Z e acrescenta a verificação correspondente. As capturas das notícias anteriores a essa correção não são distribuídas como prévia final. Demais PNG/GIF em `previews/v18-*` provêm da compilação anterior e representam os desenhos preservados.

## Bloqueio da execução final

O Smart App Control bloqueou o EXE final. O log Microsoft-Windows-CodeIntegrity/Operational registrou os eventos 3077, 3033 e 3118, citando requisitos de assinatura e a política 0283ac0f-fff1-49ae-ada1-8a933130cad6. Hash SHA-256 do binário bloqueado: 3C14FA11B7648A4A7D6616FCA18B2BFCA54C26852009DD75A233D66ABDEFC1AC.

Não foi alterada a política de segurança. A bateria completa e a captura final das notícias permanecem pendentes até que o aplicativo possa ser executado por um procedimento autorizado de assinatura/liberação. Não se afirma que a compilação final passou nos testes.

## Pacote

ZIP verificado por CRC e comparação byte a byte de seu EXE com o executável avulso. O pacote contém código, assets, fontes históricas, documentação, controles e biblioteca SDL2 estática para reconstrução.
