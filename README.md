# 🎮 ARCHER - Jogo de Funções Matemáticas

![Status](https://img.shields.io/badge/status-completo-success)
![Linguagem](https://img.shields.io/badge/linguagem-C-blue)
![Plataforma](https://img.shields.io/badge/plataforma-Windows-lightgrey)
![Educacional](https://img.shields.io/badge/prop%C3%B3sito-educacional-orange)

Um jogo educacional que ensina **6 funções matemáticas** através de mecânicas de tiro com arco, desenvolvido em C com Allegro 5.

---

## 📋 Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Funcionalidades](#funcionalidades)
- [Tecnologias Utilizadas](#tecnologias-utilizadas)
- [Como Compilar](#como-compilar)
- [Como Jogar](#como-jogar)
- [Estrutura do Código](#estrutura-do-código)
- [Processo de Desenvolvimento](#processo-de-desenvolvimento)
- [Assets](#assets)
- [Créditos](#créditos)

---

## 🎯 Sobre o Projeto

**Archer** é um jogo educacional desenvolvido para ensinar funções matemáticas do ensino médio de forma interativa e divertida. O jogador controla um arqueiro que deve atirar flechas seguindo trajetórias matemáticas para coletar chaves e completar desafios.

### Objetivo Educacional

Ensinar aos estudantes como diferentes funções matemáticas se comportam visualmente, permitindo que experimentem com parâmetros e vejam os resultados em tempo real.

### Público-Alvo

- Estudantes do ensino médio (14-18 anos)
- Professores de matemática
- Qualquer pessoa interessada em aprender funções de forma interativa

---

## ✨ Funcionalidades

### 🎓 Sistema Educacional

- **6 Funções Matemáticas Completas:**
  - Linear: `y = mx + k`
  - Quadrática: `y = a(x-h)² + k`
  - Exponencial: `y = a·b^x + c`
  - Logarítmica: `y = a·log(x+b) + c`
  - Racional: `y = (ax+b)/(cx+d)`
  - Trigonométrica: `y = a·sin(bx+c) + d`

- **10 Tutoriais Didáticos:**
  - 4 tutoriais gerais (boas-vindas, controles, coletáveis, inimigos)
  - 6 tutoriais específicos (um para cada função)
  - Estética de lousa matemática
  - Fontes grandes e legíveis (3.5x título, 2.2x conteúdo)

### 🎮 Mecânicas de Jogo

- **Sistema de Física Realista:**
  - Flechas seguem a função matemática escolhida
  - Ricochete em obstáculos com perda de velocidade
  - Gravidade após colisões
  - Preview da trajetória em tempo real

- **Controles Avançados:**
  - Movimento horizontal (← →)
  - Pulo (↑)
  - Barra de força do arco (segurar ESPAÇO = 100-350% força)
  - Edição de parâmetros em tempo real (M/K/H, Q/E)
  - Troca de funções (C)
  - Grid cartesiano (P)
  - Especial (H) - Ativa poder especial quando barra cheia
  - Menu de pausa (ESC)

- **Sistema de Progressão:**
  - 6 capítulos desbloqueáveis
  - 4 fases por capítulo (teste + 3 normais + boss fight)
  - Total: 24 fases
  - Progressão automática entre capítulos

### 👾 Inimigos e Desafios

- **Obstáculos:**
  - Pedras (ricochete com física)
  - Muros (ricochete forte)

- **Inimigos Móveis:**
  - Patrulham automaticamente
  - Destroem a flecha ao contato
  - Quantidade aumenta por fase (2-4 inimigos)

- **Boss Fights:**
  - Um boss por capítulo
  - 50 HP (4-5 acertos para vencer)
  - Dispara projéteis roxos
  - Sistema de vida do player (200 HP)
  - Preview e controles funcionam

### 🎨 Interface

- **HUD Completo:**
  - Box de valores atuais (canto inferior esquerdo, 47% opacidade)
  - Sidebar de comandos (lado direito, 47% opacidade, posicionada para não cobrir coletáveis)
  - Barra de especial (dentro da sidebar)
  - Barra de força do arco (dentro da sidebar)
  - Barra de vida (boss fight)
  - UI transparente permite ver o personagem e coletáveis por trás

- **Especiais:**
  - Super Velocidade (3x mais rápido)
  - Tempo Lento (câmera lenta)
  - Sistema de seleção de cartas

- **Grid Cartesiano:**
  - Ativa/desativa com P
  - Linhas suaves (opacidade 25)
  - Eixos X e Y destacados
  - Números a cada 5 unidades

---

## 🛠️ Tecnologias Utilizadas

### Linguagem e Bibliotecas

- **Linguagem:** C (padrão C99)
- **Engine Gráfica:** Allegro 5
  - allegro_font
  - allegro_ttf
  - allegro_image
  - allegro_primitives
  - allegro_keyboard

### IDE e Compilador

- **IDE:** Visual Studio 2022 (Windows)
- **Compilador:** MSVC (Microsoft Visual C++)
- **Arquitetura:** x64

### Assets

- **Backgrounds:** PNG/JPEG (1280x720)
- **Sprites:** PNG (transparência)
- **Fontes:** Allegro builtin (8x8 bitmap escalada)

---

## 📦 Como Compilar

### Pré-requisitos

1. **Visual Studio 2022** (ou 2019+)
2. **Allegro 5** instalado
3. **Windows 10/11**

### Instalação do Allegro 5

#### Método 1: NuGet (Recomendado)

```bash
1. Abra o Visual Studio
2. Tools → NuGet Package Manager → Manage NuGet Packages for Solution
3. Pesquise: "Allegro"
4. Instale: allegro (versão 5.2.x ou superior)
```

#### Método 2: Manual

```bash
1. Baixe Allegro 5 de: https://liballeg.org/
2. Extraia para C:\allegro5
3. Configure Include Directories e Library Directories no VS
```

### Compilação

#### Visual Studio (GUI)

```bash
1. Abra Visual Studio
2. File → New → Project → Empty Project (C++)
3. Adicione real-new-game.c ao projeto
4. Configure Properties:
   - Configuration Type: Application (.exe)
   - Platform: x64
   - C/C++ → General → Additional Include Directories: [caminho do Allegro]\include
   - Linker → General → Additional Library Directories: [caminho do Allegro]\lib
   - Linker → Input → Additional Dependencies: 
     allegro-5.0.10-monolith-md-debug.lib (ou similar)
5. Build → Build Solution (Ctrl+Shift+B)
6. Copie DLLs do Allegro para a pasta do .exe
7. Execute!
```

#### Linha de Comando (MinGW)

```bash
gcc real-new-game.c -o archer.exe -lallegro-5.0.10-monolith-md -lm
```

---

## 🎮 Como Jogar

### Controles Principais

| Tecla | Ação |
|-------|------|
| **← →** | Mover arqueiro |
| **↑** | Pular |
| **ESPAÇO** | Segurar = carregar / Soltar = atirar |
| **M, K, H** | Selecionar parâmetro da função |
| **Q, E** | Diminuir / Aumentar valor |
| **[** | Selecionar parâmetro D (Racional/Trig) |
| **C** | Trocar função (apenas desbloqueadas) |
| **P** | Toggle grid cartesiano |
| **H** | Usar especial (quando barra cheia) |
| **SHIFT** | Preview da trajetória |
| **R** | Reiniciar fase |
| **ESC** | Pausar jogo |

### Menu de Pausa (ESC)

- **ENTER:** Continuar
- **C:** Trocar função
- **R:** Reiniciar fase
- **M:** Menu principal

### Progressão

1. **Primeira vez:** Tutorial completo (4 telas)
2. **Escolha capítulo:** Linear (desbloqueado)
3. **Leia explicação** da função
4. **Complete fases:**
   - Fase Teste (5 flechas)
   - Fase 1 (6 flechas)
   - Fase 2 (6 flechas)
   - Fase 3 (7 flechas)
5. **Boss Fight!** (15 flechas)
6. **Próximo capítulo desbloqueado automaticamente**

### Dicas

- 🎯 Use **P** para ativar o grid (ajuda a calcular)
- ⚡ Segure **ESPAÇO** mais tempo = tiro mais forte (até 350%)
- 🔄 Use **C** para trocar função e experimentar
- 🎓 Leia os tutoriais - eles explicam cada parâmetro
- 🦘 **PULE** para desviar de projéteis do boss
- 📊 Observe o **box de valores** no canto inferior esquerdo
- 🎮 Use **H** para ativar especial quando a barra estiver cheia
- 👁️ UI transparente permite ver tudo por trás dela

---

## 📁 Estrutura do Código

### Arquivo Principal

```
real-new-game.c (4600+ linhas)
├── TIPOS E ESTRUTURAS (linhas 44-430)
│   ├── GameState (13 estados)
│   ├── FunctionType (6 funções)
│   ├── Player, Arrow, Enemy, Boss, Obstacle
│   ├── Phase, Chapter, Progress
│   └── UI, Special, Tutorial
│
├── FUNÇÕES MATEMÁTICAS (linhas 431-510)
│   ├── fn_eval() - Avalia todas as 6 funções
│   ├── init_function_params() - Inicializa parâmetros
│   └── cycle_function_type() - Troca funções
│
├── FÍSICA E GAMEPLAY (linhas 511-1100)
│   ├── player_init(), player_update(), player_draw()
│   ├── arrow_init(), arrow_shoot(), arrow_update()
│   ├── Colisões (ricochete, inimigos, boss)
│   └── enemies_update(), boss_update()
│
├── TUTORIAIS (linhas 1101-1450)
│   ├── 4 tutoriais gerais
│   ├── 6 tutoriais de funções
│   └── draw_tutorial_screen_with_game()
│
├── NÍVEIS E FASES (linhas 1451-1750)
│   ├── level_init_chapters() - 6 capítulos
│   ├── Coletáveis distribuídos (X: -14 a 17)
│   ├── Obstáculos estratégicos
│   └── Inimigos (2-4 por fase)
│
├── LÓGICA DO JOGO (linhas 1751-2800)
│   ├── game_init(), game_load_resources()
│   ├── game_update() - Loop principal
│   └── game_handle_key_down()
│
├── RENDERIZAÇÃO (linhas 2801-3900)
│   ├── game_draw() - Desenha tudo
│   ├── 13 estados diferentes
│   ├── HUD dinâmico
│   └── draw_command_hud()
│
└── MAIN (linhas 3901-4150)
    ├── Inicialização Allegro
    ├── Event loop
    └── Cleanup
```

### Estruturas de Dados Principais

```c
// Jogo completo (linha 367)
typedef struct {
    GameState state;              // Estado atual
    int current_chapter;          // Capítulo (0-5)
    int current_phase;            // Fase (0-4)
    Player player;                // Arqueiro
    Arrow arrow;                  // Flecha
    MathFunction current_function;// Função matemática atual
    Progress progress;            // Sistema de desbloqueio
    // ... 30+ campos adicionais
} Game;

// Função matemática (linha 115)
typedef struct {
    FunctionType type;           // LINEAR, QUADRATIC, etc
    union {
        LinearParams linear;      // m, k
        QuadraticParams quadratic;// a, h, k
        // ... outros tipos
    } params;
} MathFunction;
```

---

## 🔧 Processo de Desenvolvimento

### Cronologia de Implementação

#### Fase 1: Core (Semana 1)
- ✅ Estruturas básicas (GameState, Player, Arrow)
- ✅ Função Linear implementada
- ✅ Sistema de tiro básico
- ✅ Preview da trajetória
- ✅ Coletáveis e chaves

#### Fase 2: Física (Semana 1)
- ✅ Física realista (ricochete, gravidade)
- ✅ Sistema de colisões
- ✅ Obstáculos com física
- ✅ Barra de força do arco

#### Fase 3: Funções (Semana 2)
- ✅ 5 funções adicionais implementadas
- ✅ Sistema de parâmetros editáveis
- ✅ Limitadores (função não some)
- ✅ Troca de funções (tecla C)

#### Fase 4: Tutoriais (Semana 2)
- ✅ 10 tutoriais completos
- ✅ Estética de lousa matemática
- ✅ Sprites visíveis nos tutoriais
- ✅ Navegação por páginas

#### Fase 5: Inimigos (Semana 3)
- ✅ Inimigos móveis com patrulha
- ✅ Sistema de colisão
- ✅ Progressão de dificuldade (2-4 por fase)
- ✅ Boss fights implementados

#### Fase 6: Boss System (Semana 3)
- ✅ Boss com projéteis
- ✅ Sistema de vida (player e boss)
- ✅ Barra de vida visual
- ✅ Polvo com tentáculos (fallback)
- ✅ Balanceamento (50 HP boss, 200 HP player)

#### Fase 7: UI/UX (Semana 4)
- ✅ HUD dinâmico por função
- ✅ Box de valores em tempo real (canto inferior esquerdo)
- ✅ Sidebar de comandos transparente (lado direito)
- ✅ Menu de pausa completo
- ✅ Grid cartesiano
- ✅ Tela de congratulações épica
- ✅ Tela final épica quando completa todos os capítulos
- ✅ bg-home.png na tela inicial
- ✅ UI com 47% opacidade (não cobre personagem/coletáveis)

#### Fase 8: Polimento (Semana 4)
- ✅ Sistema de pulo (↑)
- ✅ Sprites corretos (player-idle-3.png, player-shot-3.png)
- ✅ Coletáveis 8x maiores
- ✅ Obstáculos 1.5x maiores
- ✅ Fontes aumentadas em todo o jogo
- ✅ Assíntotas na racional
- ✅ Controle do parâmetro D (trigonométrica)
- ✅ Obstáculos SPIKE removidos (não serviam ao gameplay)
- ✅ Aleatoriedade de coletáveis ajustada (Y máximo = 8.0)
- ✅ Coletáveis ocultados durante telas de transição
- ✅ Velocidade da flecha aumentada (15.0 unidades/s)
- ✅ Força máxima aumentada (3.5x)

### Desafios Técnicos Superados

1. **Física da Flecha:**
   - Problema: Flecha não ia onde o preview mostrava
   - Solução: Usar distância percorrida ao invés de posição absoluta
   ```c
   double distance = arrow->vx * arrow->t;
   arrow->current_y = arrow->y0 + fn_eval(fn, fabs(distance));
   ```

2. **Função Sumia da Tela:**
   - Problema: Parábola com 'a' pequeno sumia
   - Solução: Limitadores (clamps) em todos os parâmetros
   ```c
   if (a > -0.05 && a < 0.05) {
       a = (a >= 0) ? 0.05 : -0.05;  // Pula zona morta
   }
   ```

3. **Progressão Entre Capítulos:**
   - Problema: Ficava preso após vencer boss
   - Solução: Progressão automática com tutorial
   ```c
   current_chapter++;
   current_function.type = chapters[current_chapter].function_type;
   state = STATE_FUNCTION_INTRO;  // Mostra tutorial
   ```

4. **Boss Fight Muito Difícil:**
   - Problema: Impossível vencer (boss 100 HP, dano 20)
   - Solução: Rebalanceamento completo
   ```c
   Boss: 50 HP, cooldown 3.0s, projéteis 7.0 velocidade
   Player: 200 HP, pode pular, preview funciona
   ```

5. **Erro de Null Pointer (0xC0000005):**
   - Problema: Crash ao mudar de fase (violação de acesso)
   - Solução: Verificações de NULL em todas as funções críticas
   ```c
   if (!phase) return;  // Verificação antes de acessar
   if (screen->content[i] != NULL && strlen(...))  // Verificação segura
   ```

6. **UI Cobrindo Coletáveis:**
   - Problema: Sidebar e box cobriam coletáveis e personagem
   - Solução: UI mais transparente (47% opacidade) e reposicionada
   ```c
   sidebar_y = 80;  // Mais baixo
   al_map_rgba(0, 0, 0, 120);  // Mais transparente
   ```

---

## 🎨 Assets

### Estrutura de Pastas

```
assets/
├── bg/
│   ├── bg-home.png          (1280x720) Tela inicial
│   ├── linear.jpeg          (1280x720) Cap 1
│   ├── quadratica.png       (1280x720) Cap 2
│   ├── exponecial.jpg       (1280x720) Cap 3
│   ├── logaritmica.png      (1280x720) Cap 4
│   ├── racional.png         (1280x720) Cap 5
│   └── trigonometrica.png   (1280x720) Cap 6
│
└── sprites/
    ├── player-idle-3.png    (64x64)   Parado
    ├── player-walk-3.png    (256x64)  4 frames andando
    ├── player-shot-3.png    (192x64)  3 frames atirando
    ├── key.png              (Variável) Chave
    ├── apple.png            (Variável) Maçã
    ├── apple-gold.png       (Variável) Maçã dourada
    ├── enemy.png            (Variável) Inimigo
    ├── obstacle.png         (Variável) Obstáculo
    └── boss.png             (50x50)    Boss (opcional)
```

### Fallbacks Visuais

**Todos os sprites são opcionais!** O jogo tem fallbacks para:
- Player: Boneco palito animado
- Coletáveis: Círculos coloridos
- Obstáculos: Formas geométricas
- Boss: Polvo com 8 tentáculos desenhado
- Backgrounds: Gradientes bonitos

### Caminhos de Assets

**Windows (Visual Studio):**
- Caminhos absolutos: `C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/...`
- Configure o caminho correto no código (linhas 27-44 em `real-new-game.c`)

---

## 💻 Estrutura do Código

### Estados do Jogo (13 estados)

```c
STATE_TITLE              // Tela inicial
STATE_TUTORIAL_INTRO     // Tutorial 1-4
STATE_FUNCTION_INTRO     // Explicação da função
STATE_CHAPTER_SELECT     // Menu de capítulos
STATE_CHAPTER_INTRO      // Intro do capítulo
STATE_CHAPTER_TEST       // Fase teste
STATE_PLAY               // Fases 1-3
STATE_BOSS_FIGHT         // Luta contra boss
STATE_PAUSED             // Menu de pausa (ESC)
STATE_STAGE_CLEAR        // Fase completada
STATE_TRY_AGAIN          // Game over
STATE_SPECIAL_SELECT     // Seleção de especial
```

### Loop Principal

```c
while (running) {
    // 1. Processar eventos (teclado, mouse, fechar janela)
    al_wait_for_event(event_queue, &event);
    
    // 2. Atualizar lógica do jogo (60 FPS)
    if (event.type == ALLEGRO_EVENT_TIMER) {
        game_update(&game, 1.0/60.0);
    }
    
    // 3. Desenhar tudo
    if (redraw) {
        game_draw(&game);
        al_flip_display();
    }
}
```

### Sistema de Colisões

```c
// Flecha com obstáculos (ricochete)
bool check_arrow_obstacle_collision(Arrow* arrow, Obstacle* obstacles, int count);

// Flecha com inimigos (destruição)
bool check_arrow_enemy_collision(Arrow* arrow, Enemy* enemies, int count);

// Flecha com boss (dano)
bool check_arrow_boss_collision(Arrow* arrow, Boss* boss);

// Projéteis do boss com player (dano)
bool check_boss_arrow_player_collision(BossArrow* arrows, int count, Player* player);
```

---

## 📊 Estatísticas do Projeto

### Código

- **Linhas de Código:** 4600+
- **Structs Criados:** 25
- **Funções:** 150+
- **Estados:** 13
- **Tarefas Implementadas:** 80+

### Conteúdo

- **Funções Matemáticas:** 6
- **Capítulos:** 6
- **Fases Totais:** 24 (4 por capítulo)
- **Tutoriais:** 10
- **Boss Fights:** 6
- **Tipos de Inimigos:** 2 (móveis com patrulha)
- **Tipos de Obstáculos:** 2 (pedras e muros)
- **Especiais:** 3 (Super Velocidade, Tempo Lento, Flecha Tripla)

### Desenvolvimento

- **Tempo Total:** ~4 semanas
- **Linhas Adicionadas por Dia:** ~150
- **Bugs Corrigidos:** 50+
- **Iterações de Balanceamento:** 15+

---

## 🎓 Valor Educacional

### Conceitos Matemáticos Ensinados

1. **Função Linear:**
   - Inclinação (m)
   - Intercepto (k)
   - Crescimento constante

2. **Função Quadrática:**
   - Parábolas
   - Vértice (h, k)
   - Abertura (a)

3. **Função Exponencial:**
   - Crescimento exponencial
   - Base e amplitude
   - Decaimento

4. **Função Logarítmica:**
   - Inverso da exponencial
   - Crescimento desacelerado
   - Domínio (x > 0)

5. **Função Racional:**
   - Assíntotas verticais e horizontais
   - Comportamento perto de singularidades
   - Divisão de polinômios

6. **Função Trigonométrica:**
   - Ondas senoidais
   - Amplitude, frequência, fase
   - Deslocamento vertical

### Abordagem Pedagógica

- **Aprender Fazendo:** Experimentação ativa com parâmetros
- **Feedback Imediato:** Vê resultado visual instantâneo
- **Progressão Gradual:** Dificuldade aumenta gradualmente
- **Gamificação:** Boss fights, especiais, desbloqueios

---

## 🐛 Debug e Testes

### Mensagens de Debug

O jogo imprime mensagens úteis no console:

```
[INIT] Iniciando Archer...
[OK] Allegro inicializado!
[OK] Display criado: 1280x720
[INFO] *** ARCHER INICIADO COM SUCESSO! ***
[FASE] Iniciando fase 0 do capítulo 0
[DEBUG] Chaves coletadas: 1 / 3
[VITORIA] 3 chaves coletadas! Passando de fase...
[BOSS HIT!] Acertou! Dano: 12 | Vida restante: 38/50
[BOSS DERROTADO!] Vitoria!
```

### Troubleshooting

Consulte `TROUBLESHOOTING.txt` para resolver problemas comuns.

---

## 🏆 Créditos

### Desenvolvimento

- **Programação:** Sistema completo em C
- **Design de Níveis:** Distribuição de coletáveis e obstáculos
- **Balanceamento:** Boss fights e progressão
- **UI/UX:** Interface e tutoriais

### Bibliotecas

- **Allegro 5:** Motor gráfico e de áudio
- **Biblioteca Padrão C:** math.h, stdio.h, stdlib.h

### Assets

- **Backgrounds:** Imagens personalizadas
- **Sprites:** Pixel art personalizado
- **Fontes:** Allegro builtin bitmap font

---

## 📝 Licença

Este projeto foi desenvolvido para fins educacionais.

---

## 📞 Suporte

Para dúvidas ou problemas:
1. Consulte `TROUBLESHOOTING.txt`
2. Verifique `PROJETO_COMPLETO_FINAL.txt`
3. Leia as mensagens de debug no console

---

## 🎉 Conquistas do Projeto

- ✅ Jogo 100% funcional e jogável
- ✅ 6 funções matemáticas completas
- ✅ Sistema de progressão robusto
- ✅ Boss fights balanceados
- ✅ Interface polida e intuitiva (47% opacidade, não cobre elementos)
- ✅ Física realista
- ✅ Sem bugs críticos (null pointer corrigidos)
- ✅ Tela final épica quando completa todos os capítulos
- ✅ Valor educacional máximo
- ✅ UI otimizada para visibilidade

---

## 🚀 Versão

**Versão:** 2.0 Final  
**Data:** Novembro 2025  
**Status:** Completo e Pronto para Uso  

---

**Divirta-se aprendendo matemática!** 🎮🎯🏹📐

---

## 🔮 Expansões Futuras (Opcional)

- Sistema de save/load em arquivo
- Sons e música
- Mais tipos de bosses
- Editor de níveis
- Multiplayer
- Conquistas/achievements
- Leaderboards
- Mais especiais

---

**Desenvolvido com dedicação para tornar o aprendizado de matemática divertido e interativo.**

