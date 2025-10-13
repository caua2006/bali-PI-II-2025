ESTRUTURA DE ASSETS PARA ARCHER
================================

Este arquivo descreve a estrutura esperada de assets para o jogo funcionar
com imagens reais. O jogo possui FALLBACKS e funcionará mesmo sem assets,
usando formas geométricas coloridas.

BACKGROUNDS (4 camadas por capítulo)
------------------------------------
bg/linear_sky.png       - Céu do capítulo Linear (1280×720 recomendado)
bg/linear_layer1.png    - Camada 1 (paralaxe, opcional)
bg/linear_layer2.png    - Camada 2 (paralaxe, opcional)
bg/linear_ground.png    - Chão/plataformas

bg/quad_sky.png         - Quadrática
bg/quad_layer1.png
bg/quad_layer2.png
bg/quad_ground.png

bg/expo_sky.png         - Exponencial
bg/expo_layer1.png
bg/expo_layer2.png
bg/expo_ground.png

bg/log_sky.png          - Logarítmica
bg/log_layer1.png
bg/log_layer2.png
bg/log_ground.png

bg/rat_sky.png          - Racional (pode usar fallback)
bg/rat_layer1.png
bg/rat_layer2.png
bg/rat_ground.png

bg/trig_sky.png         - Trigonométrica (pode usar fallback)
bg/trig_layer1.png
bg/trig_layer2.png
bg/trig_ground.png

UI / INTERFACE
--------------
ui/title_screen.png         - Tela de título (1280×720)
ui/overlay_stage_clear.png  - Overlay de vitória (opcional)
ui/overlay_try_again.png    - Overlay de derrota (opcional)
ui/logo.png                 - Logo do jogo
ui/btn_primary.png          - Botão primário
ui/btn_secondary.png        - Botão secundário
ui/special_bar_empty.png    - Barra de especial vazia (200×20)
ui/special_bar_full.png     - Barra de especial cheia (200×20)

SPRITES DO JOGADOR (spritesheets horizontais)
----------------------------------------------
sprites/player_idle.png     - Idle (256×64 = 4 frames de 64×64)
sprites/player_walk.png     - Caminhando (512×64 = 8 frames de 64×64)
sprites/player_shoot.png    - Atirando (384×64 = 6 frames de 64×64)

ITENS COLETÁVEIS
----------------
sprites/arrow.png           - Flecha (32×16 recomendado)
sprites/key_silver.png      - Chave prateada (32×32)
sprites/key_gold.png        - Chave dourada (32×32)
sprites/apple_red.png       - Maçã normal (24×24)
sprites/apple_gold.png      - Maçã dourada (24×24)

INIMIGOS (VITRINE Mk1)
----------------------
enemies/enemy_sentinel.png  - Inimigo sentinela (apenas visual no Mk1)

TILES/OBSTÁCULOS (VITRINE Mk1)
-------------------------------
tiles/tile_platform.png     - Plataforma (32×32)
tiles/tile_spikes.png       - Espinhos (32×32)

FONTES
------
fonts/title.ttf             - Fonte para títulos (48pt)
fonts/main.ttf              - Fonte principal (24pt e 18pt)

CORES-TEMA POR CAPÍTULO (quando não há assets)
-----------------------------------------------
Linear:         Ciano/Azul (#00AADC)
Quadrática:     Violeta (#9632C8)
Exponencial:    Âmbar/Neon (#FFB400)
Logarítmica:    Verde Esmeralda (#00C864)
Racional:       Laranja (#C86432)
Trigonométrica: Rosa (#FF6496)

FALLBACKS AUTOMÁTICOS
----------------------
O jogo desenhará placeholders coloridos quando assets estiverem ausentes:
- Backgrounds: Gradientes por tema
- UI: Painéis com primitivas (retângulos arredondados)
- Player: Stick figure animado
- Flechas: Triângulos
- Itens: Círculos coloridos
- Fontes: Allegro builtin font

TESTANDO COM ASSETS
--------------------
1. Coloque as imagens nas pastas corretas
2. Execute o jogo
3. Verifique o console para avisos "[WARN] missing asset: ..."
4. Assets carregados com sucesso aparecerão no jogo

FORMATO RECOMENDADO
-------------------
- PNG com transparência (alpha channel)
- Paleta de cores consistente por capítulo
- Estilo: pixel art, cartoon 2D ou ilustrado
- Resolução base: 1280×720 para backgrounds

