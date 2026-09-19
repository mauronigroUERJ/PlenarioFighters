<<<<<<< HEAD
param([switch]$Test)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $compiler = Get-Command g++ -ErrorAction SilentlyContinue
    if (-not $compiler) { throw 'Instale o compilador MinGW-w64 g++ (por exemplo, MSYS2 UCRT64) e adicione a pasta bin ao PATH.' }
    & $compiler.Source '-std=c++17' '-O2' '-Wall' '-Wextra' '-Wpedantic' '-static' '-static-libgcc' '-static-libstdc++' 'src/main.cpp' '-Ivendor/include/SDL2' 'vendor/lib/libSDL2.a' '-ldinput8' '-ldxguid' '-ldxerr8' '-luser32' '-lgdi32' '-lwinmm' '-limm32' '-lole32' '-loleaut32' '-lshell32' '-lsetupapi' '-lversion' '-luuid' '-o' 'PlenarioFighter.exe'
    if ($LASTEXITCODE -ne 0) { throw 'A compilacao falhou.' }
    Write-Host 'Criado: PlenarioFighter.exe'
    if ($Test) {
        & .\PlenarioFighter.exe --expansion-test
        if ($LASTEXITCODE -ne 0) { throw "Teste do elenco e historia falhou." }
        & .\PlenarioFighter.exe --presentation-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de apresentacao, caminhada e reporter falhou.' }
        & .\PlenarioFighter.exe --bonus-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de bonus, musica e retratos falhou.' }
        & .\PlenarioFighter.exe --arcade-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de abertura, placar, demo e audio falhou.' }
        & .\PlenarioFighter.exe --combat-v12-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de dificuldade, malotes e KO falhou.' }
        & .\PlenarioFighter.exe --flow-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de menu e finais falhou.' }
        & .\PlenarioFighter.exe --combo-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de combos/audio falhou.' }
        & .\PlenarioFighter.exe --self-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de combate falhou.' }
        & .\PlenarioFighter.exe --smoke-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste SDL2 falhou.' }
        & .\PlenarioFighter.exe --input-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de controles falhou.' }
    }
} finally { Pop-Location }
=======
param([switch]$Test)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $compiler = Get-Command g++ -ErrorAction SilentlyContinue
    if (-not $compiler) { throw 'Instale o compilador MinGW-w64 g++ (por exemplo, MSYS2 UCRT64) e adicione a pasta bin ao PATH.' }
    & $compiler.Source '-std=c++17' '-O2' '-Wall' '-Wextra' '-Wpedantic' '-static' '-static-libgcc' '-static-libstdc++' 'src/main.cpp' '-Ivendor/include/SDL2' 'vendor/lib/libSDL2.a' '-ldinput8' '-ldxguid' '-ldxerr8' '-luser32' '-lgdi32' '-lwinmm' '-limm32' '-lole32' '-loleaut32' '-lshell32' '-lsetupapi' '-lversion' '-luuid' '-o' 'PlenarioFighter.exe'
    if ($LASTEXITCODE -ne 0) { throw 'A compilacao falhou.' }
    Write-Host 'Criado: PlenarioFighter.exe'
    if ($Test) {
        & .\PlenarioFighter.exe --expansion-test
        if ($LASTEXITCODE -ne 0) { throw "Teste do elenco e historia falhou." }
        & .\PlenarioFighter.exe --presentation-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de apresentacao, caminhada e reporter falhou.' }
        & .\PlenarioFighter.exe --bonus-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de bonus, musica e retratos falhou.' }
        & .\PlenarioFighter.exe --arcade-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de abertura, placar, demo e audio falhou.' }
        & .\PlenarioFighter.exe --combat-v12-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de dificuldade, malotes e KO falhou.' }
        & .\PlenarioFighter.exe --flow-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de menu e finais falhou.' }
        & .\PlenarioFighter.exe --combo-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de combos/audio falhou.' }
        & .\PlenarioFighter.exe --self-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de combate falhou.' }
        & .\PlenarioFighter.exe --smoke-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste SDL2 falhou.' }
        & .\PlenarioFighter.exe --input-test
        if ($LASTEXITCODE -ne 0) { throw 'Teste de controles falhou.' }
    }
} finally { Pop-Location }
>>>>>>> d574e720ee2a70123b601a194c3360de9f62476b
