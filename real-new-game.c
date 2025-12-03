/*
 * ========================================================================
 * ARCHER - JOGO DE FUNÇÕES MATEMÁTICAS (Mk1)
 * Arquivo único para Visual Studio + Allegro 5
 * ========================================================================
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS 60.0
#define PIXELS_PER_UNIT 32.0

 // --- CONFIG DE ASSETS (caminhos absolutos Windows) ---
static const char* ASSET_BG_LINEAR = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/linear.jpeg";
static const char* ASSET_BG_EXPO = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/exponecial.jpg";
static const char* ASSET_BG_QUADRATICA = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/quadratica.png";
static const char* ASSET_BG_LOGARITMICA = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/logaritmica.png";
static const char* ASSET_BG_RACIONAL = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/racional.png";
static const char* ASSET_BG_TRIGONOMETRICA = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/trigonometrica.png";
static const char* ASSET_BG_HOME = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/bg-home.png";
static const char* ASSET_PLAYER_IDLE = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/player-idle-3.png";
static const char* ASSET_PLAYER_WALK = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/player-walk-3.png";
static const char* ASSET_PLAYER_SHOOT = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/player-shoot-3.png";
static const char* ASSET_KEY = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/key.png";
static const char* ASSET_APPLE = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/apple.png";
static const char* ASSET_APPLE_GOLD = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/apple-gold.png";
static const char* ASSET_ENEMY = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/enemy.png";
static const char* ASSET_OBSTACLE = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/obstacle.png";
static const char* ASSET_BOSS = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/boss.png";
static const char* ASSET_TITLE_BG = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/ui/title-background.png";
static const char* ASSET_LOGO = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/ui/logo.png";

// ========================================================================
// TIPOS E ESTRUTURAS
// ========================================================================

// Estados do jogo
typedef enum {
    STATE_TITLE,
    STATE_TUTORIAL_INTRO,
    STATE_TUTORIAL_COLLECTIBLES,
    STATE_FUNCTION_INTRO,
    STATE_CHAPTER_SELECT,
    STATE_CHAPTER_INTRO,
    STATE_CHAPTER_TEST,
    STATE_PLAY,
    STATE_BOSS_FIGHT,
    STATE_PAUSED,
    STATE_STAGE_CLEAR,
    STATE_TRY_AGAIN,
    STATE_SPECIAL_SELECT
} GameState;

// Tipos de função
typedef enum {
    FN_LINEAR,
    FN_QUADRATIC,
    FN_EXPONENTIAL,
    FN_LOGARITHMIC,
    FN_RATIONAL,
    FN_TRIGONOMETRIC
} FunctionType;

// Função Linear: y = m*x + k
typedef struct {
    double m;
    double k;
} LinearParams;

typedef struct {
    double a;
    double h;
    double kq;
} QuadraticParams;

// Função Exponencial: y = a * b^x + c
typedef struct {
    double a;
    double b;
    double c;
} ExponentialParams;

// Função Logarítmica: y = a * log(x + b) + c
typedef struct {
    double a;
    double b;
    double c;
} LogarithmicParams;

// Função Racional: y = (a*x + b) / (c*x + d)
typedef struct {
    double a;
    double b;
    double c;
    double d;
} RationalParams;

// Função Trigonométrica: y = a * sin(b*x + c) + d
typedef struct {
    double a;
    double b;
    double c;
    double d;
} TrigonometricParams;

typedef struct {
    FunctionType type;
    union {
        LinearParams linear;
        QuadraticParams quadratic;
        ExponentialParams exponential;
        LogarithmicParams logarithmic;
        RationalParams rational;
        TrigonometricParams trigonometric;
    } params;
} MathFunction;

// Câmera
typedef struct {
    double center_x;
    double center_y;
    int screen_width;
    int screen_height;
} Camera;

// Player
typedef enum {
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_SHOOT
} PlayerState;

typedef struct {
    double x;          // posição X (agora móvel)
    double y;          // posição Y
    double y_base;     // Y base (chão)
    double x_min;      // limite esquerdo
    double x_max;      // limite direito
    double move_speed;
    PlayerState state;

    // Animação
    bool facing_right;
    int frame;
    int frames_idle;
    int frames_walk;
    int frames_shoot;
    double anim_fps;
    double frame_acc;

    // Idle breathing/sway animation
    double idle_timer;
    double idle_sway_offset;

    // Layout alternativo do shoot (2x4 grid)
    int shoot_cols;
    int shoot_rows;
    int shoot_frames_total;

    // Offset para pular frames cortados no walk
    int walk_frame_offset;

    // Sistema de vida (para boss fight)
    double health;
    double max_health;

    // Sistema de pulo
    double vy;              // Velocidade vertical
    bool is_jumping;        // Se está pulando
    bool on_ground;         // Se está no chão
    double jump_strength;   // Força do pulo
    double gravity;         // Gravidade

    ALLEGRO_BITMAP* sprite_idle;
    ALLEGRO_BITMAP* sprite_walk;
    ALLEGRO_BITMAP* sprite_shoot;
} Player;

// Flecha
typedef struct {
    bool active;
    double t;
    double x0;
    double y0;
    double vx;
    double vy;  // Velocidade vertical (usado após ricochete)
    double tmax;
    double current_x;
    double current_y;
    bool following_function;  // Se segue função ou física realista
    double gravity;           // Constante de gravidade (-9.8)
    double velocity_decay;    // Multiplicador de perda de velocidade (0.7 = 30% perda)
    int ricochets;            // Contador de ricochetes

    // Sistema de força do arco
    double charge_time;      // Tempo que segurou o arco
    double max_charge_time;  // Tempo máximo de carga (2 segundos)
    double charge_power;     // Multiplicador de velocidade (1.0 a 2.0)

    ALLEGRO_BITMAP* sprite;
} Arrow;

// Coletáveis
typedef enum {
    COLLECT_KEY,
    COLLECT_APPLE,
    COLLECT_APPLE_GOLD
} CollectibleType;

typedef struct {
    double x, y;
    double radius;
    CollectibleType type;
    bool collected;
} Collectible;

// Obstáculos
typedef enum {
    OBS_ROCK,
    OBS_WALL
} ObstacleType;

typedef struct {
    double x, y;
    double width, height;
    ObstacleType type;
    bool has_physics;
} Obstacle;

// Inimigos
typedef struct {
    double x, y;
    double move_speed;
    double move_range;
    double move_dir;
    double start_x;
    bool active;
    ALLEGRO_BITMAP* sprite;
} Enemy;

// Boss
typedef struct {
    double x, y;
    double health;
    double max_health;
    double shoot_timer;
    double shoot_cooldown;
    double move_speed;
    double move_range;
    double start_x;
    double move_dir;
    bool active;
    ALLEGRO_BITMAP* sprite;
} Boss;

// Projétil do Boss
typedef struct {
    bool active;
    double x, y;
    double vx, vy;
    double lifetime;
} BossArrow;

// Sistema de Especiais v2
typedef enum {
    SPECIAL_SUPER_SPEED,
    SPECIAL_SLOW_TIME,
    SPECIAL_TRIPLE_ARROW
} SpecialType;

typedef struct {
    SpecialType type;
    const char* name;
    const char* description;
} SpecialOption;

// Tutorial
typedef struct {
    const char* title;
    const char* content[15];  // Aumentado para 15 linhas
    int line_count;
} TutorialScreen;

// Fase
typedef enum {
    TINT_BRONZE = 0,
    TINT_SILVER = 1,
    TINT_GOLD = 2
} PhaseTint;

typedef struct {
    int arrows_allowed;
    double epsilon_px;
    PhaseTint color_tint;
    int collectible_count;
    Collectible items[16];
    int obstacle_count;
    Obstacle obstacles[12];
    int enemy_count;
    Enemy enemies[8];
    Boss boss;
    int boss_count;  // Número de bosses na fase (0-4)
    Boss bosses[4];  // Array de bosses para múltiplos bosses
    bool has_boss;
} Phase;

// Capítulo
typedef struct {
    const char* name;
    FunctionType function_type;
    ALLEGRO_COLOR theme_color;
    Phase test;
    Phase phases[3];
    Phase boss_phase;
} Chapter;

// Editor de parâmetros
typedef enum {
    PARAM_NONE,
    PARAM_M,       // Linear: inclinação
    PARAM_K,       // Linear: intercepto
    PARAM_A,       // Quadrática/outras: parâmetro A
    PARAM_H,       // Quadrática: vértice X / outras: parâmetro B
    PARAM_KQ,      // Quadrática: vértice Y / outras: parâmetro C
    PARAM_D        // Racional/Trigonométrica: parâmetro D
} ParamSelection;

typedef struct {
    char buffer[32];
    int cursor;
    ParamSelection selected;
    bool is_negative;
} ParamEditor;

// Barra de especial
typedef struct {
    double value;
} SpecialBar;

// UI
typedef struct {
    ALLEGRO_FONT* font_main;
    ALLEGRO_FONT* font_title;
    ALLEGRO_FONT* font_small;
    ParamEditor editor;
    SpecialBar special;
    bool preview_enabled;
} UI;

// Progresso do jogo
typedef struct {
    int unlocked_chapters;
    bool chapters_completed[6];
    bool boss_defeated[6];
} Progress;

// Jogo completo
typedef struct {
    GameState state;
    GameState paused_from_state;  // Estado antes de pausar
    int current_chapter;
    int current_phase;
    int arrows_remaining;

    Player player;
    Arrow arrow;
    MathFunction current_function;
    Camera camera;
    UI ui;
    Progress progress;

    bool key_up, key_down, key_space, key_left, key_right;
    bool key_space_down, was_space_down;  // Para detectar soltar
    bool key_r, key_p, key_h, key_c;  // key_c para trocar função
    bool key_enter, key_escape;
    bool key_shift;
    bool key_m, key_k, key_q, key_e;  // Controles com letras
    bool key_bracket_left, key_bracket_right;

    bool show_max_hint;
    double hint_timer;
    int chapter_menu_selection;
    bool show_grid;  // Toggle grid cartesiano com tecla P

    // Tutorial
    int tutorial_page;
    TutorialScreen* current_tutorial;
    int tutorial_total_pages;

    // Boss fight
    BossArrow boss_arrows[5];

    // Sistema de Especiais v2
    SpecialOption available_specials[3];
    int selected_special_index;
    SpecialType active_special;
    bool special_active;
    double special_timer;

    // Troca de funções
    double function_change_message_timer;

    Chapter chapters[6];

    // Backgrounds e sprites
    ALLEGRO_BITMAP* bg_home;
    ALLEGRO_BITMAP* bg_linear;
    ALLEGRO_BITMAP* bg_expo;
    ALLEGRO_BITMAP* bg_quadratica;
    ALLEGRO_BITMAP* bg_logaritmica;
    ALLEGRO_BITMAP* bg_racional;
    ALLEGRO_BITMAP* bg_trigonometrica;
    ALLEGRO_BITMAP* title_background;
    ALLEGRO_BITMAP* logo;
    ALLEGRO_BITMAP* sprite_boss;
    ALLEGRO_BITMAP* sprite_obstacle;
    ALLEGRO_BITMAP* sprite_enemy;
    ALLEGRO_BITMAP* sprite_key;
    ALLEGRO_BITMAP* sprite_apple;
    ALLEGRO_BITMAP* sprite_apple_gold;
    ALLEGRO_FONT* font_title_large;
} Game;

// ========================================================================
// PROTÓTIPOS DE FUNÇÕES
// ========================================================================

void draw_text_scaled(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y,
    int flags, float scale, const char* text);
bool check_position_collision(double x, double y, double radius,
    Collectible* collectibles, int collectible_count,
    Obstacle* obstacles, int obstacle_count,
    Enemy* enemies, int enemy_count,
    int skip_collectible, int skip_obstacle, int skip_enemy);
void randomize_phase_layout(Phase* phase, int seed);

// ========================================================================
// FUNÇÕES - MATEMÁTICA
// ========================================================================

double fn_eval(const MathFunction* fn, double x) {
    switch (fn->type) {
    case FN_LINEAR:
        return fn->params.linear.m * x + fn->params.linear.k;
    case FN_QUADRATIC: {
        double dx = x - fn->params.quadratic.h;
        return fn->params.quadratic.a * dx * dx + fn->params.quadratic.kq;
    }
    case FN_EXPONENTIAL:
        return fn->params.exponential.a * pow(fn->params.exponential.b, x) + fn->params.exponential.c;
    case FN_LOGARITHMIC:
        if (x + fn->params.logarithmic.b <= 0) return 0.0;  // Evitar log de número negativo
        return fn->params.logarithmic.a * log(x + fn->params.logarithmic.b) + fn->params.logarithmic.c;
    case FN_RATIONAL: {
        double denominator = fn->params.rational.c * x + fn->params.rational.d;
        if (fabs(denominator) < 0.001) return 0.0;  // Evitar divisão por zero
        return (fn->params.rational.a * x + fn->params.rational.b) / denominator;
    }
    case FN_TRIGONOMETRIC:
        return fn->params.trigonometric.a * sin(fn->params.trigonometric.b * x + fn->params.trigonometric.c) + fn->params.trigonometric.d;
    default:
        return 0.0;
    }
}

// Inicializar parâmetros padrão para cada tipo de função
void init_function_params(MathFunction* fn) {
    switch (fn->type) {
    case FN_LINEAR:
        fn->params.linear.m = 1.0;
        fn->params.linear.k = 0.0;
        break;
    case FN_QUADRATIC:
        fn->params.quadratic.a = 0.5;
        fn->params.quadratic.h = 0.0;
        fn->params.quadratic.kq = 0.0;
        break;
    case FN_EXPONENTIAL:
        fn->params.exponential.a = 1.0;
        fn->params.exponential.b = 1.1;  // Base ligeiramente > 1
        fn->params.exponential.c = 0.0;
        break;
    case FN_LOGARITHMIC:
        fn->params.logarithmic.a = 2.0;
        fn->params.logarithmic.b = 1.0;
        fn->params.logarithmic.c = 0.0;
        break;
    case FN_RATIONAL:
        fn->params.rational.a = 2.0;
        fn->params.rational.b = 0.0;
        fn->params.rational.c = 0.1;  // Pequeno para permitir assíntotas visíveis!
        fn->params.rational.d = 0.5;  // Pequeno - quando d=0, assíntota vertical em x=-b/c
        break;
    case FN_TRIGONOMETRIC:
        fn->params.trigonometric.a = 4.0;  // AUMENTADO! (era 2.0) - ondas MUITO mais altas
        fn->params.trigonometric.b = 0.6;  // REDUZIDO (era 1.0) - ondas mais longas
        fn->params.trigonometric.c = 0.0;
        fn->params.trigonometric.d = 0.0;
        break;
    }
}

// Trocar para próxima função (ciclo)
void cycle_function_type(MathFunction* fn) {
    fn->type = (FunctionType)((fn->type + 1) % 6);
    init_function_params(fn);
}

// ========================================================================
// FUNÇÕES - CÂMERA
// ========================================================================

void camera_init(Camera* cam, int screen_w, int screen_h) {
    cam->center_x = 0.0;
    cam->center_y = 0.0;
    cam->screen_width = screen_w;
    cam->screen_height = screen_h;
}

void world_to_screen(const Camera* cam, double wx, double wy, int* sx, int* sy) {
    double dx = wx - cam->center_x;
    double dy = wy - cam->center_y;
    *sx = (int)(cam->screen_width / 2.0 + dx * PIXELS_PER_UNIT);
    *sy = (int)(cam->screen_height / 2.0 - dy * PIXELS_PER_UNIT);
}

// ========================================================================
// FUNÇÕES - PLAYER
// ========================================================================

void player_init(Player* p, double x0) {
    p->x = x0;
    p->y_base = -8.5;  // Alinhado com a plataforma visual
    p->y = p->y_base;
    p->x_min = -18.0;  // Aumentado: permite andar em quase toda a tela
    p->x_max = 18.0;   // Aumentado: permite andar em quase toda a tela
    p->move_speed = 9.0;  // AUMENTADO: mais rápido e frenético!
    p->state = PLAYER_IDLE;

    // Animação (sprite original vai pra ESQUERDA)
    p->facing_right = false;  // Começa virado pra esquerda
    p->frame = 0;
    p->frames_idle = 4;
    p->frames_walk = 4;
    p->frames_shoot = 6;
    p->anim_fps = 18.0;  // AUMENTADO: animação mais rápida e frenética!
    p->frame_acc = 0.0;

    // Idle breathing/sway
    p->idle_timer = 0.0;
    p->idle_sway_offset = 0.0;

    // Layout alternativo shoot (2x4 grid)
    p->shoot_cols = 4;
    p->shoot_rows = 2;
    p->shoot_frames_total = 8;

    // Offset para pular frames cortados
    p->walk_frame_offset = 0;

    // Sistema de vida
    p->health = 200.0;      // AUMENTADO! Era 100.0
    p->max_health = 200.0;  // Player mais resistente

    // Sistema de pulo
    p->vy = 0.0;
    p->is_jumping = false;
    p->on_ground = true;
    p->jump_strength = 12.0;  // Força do pulo
    p->gravity = -25.0;       // Gravidade (negativo = para baixo)

    p->sprite_idle = NULL;
    p->sprite_walk = NULL;
    p->sprite_shoot = NULL;
}

void player_update(Player* p, double dt, bool moving_left, bool moving_right) {
    // Movimento horizontal
    bool moving = false;
    if (moving_left) {
        p->x -= p->move_speed * dt;
        if (p->x < p->x_min) p->x = p->x_min;
        p->facing_right = false;
        moving = true;
    }
    if (moving_right) {
        p->x += p->move_speed * dt;
        if (p->x > p->x_max) p->x = p->x_max;
        p->facing_right = true;
        moving = true;
    }

    // Estado: se não está atirando, muda para walk ou idle
    if (p->state != PLAYER_SHOOT) {
        p->state = moving ? PLAYER_WALK : PLAYER_IDLE;
    }

    // Idle breathing/sway: movimento sutil quando parado
    if (p->state == PLAYER_IDLE) {
        p->idle_timer += dt;
        // Balança suavemente (vai e volta em 2 segundos)
        p->idle_sway_offset = sin(p->idle_timer * 3.14159 * 0.5) * 3.0;  // ±3 pixels
    }
    else {
        p->idle_timer = 0.0;
        p->idle_sway_offset = 0.0;
    }

    // Sistema de pulo com gravidade
    if (p->on_ground) {
        p->y = p->y_base;
        p->vy = 0.0;
    }
    else {
        // Aplicar gravidade quando no ar
        p->vy += p->gravity * dt;
        p->y += p->vy * dt;

        // Verificar se tocou o chão
        if (p->y <= p->y_base) {
            p->y = p->y_base;
            p->vy = 0.0;
            p->on_ground = true;
            p->is_jumping = false;
        }
    }

    // Animação
    if (p->state == PLAYER_IDLE) {
        // IDLE: sempre no frame 0 (sem animação)
        p->frame = 0;
    }
    else {
        // WALK e SHOOT: animar normalmente
        p->frame_acc += dt;
        double frame_time = 1.0 / p->anim_fps;

        if (p->frame_acc >= frame_time) {
            p->frame_acc -= frame_time;

            switch (p->state) {
            case PLAYER_WALK:
                p->frame = (p->frame + 1) % p->frames_walk;
                break;
            case PLAYER_SHOOT:
                // Não-loop: para no último frame
                if (p->frame < p->frames_shoot - 1) {
                    p->frame++;
                }
                break;
            default:
                break;
            }
        }
    }
}

// Função auxiliar para recortar frame de grid (para shoot 2x4)
void draw_grid_frame(ALLEGRO_BITMAP* sheet, int frame, int cols, int rows,
    int dest_x, int dest_y, int dest_w, int dest_h, int flags) {
    int sheet_w = al_get_bitmap_width(sheet);
    int sheet_h = al_get_bitmap_height(sheet);
    int cell_w = sheet_w / cols;
    int cell_h = sheet_h / rows;

    int col = frame % cols;
    int row = frame / cols;

    int sx = col * cell_w;
    int sy = row * cell_h;

    al_draw_scaled_bitmap(sheet, sx, sy, cell_w, cell_h,
        dest_x, dest_y, dest_w, dest_h, flags);
}

void player_draw(const Player* p, int screen_x, int screen_y) {
    ALLEGRO_BITMAP* current_sprite = NULL;
    // Sprite original vai pra ESQUERDA, então invertemos quando vai pra DIREITA
    int flags = p->facing_right ? ALLEGRO_FLIP_HORIZONTAL : 0;

    switch (p->state) {
    case PLAYER_IDLE:
        current_sprite = p->sprite_idle;
        break;
    case PLAYER_WALK:
        current_sprite = p->sprite_walk;
        break;
    case PLAYER_SHOOT:
        current_sprite = p->sprite_shoot;
        break;
    }

    if (current_sprite) {
        int bmp_w = al_get_bitmap_width(current_sprite);
        int bmp_h = al_get_bitmap_height(current_sprite);

        // Calcular tamanho de cada frame baseado no número de frames
        int num_frames = 1;
        if (p->state == PLAYER_IDLE) num_frames = p->frames_idle;
        else if (p->state == PLAYER_WALK) num_frames = p->frames_walk;
        else if (p->state == PLAYER_SHOOT) num_frames = p->frames_shoot;

        // Calcular dimensões do frame individual
        int frame_width_src = bmp_w / num_frames;
        int frame_height_src = bmp_h;

        // Tamanho visual na tela (escalado)
        int visual_width = 128;
        int visual_height = 128;

        // Shoot: verificar layouts especiais
        if (p->state == PLAYER_SHOOT) {
            if (bmp_w == 384 && bmp_h == 64) {
                // Layout 1x6 (384x64)
                frame_width_src = 64;
                frame_height_src = 64;
                visual_width = 96;
                visual_height = 96;
            }
            else if (p->frames_shoot == 3 && bmp_h < bmp_w / 2) {
                // player-shoot-3.png: 3 frames horizontais (ex: 192x64 = 3x 64x64)
                frame_width_src = bmp_w / 3;
                frame_height_src = bmp_h;
                visual_width = 128;
                visual_height = 128;
                // IMPORTANTE: animar os 3 frames!
            }
            else if (p->shoot_cols > 1) {
                // Layout 2x4 ou outro grid
                draw_grid_frame(current_sprite, p->frame, p->shoot_cols, p->shoot_rows,
                    screen_x - visual_width / 2, screen_y - visual_height,
                    visual_width, visual_height, flags);
                return;
            }
        }

        // Idle e Walk: desenhar frame da linha horizontal
        int actual_frame = p->frame;
        if (p->state == PLAYER_WALK) {
            actual_frame = p->frame + p->walk_frame_offset;
        }
        int sx = actual_frame * frame_width_src;

        // Aplicar movimento sutil (idle sway) na posição Y
        int draw_y = screen_y - visual_height;
        if (p->state == PLAYER_IDLE) {
            draw_y += (int)p->idle_sway_offset;
        }

        // Usar al_draw_scaled_bitmap para escalar do tamanho original para o visual
        al_draw_scaled_bitmap(current_sprite,
            sx, 0, frame_width_src, frame_height_src,
            screen_x - visual_width / 2, draw_y,
            visual_width, visual_height, flags);
    }
    else {
        // Fallback: boneco palito melhorado
        ALLEGRO_COLOR color = al_map_rgb(100, 150, 255);

        // Corpo
        al_draw_line(screen_x, screen_y, screen_x, screen_y + 20, color, 3);
        // Cabeça
        al_draw_filled_circle(screen_x, screen_y - 10, 8, color);
        // Braços
        int arm_dir = p->facing_right ? 1 : -1;
        al_draw_line(screen_x, screen_y + 5, screen_x - 15 * arm_dir, screen_y + 15, color, 2);
        al_draw_line(screen_x, screen_y + 5, screen_x + 15 * arm_dir, screen_y + 10, color, 2);
        // Pernas
        al_draw_line(screen_x, screen_y + 20, screen_x - 8, screen_y + 35, color, 2);
        al_draw_line(screen_x, screen_y + 20, screen_x + 8, screen_y + 35, color, 2);
    }
}

// ========================================================================
// FUNÇÕES - FLECHA
// ========================================================================

void arrow_init(Arrow* arrow) {
    arrow->active = false;
    arrow->t = 0.0;
    arrow->x0 = 0.0;
    arrow->y0 = 0.0;
    arrow->vx = 15.0;  // AUMENTADO: velocidade base mais rápida (era 8.0)
    arrow->vy = 0.0;
    arrow->tmax = 5.0;
    arrow->current_x = 0.0;
    arrow->current_y = 0.0;
    arrow->following_function = true;
    arrow->gravity = -9.8;
    arrow->velocity_decay = 0.7;
    arrow->ricochets = 0;

    // Sistema de força
    arrow->charge_time = 0.0;
    arrow->max_charge_time = 2.0;  // 2 segundos = força máxima
    arrow->charge_power = 1.0;

    arrow->sprite = NULL;
}

void arrow_shoot(Arrow* arrow, double x0, double y0, double vx, double charge_power) {
    arrow->active = true;
    arrow->t = 0.0;
    arrow->x0 = x0;
    arrow->y0 = y0;
    arrow->vx = vx * charge_power;  // Aplicar multiplicador de força!
    arrow->vy = 0.0;
    arrow->current_x = x0;
    arrow->current_y = y0;
    arrow->following_function = true;  // Começa seguindo a função
    arrow->ricochets = 0;
    arrow->charge_power = charge_power;
}

void arrow_update(Arrow* arrow, double dt, const MathFunction* fn) {
    if (!arrow->active) return;

    arrow->t += dt;

    if (arrow->following_function) {
        // Modo normal: segue a função matemática
        arrow->current_x = arrow->x0 + arrow->vx * arrow->t;

        // CORRIGIDO: usar distância percorrida, não posição absoluta!
        double distance = arrow->vx * arrow->t;
        arrow->current_y = arrow->y0 + fn_eval(fn, fabs(distance));
    }
    else {
        // Modo física realista: após ricochete
        arrow->current_x += arrow->vx * dt;
        arrow->vy += arrow->gravity * dt;  // Aplicar gravidade
        arrow->current_y += arrow->vy * dt;
    }

    // Desativar se sair dos limites ou tempo excedido
    if (arrow->t > arrow->tmax || arrow->current_x > 25.0 || arrow->current_x < -25.0 || arrow->current_y < -15.0) {
        arrow->active = false;
    }
}

void arrow_draw(const Arrow* arrow, int screen_x, int screen_y) {
    if (!arrow->active) return;

    // FLECHA MAIOR E MAIS DETALHADA
    ALLEGRO_COLOR color = al_map_rgb(200, 200, 50);
    ALLEGRO_COLOR tip_color = al_map_rgb(255, 100, 100);
    ALLEGRO_COLOR shaft_color = al_map_rgb(139, 69, 19);

    // Corpo da flecha (mais grosso)
    al_draw_line(screen_x - 20, screen_y, screen_x + 20, screen_y, shaft_color, 4);

    // Ponta da flecha (maior)
    al_draw_filled_triangle(screen_x + 20, screen_y,
        screen_x + 8, screen_y - 8,
        screen_x + 8, screen_y + 8, tip_color);

    // Penas da flecha (maiores)
    al_draw_filled_triangle(screen_x - 20, screen_y,
        screen_x - 12, screen_y - 6,
        screen_x - 12, screen_y + 6, color);
    al_draw_filled_triangle(screen_x - 20, screen_y,
        screen_x - 16, screen_y - 4,
        screen_x - 16, screen_y + 4, color);
}

void arrow_draw_preview(double x0, double y0, double vx, const MathFunction* fn,
    ALLEGRO_COLOR color, const Camera* cam) {
    const int SAMPLES = 200;  // Mais pontos = linha mais suave
    const double DT = 0.03;   // Tempo menor entre pontos
    int prev_sx = -1, prev_sy = -1;

    for (int i = 0; i < SAMPLES; i++) {
        double t = i * DT;
        double x = x0 + vx * t;

        // CORRIGIDO: avaliar função na DISTÂNCIA percorrida, não na posição absoluta
        double distance = vx * t;  // Distância percorrida (positiva ou negativa)
        double y = y0 + fn_eval(fn, fabs(distance));  // Sempre para cima a partir de y0

        // Para se sair dos limites visíveis
        if (x > 20.0 || x < -20.0 || y > 15.0 || y < -15.0) break;

        int sx, sy;
        world_to_screen(cam, x, y, &sx, &sy);

        if (i > 0 && prev_sx >= 0) {
            // Linha mais grossa e com alpha para ficar mais visível
            al_draw_line(prev_sx, prev_sy, sx, sy, color, 3);
        }

        // Desenha pontos ao longo da linha (efeito tracejado)
        if (i % 10 == 0) {
            al_draw_filled_circle(sx, sy, 2, color);
        }

        prev_sx = sx;
        prev_sy = sy;
    }
}

// Colisão flecha com obstáculos (ricochete com física realista)
bool check_arrow_obstacle_collision(Arrow* arrow, Obstacle* obstacles, int count) {
    if (!arrow->active || !arrow->following_function || !obstacles || count <= 0) return false;

    for (int i = 0; i < count; i++) {
        Obstacle* obs = &obstacles[i];
        if (!obs->has_physics) continue;

        // Verificar colisão simples (AABB)
        double dx = fabs(arrow->current_x - obs->x);
        double dy = fabs(arrow->current_y - obs->y);

        if (dx < obs->width / 2.0 && dy < obs->height / 2.0) {
            // Colidiu! Aplicar ricochete com física realista
            arrow->following_function = false;

            // Calcular ângulo de reflexão baseado na face de colisão
            bool hit_horizontal = dx / obs->width > dy / obs->height;

            if (hit_horizontal) {
                // Colisão lateral - inverter vx
                arrow->vx = -arrow->vx * arrow->velocity_decay;
            }
            else {
                // Colisão topo/base - inverter vy (calcular baseado na derivada)
                double slope = 0.0;
                if (arrow->vx != 0) {
                    slope = (arrow->current_y - arrow->y0) / (arrow->current_x - arrow->x0);
                }
                arrow->vy = -slope * fabs(arrow->vx) * arrow->velocity_decay;
            }

            // Ajustar posição para fora do obstáculo
            if (hit_horizontal) {
                arrow->current_x = obs->x + (arrow->vx > 0 ? obs->width / 2.0 + 0.1 : -obs->width / 2.0 - 0.1);
            }
            else {
                arrow->current_y = obs->y + (arrow->vy > 0 ? obs->height / 2.0 + 0.1 : -obs->height / 2.0 - 0.1);
            }

            arrow->ricochets++;
            arrow->t = 0.0;  // Reset timer

            printf("[RICOCHET] Flecha ricocheteou! Contagem: %d\n", arrow->ricochets);
            return true;
        }
    }
    return false;
}

// Atualizar inimigos (movimento de patrulha)
void enemies_update(Enemy* enemies, int count, double dt) {
    if (!enemies || count <= 0) return;  // CRÍTICO: evitar null pointer

    for (int i = 0; i < count; i++) {
        Enemy* enemy = &enemies[i];
        if (!enemy->active) continue;

        // Movimento de patrulha
        enemy->x += enemy->move_speed * enemy->move_dir * dt;

        // Inverter direção nos limites
        if (enemy->move_dir > 0 && enemy->x > enemy->start_x + enemy->move_range) {
            enemy->move_dir = -1.0;
        }
        else if (enemy->move_dir < 0 && enemy->x < enemy->start_x - enemy->move_range) {
            enemy->move_dir = 1.0;
        }
    }
}

// Colisão flecha com inimigos
bool check_arrow_enemy_collision(Arrow* arrow, Enemy* enemies, int count) {
    if (!arrow->active || !enemies || count <= 0) return false;

    for (int i = 0; i < count; i++) {
        Enemy* enemy = &enemies[i];
        if (!enemy->active) continue;

        double dx = arrow->current_x - enemy->x;
        double dy = arrow->current_y - enemy->y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist < 0.8) {  // Raio de colisão
            arrow->active = false;
            printf("[COLISAO] Flecha atingiu inimigo! Flecha perdida.\n");
            return true;
        }
    }
    return false;
}

// ========================================================================
// FUNÇÕES - BOSS
// ========================================================================

// Inicializar projéteis do boss
void boss_arrows_init(BossArrow* arrows, int count) {
    for (int i = 0; i < count; i++) {
        arrows[i].active = false;
        arrows[i].lifetime = 0.0;
    }
}

// Boss atira um projétil
void boss_shoot(Boss* boss, BossArrow* arrows, int max_arrows, double target_x, double target_y) {
    // Encontrar slot vazio
    for (int i = 0; i < max_arrows; i++) {
        if (!arrows[i].active) {
            arrows[i].active = true;
            arrows[i].x = boss->x;
            arrows[i].y = boss->y;
            arrows[i].lifetime = 5.0;  // 5 segundos de vida

            // Calcular direção para o alvo
            double dx = target_x - boss->x;
            double dy = target_y - boss->y;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist > 0.1) {
                double speed = 7.0;  // REDUZIDO! Era 10.0, agora 7.0 (mais esquivável)
                arrows[i].vx = (dx / dist) * speed;
                arrows[i].vy = (dy / dist) * speed;
            }
            else {
                arrows[i].vx = -7.0;
                arrows[i].vy = 0.0;
            }

            printf("[BOSS] Atirou projétil! Velocidade: (%.2f, %.2f)\n", arrows[i].vx, arrows[i].vy);
            return;
        }
    }
}

// Atualizar boss (movimento e tiro)
void boss_update(Boss* boss, BossArrow* arrows, int max_arrows, double dt, double player_x, double player_y) {
    if (!boss->active) return;

    // Movimento de patrulha
    boss->x += boss->move_speed * boss->move_dir * dt;

    // Inverter direção nos limites
    if (boss->move_dir > 0 && boss->x > boss->start_x + boss->move_range) {
        boss->move_dir = -1.0;
    }
    else if (boss->move_dir < 0 && boss->x < boss->start_x - boss->move_range) {
        boss->move_dir = 1.0;
    }

    // Sistema de tiro
    boss->shoot_timer += dt;
    if (boss->shoot_timer >= boss->shoot_cooldown) {
        boss->shoot_timer = 0.0;
        boss_shoot(boss, arrows, max_arrows, player_x, player_y);
    }
}

// Atualizar projéteis do boss
void boss_arrows_update(BossArrow* arrows, int count, double dt) {
    for (int i = 0; i < count; i++) {
        if (!arrows[i].active) continue;

        arrows[i].x += arrows[i].vx * dt;
        arrows[i].y += arrows[i].vy * dt;
        arrows[i].lifetime -= dt;

        // Desativar se tempo acabou ou saiu dos limites
        if (arrows[i].lifetime <= 0.0 ||
            arrows[i].x > 25.0 || arrows[i].x < -25.0 ||
            arrows[i].y > 15.0 || arrows[i].y < -15.0) {
            arrows[i].active = false;
        }
    }
}

// Colisão projétil do boss com flecha do jogador
bool check_boss_arrow_player_arrow_collision(BossArrow* boss_arrows, int boss_count, Arrow* player_arrow) {
    if (!player_arrow->active) return false;

    for (int i = 0; i < boss_count; i++) {
        if (!boss_arrows[i].active) continue;

        double dx = player_arrow->current_x - boss_arrows[i].x;
        double dy = player_arrow->current_y - boss_arrows[i].y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist < 0.5) {  // Raio de colisão
            // Ambas as flechas são destruídas
            boss_arrows[i].active = false;
            player_arrow->active = false;
            printf("[COLISAO] Flechas colidiram! Ambas destruídas.\n");
            return true;
        }
    }
    return false;
}

// Colisão flecha do jogador com boss
bool check_arrow_boss_collision(Arrow* arrow, Boss* boss) {
    if (!arrow->active || !boss->active) return false;

    double dx = arrow->current_x - boss->x;
    double dy = arrow->current_y - boss->y;
    double dist = sqrt(dx * dx + dy * dy);

    // Debug: mostrar distância
    if (arrow->active) {
        printf("[DEBUG] Flecha pos: (%.2f, %.2f) | Boss pos: (%.2f, %.2f) | Dist: %.2f\n",
            arrow->current_x, arrow->current_y, boss->x, boss->y, dist);
    }

    if (dist < 2.5) {  // AUMENTADO: raio de colisão do boss (era 1.5, agora 2.5)
        boss->health -= 12.0;  // AUMENTADO! Cada flecha tira 12 de vida (era 10)
        arrow->active = false;

        printf("[BOSS HIT!] Acertou! Dano: 12 | Vida restante: %.0f/%.0f\n", boss->health, boss->max_health);

        if (boss->health <= 0) {
            boss->active = false;
            printf("[BOSS DERROTADO!] Vitoria!\n");
        }
        return true;
    }
    return false;
}

// Colisão projétil do boss com player (NOVO!)
bool check_boss_arrow_player_collision(BossArrow* arrows, int count, Player* player) {
    for (int i = 0; i < count; i++) {
        if (!arrows[i].active) continue;

        double dx = arrows[i].x - player->x;
        double dy = arrows[i].y - player->y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist < 0.8) {  // Raio de colisão com player
            player->health -= 10.0;  // REDUZIDO! Era 20 HP, agora 10 HP
            arrows[i].active = false;

            printf("[DANO] Player atingido! -10 HP | Vida: %.0f/%.0f HP\n", player->health, player->max_health);

            if (player->health <= 0) {
                printf("[GAME OVER] Player morreu!\n");
                return true;
            }
            return true;
        }
    }
    return false;
}

// ========================================================================
// FUNÇÕES - TUTORIAIS
// ========================================================================

// Tutoriais globais (inicializados uma vez)
static TutorialScreen tutorial_intro_screens[] = {
    {
        "BEM-VINDO AO ARCHER!",
        {
            "Aprenda funcoes matematicas de forma divertida!",
            "",
            "Objetivo: Acerte as CHAVES usando trajetorias matematicas.",
            "Complete todas as fases para dominar cada funcao!",
            "",
            "Pressione ENTER para continuar..."
        },
        6
    },
    {
        "COMO JOGAR",
        {
            "Use as SETAS <- -> para mover o arqueiro",
            "Segure ESPACO para carregar a flecha",
            "Solte ESPACO para atirar!",
            "",
            "Quanto mais tempo segurar, mais forte o tiro!",
            "A flecha segue a funcao matematica escolhida.",
            "",
            "Pressione ENTER para continuar..."
        },
        8
    },
    {
        "COLETAVEIS",
        {
            "CHAVE (amarela): Objetivo principal! Pegue 3 para ganhar.",
            "MACA (vermelha): +10% de energia especial",
            "MACA DOURADA: +30% de energia especial",
            "",
            "Use o ESPECIAL (tecla H) quando a barra encher!",
            "Especial te da +1 flecha extra e mostra dicas!",
            "",
            "Pressione ENTER para continuar..."
        },
        8
    },
    {
        "OBSTACULOS E INIMIGOS",
        {
            "PEDRAS: A flecha RICOCHETEARA com fisica realista!",
            "ESPINHOS: Decorativos, nao afetam a flecha.",
            "INIMIGOS (vermelhos): Se a flecha acertar, PERDE A FLECHA!",
            "",
            "Cuidado: apos ricochetear, a flecha para de seguir a funcao",
            "e passa a ter gravidade e perda de velocidade!",
            "",
            "Pressione ENTER para jogar!"
        },
        8
    }
};

// Tutoriais por função
static TutorialScreen tutorial_linear[] = {
    {
        "FUNCAO LINEAR",
        {
            "Equacao: y = mx + k",
            "",
            "m: Inclinacao da reta (coeficiente angular)",
            "k: Intercepto (onde cruza o eixo Y)",
            "",
            "CONTROLES:",
            "[M] seleciona m | [K] seleciona k",
            "[Q/E] ou numeros ajustam o valor",
            "[ENTER] confirma a alteracao",
            "",
            "Pressione ENTER para iniciar!"
        },
        11
    }
};

static TutorialScreen tutorial_quadratic[] = {
    {
        "FUNCAO QUADRATICA",
        {
            "Equacao: y = a(x-h)^2 + k",
            "",
            "a: Abertura da parabola (+ abre pra cima, - pra baixo)",
            "h: Deslocamento horizontal do vertice",
            "k: Deslocamento vertical do vertice",
            "",
            "CONTROLES:",
            "[M] seleciona a | [H] seleciona h | [K] seleciona k",
            "[Q/E] ou numeros ajustam o valor",
            "[ENTER] confirma a alteracao",
            "",
            "Pressione ENTER para iniciar!"
        },
        11
    }
};

static TutorialScreen tutorial_exponential[] = {
    {
        "FUNCAO EXPONENCIAL",
        {
            "Equacao: y = a * b^x + c",
            "",
            "a: Amplitude (tamanho do crescimento)",
            "b: Base (velocidade de crescimento)",
            "c: Deslocamento vertical",
            "",
            "CONTROLES:",
            "[M] seleciona a | [H] seleciona b | [K] seleciona c",
            "[Q/E] ou numeros ajustam o valor",
            "[ENTER] confirma a alteracao",
            "",
            "Pressione ENTER para iniciar!"
        },
        11
    }
};

static TutorialScreen tutorial_logarithmic[] = {
    {
        "FUNCAO LOGARITMICA",
        {
            "Equacao: y = a * log(x+b) + c",
            "",
            "a: Amplitude (escala vertical)",
            "b: Deslocamento horizontal (deve manter x+b > 0)",
            "c: Deslocamento vertical",
            "",
            "CONTROLES:",
            "[M] seleciona a | [H] seleciona b | [K] seleciona c",
            "[Q/E] ou numeros ajustam o valor",
            "[ENTER] confirma a alteracao",
            "",
            "Pressione ENTER para iniciar!"
        },
        11
    }
};

static TutorialScreen tutorial_rational[] = {
    {
        "FUNCAO RACIONAL",
        {
            "Equacao: y = (ax + b) / (cx + d)",
            "",
            "Divisao de dois polinomios",
            "Cuidado: se cx + d = 0, temos uma assintota!",
            "",
            "CONTROLES:",
            "[M] seleciona a | [H] seleciona b | [K] seleciona c",
            "[[] seleciona d | [Q/E] ou numeros ajustam",
            "[ENTER] confirma a alteracao",
            "",
            "Pressione ENTER para iniciar!"
        },
        12
    }
};

static TutorialScreen tutorial_trigonometric[] = {
    {
        "FUNCAO TRIGONOMETRICA",
        {
            "Equacao: y = a * sin(bx + c) + d",
            "",
            "a: Amplitude (altura da onda)",
            "b: Frequencia (quantas ondas por unidade)",
            "c: Fase (deslocamento horizontal)",
            "d: Deslocamento vertical",
            "",
            "CONTROLES:",
            "[M] seleciona a | [H] seleciona b | [K] seleciona c",
            "[[] seleciona d | [Q/E] ou numeros ajustam",
            "[ENTER] confirma a alteracao",
            "",
            "Pressione ENTER para iniciar!"
        },
        12
    }
};

// Desenhar tela de tutorial com estética de lousa matemática
void draw_tutorial_screen_with_game(const TutorialScreen* screen, ALLEGRO_FONT* font, int page, int total_pages, Game* game) {
    // Background MELHORADO - gradiente verde escuro suave (lousa)
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        float ratio = (float)i / SCREEN_HEIGHT;
        ALLEGRO_COLOR line_color = al_map_rgb(
            (int)(30 + 15 * ratio),   // 30 → 45
            (int)(55 + 10 * ratio),   // 55 → 65 (verde)
            (int)(30 + 15 * ratio)    // 30 → 45
        );
        al_draw_line(0, i, SCREEN_WIDTH, i, line_color, 1);
    }

    // Borda da lousa (moldura de madeira marrom)
    al_draw_filled_rounded_rectangle(35, 35, SCREEN_WIDTH - 35, SCREEN_HEIGHT - 35,
        12, 12, al_map_rgb(80, 60, 40));  // Moldura grossa
    al_draw_filled_rounded_rectangle(45, 45, SCREEN_WIDTH - 45, SCREEN_HEIGHT - 45,
        10, 10, al_map_rgb(35, 55, 35));  // Lousa interna
    al_draw_rounded_rectangle(45, 45, SCREEN_WIDTH - 45, SCREEN_HEIGHT - 45,
        10, 10, al_map_rgb(200, 200, 150), 2);  // Borda interna clara

    // Título (giz amarelo claro) - MUITO MAIOR!
    draw_text_scaled(font, al_map_rgba(0, 0, 0, 100), SCREEN_WIDTH / 2 + 3, 83,
        ALLEGRO_ALIGN_CENTER, 3.5, screen->title);  // 3.5 (era 2.5)
    draw_text_scaled(font, al_map_rgb(255, 255, 200), SCREEN_WIDTH / 2, 80,
        ALLEGRO_ALIGN_CENTER, 3.5, screen->title);

    // Linha divisória sob o título (giz branco)
    al_draw_line(100, 140, SCREEN_WIDTH - 100, 140, al_map_rgb(220, 220, 220), 2);

    // Conteúdo (giz branco) - FONTE MUITO MAIOR!
    int y_pos = 180;
    for (int i = 0; i < screen->line_count; i++) {
        // Verifica se NÃO é nulo E DEPOIS verifica o tamanho
        if (screen->content[i] != NULL && strlen(screen->content[i]) > 0) {
            // Sombra leve
            draw_text_scaled(font, al_map_rgba(0, 0, 0, 60), 122, y_pos + 2, 0, 2.2, screen->content[i]);
            // Texto principal
            draw_text_scaled(font, al_map_rgb(240, 240, 240), 120, y_pos, 0, 2.2, screen->content[i]);
        }
        y_pos += 45;  // Mais espaço para texto maior
    }

    // Indicador de página (giz cinza) - maior
    char page_text[32];
    snprintf(page_text, sizeof(page_text), "Pagina %d/%d", page + 1, total_pages);
    draw_text_scaled(font, al_map_rgb(180, 180, 180), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80,
        ALLEGRO_ALIGN_CENTER, 1.5, page_text);

    // Dica de navegação - maior
    if (page < total_pages - 1) {
        draw_text_scaled(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50,
            ALLEGRO_ALIGN_CENTER, 1.6, "ENTER para proximo | ESC para pular");
    }

    // Se for tutorial de coletáveis (página 2 do intro), mostrar sprites!
    if (page == 2 && total_pages == 4) {
        int icon_y = 450;
        int icon_size = 64;
        int spacing = 250;
        int start_x = SCREEN_WIDTH / 2 - spacing;

        // CHAVE
        if (game->sprite_key) {
            int sw = al_get_bitmap_width(game->sprite_key);
            int sh = al_get_bitmap_height(game->sprite_key);
            al_draw_scaled_bitmap(game->sprite_key, 0, 0, sw, sh,
                start_x - icon_size / 2, icon_y - icon_size / 2, icon_size, icon_size, 0);
        }
        else {
            al_draw_filled_circle(start_x, icon_y, 20, al_map_rgb(255, 255, 100));
        }

        // MAÇÃ
        if (game->sprite_apple) {
            int sw = al_get_bitmap_width(game->sprite_apple);
            int sh = al_get_bitmap_height(game->sprite_apple);
            al_draw_scaled_bitmap(game->sprite_apple, 0, 0, sw, sh,
                start_x + spacing - icon_size / 2, icon_y - icon_size / 2, icon_size, icon_size, 0);
        }
        else {
            al_draw_filled_circle(start_x + spacing, icon_y, 20, al_map_rgb(255, 50, 50));
        }

        // MAÇÃ DOURADA
        if (game->sprite_apple_gold) {
            int sw = al_get_bitmap_width(game->sprite_apple_gold);
            int sh = al_get_bitmap_height(game->sprite_apple_gold);
            al_draw_scaled_bitmap(game->sprite_apple_gold, 0, 0, sw, sh,
                start_x + spacing * 2 - icon_size / 2, icon_y - icon_size / 2, icon_size, icon_size, 0);
        }
        else {
            al_draw_filled_circle(start_x + spacing * 2, icon_y, 20, al_map_rgb(255, 200, 50));
        }
    }

    // Se for tutorial de obstáculos/inimigos (página 3), mostrar sprites!
    if (page == 3 && total_pages == 4) {
        int icon_y = 500;
        int icon_size = 64;
        int spacing = 200;
        int start_x = SCREEN_WIDTH / 2 - spacing;

        // OBSTÁCULO (Pedra)
        if (game->sprite_obstacle) {
            int sw = al_get_bitmap_width(game->sprite_obstacle);
            int sh = al_get_bitmap_height(game->sprite_obstacle);
            al_draw_scaled_bitmap(game->sprite_obstacle, 0, 0, sw, sh,
                start_x - icon_size / 2, icon_y - icon_size / 2, icon_size, icon_size, 0);
        }
        else {
            al_draw_filled_rectangle(start_x - 20, icon_y - 20, start_x + 20, icon_y + 20,
                al_map_rgb(100, 100, 100));
        }

        // INIMIGO
        if (game->sprite_enemy) {
            int sw = al_get_bitmap_width(game->sprite_enemy);
            int sh = al_get_bitmap_height(game->sprite_enemy);
            al_draw_scaled_bitmap(game->sprite_enemy, 0, 0, sw, sh,
                start_x + spacing - icon_size / 2, icon_y - icon_size / 2, icon_size, icon_size, 0);
        }
        else {
            al_draw_filled_rectangle(start_x + spacing - 20, icon_y - 20,
                start_x + spacing + 20, icon_y + 20, al_map_rgb(200, 50, 50));
        }

        // ESPINHO (triângulo)
        int spike_x = start_x + spacing * 2;
        al_draw_filled_triangle(spike_x, icon_y - 30, spike_x - 25, icon_y + 20,
            spike_x + 25, icon_y + 20, al_map_rgb(150, 150, 150));
        al_draw_triangle(spike_x, icon_y - 30, spike_x - 25, icon_y + 20,
            spike_x + 25, icon_y + 20, al_map_rgb(255, 255, 255), 2);
    }
}

// ========================================================================
// FUNÇÕES - NÍVEIS
// ========================================================================

void level_init_chapters(Chapter* chapters) {
    // CAPÍTULO 0: LINEAR
    chapters[0].name = "Linear";
    chapters[0].function_type = FN_LINEAR;
    chapters[0].theme_color = al_map_rgb(0, 170, 220);

    // TEST - REDISTRIBUÍDO: ESPALHAR MUITO! Esquerda E direita
    chapters[0].test.arrows_allowed = 6;
    chapters[0].test.epsilon_px = 18.0;
    chapters[0].test.color_tint = TINT_BRONZE;
    chapters[0].test.collectible_count = 6;
    chapters[0].test.items[0] = (Collectible){ -8.0, 0.8, 0.40, COLLECT_KEY, false };   // ESQUERDA NEGATIVA!
    chapters[0].test.items[1] = (Collectible){ 2.0, 1.5, 0.40, COLLECT_KEY, false };    // Quase centro esquerda
    chapters[0].test.items[2] = (Collectible){ 12.0, 3.0, 0.40, COLLECT_KEY, false };   // Direita média
    chapters[0].test.items[3] = (Collectible){ -5.0, 1.2, 0.35, COLLECT_APPLE, false }; // ESQUERDA
    chapters[0].test.items[4] = (Collectible){ 7.0, 2.0, 0.35, COLLECT_APPLE, false };  // Centro-direita
    chapters[0].test.items[5] = (Collectible){ 16.0, 4.5, 0.40, COLLECT_APPLE_GOLD, false };  // Extrema direita

    // FASE 1 - ESQUERDA E DIREITA! Aleatoriedade total
    chapters[0].phases[0].arrows_allowed = 6;
    chapters[0].phases[0].epsilon_px = 18.0;
    chapters[0].phases[0].color_tint = TINT_BRONZE;
    chapters[0].phases[0].collectible_count = 6;
    chapters[0].phases[0].items[0] = (Collectible){ -10.0, 1.2, 0.40, COLLECT_KEY, false };  // ESQUERDA longe
    chapters[0].phases[0].items[1] = (Collectible){ 0.0, 2.0, 0.40, COLLECT_KEY, false };    // Centro origem
    chapters[0].phases[0].items[2] = (Collectible){ 10.0, 2.8, 0.40, COLLECT_KEY, false };   // Direita
    chapters[0].phases[0].items[3] = (Collectible){ -6.0, 1.5, 0.35, COLLECT_APPLE, false }; // Esquerda
    chapters[0].phases[0].items[4] = (Collectible){ 5.0, 2.2, 0.35, COLLECT_APPLE, false };  // Centro-direita
    chapters[0].phases[0].items[5] = (Collectible){ 14.0, 3.8, 0.40, COLLECT_APPLE_GOLD, false };  // Direita longe

    // FASE 2 - ESQUERDA E DIREITA distribuído!
    chapters[0].phases[1].arrows_allowed = 6;
    chapters[0].phases[1].epsilon_px = 18.0;
    chapters[0].phases[1].color_tint = TINT_SILVER;
    chapters[0].phases[1].collectible_count = 6;
    chapters[0].phases[1].items[0] = (Collectible){ -12.0, 1.5, 0.40, COLLECT_KEY, false };  // Muito ESQUERDA
    chapters[0].phases[1].items[1] = (Collectible){ -3.0, 2.2, 0.40, COLLECT_KEY, false };   // Esquerda média
    chapters[0].phases[1].items[2] = (Collectible){ 8.0, 2.8, 0.40, COLLECT_KEY, false };    // Direita
    chapters[0].phases[1].items[3] = (Collectible){ -7.0, 1.8, 0.35, COLLECT_APPLE, false }; // Esquerda
    chapters[0].phases[1].items[4] = (Collectible){ 4.0, 2.4, 0.35, COLLECT_APPLE, false };  // Centro
    chapters[0].phases[1].items[5] = (Collectible){ 15.0, 4.0, 0.40, COLLECT_APPLE_GOLD, false };  // Direita longe

    // Obstáculos - MAIS E MAIORES!
    chapters[0].test.obstacle_count = 2;  // Reduzido (removido SPIKE)
    chapters[0].test.obstacles[0].x = -4.0;
    chapters[0].test.obstacles[0].y = 1.8;
    chapters[0].test.obstacles[0].width = 1.5;  // MAIOR!
    chapters[0].test.obstacles[0].height = 1.5;  // MAIOR!
    chapters[0].test.obstacles[0].type = OBS_ROCK;
    chapters[0].test.obstacles[0].has_physics = true;

    chapters[0].test.obstacles[1].x = 4.0;
    chapters[0].test.obstacles[1].y = 2.5;
    chapters[0].test.obstacles[1].width = 1.2;
    chapters[0].test.obstacles[1].height = 1.2;
    chapters[0].test.obstacles[1].type = OBS_WALL;
    chapters[0].test.obstacles[1].has_physics = true;

    chapters[0].test.enemy_count = 2;  // Progressão: fase 1 tem 2 inimigos
    chapters[0].test.enemies[0].x = 8.0;
    chapters[0].test.enemies[0].y = 2.0;
    chapters[0].test.enemies[0].move_speed = 2.0;
    chapters[0].test.enemies[0].move_range = 3.0;
    chapters[0].test.enemies[0].move_dir = 1.0;
    chapters[0].test.enemies[0].start_x = 8.0;
    chapters[0].test.enemies[0].active = true;
    chapters[0].test.enemies[0].sprite = NULL;
    chapters[0].test.enemies[1].x = -6.0;
    chapters[0].test.enemies[1].y = 1.8;
    chapters[0].test.enemies[1].move_speed = 2.2;
    chapters[0].test.enemies[1].move_range = 2.8;
    chapters[0].test.enemies[1].move_dir = -1.0;
    chapters[0].test.enemies[1].start_x = -6.0;
    chapters[0].test.enemies[1].active = true;
    chapters[0].test.enemies[1].sprite = NULL;
    chapters[0].test.has_boss = false;
    chapters[0].test.boss_count = 0;  // Fase 1: sem boss

    // FASE 1 - MUITOS obstáculos espalhados!
    chapters[0].phases[0].obstacle_count = 3;  // Reduzido (removido SPIKE)
    chapters[0].phases[0].obstacles[0].x = -7.0;
    chapters[0].phases[0].obstacles[0].y = 1.5;
    chapters[0].phases[0].obstacles[0].width = 1.5;  // MAIOR!
    chapters[0].phases[0].obstacles[0].height = 1.5;
    chapters[0].phases[0].obstacles[0].type = OBS_ROCK;
    chapters[0].phases[0].obstacles[0].has_physics = true;

    chapters[0].phases[0].obstacles[1].x = 2.0;
    chapters[0].phases[0].obstacles[1].y = 2.2;
    chapters[0].phases[0].obstacles[1].width = 1.3;
    chapters[0].phases[0].obstacles[1].height = 1.3;
    chapters[0].phases[0].obstacles[1].type = OBS_WALL;
    chapters[0].phases[0].obstacles[1].has_physics = true;

    chapters[0].phases[0].obstacles[2].x = 9.0;
    chapters[0].phases[0].obstacles[2].y = 2.8;
    chapters[0].phases[0].obstacles[2].width = 1.2;
    chapters[0].phases[0].obstacles[2].height = 1.0;
    chapters[0].phases[0].obstacles[2].type = OBS_ROCK;
    chapters[0].phases[0].obstacles[2].has_physics = true;

    chapters[0].phases[0].enemy_count = 2;  // AUMENTADO!
    chapters[0].phases[0].enemies[0].x = -5.0;  // Esquerda
    chapters[0].phases[0].enemies[0].y = 1.5;
    chapters[0].phases[0].enemies[0].move_speed = 2.0;
    chapters[0].phases[0].enemies[0].move_range = 3.0;
    chapters[0].phases[0].enemies[0].move_dir = 1.0;
    chapters[0].phases[0].enemies[0].start_x = -5.0;
    chapters[0].phases[0].enemies[0].active = true;
    chapters[0].phases[0].enemies[0].sprite = NULL;

    chapters[0].phases[0].enemies[1].x = 11.0;  // Direita
    chapters[0].phases[0].enemies[1].y = 2.5;
    chapters[0].phases[0].enemies[1].move_speed = 2.5;
    chapters[0].phases[0].enemies[1].move_range = 3.5;
    chapters[0].phases[0].enemies[1].move_dir = -1.0;
    chapters[0].phases[0].enemies[1].start_x = 11.0;
    chapters[0].phases[0].enemies[1].active = true;
    chapters[0].phases[0].enemies[1].sprite = NULL;
    chapters[0].phases[0].has_boss = true;  // Fase 2: tem 1 boss
    chapters[0].phases[0].boss_count = 1;
    // Configurar boss da fase 2
    chapters[0].phases[0].bosses[0].x = 10.0;
    chapters[0].phases[0].bosses[0].y = 2.5;
    chapters[0].phases[0].bosses[0].health = 40.0;
    chapters[0].phases[0].bosses[0].max_health = 40.0;
    chapters[0].phases[0].bosses[0].shoot_timer = 0.0;
    chapters[0].phases[0].bosses[0].shoot_cooldown = 3.5;
    chapters[0].phases[0].bosses[0].move_speed = 2.0;
    chapters[0].phases[0].bosses[0].move_range = 3.5;
    chapters[0].phases[0].bosses[0].start_x = 10.0;
    chapters[0].phases[0].bosses[0].move_dir = 1.0;
    chapters[0].phases[0].bosses[0].active = true;
    chapters[0].phases[0].bosses[0].sprite = NULL;

    // FASE 2 - MUITO MAIS obstáculos!
    chapters[0].phases[1].obstacle_count = 4;  // Reduzido (removido SPIKE)
    chapters[0].phases[1].obstacles[0].x = -9.0;
    chapters[0].phases[1].obstacles[0].y = 1.3;
    chapters[0].phases[1].obstacles[0].width = 1.5;  // MAIOR!
    chapters[0].phases[1].obstacles[0].height = 1.5;
    chapters[0].phases[1].obstacles[0].type = OBS_ROCK;
    chapters[0].phases[1].obstacles[0].has_physics = true;

    chapters[0].phases[1].obstacles[1].x = -2.0;
    chapters[0].phases[1].obstacles[1].y = 2.0;
    chapters[0].phases[1].obstacles[1].width = 1.3;
    chapters[0].phases[1].obstacles[1].height = 1.3;
    chapters[0].phases[1].obstacles[1].type = OBS_WALL;
    chapters[0].phases[1].obstacles[1].has_physics = true;

    chapters[0].phases[1].obstacles[2].x = 5.0;
    chapters[0].phases[1].obstacles[2].y = 2.5;
    chapters[0].phases[1].obstacles[2].width = 1.2;
    chapters[0].phases[1].obstacles[2].height = 1.2;
    chapters[0].phases[1].obstacles[2].type = OBS_ROCK;
    chapters[0].phases[1].obstacles[2].has_physics = true;

    chapters[0].phases[1].obstacles[3].x = 11.0;
    chapters[0].phases[1].obstacles[3].y = 3.0;
    chapters[0].phases[1].obstacles[3].width = 1.4;
    chapters[0].phases[1].obstacles[3].height = 1.2;
    chapters[0].phases[1].obstacles[3].type = OBS_WALL;
    chapters[0].phases[1].obstacles[3].has_physics = true;

    chapters[0].phases[1].enemy_count = 3;  // AUMENTADO!
    chapters[0].phases[1].enemies[0].x = -8.0;  // Esquerda longe
    chapters[0].phases[1].enemies[0].y = 1.3;
    chapters[0].phases[1].enemies[0].move_speed = 2.0;
    chapters[0].phases[1].enemies[0].move_range = 2.5;
    chapters[0].phases[1].enemies[0].move_dir = 1.0;
    chapters[0].phases[1].enemies[0].start_x = -8.0;
    chapters[0].phases[1].enemies[0].active = true;
    chapters[0].phases[1].enemies[0].sprite = NULL;

    chapters[0].phases[1].enemies[1].x = 3.0;  // Centro
    chapters[0].phases[1].enemies[1].y = 2.2;
    chapters[0].phases[1].enemies[1].move_speed = 2.5;
    chapters[0].phases[1].enemies[1].move_range = 3.0;
    chapters[0].phases[1].enemies[1].move_dir = -1.0;
    chapters[0].phases[1].enemies[1].start_x = 3.0;
    chapters[0].phases[1].enemies[1].active = true;
    chapters[0].phases[1].enemies[1].sprite = NULL;

    chapters[0].phases[1].enemies[2].x = 14.0;  // Direita
    chapters[0].phases[1].enemies[2].y = 3.5;
    chapters[0].phases[1].enemies[2].move_speed = 3.0;
    chapters[0].phases[1].enemies[2].move_range = 2.5;
    chapters[0].phases[1].enemies[2].move_dir = 1.0;
    chapters[0].phases[1].enemies[2].start_x = 14.0;
    chapters[0].phases[1].enemies[2].active = true;
    chapters[0].phases[1].enemies[2].sprite = NULL;
    chapters[0].phases[1].has_boss = true;  // Fase 3: tem 2 bosses
    chapters[0].phases[1].boss_count = 2;
    // Configurar 2 bosses da fase 3
    chapters[0].phases[1].bosses[0].x = 8.0;
    chapters[0].phases[1].bosses[0].y = 2.0;
    chapters[0].phases[1].bosses[0].health = 45.0;
    chapters[0].phases[1].bosses[0].max_health = 45.0;
    chapters[0].phases[1].bosses[0].shoot_timer = 0.0;
    chapters[0].phases[1].bosses[0].shoot_cooldown = 3.2;
    chapters[0].phases[1].bosses[0].move_speed = 2.2;
    chapters[0].phases[1].bosses[0].move_range = 3.8;
    chapters[0].phases[1].bosses[0].start_x = 8.0;
    chapters[0].phases[1].bosses[0].move_dir = 1.0;
    chapters[0].phases[1].bosses[0].active = true;
    chapters[0].phases[1].bosses[0].sprite = NULL;

    chapters[0].phases[1].bosses[1].x = -8.0;
    chapters[0].phases[1].bosses[1].y = 2.5;
    chapters[0].phases[1].bosses[1].health = 45.0;
    chapters[0].phases[1].bosses[1].max_health = 45.0;
    chapters[0].phases[1].bosses[1].shoot_timer = 0.0;
    chapters[0].phases[1].bosses[1].shoot_cooldown = 3.2;
    chapters[0].phases[1].bosses[1].move_speed = 2.2;
    chapters[0].phases[1].bosses[1].move_range = 3.8;
    chapters[0].phases[1].bosses[1].start_x = -8.0;
    chapters[0].phases[1].bosses[1].move_dir = -1.0;
    chapters[0].phases[1].bosses[1].active = true;
    chapters[0].phases[1].bosses[1].sprite = NULL;

    // Boss phase do capítulo 0 - MÚLTIPLOS BOSSES!
    chapters[0].boss_phase.arrows_allowed = 20;  // Mais flechas para múltiplos bosses
    chapters[0].boss_phase.epsilon_px = 16.0;
    chapters[0].boss_phase.color_tint = TINT_GOLD;
    chapters[0].boss_phase.collectible_count = 0;
    chapters[0].boss_phase.obstacle_count = 0;
    chapters[0].boss_phase.enemy_count = 0;
    chapters[0].boss_phase.has_boss = true;
    chapters[0].boss_phase.boss_count = 2;  // Boss fight: 2 bosses!

    // Boss 1
    chapters[0].boss_phase.bosses[0].x = 10.0;
    chapters[0].boss_phase.bosses[0].y = 2.0;
    chapters[0].boss_phase.bosses[0].health = 40.0;
    chapters[0].boss_phase.bosses[0].max_health = 40.0;
    chapters[0].boss_phase.bosses[0].shoot_timer = 0.0;
    chapters[0].boss_phase.bosses[0].shoot_cooldown = 3.0;
    chapters[0].boss_phase.bosses[0].move_speed = 2.5;
    chapters[0].boss_phase.bosses[0].move_range = 4.0;
    chapters[0].boss_phase.bosses[0].start_x = 10.0;
    chapters[0].boss_phase.bosses[0].move_dir = 1.0;
    chapters[0].boss_phase.bosses[0].active = true;
    chapters[0].boss_phase.bosses[0].sprite = NULL;

    // Boss 2
    chapters[0].boss_phase.bosses[1].x = -10.0;
    chapters[0].boss_phase.bosses[1].y = 2.5;
    chapters[0].boss_phase.bosses[1].health = 50.0;  // Boss 2 mais forte (era boss 3)
    chapters[0].boss_phase.bosses[1].max_health = 50.0;
    chapters[0].boss_phase.bosses[1].shoot_timer = 1.0;  // Offset para não atirar ao mesmo tempo
    chapters[0].boss_phase.bosses[1].shoot_cooldown = 2.8;  // Mais rápido
    chapters[0].boss_phase.bosses[1].move_speed = 2.8;
    chapters[0].boss_phase.bosses[1].move_range = 5.0;
    chapters[0].boss_phase.bosses[1].start_x = -10.0;
    chapters[0].boss_phase.bosses[1].move_dir = -1.0;
    chapters[0].boss_phase.bosses[1].active = true;
    chapters[0].boss_phase.bosses[1].sprite = NULL;

    // Manter compatibilidade com boss antigo (usar primeiro boss do array)
    chapters[0].boss_phase.boss = chapters[0].boss_phase.bosses[0];

    // Copiar estrutura para outros capítulos e configurar funções
    for (int i = 1; i < 6; i++) {
        chapters[i] = chapters[0];
        switch (i) {
        case 1:
            chapters[i].name = "Quadratica";
            chapters[i].function_type = FN_QUADRATIC;
            chapters[i].theme_color = al_map_rgb(150, 50, 200);
            break;
        case 2:
            chapters[i].name = "Exponencial";
            chapters[i].function_type = FN_EXPONENTIAL;
            chapters[i].theme_color = al_map_rgb(255, 180, 0);
            break;
        case 3:
            chapters[i].name = "Logaritmica";
            chapters[i].function_type = FN_LOGARITHMIC;
            chapters[i].theme_color = al_map_rgb(0, 200, 100);
            break;
        case 4:
            chapters[i].name = "Racional";
            chapters[i].function_type = FN_RATIONAL;
            chapters[i].theme_color = al_map_rgb(200, 100, 50);
            break;
        case 5:
            chapters[i].name = "Trigonometrica";
            chapters[i].function_type = FN_TRIGONOMETRIC;
            chapters[i].theme_color = al_map_rgb(255, 100, 150);
            break;
        }
    }
}

Phase* get_current_phase(Game* game) {
    // Verificar se game e chapter são válidos
    if (!game || game->current_chapter < 0 || game->current_chapter >= 6) {
        return NULL;
    }

    if (game->current_phase == 0) {
        return &game->chapters[game->current_chapter].test;
    }
    else if (game->current_phase == 4) {
        // Fase especial do boss
        return &game->chapters[game->current_chapter].boss_phase;
    }
    else if (game->current_phase >= 1 && game->current_phase <= 3) {
        return &game->chapters[game->current_chapter].phases[game->current_phase - 1];
    }

    // Fallback: retornar fase de teste se índice inválido
    return &game->chapters[game->current_chapter].test;
}

void phase_reset_collectibles(Phase* phase) {
    if (!phase) return;  // CRÍTICO: evitar null pointer
    for (int i = 0; i < phase->collectible_count; i++) {
        phase->items[i].collected = false;
    }
}

// Verificar se uma posição colide com outros objetos (distância mínima)
bool check_position_collision(double x, double y, double radius,
    Collectible* collectibles, int collectible_count,
    Obstacle* obstacles, int obstacle_count,
    Enemy* enemies, int enemy_count,
    int skip_collectible, int skip_obstacle, int skip_enemy) {

    const double MIN_DISTANCE = 2.0;  // Distância mínima entre objetos

    // Verificar coletáveis
    for (int i = 0; i < collectible_count; i++) {
        if (i == skip_collectible) continue;
        double dx = x - collectibles[i].x;
        double dy = y - collectibles[i].y;
        double dist = sqrt(dx * dx + dy * dy);
        if (dist < MIN_DISTANCE + radius + collectibles[i].radius) {
            return true;  // Colisão detectada
        }
    }

    // Verificar obstáculos
    for (int i = 0; i < obstacle_count; i++) {
        if (i == skip_obstacle) continue;
        double dx = x - obstacles[i].x;
        double dy = y - obstacles[i].y;
        double dist = sqrt(dx * dx + dy * dy);
        double obs_radius = (obstacles[i].width + obstacles[i].height) / 2.0;
        if (dist < MIN_DISTANCE + radius + obs_radius) {
            return true;  // Colisão detectada
        }
    }

    // Verificar inimigos
    for (int i = 0; i < enemy_count; i++) {
        if (i == skip_enemy) continue;
        double dx = x - enemies[i].x;
        double dy = y - enemies[i].y;
        double dist = sqrt(dx * dx + dy * dy);
        if (dist < MIN_DISTANCE + radius + 0.8) {  // Raio do inimigo ~0.8
            return true;  // Colisão detectada
        }
    }

    return false;  // Sem colisão
}

// Randomizar posições de elementos da fase (evitando sobreposições)
void randomize_phase_layout(Phase* phase, int seed) {
    if (!phase) return;  // CRÍTICO: evitar null pointer

    srand(seed);
    const int MAX_ATTEMPTS = 50;  // Máximo de tentativas por objeto

    // Randomizar coletáveis
    for (int i = 0; i < phase->collectible_count; i++) {
        int attempts = 0;
        double x, y;
        bool valid_position = false;

        while (!valid_position && attempts < MAX_ATTEMPTS) {
            // X entre -16 e 16 (evitando extremos)
            x = -16.0 + (rand() % 320) / 10.0;  // -16.0 a 16.0 com passo 0.1
            // Y entre 0.5 e 8.0 (acima do chão, mas não muito alto)
            y = 0.5 + (rand() % 75) / 10.0;  // 0.5 a 8.0 com passo 0.1

            // Verificar colisão com outros objetos já posicionados
            if (!check_position_collision(x, y, phase->items[i].radius,
                phase->items, i, phase->obstacles, phase->obstacle_count,
                phase->enemies, phase->enemy_count, i, -1, -1)) {
                valid_position = true;
            }
            attempts++;
        }

        if (valid_position) {
            phase->items[i].x = x;
            phase->items[i].y = y;
        }
        else {
            // Fallback: posição padrão se não encontrar espaço
            phase->items[i].x = -10.0 + i * 5.0;
            phase->items[i].y = 2.0 + i * 1.5;
        }
    }

    // Randomizar obstáculos
    for (int i = 0; i < phase->obstacle_count; i++) {
        int attempts = 0;
        double x, y, size;
        bool valid_position = false;

        while (!valid_position && attempts < MAX_ATTEMPTS) {
            // X entre -15 e 15
            x = -15.0 + (rand() % 300) / 10.0;
            // Y entre 1.0 e 10.0
            y = 1.0 + (rand() % 90) / 10.0;
            // Tamanho aleatório
            size = 1.0 + (rand() % 10) / 10.0;  // 1.0 a 2.0

            // Verificar colisão
            double radius = size / 2.0;
            if (!check_position_collision(x, y, radius,
                phase->items, phase->collectible_count,
                phase->obstacles, i, phase->enemies, phase->enemy_count,
                -1, i, -1)) {
                valid_position = true;
            }
            attempts++;
        }

        if (valid_position) {
            phase->obstacles[i].x = x;
            phase->obstacles[i].y = y;
            phase->obstacles[i].width = size;
            phase->obstacles[i].height = size;
        }
        else {
            // Fallback
            phase->obstacles[i].x = -12.0 + i * 6.0;
            phase->obstacles[i].y = 2.0 + i * 2.0;
            phase->obstacles[i].width = 1.2;
            phase->obstacles[i].height = 1.2;
        }
    }

    // Randomizar inimigos
    for (int i = 0; i < phase->enemy_count; i++) {
        int attempts = 0;
        double x, y;
        bool valid_position = false;

        while (!valid_position && attempts < MAX_ATTEMPTS) {
            // X entre -14 e 14
            x = -14.0 + (rand() % 280) / 10.0;
            // Y entre 1.0 e 8.0
            y = 1.0 + (rand() % 70) / 10.0;

            // Verificar colisão
            if (!check_position_collision(x, y, 0.8,
                phase->items, phase->collectible_count,
                phase->obstacles, phase->obstacle_count,
                phase->enemies, i, -1, -1, i)) {
                valid_position = true;
            }
            attempts++;
        }

        if (valid_position) {
            phase->enemies[i].x = x;
            phase->enemies[i].y = y;
            phase->enemies[i].start_x = x;
        }
        else {
            // Fallback
            phase->enemies[i].x = -11.0 + i * 5.5;
            phase->enemies[i].y = 2.0 + i * 1.5;
            phase->enemies[i].start_x = phase->enemies[i].x;
        }

        // Velocidade e range aleatórios
        phase->enemies[i].move_speed = 2.0 + (rand() % 20) / 10.0;  // 2.0 a 4.0
        phase->enemies[i].move_range = 2.0 + (rand() % 30) / 10.0;  // 2.0 a 5.0
        phase->enemies[i].move_dir = (rand() % 2) ? 1.0 : -1.0;  // Direção aleatória
    }
}

bool collectible_check(Collectible* c, double arrow_x, double arrow_y, double epsilon) {
    if (!c || c->collected) return false;  // CRÍTICO: verificar c não NULL
    double dx = arrow_x - c->x;
    double dy = arrow_y - c->y;
    double dist = sqrt(dx * dx + dy * dy);
    double epsilon_world = epsilon / PIXELS_PER_UNIT;
    if (dist <= c->radius + epsilon_world) {
        c->collected = true;
        return true;
    }
    return false;
}

int phase_count_keys_collected(Phase* phase) {
    if (!phase) return 0;  // CRÍTICO: evitar null pointer
    int count = 0;
    for (int i = 0; i < phase->collectible_count; i++) {
        if (phase->items[i].type == COLLECT_KEY && phase->items[i].collected) {
            count++;
        }
    }
    return count;
}

// ========================================================================
// FUNÇÕES - UI
// ========================================================================

// Desenhar texto escalado (para fontes maiores)
void draw_text_scaled(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y,
    int flags, float scale, const char* text) {
    ALLEGRO_TRANSFORM trans, prev_trans;
    al_copy_transform(&prev_trans, al_get_current_transform());
    al_identity_transform(&trans);
    al_scale_transform(&trans, scale, scale);
    al_translate_transform(&trans, x, y);
    al_compose_transform(&trans, &prev_trans);
    al_use_transform(&trans);
    al_draw_text(font, color, 0, 0, flags, text);
    al_use_transform(&prev_trans);
}

void editor_init(ParamEditor* ed) {
    ed->cursor = 0;
    ed->selected = PARAM_NONE;
    ed->is_negative = false;
    memset(ed->buffer, 0, sizeof(ed->buffer));
}

void editor_select_param(ParamEditor* ed, ParamSelection param) {
    ed->selected = param;
    ed->cursor = 0;
    ed->is_negative = false;
    memset(ed->buffer, 0, sizeof(ed->buffer));
}

void editor_input_digit(ParamEditor* ed, int digit) {
    if (ed->cursor < 30) {
        ed->buffer[ed->cursor++] = '0' + digit;
    }
}

void editor_toggle_sign(ParamEditor* ed) {
    ed->is_negative = !ed->is_negative;
}

void editor_backspace(ParamEditor* ed) {
    if (ed->cursor > 0) {
        ed->buffer[--ed->cursor] = '\0';
    }
}

double editor_get_value(const ParamEditor* ed) {
    if (ed->cursor == 0) return 0.0;
    double val = atof(ed->buffer);
    return ed->is_negative ? -val : val;
}

void special_bar_add(SpecialBar* bar, double amount) {
    bar->value += amount;
    if (bar->value > 2.0) bar->value = 2.0;
}

bool special_bar_can_use(const SpecialBar* bar) {
    return bar->value >= 1.0;
}

void special_bar_use(SpecialBar* bar) {
    if (special_bar_can_use(bar)) {
        bar->value = 0.0;
    }
}

// ========================================================================
// FUNÇÕES - ESPECIAIS V2
// ========================================================================

// Gerar 3 especiais aleatórios
void generate_random_specials(SpecialOption* options) {
    // Definir todos os especiais disponíveis
    const SpecialOption all_specials[] = {
        {SPECIAL_SUPER_SPEED, "SUPER VELOCIDADE", "Flecha 3x mais rapida!"},
        {SPECIAL_SLOW_TIME, "TEMPO LENTO", "Jogo em camera lenta"},
        {SPECIAL_TRIPLE_ARROW, "FLECHA TRIPLA", "Dispara 3 flechas!"}
    };

    // Por ora, sempre oferece as 3 opções (pode randomizar depois)
    for (int i = 0; i < 3; i++) {
        options[i] = all_specials[i];
    }
}

// Aplicar efeito do especial selecionado
void apply_special_effect(Game* game, SpecialType type) {
    game->active_special = type;
    game->special_active = true;
    game->special_timer = 10.0;  // Dura 10 segundos

    printf("[ESPECIAL] Ativado: %d\n", type);
}

// ========================================================================
// FUNÇÕES - JOGO PRINCIPAL
// ========================================================================

void game_init(Game* game) {
    game->state = STATE_TITLE;
    game->current_chapter = 0;
    game->current_phase = 0;
    game->arrows_remaining = 5;

    player_init(&game->player, -6.0);
    arrow_init(&game->arrow);
    camera_init(&game->camera, SCREEN_WIDTH, SCREEN_HEIGHT);

    game->ui.font_main = NULL;
    game->ui.font_title = NULL;
    game->ui.font_small = NULL;
    editor_init(&game->ui.editor);
    game->ui.special.value = 0.0;
    game->ui.preview_enabled = true;

    game->current_function.type = FN_LINEAR;
    game->current_function.params.linear.m = 1.0;
    game->current_function.params.linear.k = 0.0;

    game->key_up = game->key_down = game->key_space = false;
    game->key_left = game->key_right = false;
    game->key_space_down = game->was_space_down = false;
    game->key_r = game->key_p = game->key_h = game->key_c = false;
    game->key_enter = game->key_escape = false;
    game->key_shift = false;
    game->key_m = game->key_k = game->key_q = game->key_e = false;
    game->key_bracket_left = game->key_bracket_right = false;

    // Backgrounds e sprites
    game->bg_home = NULL;
    game->bg_linear = NULL;
    game->bg_expo = NULL;
    game->bg_quadratica = NULL;
    game->bg_logaritmica = NULL;
    game->bg_racional = NULL;
    game->bg_trigonometrica = NULL;
    game->title_background = NULL;
    game->logo = NULL;
    game->sprite_boss = NULL;
    game->sprite_obstacle = NULL;
    game->sprite_enemy = NULL;
    game->sprite_key = NULL;
    game->sprite_apple = NULL;
    game->sprite_apple_gold = NULL;
    game->font_title_large = NULL;

    // Progresso
    game->progress.unlocked_chapters = 1;
    for (int i = 0; i < 6; i++) {
        game->progress.chapters_completed[i] = false;
        game->progress.boss_defeated[i] = false;
    }

    // Inicializar boss_count para todas as fases
    for (int i = 0; i < 6; i++) {
        game->chapters[i].test.boss_count = 0;
        game->chapters[i].phases[0].boss_count = 0;
        game->chapters[i].phases[1].boss_count = 0;
        game->chapters[i].boss_phase.boss_count = 0;
    }

    // Tutorial
    game->tutorial_page = 0;
    game->current_tutorial = NULL;
    game->tutorial_total_pages = 0;

    // Boss arrows
    for (int i = 0; i < 5; i++) {
        game->boss_arrows[i].active = false;
    }

    // Especiais v2
    game->selected_special_index = 0;
    game->active_special = SPECIAL_SUPER_SPEED;
    game->special_active = false;
    game->special_timer = 0.0;

    // Troca de função
    game->function_change_message_timer = 0.0;

    game->show_max_hint = false;
    game->hint_timer = 0.0;
    game->chapter_menu_selection = 0;
    game->show_grid = false;  // Grid começa desligado

    level_init_chapters(game->chapters);
}

void game_load_resources(Game* game) {
    game->ui.font_title = al_create_builtin_font();
    game->ui.font_main = al_create_builtin_font();
    game->ui.font_small = al_create_builtin_font();

    // Carregar sprites do player
   // IDLE dedicado (player-idle-3.png)
    printf("[DEBUG] Tentando carregar: %s\n", ASSET_PLAYER_IDLE);
    game->player.sprite_idle = al_load_bitmap(ASSET_PLAYER_IDLE);
    if (!game->player.sprite_idle) {
        printf("[WARN] Sprite idle não encontrado, usando walk[0] como fallback\n");
        game->player.sprite_idle = NULL;  // Será configurado depois
    }
    else {
        printf("[SUCCESS] Player idle sprite carregado!\n");
        game->player.frames_idle = 1;  // Frame único
    }

    // WALK (player-walk-3.png)
    printf("[DEBUG] Tentando carregar: %s\n", ASSET_PLAYER_WALK);
    game->player.sprite_walk = al_load_bitmap(ASSET_PLAYER_WALK);
    if (!game->player.sprite_walk) {
        printf("[WARN] Sprite walk não encontrado: %s (usando fallback)\n", ASSET_PLAYER_WALK);
        printf("[DEBUG] Erro do Allegro: %d\n", al_get_errno());
    }
    else {
        int w = al_get_bitmap_width(game->player.sprite_walk);
        int h = al_get_bitmap_height(game->player.sprite_walk);

        game->player.walk_frame_offset = 0;
        game->player.frames_walk = 4;
        game->player.anim_fps = 12.0;

        // Se idle não carregou, usar walk[0]
        if (!game->player.sprite_idle) {
            game->player.sprite_idle = game->player.sprite_walk;
            game->player.frames_idle = 1;
            printf("[INFO] Idle usando walk[0] como fallback\n");
        }

        printf("[INFO] Walk sprite: %dx%d, %d frames\n", w, h, game->player.frames_walk);
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_PLAYER_SHOOT);
    game->player.sprite_shoot = al_load_bitmap(ASSET_PLAYER_SHOOT);
    if (!game->player.sprite_shoot) {
        printf("[WARN] Sprite shoot não encontrado: %s (usando fallback)\n", ASSET_PLAYER_SHOOT);
    }
    else {
        int w = al_get_bitmap_width(game->player.sprite_shoot);
        int h = al_get_bitmap_height(game->player.sprite_shoot);

        // player-shot-3.png tem 3 frames horizontais
        if (h == w / 3) {
            game->player.frames_shoot = 3;
            game->player.anim_fps = 10.0;
            printf("[INFO] Shoot sprite: 3 frames detectados (%dx%d)\n", w, h);
        }
        else if (w == 384 && h == 64) {
            game->player.frames_shoot = 6;
            printf("[INFO] Shoot sprite: 6 frames\n");
        }
        else {
            game->player.frames_shoot = game->player.shoot_frames_total;
            printf("[INFO] Shoot sprite: grid %dx%d\n", game->player.shoot_cols, game->player.shoot_rows);
        }
    }

    // Carregar backgrounds
    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_HOME);
    game->bg_home = al_load_bitmap(ASSET_BG_HOME);
    if (game->bg_home) {
        printf("[SUCCESS] Background home carregado!\n");
    }
    else {
        printf("[WARN] bg-home.png não encontrado (usando gradiente)\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_LINEAR);
    game->bg_linear = al_load_bitmap(ASSET_BG_LINEAR);
    if (!game->bg_linear) {
        printf("[WARN] Background não encontrado: %s (usando gradiente)\n", ASSET_BG_LINEAR);
        printf("[DEBUG] Erro do Allegro: %d\n", al_get_errno());
    }
    else {
        printf("[SUCCESS] Background linear carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_EXPO);
    game->bg_expo = al_load_bitmap(ASSET_BG_EXPO);
    if (!game->bg_expo) {
        printf("[WARN] Background não encontrado: %s (usando gradiente)\n", ASSET_BG_EXPO);
        printf("[DEBUG] Erro do Allegro: %d\n", al_get_errno());
    }
    else {
        printf("[SUCCESS] Background expo carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_QUADRATICA);
    game->bg_quadratica = al_load_bitmap(ASSET_BG_QUADRATICA);
    if (!game->bg_quadratica) {
        printf("[WARN] Background quadratica não encontrado (usando gradiente)\n");
    }
    else {
        printf("[SUCCESS] Background quadratica carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_LOGARITMICA);
    game->bg_logaritmica = al_load_bitmap(ASSET_BG_LOGARITMICA);
    if (!game->bg_logaritmica) {
        printf("[WARN] Background logaritmica não encontrado (usando gradiente)\n");
    }
    else {
        printf("[SUCCESS] Background logaritmica carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_RACIONAL);
    game->bg_racional = al_load_bitmap(ASSET_BG_RACIONAL);
    if (!game->bg_racional) {
        printf("[WARN] Background racional não encontrado (usando gradiente)\n");
    }
    else {
        printf("[SUCCESS] Background racional carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BG_TRIGONOMETRICA);
    game->bg_trigonometrica = al_load_bitmap(ASSET_BG_TRIGONOMETRICA);
    if (!game->bg_trigonometrica) {
        printf("[WARN] Background trigonometrica não encontrado (usando gradiente)\n");
    }
    else {
        printf("[SUCCESS] Background trigonometrica carregado!\n");
    }

    // Carregar sprites de gameplay
    printf("[DEBUG] Tentando carregar: %s\n", ASSET_BOSS);
    game->sprite_boss = al_load_bitmap(ASSET_BOSS);
    if (!game->sprite_boss) {
        printf("[WARN] Sprite boss não encontrado (usando fallback)\n");
    }
    else {
        printf("[SUCCESS] Sprite boss carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_ENEMY);
    game->sprite_enemy = al_load_bitmap(ASSET_ENEMY);
    if (!game->sprite_enemy) {
        printf("[WARN] Sprite enemy não encontrado (usando fallback)\n");
    }
    else {
        printf("[SUCCESS] Sprite enemy carregado!\n");
    }

    printf("[DEBUG] Tentando carregar: %s\n", ASSET_OBSTACLE);
    game->sprite_obstacle = al_load_bitmap(ASSET_OBSTACLE);
    if (!game->sprite_obstacle) {
        printf("[WARN] Sprite obstacle não encontrado (usando fallback)\n");
    }
    else {
        printf("[SUCCESS] Sprite obstacle carregado!\n");
    }

    // Carregar sprites dos coletáveis
    printf("[DEBUG] Carregando sprites de coletaveis...\n");

    game->sprite_key = al_load_bitmap(ASSET_KEY);
    if (game->sprite_key) {
        printf("[SUCCESS] Sprite key.png carregado!\n");
    }
    else {
        printf("[WARN] key.png não encontrado (usando círculo amarelo)\n");
    }

    game->sprite_apple = al_load_bitmap(ASSET_APPLE);
    if (game->sprite_apple) {
        printf("[SUCCESS] Sprite apple.png carregado!\n");
    }
    else {
        printf("[WARN] apple.png não encontrado (usando círculo vermelho)\n");
    }

    game->sprite_apple_gold = al_load_bitmap(ASSET_APPLE_GOLD);
    if (game->sprite_apple_gold) {
        printf("[SUCCESS] Sprite apple-gold.png carregado!\n");
    }
    else {
        printf("[WARN] apple-gold.png não encontrado (usando círculo dourado)\n");
    }

    // UI sprites (opcional - por ora não usados)
    game->title_background = NULL;
    game->logo = NULL;
    game->font_title_large = al_create_builtin_font();

    printf("[INFO] Carregamento de recursos concluído!\n");
}

void game_start_phase(Game* game, int phase_index) {
    if (!game) return;

    game->current_phase = phase_index;
    Phase* phase = get_current_phase(game);
    if (!phase) {
        printf("[ERRO] Fase inválida: chapter=%d, phase=%d\n", game->current_chapter, phase_index);
        return;
    }

    game->arrows_remaining = phase->arrows_allowed;
    phase_reset_collectibles(phase);

    // Randomizar layout da fase (exceto boss fight)
    if (phase_index != 4) {
        int seed = (game->current_chapter * 1000) + (phase_index * 100) + (int)time(NULL);
        randomize_phase_layout(phase, seed);
    }

    game->player.y = 0.0;
    arrow_init(&game->arrow);

    printf("[FASE] Iniciando fase %d do capítulo %d\n", phase_index, game->current_chapter);
    printf("[FASE] Flechas: %d | Chaves necessárias: 3\n", phase->arrows_allowed);

    if (phase_index == 0) {
        game->ui.special.value = 0.0;
        game->state = STATE_CHAPTER_TEST;
        printf("[FASE] Estado: CHAPTER_TEST\n");
    }
    else if (phase_index == 4) {
        game->state = STATE_BOSS_FIGHT;
        printf("[FASE] Estado: BOSS_FIGHT\n");
    }
    else {
        game->state = STATE_PLAY;
        printf("[FASE] Estado: PLAY (fase %d)\n", phase_index);
    }
}

void game_update(Game* game, double dt) {
    if (!game) return;

    Phase* phase = get_current_phase(game);

    // Aplicar efeito SLOW_TIME se ativo
    if (game->special_active && game->active_special == SPECIAL_SLOW_TIME) {
        dt *= 0.3;  // Tempo 70% mais lento
    }

    switch (game->state) {
    case STATE_TITLE:
        if (game->key_enter) {
            // Primeira vez: mostra tutorial
            if (game->progress.unlocked_chapters == 1 && !game->progress.chapters_completed[0]) {
                game->tutorial_page = 0;
                game->current_tutorial = tutorial_intro_screens;
                game->tutorial_total_pages = 4;
                game->state = STATE_TUTORIAL_INTRO;
            }
            else {
                game->state = STATE_CHAPTER_SELECT;
            }
            game->key_enter = false;
        }
        break;

    case STATE_TUTORIAL_INTRO:
    case STATE_TUTORIAL_COLLECTIBLES:
    case STATE_FUNCTION_INTRO:
        if (game->key_enter) {
            game->tutorial_page++;
            if (game->tutorial_page >= game->tutorial_total_pages) {
                // Completou tutoriais, ir para seleção ou jogo
                if (game->state == STATE_FUNCTION_INTRO) {
                    // Após tutorial da função, inicia capítulo
                    game_start_phase(game, 0);
                }
                else {
                    // Após tutorial geral, vai para seleção
                    game->state = STATE_CHAPTER_SELECT;
                }
            }
            game->key_enter = false;
        }
        if (game->key_escape) {
            // Pular tutorial
            if (game->state == STATE_FUNCTION_INTRO) {
                game_start_phase(game, 0);
            }
            else {
                game->state = STATE_CHAPTER_SELECT;
            }
            game->key_escape = false;
        }
        break;

    case STATE_CHAPTER_SELECT:
        if (game->key_enter) {
            game->current_chapter = game->chapter_menu_selection;
            game->current_function.type = game->chapters[game->current_chapter].function_type;
            init_function_params(&game->current_function);

            // Iniciar tutorial da função específica
            game->tutorial_page = 0;
            game->tutorial_total_pages = 1;

            switch (game->current_function.type) {
            case FN_LINEAR:
                game->current_tutorial = tutorial_linear;
                break;
            case FN_QUADRATIC:
                game->current_tutorial = tutorial_quadratic;
                break;
            case FN_EXPONENTIAL:
                game->current_tutorial = tutorial_exponential;
                break;
            case FN_LOGARITHMIC:
                game->current_tutorial = tutorial_logarithmic;
                break;
            case FN_RATIONAL:
                game->current_tutorial = tutorial_rational;
                break;
            case FN_TRIGONOMETRIC:
                game->current_tutorial = tutorial_trigonometric;
                break;
            }

            game->state = STATE_FUNCTION_INTRO;
            game->key_enter = false;
        }
        if (game->key_escape) {
            game->state = STATE_TITLE;
            game->key_escape = false;
        }
        break;

    case STATE_CHAPTER_INTRO:
        if (game->key_enter) {
            game_start_phase(game, 0);
            game->key_enter = false;
        }
        break;

    case STATE_CHAPTER_TEST:
    case STATE_PLAY:
        if (!phase) phase = get_current_phase(game);  // Garantir que phase existe
        if (!phase) break;  // CRÍTICO: se phase for NULL, não continuar

        // PULO: tecla UP
        if (game->key_up && game->player.on_ground) {
            game->player.vy = game->player.jump_strength;
            game->player.is_jumping = true;
            game->player.on_ground = false;
            game->key_up = false;  // Evitar pulo contínuo
        }

        player_update(&game->player, dt, game->key_left, game->key_right);

        // Atualizar inimigos (patrulha)
        if (phase) {
            enemies_update(phase->enemies, phase->enemy_count, dt);
        }

        // Sistema de tiro: segurar = puxar, soltar = disparar
        bool space_pressed_now = game->key_space_down && !game->was_space_down;
        bool space_released_now = !game->key_space_down && game->was_space_down;

        if (space_pressed_now) {
            // Começou a segurar: entra em SHOOT e reseta carga
            game->player.state = PLAYER_SHOOT;
            game->player.frame = 0;
            game->player.frame_acc = 0.0;
            game->arrow.charge_time = 0.0;  // Reset da carga
        }

        // Enquanto segura: aumenta a carga
        if (game->key_space_down && game->player.state == PLAYER_SHOOT) {
            game->arrow.charge_time += dt;
            if (game->arrow.charge_time > game->arrow.max_charge_time) {
                game->arrow.charge_time = game->arrow.max_charge_time;  // Limita a 2 segundos
            }
        }

        if (space_released_now) {
            // Soltou: dispara se possível COM A FORÇA ACUMULADA!
            if (!game->arrow.active && game->arrows_remaining > 0) {
                // Calcular força: 1.0 (mínimo) a 3.5 (máximo após 2 segundos) - AUMENTADO!
                double charge_power = 1.0 + (game->arrow.charge_time / game->arrow.max_charge_time) * 2.5;
                if (charge_power > 3.5) charge_power = 3.5;  // Máximo aumentado de 2.0 para 3.5

                double vx = game->player.facing_right ? 15.0 : -15.0;  // AUMENTADO: velocidade base (era 8.0)

                // Aplicar efeito SUPER_SPEED se ativo
                if (game->special_active && game->active_special == SPECIAL_SUPER_SPEED) {
                    vx *= 3.0;  // 3x mais rápido!
                    printf("[ESPECIAL] Super velocidade aplicada!\n");
                }

                // Flecha sai do peito, não do pé (+1.5 unidades do mundo)
                arrow_shoot(&game->arrow, game->player.x, game->player.y + 1.5, vx, charge_power);
                game->arrows_remaining--;
            }
            // Volta ao idle
            game->player.state = PLAYER_IDLE;
            game->player.frame = 0;
        }

        game->was_space_down = game->key_space_down;

        if (game->arrow.active && phase) {
            arrow_update(&game->arrow, dt, &game->current_function);

            // Verificar colisão com obstáculos (ricochete)
            check_arrow_obstacle_collision(&game->arrow, phase->obstacles, phase->obstacle_count);

            // Verificar colisão com inimigos (flecha perdida)
            check_arrow_enemy_collision(&game->arrow, phase->enemies, phase->enemy_count);

            // Verificar coletáveis
            for (int i = 0; i < phase->collectible_count; i++) {
                Collectible* c = &phase->items[i];
                if (collectible_check(c, game->arrow.current_x, game->arrow.current_y, phase->epsilon_px)) {
                    if (c->type == COLLECT_APPLE) {
                        special_bar_add(&game->ui.special, 0.10);
                    }
                    else if (c->type == COLLECT_APPLE_GOLD) {
                        special_bar_add(&game->ui.special, 0.30);
                    }
                }
            }
        }

        // (Removido - agora usa sistema de segurar/soltar acima)

        if (game->key_p) {
            game->show_grid = !game->show_grid;  // Toggle grid ao invés de preview
            printf("[GRID] %s\n", game->show_grid ? "ATIVADO" : "DESATIVADO");
            game->key_p = false;
        }

        if (game->key_r) {
            game_start_phase(game, game->current_phase);
            game->key_r = false;
        }

        // ESC: Pausar jogo
        if (game->key_escape) {
            game->paused_from_state = game->state;
            game->state = STATE_PAUSED;
            game->key_escape = false;
            printf("[PAUSA] Jogo pausado\n");
        }

        // ESPECIAL V2: Abrir menu de seleção
        if (game->key_h && special_bar_can_use(&game->ui.special)) {
            special_bar_use(&game->ui.special);
            generate_random_specials(game->available_specials);
            game->selected_special_index = 0;  // Começa na primeira carta
            game->state = STATE_SPECIAL_SELECT;
            game->key_h = false;
            printf("[ESPECIAL] Abrindo menu de selecao...\n");
        }

        // Tecla C: trocar função matemática (APENAS entre funções desbloqueadas!)
        if (game->key_c) {
            const char* function_names[] = { "Linear", "Quadratica", "Exponencial", "Logaritmica", "Racional", "Trigonometrica" };

            // Só permite trocar se tiver pelo menos 2 capítulos desbloqueados
            if (game->progress.unlocked_chapters >= 2) {
                FunctionType old_type = game->current_function.type;

                // Ciclar apenas entre funções desbloqueadas
                do {
                    game->current_function.type = (FunctionType)((game->current_function.type + 1) % game->progress.unlocked_chapters);
                } while (game->current_function.type == old_type && game->progress.unlocked_chapters > 1);

                init_function_params(&game->current_function);
                game->function_change_message_timer = 2.0;
                printf("[FUNCAO] Trocou para: %s\n", function_names[game->current_function.type]);
            }
            else {
                printf("[INFO] Desbloqueie mais capitulos para trocar de funcao!\n");
            }

            game->key_c = false;
        }

        // CONTROLES COM LETRAS - Adaptado por tipo de função!
        if (game->current_function.type == FN_LINEAR) {
            // LINEAR: M e K
            if (game->key_m) { game->ui.editor.selected = PARAM_M; game->key_m = false; }
            if (game->key_k) { game->ui.editor.selected = PARAM_K; game->key_k = false; }

            if (game->key_q) {
                if (game->ui.editor.selected == PARAM_M) game->current_function.params.linear.m -= 0.1;
                else if (game->ui.editor.selected == PARAM_K) game->current_function.params.linear.k -= 0.1;
                game->key_q = false;
            }
            if (game->key_e) {
                if (game->ui.editor.selected == PARAM_M) game->current_function.params.linear.m += 0.1;
                else if (game->ui.editor.selected == PARAM_K) game->current_function.params.linear.k += 0.1;
                game->key_e = false;
            }
        }
        else if (game->current_function.type == FN_QUADRATIC) {
            // QUADRÁTICA: A, H, K
            if (game->key_m) { game->ui.editor.selected = PARAM_A; game->key_m = false; }  // M → A
            if (game->key_h) { game->ui.editor.selected = PARAM_H; game->key_h = false; }  // H
            if (game->key_k) { game->ui.editor.selected = PARAM_KQ; game->key_k = false; } // K

            if (game->key_q) {
                if (game->ui.editor.selected == PARAM_A) game->current_function.params.quadratic.a -= 0.05;
                else if (game->ui.editor.selected == PARAM_H) game->current_function.params.quadratic.h -= 0.5;
                else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.quadratic.kq -= 0.5;
                game->key_q = false;
            }
            if (game->key_e) {
                if (game->ui.editor.selected == PARAM_A) game->current_function.params.quadratic.a += 0.05;
                else if (game->ui.editor.selected == PARAM_H) game->current_function.params.quadratic.h += 0.5;
                else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.quadratic.kq += 0.5;
                game->key_e = false;
            }
        }
        else if (game->current_function.type == FN_EXPONENTIAL ||
            game->current_function.type == FN_LOGARITHMIC) {
            // EXPONENCIAL e LOGARÍTMICA: A, H (vira B), K (vira C)
            if (game->key_m) { game->ui.editor.selected = PARAM_A; game->key_m = false; }
            if (game->key_h) { game->ui.editor.selected = PARAM_H; game->key_h = false; }
            if (game->key_k) { game->ui.editor.selected = PARAM_KQ; game->key_k = false; }

            if (game->key_q) {
                if (game->current_function.type == FN_EXPONENTIAL) {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.exponential.a -= 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.exponential.b -= 0.05;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.exponential.c -= 0.5;
                }
                else {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.logarithmic.a -= 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.logarithmic.b -= 0.5;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.logarithmic.c -= 0.5;
                }
                game->key_q = false;
            }
            if (game->key_e) {
                if (game->current_function.type == FN_EXPONENTIAL) {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.exponential.a += 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.exponential.b += 0.05;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.exponential.c += 0.5;
                }
                else {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.logarithmic.a += 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.logarithmic.b += 0.5;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.logarithmic.c += 0.5;
                }
                game->key_e = false;
            }
        }
        else if (game->current_function.type == FN_RATIONAL ||
            game->current_function.type == FN_TRIGONOMETRIC) {
            // RACIONAL e TRIGONOMÉTRICA: M→A, H→B, K→C, [→D
            if (game->key_m) { game->ui.editor.selected = PARAM_A; game->key_m = false; }
            if (game->key_h) { game->ui.editor.selected = PARAM_H; game->key_h = false; }
            if (game->key_k) { game->ui.editor.selected = PARAM_KQ; game->key_k = false; }
            if (game->key_bracket_left || game->key_bracket_right) {
                game->ui.editor.selected = PARAM_D;
                game->key_bracket_left = game->key_bracket_right = false;
                printf("[CONTROLE] Parâmetro D selecionado\n");
            }

            if (game->key_q) {
                if (game->current_function.type == FN_RATIONAL) {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.rational.a -= 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.rational.b -= 0.5;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.rational.c -= 0.1;
                    else if (game->ui.editor.selected == PARAM_D) game->current_function.params.rational.d -= 0.5;
                }
                else {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.trigonometric.a -= 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.trigonometric.b -= 0.1;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.trigonometric.c -= 0.2;
                    else if (game->ui.editor.selected == PARAM_D) game->current_function.params.trigonometric.d -= 0.5;
                }
                game->key_q = false;
            }
            if (game->key_e) {
                if (game->current_function.type == FN_RATIONAL) {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.rational.a += 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.rational.b += 0.5;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.rational.c += 0.1;
                    else if (game->ui.editor.selected == PARAM_D) game->current_function.params.rational.d += 0.5;
                }
                else {
                    if (game->ui.editor.selected == PARAM_A) game->current_function.params.trigonometric.a += 0.1;
                    else if (game->ui.editor.selected == PARAM_H) game->current_function.params.trigonometric.b += 0.1;
                    else if (game->ui.editor.selected == PARAM_KQ) game->current_function.params.trigonometric.c += 0.2;
                    else if (game->ui.editor.selected == PARAM_D) game->current_function.params.trigonometric.d += 0.5;
                }
                game->key_e = false;
            }
        }

        int keys = phase_count_keys_collected(phase);

        // Debug: mostrar status das chaves
        printf("[DEBUG] Chaves coletadas: %d / 3\n", keys);

        if (keys >= 3 && phase) {
            printf("[VITORIA] 3 chaves coletadas! Passando de fase...\n");
            if (game->arrows_remaining == (phase->arrows_allowed - 1)) {
                special_bar_add(&game->ui.special, 0.40);
            }
            game->state = STATE_STAGE_CLEAR;
        }

        if (game->arrows_remaining <= 0 && !game->arrow.active && keys < 3) {
            printf("[DERROTA] Sem flechas e apenas %d chaves. Tente novamente.\n", keys);
            game->state = STATE_TRY_AGAIN;
        }
        break;

    case STATE_BOSS_FIGHT:
        if (!phase) phase = get_current_phase(game);  // CRÍTICO: garantir phase carregado!

        // PULO: tecla UP (também no boss fight!)
        if (game->key_up && game->player.on_ground) {
            game->player.vy = game->player.jump_strength;
            game->player.is_jumping = true;
            game->player.on_ground = false;
            game->key_up = false;
        }

        player_update(&game->player, dt, game->key_left, game->key_right);

        // Atualizar todos os bosses
        if (phase && phase->boss_count > 0 && phase->boss_count <= 4) {
            for (int i = 0; i < phase->boss_count; i++) {
                if (phase->bosses[i].active) {
                    boss_update(&phase->bosses[i], game->boss_arrows, 5, dt, game->player.x, game->player.y);
                }
            }
        }

        // Manter compatibilidade com boss antigo (se não usar array)
        if (phase && phase->boss_count == 0 && phase->has_boss) {
            Boss* boss = &phase->boss;
            boss_update(boss, game->boss_arrows, 5, dt, game->player.x, game->player.y);
        }

        // Atualizar projéteis do boss
        boss_arrows_update(game->boss_arrows, 5, dt);

        // Sistema de tiro com BARRA DE FORÇA (igual ao STATE_PLAY)
        bool space_pressed_boss = game->key_space_down && !game->was_space_down;
        bool space_released_boss = !game->key_space_down && game->was_space_down;

        if (space_pressed_boss) {
            game->player.state = PLAYER_SHOOT;
            game->player.frame = 0;
            game->player.frame_acc = 0.0;
            game->arrow.charge_time = 0.0;  // Reset da carga
        }

        // Enquanto segura: aumenta a carga
        if (game->key_space_down && game->player.state == PLAYER_SHOOT) {
            game->arrow.charge_time += dt;
            if (game->arrow.charge_time > game->arrow.max_charge_time) {
                game->arrow.charge_time = game->arrow.max_charge_time;
            }
        }

        if (space_released_boss) {
            if (!game->arrow.active && game->arrows_remaining > 0) {
                // Calcular força
                double charge_power = 1.0 + (game->arrow.charge_time / game->arrow.max_charge_time) * 2.5;
                if (charge_power > 3.5) charge_power = 3.5;  // Máximo aumentado

                double vx = game->player.facing_right ? 15.0 : -15.0;  // AUMENTADO: velocidade base
                // Flecha sai do peito, não do pé (+1.5 unidades do mundo)
                arrow_shoot(&game->arrow, game->player.x, game->player.y + 1.5, vx, charge_power);
                game->arrows_remaining--;
            }
            game->player.state = PLAYER_IDLE;
            game->player.frame = 0;
        }

        game->was_space_down = game->key_space_down;

        // Verificar colisão dos projéteis do boss com o player
        if (check_boss_arrow_player_collision(game->boss_arrows, 5, &game->player)) {
            // Player levou dano
        }

        if (game->arrow.active) {
            arrow_update(&game->arrow, dt, &game->current_function);

            // Colisão com projéteis do boss
            check_boss_arrow_player_arrow_collision(game->boss_arrows, 5, &game->arrow);

            // Colisão com todos os bosses
            if (phase && phase->boss_count > 0 && phase->boss_count <= 4) {
                for (int i = 0; i < phase->boss_count; i++) {
                    if (phase->bosses[i].active) {
                        check_arrow_boss_collision(&game->arrow, &phase->bosses[i]);
                    }
                }
            }

            // Compatibilidade com boss antigo
            if (phase && phase->boss_count == 0 && phase->has_boss) {
                check_arrow_boss_collision(&game->arrow, &phase->boss);
            }
        }

        if (game->key_r) {
            // Reiniciar boss fight
            game->player.health = game->player.max_health;  // Restaurar vida
            Phase* boss_phase = get_current_phase(game);
            // Resetar todos os bosses
            for (int i = 0; i < boss_phase->boss_count; i++) {
                boss_phase->bosses[i].health = boss_phase->bosses[i].max_health;
                boss_phase->bosses[i].active = true;
                boss_phase->bosses[i].shoot_timer = 0.0;
            }
            if (boss_phase->boss_count == 0 && boss_phase->has_boss) {
                boss_phase->boss.health = boss_phase->boss.max_health;
                boss_phase->boss.active = true;
                boss_phase->boss.shoot_timer = 0.0;
            }
            game_start_phase(game, 4);
            game->key_r = false;
        }

        // ESC: Pausar boss fight
        if (game->key_escape) {
            game->paused_from_state = game->state;
            game->state = STATE_PAUSED;
            game->key_escape = false;
        }

        // Tecla C: trocar função (CRÍTICO! Estava faltando!)
        if (game->key_c && game->progress.unlocked_chapters >= 2) {
            FunctionType old_type = game->current_function.type;
            do {
                game->current_function.type = (FunctionType)((game->current_function.type + 1) % game->progress.unlocked_chapters);
            } while (game->current_function.type == old_type && game->progress.unlocked_chapters > 1);
            init_function_params(&game->current_function);
            game->function_change_message_timer = 2.0;
            game->key_c = false;
            printf("[BOSS] Função trocada!\n");
        }

        // CONTROLES DE EDIÇÃO NO BOSS FIGHT (igual ao STATE_PLAY)
        if (game->current_function.type == FN_LINEAR) {
            if (game->key_m) { game->ui.editor.selected = PARAM_M; game->key_m = false; }
            if (game->key_k) { game->ui.editor.selected = PARAM_K; game->key_k = false; }

            if (game->key_q) {
                if (game->ui.editor.selected == PARAM_M) game->current_function.params.linear.m -= 0.1;
                else if (game->ui.editor.selected == PARAM_K) game->current_function.params.linear.k -= 0.1;
                game->key_q = false;
            }
            if (game->key_e) {
                if (game->ui.editor.selected == PARAM_M) game->current_function.params.linear.m += 0.1;
                else if (game->ui.editor.selected == PARAM_K) game->current_function.params.linear.k += 0.1;
                game->key_e = false;
            }
        }
        else if (game->current_function.type == FN_QUADRATIC) {
            if (game->key_m) { game->ui.editor.selected = PARAM_A; game->key_m = false; }
            if (game->key_h) { game->ui.editor.selected = PARAM_H; game->key_h = false; }
            if (game->key_k) { game->ui.editor.selected = PARAM_KQ; game->key_k = false; }

            if (game->key_q) {
                if (game->ui.editor.selected == PARAM_A) {
                    game->current_function.params.quadratic.a -= 0.05;
                    // CLAMP: não deixar próximo de zero!
                    if (game->current_function.params.quadratic.a > -0.05 && game->current_function.params.quadratic.a < 0.05) {
                        game->current_function.params.quadratic.a = (game->current_function.params.quadratic.a >= 0) ? 0.05 : -0.05;
                    }
                    if (game->current_function.params.quadratic.a < -5.0) game->current_function.params.quadratic.a = -5.0;
                }
                else if (game->ui.editor.selected == PARAM_H) {
                    game->current_function.params.quadratic.h -= 0.5;
                    if (game->current_function.params.quadratic.h < -15.0) game->current_function.params.quadratic.h = -15.0;
                }
                else if (game->ui.editor.selected == PARAM_KQ) {
                    game->current_function.params.quadratic.kq -= 0.5;
                    if (game->current_function.params.quadratic.kq < -10.0) game->current_function.params.quadratic.kq = -10.0;
                }
                game->key_q = false;
            }
            if (game->key_e) {
                if (game->ui.editor.selected == PARAM_A) {
                    game->current_function.params.quadratic.a += 0.05;
                    // CLAMP: não deixar próximo de zero!
                    if (game->current_function.params.quadratic.a > -0.05 && game->current_function.params.quadratic.a < 0.05) {
                        game->current_function.params.quadratic.a = (game->current_function.params.quadratic.a >= 0) ? 0.05 : -0.05;
                    }
                    if (game->current_function.params.quadratic.a > 5.0) game->current_function.params.quadratic.a = 5.0;
                }
                else if (game->ui.editor.selected == PARAM_H) {
                    game->current_function.params.quadratic.h += 0.5;
                    if (game->current_function.params.quadratic.h > 15.0) game->current_function.params.quadratic.h = 15.0;
                }
                else if (game->ui.editor.selected == PARAM_KQ) {
                    game->current_function.params.quadratic.kq += 0.5;
                    if (game->current_function.params.quadratic.kq > 10.0) game->current_function.params.quadratic.kq = 10.0;
                }
                game->key_e = false;
            }
        }

        // Vitória: todos os bosses derrotados
        bool all_bosses_defeated = true;
        if (phase && phase->boss_count > 0 && phase->boss_count <= 4) {
            for (int i = 0; i < phase->boss_count; i++) {
                if (phase->bosses[i].active) {
                    all_bosses_defeated = false;
                    break;
                }
            }
        }
        else if (phase && phase->has_boss) {
            all_bosses_defeated = !phase->boss.active;
        }
        else {
            all_bosses_defeated = false;
        }

        if (all_bosses_defeated) {
            game->state = STATE_STAGE_CLEAR;
        }

        // Derrota: sem flechas e bosses ainda vivos, OU player morreu
        bool any_boss_alive = false;
        if (phase && phase->boss_count > 0 && phase->boss_count <= 4) {
            for (int i = 0; i < phase->boss_count; i++) {
                if (phase->bosses[i].active) {
                    any_boss_alive = true;
                    break;
                }
            }
        }
        else if (phase && phase->has_boss) {
            any_boss_alive = phase->boss.active;
        }

        if ((game->arrows_remaining <= 0 && !game->arrow.active && any_boss_alive) || game->player.health <= 0) {
            game->state = STATE_TRY_AGAIN;
        }
        break;

    case STATE_STAGE_CLEAR:
        if (game->key_enter) {
            // Lógica de progressão de fases
            if (game->current_phase == 0) {
                // Completou teste, vai para fase 1
                game_start_phase(game, 1);
            }
            else if (game->current_phase < 2) {
                // Completou fase 1, vai para próxima
                game_start_phase(game, game->current_phase + 1);
            }
            else if (game->current_phase == 2) {
                // Completou fase 2, inicia BOSS FIGHT!
                printf("[BOSS] Iniciando luta contra o boss do capítulo %d!\n", game->current_chapter);
                game->current_phase = 4;
                Phase* boss_phase = get_current_phase(game);
                game->arrows_remaining = boss_phase->arrows_allowed;
                arrow_init(&game->arrow);
                boss_arrows_init(game->boss_arrows, 5);
                // Resetar todos os bosses
                for (int i = 0; i < boss_phase->boss_count; i++) {
                    boss_phase->bosses[i].health = boss_phase->bosses[i].max_health;
                    boss_phase->bosses[i].active = true;
                    boss_phase->bosses[i].shoot_timer = 0.0;
                }
                // Compatibilidade com boss antigo
                if (boss_phase->boss_count == 0 && boss_phase->has_boss) {
                    boss_phase->boss.health = boss_phase->boss.max_health;
                    boss_phase->boss.active = true;
                    boss_phase->boss.shoot_timer = 0.0;
                }
                // Restaurar vida do player
                game->player.health = game->player.max_health;
                game->state = STATE_BOSS_FIGHT;
            }
            else {
                // Completou boss (fase 4), marca capítulo completo e desbloqueia próximo
                printf("[BOSS] Derrotou o boss do capítulo %d!\n", game->current_chapter);
                game->progress.chapters_completed[game->current_chapter] = true;
                game->progress.boss_defeated[game->current_chapter] = true;

                if (game->current_chapter + 1 < 6) {
                    // Desbloquear próximo capítulo
                    game->progress.unlocked_chapters = game->current_chapter + 2;
                    printf("[PROGRESSO] Capítulo %d COMPLETO! Capítulo %d desbloqueado!\n",
                        game->current_chapter, game->current_chapter + 1);

                    // IR PARA O PRÓXIMO CAPÍTULO AUTOMATICAMENTE!
                    game->current_chapter++;
                    game->current_function.type = game->chapters[game->current_chapter].function_type;
                    init_function_params(&game->current_function);
                    printf("[PROGRESSO] Iniciando capítulo %d: %s\n",
                        game->current_chapter, game->chapters[game->current_chapter].name);

                    // Mostrar tutorial da nova função
                    game->tutorial_page = 0;
                    game->tutorial_total_pages = 1;

                    switch (game->current_function.type) {
                    case FN_LINEAR: game->current_tutorial = tutorial_linear; break;
                    case FN_QUADRATIC: game->current_tutorial = tutorial_quadratic; break;
                    case FN_EXPONENTIAL: game->current_tutorial = tutorial_exponential; break;
                    case FN_LOGARITHMIC: game->current_tutorial = tutorial_logarithmic; break;
                    case FN_RATIONAL: game->current_tutorial = tutorial_rational; break;
                    case FN_TRIGONOMETRIC: game->current_tutorial = tutorial_trigonometric; break;
                    }

                    game->state = STATE_FUNCTION_INTRO;  // Mostra tutorial da função!
                }
                else {
                    // Completou todos os 6 capítulos!
                    printf("[VITORIA FINAL] Todos os capitulos completos!\n");
                    game->state = STATE_CHAPTER_SELECT;
                }
            }
            game->key_enter = false;
        }
        if (game->key_escape) {
            game->state = STATE_CHAPTER_SELECT;
            game->key_escape = false;
        }
        break;

    case STATE_PAUSED:
        if (game->key_enter) {
            // Continuar jogo
            game->state = game->paused_from_state;
            game->key_enter = false;
        }
        if (game->key_c && game->progress.unlocked_chapters >= 2) {
            // Trocar função
            cycle_function_type(&game->current_function);
            game->key_c = false;
        }
        if (game->key_r) {
            // Reiniciar fase
            game->state = game->paused_from_state;
            game_start_phase(game, game->current_phase);
            game->key_r = false;
        }
        if (game->key_m) {
            // Voltar ao menu
            game->state = STATE_CHAPTER_SELECT;
            game->key_m = false;
        }
        if (game->key_escape) {
            // ESC também despau sa
            game->state = game->paused_from_state;
            game->key_escape = false;
        }
        break;

    case STATE_TRY_AGAIN:
        printf("[TRY_AGAIN] Estado ativo. Aguardando ENTER...\n");

        if (game->key_enter) {
            printf("[TRY_AGAIN] ENTER detectado! Reiniciando fase %d...\n", game->current_phase);
            // Restaurar vida do player se estiver no boss fight
            if (game->current_phase == 4) {
                game->player.health = game->player.max_health;
            }
            game_start_phase(game, game->current_phase);
            game->key_enter = false;
        }
        if (game->key_escape) {
            printf("[TRY_AGAIN] ESC detectado! Voltando ao menu...\n");
            game->state = STATE_CHAPTER_SELECT;
            game->key_escape = false;
        }
        break;

    case STATE_SPECIAL_SELECT:
        // Navegação com setas esquerda/direita
        if (game->key_left) {
            game->selected_special_index--;
            if (game->selected_special_index < 0) game->selected_special_index = 0;
            game->key_left = false;
        }
        if (game->key_right) {
            game->selected_special_index++;
            if (game->selected_special_index > 2) game->selected_special_index = 2;
            game->key_right = false;
        }

        // Seleção com teclas 1, 2, 3
        if (game->key_enter) {
            // Aplicar especial selecionado
            apply_special_effect(game, game->available_specials[game->selected_special_index].type);
            // Voltar ao jogo
            if (game->current_phase == 0) game->state = STATE_CHAPTER_TEST;
            else if (game->current_phase == 4) game->state = STATE_BOSS_FIGHT;
            else game->state = STATE_PLAY;
            game->key_enter = false;
        }
        break;
    }

    if (game->show_max_hint) {
        game->hint_timer += dt;
        if (game->hint_timer > 5.0) {
            game->show_max_hint = false;
        }
    }

    // Decrementar timer da mensagem de troca de função
    if (game->function_change_message_timer > 0.0) {
        game->function_change_message_timer -= dt;
        if (game->function_change_message_timer < 0.0) {
            game->function_change_message_timer = 0.0;
        }
    }

    // Decrementar timer do especial ativo
    if (game->special_active) {
        game->special_timer -= dt;
        if (game->special_timer <= 0.0) {
            game->special_active = false;
            game->special_timer = 0.0;
            printf("[ESPECIAL] Efeito terminou!\n");
        }
    }
}

// Desenhar HUD de comandos melhorado
void draw_command_hud(Game* game, ALLEGRO_FONT* font) {
    // SIDEBAR DIREITA - COMANDOS (com ~47% opacidade, mais transparente)
    int sidebar_x = SCREEN_WIDTH - 250;
    int sidebar_y = 80;  // Mais baixo para não cobrir coletáveis
    int sidebar_w = 230;
    int sidebar_h = 400;

    // Fundo da sidebar
    al_draw_filled_rounded_rectangle(sidebar_x, sidebar_y,
        sidebar_x + sidebar_w, sidebar_y + sidebar_h,
        8, 8, al_map_rgba(0, 0, 0, 120));  // ~47% opacidade (mais transparente)
    al_draw_rounded_rectangle(sidebar_x, sidebar_y,
        sidebar_x + sidebar_w, sidebar_y + sidebar_h,
        8, 8, al_map_rgb(100, 200, 255), 2);

    // Título
    draw_text_scaled(font, al_map_rgb(200, 255, 200), sidebar_x + 10, sidebar_y + 10, 0, 1.3, "COMANDOS:");

    // Lista de comandos em colunas
    int cmd_y = sidebar_y + 40;
    int cmd_spacing = 25;

    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "SETAS - Mover");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "ESPAÇO - Atirar");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "(segurar = força)");
    cmd_y += cmd_spacing;

    // Comandos de edição baseados no tipo de função
    if (game->current_function.type == FN_LINEAR) {
        al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "M - Editar m");
        cmd_y += cmd_spacing;
        al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "K - Editar k");
    }
    else if (game->current_function.type == FN_QUADRATIC) {
        al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "A - Editar a");
        cmd_y += cmd_spacing;
        al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "H - Editar h");
        cmd_y += cmd_spacing;
        al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "K - Editar k");
    }
    else {
        al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "A/H/K/D - Params");
    }
    cmd_y += cmd_spacing * 2;

    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "SHIFT - Preview");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "P - Grid");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "ESC - Menu/Pausar");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "R - Reiniciar");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "UP - Pular");
    cmd_y += cmd_spacing;
    al_draw_text(font, al_map_rgb(255, 255, 255), sidebar_x + 10, cmd_y, 0, "H - Especial");

    // Indicador de especial ativo
    if (game->special_active) {
        const char* special_name = "";
        ALLEGRO_COLOR special_color;

        switch (game->active_special) {
        case SPECIAL_SUPER_SPEED:
            special_name = "SUPER VELOCIDADE";
            special_color = al_map_rgb(255, 100, 100);
            break;
        case SPECIAL_SLOW_TIME:
            special_name = "TEMPO LENTO";
            special_color = al_map_rgb(100, 100, 255);
            break;
        case SPECIAL_TRIPLE_ARROW:
            special_name = "FLECHA TRIPLA";
            special_color = al_map_rgb(100, 255, 100);
            break;
        }

        // Banner de especial ativo (lado direito) - TEXTO MAIOR
        int banner_x = SCREEN_WIDTH - 250;
        int banner_y = 80 + 400 + 20;  // Abaixo da sidebar (ajustado para novo sidebar_y)
        al_draw_filled_rounded_rectangle(banner_x, banner_y, SCREEN_WIDTH - 10, banner_y + 45,
            8, 8, al_map_rgba(0, 0, 0, 200));
        draw_text_scaled(font, special_color, (float)(banner_x + 120), (float)(banner_y + 5),
            ALLEGRO_ALIGN_CENTER, 1.4f, special_name);

        char timer_text[32];
        snprintf(timer_text, sizeof(timer_text), "Tempo: %.1fs", game->special_timer);
        draw_text_scaled(font, al_map_rgb(200, 200, 200), (float)(banner_x + 120), (float)(banner_y + 25),
            ALLEGRO_ALIGN_CENTER, 1.4f, timer_text);
    }
}

void game_draw(Game* game) {
    if (!game) return;  // CRÍTICO: verificar game válido

    al_clear_to_color(al_map_rgb(20, 20, 40));
    Phase* phase = get_current_phase(game);

    // Verificar se chapter é válido antes de acessar
    if (game->current_chapter < 0 || game->current_chapter >= 6) {
        return;
    }

    Chapter* ch = &game->chapters[game->current_chapter];
    ALLEGRO_FONT* font = game->ui.font_main;
    if (!font) return;  // CRÍTICO: verificar font válido

    switch (game->state) {
    case STATE_TITLE: {
        // Background HOME (bg-home.png)
        if (game->bg_home) {
            al_draw_scaled_bitmap(game->bg_home, 0, 0,
                al_get_bitmap_width(game->bg_home),
                al_get_bitmap_height(game->bg_home),
                0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        }
        else {
            // Fallback: Gradiente azul → roxo
            for (int i = 0; i < SCREEN_HEIGHT; i++) {
                float ratio = (float)i / SCREEN_HEIGHT;
                ALLEGRO_COLOR line_color = al_map_rgb(
                    (int)(20 + 60 * ratio),
                    (int)(20 + 20 * ratio),
                    (int)(80 - 20 * ratio)
                );
                al_draw_line(0, i, SCREEN_WIDTH, i, line_color, 1);
            }
        }

        // Logo (se disponível)
        if (game->logo) {
            int logo_w = al_get_bitmap_width(game->logo);
            int logo_h = al_get_bitmap_height(game->logo);
            al_draw_scaled_bitmap(game->logo, 0, 0, logo_w, logo_h,
                SCREEN_WIDTH / 2 - 100, 80, 200, 100, 0);
        }

        // Título ARCHER - MUITO MAIOR!
        draw_text_scaled(font, al_map_rgba(0, 0, 0, 180), SCREEN_WIDTH / 2 + 4, 154,
            ALLEGRO_ALIGN_CENTER, 5.0, "ARCHER");  // Sombra
        draw_text_scaled(font, al_map_rgb(255, 255, 150), SCREEN_WIDTH / 2, 150,
            ALLEGRO_ALIGN_CENTER, 5.0, "ARCHER");
        draw_text_scaled(font, al_map_rgb(255, 200, 50), SCREEN_WIDTH / 2, 152,
            ALLEGRO_ALIGN_CENTER, 5.0, "ARCHER");

        // Subtítulo - MAIOR!
        draw_text_scaled(font, al_map_rgba(0, 0, 0, 120), SCREEN_WIDTH / 2 + 2, 282,
            ALLEGRO_ALIGN_CENTER, 2.5, "Jogo de Funcoes Matematicas");  // Sombra
        draw_text_scaled(font, al_map_rgb(200, 220, 255), SCREEN_WIDTH / 2, 280,
            ALLEGRO_ALIGN_CENTER, 2.5, "Jogo de Funcoes Matematicas");

        // Botão JOGAR - estilizado
        int btn_y = 380;
        int btn_w = 300;
        int btn_h = 50;
        al_draw_filled_rounded_rectangle(SCREEN_WIDTH / 2 - btn_w / 2, btn_y,
            SCREEN_WIDTH / 2 + btn_w / 2, btn_y + btn_h, 8, 8, al_map_rgb(50, 150, 50));
        al_draw_rounded_rectangle(SCREEN_WIDTH / 2 - btn_w / 2, btn_y,
            SCREEN_WIDTH / 2 + btn_w / 2, btn_y + btn_h, 8, 8, al_map_rgb(100, 255, 100), 3);
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, btn_y + 16,
            ALLEGRO_ALIGN_CENTER, "ENTER - Jogar");

        // Botão SAIR - estilizado
        btn_y = 450;
        al_draw_filled_rounded_rectangle(SCREEN_WIDTH / 2 - btn_w / 2, btn_y,
            SCREEN_WIDTH / 2 + btn_w / 2, btn_y + btn_h, 8, 8, al_map_rgb(150, 50, 50));
        al_draw_rounded_rectangle(SCREEN_WIDTH / 2 - btn_w / 2, btn_y,
            SCREEN_WIDTH / 2 + btn_w / 2, btn_y + btn_h, 8, 8, al_map_rgb(255, 100, 100), 3);
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, btn_y + 16,
            ALLEGRO_ALIGN_CENTER, "ESC - Sair");

        // Instruções no rodapé
        al_draw_text(font, al_map_rgb(150, 150, 150), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40,
            ALLEGRO_ALIGN_CENTER, "v1.0 | Aprenda matematica jogando!");
        break;
    }

    case STATE_TUTORIAL_INTRO:
    case STATE_TUTORIAL_COLLECTIBLES:
    case STATE_FUNCTION_INTRO:
        if (game->current_tutorial) {
            draw_tutorial_screen_with_game(&game->current_tutorial[game->tutorial_page], font,
                game->tutorial_page, game->tutorial_total_pages, game);
        }
        break;

    case STATE_CHAPTER_SELECT:
        al_draw_text(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2, 50,
            ALLEGRO_ALIGN_CENTER, "SELECIONE O CAPITULO");

        for (int i = 0; i < 6; i++) {
            bool unlocked = (i < game->progress.unlocked_chapters);
            bool completed = game->progress.chapters_completed[i];
            bool boss_defeated = game->progress.boss_defeated[i];
            bool selected = (i == game->chapter_menu_selection);

            ALLEGRO_COLOR color;
            const char* status = "";

            if (!unlocked) {
                color = al_map_rgb(80, 80, 80);
                status = " [BLOQUEADO]";
            }
            else if (boss_defeated) {
                color = selected ? al_map_rgb(255, 255, 100) : al_map_rgb(100, 255, 100);
                status = " [COMPLETO]";
            }
            else if (completed) {
                color = selected ? al_map_rgb(255, 255, 100) : al_map_rgb(150, 255, 150);
                status = " [BOSS DISPONIVEL]";
            }
            else {
                color = selected ? al_map_rgb(255, 255, 100) : al_map_rgb(255, 255, 255);
            }

            char text[128];
            snprintf(text, sizeof(text), "%d. %s%s", i + 1, game->chapters[i].name, status);
            al_draw_text(font, color, SCREEN_WIDTH / 2, 150 + i * 50,
                ALLEGRO_ALIGN_CENTER, text);
        }

        al_draw_text(font, al_map_rgb(150, 150, 150), SCREEN_WIDTH / 2, 500,
            ALLEGRO_ALIGN_CENTER, "Use SETAS para navegar | ENTER para selecionar | ESC para voltar");
        break;

    case STATE_CHAPTER_INTRO:
        al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ch->theme_color);
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 200,
            ALLEGRO_ALIGN_CENTER, ch->name);
        al_draw_text(font, al_map_rgb(230, 230, 230), SCREEN_WIDTH / 2, 350,
            ALLEGRO_ALIGN_CENTER, "ENTER para comecar");
        break;

    case STATE_CHAPTER_TEST:
    case STATE_PLAY: {
        if (!phase) phase = get_current_phase(game);  // Garantir que phase existe
        if (!phase) break;  // CRÍTICO: se phase for NULL, não desenhar nada

        // Background - usar imagem correta do capítulo atual
        ALLEGRO_BITMAP* current_bg = NULL;
        switch (game->current_chapter) {
        case 0: current_bg = game->bg_linear; break;
        case 1: current_bg = game->bg_quadratica; break;
        case 2: current_bg = game->bg_expo; break;
        case 3: current_bg = game->bg_logaritmica; break;
        case 4: current_bg = game->bg_racional; break;
        case 5: current_bg = game->bg_trigonometrica; break;
        }

        if (current_bg) {
            // Desenha background escalado para tela cheia
            al_draw_scaled_bitmap(current_bg,
                0, 0,
                al_get_bitmap_width(current_bg),
                al_get_bitmap_height(current_bg),
                0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        }
        else {
            // Fallback: gradiente suave
            for (int i = 0; i < SCREEN_HEIGHT; i++) {
                float ratio = (float)i / SCREEN_HEIGHT;
                ALLEGRO_COLOR line_color = al_map_rgb(
                    30 - (int)(20 * ratio),
                    30 - (int)(20 * ratio),
                    60 - (int)(40 * ratio)
                );
                al_draw_line(0, i, SCREEN_WIDTH, i, line_color, 1);
            }
        }

        // GRID CARTESIANO (tecla P para ativar)
        if (game->show_grid) {
            ALLEGRO_COLOR grid_color = al_map_rgba(255, 255, 255, 25);  // REDUZIDO: 50 → 25
            ALLEGRO_COLOR axis_color = al_map_rgba(255, 255, 255, 100);  // REDUZIDO: 120 → 100

            // Linhas verticais
            for (int x = -20; x <= 20; x++) {
                int sx, sy_top, sy_bot;
                world_to_screen(&game->camera, x, 15.0, &sx, &sy_top);
                world_to_screen(&game->camera, x, -15.0, &sx, &sy_bot);
                ALLEGRO_COLOR color = (x == 0) ? axis_color : grid_color;
                al_draw_line(sx, sy_top, sx, sy_bot, color, (x == 0) ? 2 : 1);

                if (x % 5 == 0 && x != 0) {
                    char num[8];
                    snprintf(num, sizeof(num), "%d", x);
                    al_draw_text(font, al_map_rgba(255, 255, 255, 200), sx, sy_bot + 5,
                        ALLEGRO_ALIGN_CENTER, num);
                }
            }

            // Linhas horizontais
            for (int y = -15; y <= 15; y++) {
                int sx_left, sx_right, sy;
                world_to_screen(&game->camera, -20.0, y, &sx_left, &sy);
                world_to_screen(&game->camera, 20.0, y, &sx_right, &sy);
                ALLEGRO_COLOR color = (y == 0) ? axis_color : grid_color;
                al_draw_line(sx_left, sy, sx_right, sy, color, (y == 0) ? 2 : 1);

                if (y % 5 == 0 && y != 0) {
                    char num[8];
                    snprintf(num, sizeof(num), "%d", y);
                    int text_x, text_y;
                    world_to_screen(&game->camera, -1.0, y, &text_x, &text_y);
                    al_draw_text(font, al_map_rgba(255, 255, 255, 200), text_x, text_y - 5,
                        ALLEGRO_ALIGN_RIGHT, num);
                }
            }

            // Origem (0,0)
            int ox, oy;
            world_to_screen(&game->camera, 0, 0, &ox, &oy);
            al_draw_filled_circle(ox, oy, 4, al_map_rgba(255, 255, 0, 200));
            al_draw_text(font, al_map_rgb(255, 255, 100), ox + 10, oy - 10, 0, "O(0,0)");
        }

        // Preview - linha da função (SEMPRE ATIVO quando não está atirando!)
        if ((game->ui.preview_enabled || game->key_shift) && !game->arrow.active) {
            // Cor do preview: amarelo brilhante com transparência
            ALLEGRO_COLOR preview_color = al_map_rgba(255, 255, 0, 200);
            double vx_preview = game->player.facing_right ? 15.0 : -15.0;  // AUMENTADO: preview mais rápido

            // Preview sai do peito (meio do corpo, não do pé) - +1.5 unidades
            arrow_draw_preview(game->player.x, game->player.y + 1.5, vx_preview,
                &game->current_function, preview_color, &game->camera);
        }

        // PLATAFORMA NO CHÃO - Verde com gradiente
        int ground_y = SCREEN_HEIGHT - 100; // Altura da plataforma
        ALLEGRO_COLOR ground_color1 = al_map_rgb(50, 150, 50);   // Verde escuro
        ALLEGRO_COLOR ground_color2 = al_map_rgb(100, 200, 100); // Verde claro
        al_draw_filled_rectangle(0, ground_y, SCREEN_WIDTH, SCREEN_HEIGHT, ground_color1);

        // Gradiente na plataforma
        for (int i = 0; i < 20; i++) {
            float ratio = (float)i / 20.0f;
            ALLEGRO_COLOR line_color = al_map_rgb(
                50 + (int)(50 * ratio),
                150 + (int)(50 * ratio),
                50 + (int)(50 * ratio)
            );
            al_draw_line(0, ground_y + i, SCREEN_WIDTH, ground_y + i, line_color, 1);
        }

        // Borda da plataforma
        al_draw_line(0, ground_y, SCREEN_WIDTH, ground_y, al_map_rgb(200, 255, 200), 3);

        // Obstáculos
        if (phase) {
            for (int i = 0; i < phase->obstacle_count; i++) {
                Obstacle* obs = &phase->obstacles[i];
                int ox, oy;
                world_to_screen(&game->camera, obs->x, obs->y, &ox, &oy);

                int obs_w = (int)(obs->width * PIXELS_PER_UNIT * 1.5);  // 50% MAIOR!
                int obs_h = (int)(obs->height * PIXELS_PER_UNIT * 1.5);  // 50% MAIOR!

                ALLEGRO_COLOR obs_color;
                ALLEGRO_BITMAP* obs_sprite = NULL;

                // Escolher sprite/cor baseado no tipo
                switch (obs->type) {
                case OBS_ROCK:
                    obs_sprite = game->sprite_obstacle;
                    obs_color = al_map_rgb(100, 100, 100);
                    break;
                case OBS_WALL:
                    obs_color = al_map_rgb(120, 80, 60);
                    break;
                }

                // Desenhar sprite se disponível, senão forma geométrica
                if (obs_sprite && obs->type == OBS_ROCK) {
                    int sw = al_get_bitmap_width(obs_sprite);
                    int sh = al_get_bitmap_height(obs_sprite);
                    al_draw_scaled_bitmap(obs_sprite, 0, 0, sw, sh,
                        ox - obs_w / 2, oy - obs_h, obs_w, obs_h, 0);
                }
                else {
                    // Fallback: formas geométricas
                    if (obs->type == OBS_WALL) {
                        // Retângulo para parede
                        al_draw_filled_rectangle(ox - obs_w / 2, oy - obs_h,
                            ox + obs_w / 2, oy, obs_color);
                        al_draw_rectangle(ox - obs_w / 2, oy - obs_h,
                            ox + obs_w / 2, oy,
                            al_map_rgb(255, 255, 255), 2);
                    }
                    else {
                        // Retângulo para rock
                        al_draw_filled_rectangle(ox - obs_w / 2, oy - obs_h,
                            ox + obs_w / 2, oy, obs_color);
                        al_draw_rectangle(ox - obs_w / 2, oy - obs_h,
                            ox + obs_w / 2, oy,
                            al_map_rgb(255, 255, 255), 2);
                    }
                }
            }
        }

        // Inimigos
        if (phase) {
            for (int i = 0; i < phase->enemy_count; i++) {
                Enemy* enemy = &phase->enemies[i];
                if (!enemy->active) continue;

                int ex, ey;
                world_to_screen(&game->camera, enemy->x, enemy->y, &ex, &ey);

                // Tamanho do inimigo
                int enemy_size = (int)(1.5 * PIXELS_PER_UNIT);

                // Desenhar sprite se disponível
                if (game->sprite_enemy) {
                    int sw = al_get_bitmap_width(game->sprite_enemy);
                    int sh = al_get_bitmap_height(game->sprite_enemy);
                    // Flip horizontal baseado na direção
                    int flags = (enemy->move_dir < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                    al_draw_scaled_bitmap(game->sprite_enemy, 0, 0, sw, sh,
                        ex - enemy_size / 2, ey - enemy_size, enemy_size, enemy_size, flags);
                }
                else {
                    // Fallback: quadrado vermelho
                    al_draw_filled_rectangle(ex - enemy_size / 2, ey - enemy_size,
                        ex + enemy_size / 2, ey, al_map_rgb(200, 50, 50));
                    al_draw_rectangle(ex - enemy_size / 2, ey - enemy_size,
                        ex + enemy_size / 2, ey, al_map_rgb(255, 100, 100), 2);
                    // Olhos para indicar direção
                    int eye_offset = (enemy->move_dir > 0) ? 5 : -5;
                    al_draw_filled_circle(ex + eye_offset, ey - enemy_size + 10, 3, al_map_rgb(255, 255, 255));
                }
            }
        }

        // Coletáveis - com sprites BEM MAIORES! (não desenhar em telas de transição)
        if (phase && (game->state == STATE_PLAY || game->state == STATE_CHAPTER_TEST || game->state == STATE_BOSS_FIGHT)) {
            for (int i = 0; i < phase->collectible_count; i++) {
                Collectible* c = &phase->items[i];
                if (c->collected) continue;
                int cx, cy;
                world_to_screen(&game->camera, c->x, c->y, &cx, &cy);

                // Tamanho do sprite (DOBRADO para ficar bem visível!)
                int sprite_size = (int)(c->radius * PIXELS_PER_UNIT * 8.0);  // 8x MAIOR!

                ALLEGRO_BITMAP* sprite = NULL;
                ALLEGRO_COLOR fallback_color;

                // Escolher sprite baseado no tipo
                if (c->type == COLLECT_KEY) {
                    sprite = game->sprite_key;
                    fallback_color = al_map_rgb(255, 255, 100);  // Amarelo
                }
                else if (c->type == COLLECT_APPLE_GOLD) {
                    sprite = game->sprite_apple_gold;
                    fallback_color = al_map_rgb(255, 200, 50);   // Dourado
                }
                else {  // COLLECT_APPLE
                    sprite = game->sprite_apple;
                    fallback_color = al_map_rgb(255, 50, 50);    // Vermelho
                }

                // Desenhar sprite se disponível, senão círculo
                if (sprite) {
                    int sw = al_get_bitmap_width(sprite);
                    int sh = al_get_bitmap_height(sprite);
                    al_draw_scaled_bitmap(sprite, 0, 0, sw, sh,
                        cx - sprite_size / 2, cy - sprite_size / 2,
                        sprite_size, sprite_size, 0);
                }
                else {
                    // Fallback: círculo colorido
                    int radius = (int)(c->radius * PIXELS_PER_UNIT);
                    al_draw_filled_circle(cx, cy, radius, fallback_color);
                    al_draw_circle(cx, cy, radius, al_map_rgb(200, 200, 200), 2);
                }
            }
        }

        // Player
        int px, py;
        world_to_screen(&game->camera, game->player.x, game->player.y, &px, &py);
        player_draw(&game->player, px, py);

        // Flecha
        if (game->arrow.active) {
            int ax, ay;
            world_to_screen(&game->camera, game->arrow.current_x, game->arrow.current_y, &ax, &ay);
            arrow_draw(&game->arrow, ax, ay);
        }

        // HUD
        char text[128];
        snprintf(text, sizeof(text), "FLECHAS: %d", game->arrows_remaining);
        al_draw_text(font, al_map_rgb(255, 255, 255), 20, 20, 0, text);

        int keys = phase_count_keys_collected(phase);
        snprintf(text, sizeof(text), "CHAVES: %d / 3", keys);
        al_draw_text(font, al_map_rgb(255, 255, 100), 20, 50, 0, text);

        // BOX VALORES ATUAIS (canto inferior esquerdo, com transparência)
        int box_x = 20;
        int box_y = SCREEN_HEIGHT - 180;  // Mais próximo do fundo
        int box_w = 280;
        int box_h = 200;
        al_draw_filled_rounded_rectangle(box_x, box_y, box_x + box_w, box_y + box_h,
            8, 8, al_map_rgba(0, 0, 0, 120));  // ~47% opacidade (mais transparente)
        al_draw_rounded_rectangle(box_x, box_y, box_x + box_w, box_y + box_h,
            8, 8, al_map_rgb(100, 200, 255), 2);
        draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, box_y + 8, 0, 1.3, "VALORES ATUAIS:");

        int param_y = box_y + 30;
        if (game->current_function.type == FN_LINEAR) {
            snprintf(text, sizeof(text), "m = %.2f  k = %.2f",
                game->current_function.params.linear.m, game->current_function.params.linear.k);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y, 0, 1.3, text);
        }
        else if (game->current_function.type == FN_QUADRATIC) {
            snprintf(text, sizeof(text), "a = %.2f  h = %.2f",
                game->current_function.params.quadratic.a, game->current_function.params.quadratic.h);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y, 0, 1.3, text);
            snprintf(text, sizeof(text), "k = %.2f", game->current_function.params.quadratic.kq);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y + 25, 0, 1.3, text);
        }
        else if (game->current_function.type == FN_EXPONENTIAL) {
            snprintf(text, sizeof(text), "a=%.2f b=%.2f c=%.2f",
                game->current_function.params.exponential.a,
                game->current_function.params.exponential.b,
                game->current_function.params.exponential.c);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y, 0, 1.3, text);
        }
        else if (game->current_function.type == FN_LOGARITHMIC) {
            snprintf(text, sizeof(text), "a=%.2f b=%.2f c=%.2f",
                game->current_function.params.logarithmic.a,
                game->current_function.params.logarithmic.b,
                game->current_function.params.logarithmic.c);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y, 0, 1.3, text);
        }
        else if (game->current_function.type == FN_RATIONAL) {
            snprintf(text, sizeof(text), "a=%.2f b=%.2f",
                game->current_function.params.rational.a, game->current_function.params.rational.b);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y, 0, 1.3, text);
            snprintf(text, sizeof(text), "c=%.2f d=%.2f",
                game->current_function.params.rational.c, game->current_function.params.rational.d);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y + 25, 0, 1.3, text);
        }
        else if (game->current_function.type == FN_TRIGONOMETRIC) {
            snprintf(text, sizeof(text), "a=%.2f b=%.2f",
                game->current_function.params.trigonometric.a, game->current_function.params.trigonometric.b);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y, 0, 1.3, text);
            snprintf(text, sizeof(text), "c=%.2f d=%.2f",
                game->current_function.params.trigonometric.c, game->current_function.params.trigonometric.d);
            draw_text_scaled(font, al_map_rgb(255, 255, 255), box_x + 10, param_y + 25, 0, 1.3, text);
        }

        // Mostrar equação atual
        int eq_y = param_y + 60;
        if (game->current_function.type == FN_LINEAR) {
            snprintf(text, sizeof(text), "y = %.2fx %+.2f",
                game->current_function.params.linear.m,
                game->current_function.params.linear.k);
            draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, eq_y, 0, 1.2, text);
        }
        else if (game->current_function.type == FN_QUADRATIC) {
            snprintf(text, sizeof(text), "y = %.2f(x-%.2f)^2+%.2f",
                game->current_function.params.quadratic.a,
                game->current_function.params.quadratic.h,
                game->current_function.params.quadratic.kq);
            draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, eq_y, 0, 1.1, text);
        }
        else if (game->current_function.type == FN_EXPONENTIAL) {
            snprintf(text, sizeof(text), "y = %.2f*%.2f^x+%.2f",
                game->current_function.params.exponential.a,
                game->current_function.params.exponential.b,
                game->current_function.params.exponential.c);
            draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, eq_y, 0, 1.1, text);
        }
        else if (game->current_function.type == FN_LOGARITHMIC) {
            snprintf(text, sizeof(text), "y = %.2f*log(x+%.2f)+%.2f",
                game->current_function.params.logarithmic.a,
                game->current_function.params.logarithmic.b,
                game->current_function.params.logarithmic.c);
            draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, eq_y, 0, 1.0, text);
        }
        else if (game->current_function.type == FN_RATIONAL) {
            snprintf(text, sizeof(text), "y = (%.2fx+%.2f)/(%.2fx+%.2f)",
                game->current_function.params.rational.a,
                game->current_function.params.rational.b,
                game->current_function.params.rational.c,
                game->current_function.params.rational.d);
            draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, eq_y, 0, 1.0, text);
        }
        else if (game->current_function.type == FN_TRIGONOMETRIC) {
            snprintf(text, sizeof(text), "y = %.2f*sin(%.2fx+%.2f)+%.2f",
                game->current_function.params.trigonometric.a,
                game->current_function.params.trigonometric.b,
                game->current_function.params.trigonometric.c,
                game->current_function.params.trigonometric.d);
            draw_text_scaled(font, al_map_rgb(200, 255, 200), box_x + 10, eq_y, 0, 1.0, text);
        }

        // Mostrar o que está editando
        if (game->ui.editor.selected != PARAM_NONE) {
            int edit_y = eq_y + 30;
            const char* param_name = "";
            if (game->current_function.type == FN_LINEAR) {
                if (game->ui.editor.selected == PARAM_M) param_name = "m (inclinacao)";
                else if (game->ui.editor.selected == PARAM_K) param_name = "k (intercepto)";
            }
            else if (game->current_function.type == FN_QUADRATIC) {
                if (game->ui.editor.selected == PARAM_A) param_name = "a";
                else if (game->ui.editor.selected == PARAM_H) param_name = "h";
                else if (game->ui.editor.selected == PARAM_KQ) param_name = "k";
            }
            else {
                if (game->ui.editor.selected == PARAM_A) param_name = "a";
                else if (game->ui.editor.selected == PARAM_H) param_name = "b";
                else if (game->ui.editor.selected == PARAM_KQ) param_name = "c";
                else if (game->ui.editor.selected == PARAM_D) param_name = "d";
            }

            char edit_text[128];
            snprintf(edit_text, sizeof(edit_text), "Editando: %s", param_name);
            draw_text_scaled(font, al_map_rgb(255, 255, 100), box_x + 10, edit_y, 0, 1.2, edit_text);

            snprintf(text, sizeof(text), "Valor: %s%s",
                game->ui.editor.is_negative ? "-" : "", game->ui.editor.buffer);
            draw_text_scaled(font, al_map_rgb(255, 200, 100), box_x + 10, edit_y + 25, 0, 1.2, text);
        }

        // Chamar função que desenha sidebar de comandos
        draw_command_hud(game, font);

        // Barra especial (dentro da sidebar)
        int sidebar_x = SCREEN_WIDTH - 250;
        int sidebar_w = 230;
        int bar_x = sidebar_x + 10;
        int bar_y = 80 + 40 + (25 * 11) + 40;  // Posição após comandos (ajustado para novo sidebar_y e comando H)
        int bar_w = sidebar_w - 20;
        int bar_h = 20;
        al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(40, 40, 40));
        int fill_w = (int)(bar_w * game->ui.special.value);
        if (fill_w > bar_w) fill_w = bar_w;
        ALLEGRO_COLOR fill_color = game->ui.special.value >= 1.0 ?
            al_map_rgb(255, 200, 50) : al_map_rgb(100, 100, 255);
        if (fill_w > 0) {
            al_draw_filled_rectangle(bar_x, bar_y, bar_x + fill_w, bar_y + bar_h, fill_color);
        }
        al_draw_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(200, 200, 200), 2);

        snprintf(text, sizeof(text), "ESPECIAL: %.0f%%", game->ui.special.value * 100);
        al_draw_text(font, al_map_rgb(255, 255, 255), bar_x + bar_w / 2, bar_y - 20,
            ALLEGRO_ALIGN_CENTER, text);

        // BARRA DE FORÇA DO ARCO (mostra enquanto segura ESPAÇO)
        if (game->key_space_down && game->player.state == PLAYER_SHOOT) {
            int charge_bar_y = bar_y + 50;  // Abaixo da barra especial
            // Fundo da barra
            al_draw_filled_rectangle(bar_x, charge_bar_y, bar_x + bar_w, charge_bar_y + bar_h,
                al_map_rgb(40, 40, 40));

            // Preenchimento (verde → amarelo → vermelho conforme carrega)
            double charge_ratio = game->arrow.charge_time / game->arrow.max_charge_time;
            if (charge_ratio > 1.0) charge_ratio = 1.0;
            int charge_fill_w = (int)(bar_w * charge_ratio);

            ALLEGRO_COLOR charge_color;
            if (charge_ratio < 0.5) {
                // Verde → Amarelo (0% a 50%)
                charge_color = al_map_rgb(
                    (int)(255 * charge_ratio * 2),
                    255,
                    0);
            }
            else {
                // Amarelo → Vermelho (50% a 100%)
                charge_color = al_map_rgb(
                    255,
                    (int)(255 * (1.0 - charge_ratio) * 2),
                    0);
            }

            if (charge_fill_w > 0) {
                al_draw_filled_rectangle(bar_x, charge_bar_y, bar_x + charge_fill_w,
                    charge_bar_y + bar_h, charge_color);
            }

            // Borda da barra
            al_draw_rectangle(bar_x, charge_bar_y, bar_x + bar_w, charge_bar_y + bar_h,
                al_map_rgb(200, 200, 200), 2);

            // Texto da força
            double power_percent = (1.0 + charge_ratio) * 100.0;  // 100% a 200%
            snprintf(text, sizeof(text), "FORCA: %.0f%%", power_percent);
            al_draw_text(font, al_map_rgb(255, 255, 255), bar_x + bar_w / 2, charge_bar_y - 20,
                ALLEGRO_ALIGN_CENTER, text);
        }

        if (game->show_max_hint) {
            int panel_w = 600;
            int panel_h = 200;
            int panel_x = (SCREEN_WIDTH - panel_w) / 2;
            int panel_y = 100;
            al_draw_filled_rounded_rectangle(panel_x, panel_y, panel_x + panel_w, panel_y + panel_h,
                8, 8, al_map_rgba(20, 20, 80, 220));
            al_draw_rounded_rectangle(panel_x, panel_y, panel_x + panel_w, panel_y + panel_h,
                8, 8, al_map_rgb(150, 150, 255), 2);
            al_draw_text(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2, panel_y + 20,
                ALLEGRO_ALIGN_CENTER, "*** DICA MAXIMA ***");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, panel_y + 70,
                ALLEGRO_ALIGN_CENTER, "Funcao Linear: y = m*x + k");
        }

        // Mensagem de troca de função
        if (game->function_change_message_timer > 0.0) {
            const char* function_names[] = { "Linear", "Quadratica", "Exponencial", "Logaritmica", "Racional", "Trigonometrica" };
            char msg[128];
            snprintf(msg, sizeof(msg), "Funcao alterada para: %s", function_names[game->current_function.type]);

            // Fade out effect baseado no timer
            float alpha = (game->function_change_message_timer / 2.0) * 255.0;
            if (alpha > 255.0) alpha = 255.0;

            al_draw_filled_rounded_rectangle(SCREEN_WIDTH / 2 - 250, 150, SCREEN_WIDTH / 2 + 250, 220,
                8, 8, al_map_rgba(50, 50, 100, (int)(alpha * 0.9)));
            al_draw_text(font, al_map_rgba(255, 255, 100, (int)alpha), SCREEN_WIDTH / 2, 170,
                ALLEGRO_ALIGN_CENTER, msg);
        }
        break;
    }

    case STATE_BOSS_FIGHT: {
        if (!phase) phase = get_current_phase(game);  // CRÍTICO: garantir phase carregado!

        // Background correto do capítulo
        ALLEGRO_BITMAP* current_bg = NULL;
        switch (game->current_chapter) {
        case 0: current_bg = game->bg_linear; break;
        case 1: current_bg = game->bg_quadratica; break;
        case 2: current_bg = game->bg_expo; break;
        case 3: current_bg = game->bg_logaritmica; break;
        case 4: current_bg = game->bg_racional; break;
        case 5: current_bg = game->bg_trigonometrica; break;
        }

        if (current_bg) {
            al_draw_scaled_bitmap(current_bg, 0, 0,
                al_get_bitmap_width(current_bg), al_get_bitmap_height(current_bg),
                0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        }
        else {
            // Fallback: gradiente escuro (atmosfera tensa)
            for (int i = 0; i < SCREEN_HEIGHT; i++) {
                float ratio = (float)i / SCREEN_HEIGHT;
                ALLEGRO_COLOR line_color = al_map_rgb(
                    20 - (int)(10 * ratio),
                    20 - (int)(10 * ratio),
                    40 - (int)(20 * ratio)
                );
                al_draw_line(0, i, SCREEN_WIDTH, i, line_color, 1);
            }
        }

        // Plataforma
        int ground_y = SCREEN_HEIGHT - 100;
        al_draw_filled_rectangle(0, ground_y, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgb(50, 50, 70));
        al_draw_line(0, ground_y, SCREEN_WIDTH, ground_y, al_map_rgb(150, 150, 200), 3);

        // PREVIEW da trajetória no boss fight (ESSENCIAL!)
        if ((game->ui.preview_enabled || game->key_shift) && !game->arrow.active) {
            ALLEGRO_COLOR preview_color = al_map_rgba(255, 255, 0, 200);
            double vx_preview = game->player.facing_right ? 15.0 : -15.0;  // AUMENTADO: preview mais rápido
            arrow_draw_preview(game->player.x, game->player.y + 1.5, vx_preview,
                &game->current_function, preview_color, &game->camera);
        }

        // Desenhar todos os bosses
        if (phase && phase->boss_count > 0 && phase->boss_count <= 4) {
            for (int boss_idx = 0; boss_idx < phase->boss_count; boss_idx++) {
                Boss* boss = &phase->bosses[boss_idx];
                if (!boss || !boss->active) continue;

                int bx, by;
                world_to_screen(&game->camera, boss->x, boss->y, &bx, &by);
                int boss_size = (int)(3.0 * PIXELS_PER_UNIT);  // Boss é GRANDE!

                if (game->sprite_boss) {
                    int sw = al_get_bitmap_width(game->sprite_boss);
                    int sh = al_get_bitmap_height(game->sprite_boss);
                    int flags = (boss->move_dir < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;

                    // Verificar se é sprite sheet (múltiplos frames)
                    // Se altura for menor que largura/4, provavelmente é sprite sheet horizontal
                    if (sh < sw / 4) {
                        // Sprite sheet horizontal - usar apenas primeiro frame
                        int frame_width = sw / 4;  // Assumindo 4 frames
                        al_draw_scaled_bitmap(game->sprite_boss, 0, 0, frame_width, sh,
                            bx - boss_size / 2, by - boss_size, boss_size, boss_size, flags);
                    }
                    else if (sw > sh * 2) {
                        // Sprite sheet horizontal com mais frames
                        int frame_width = sw / 4;  // Assumindo 4 frames
                        al_draw_scaled_bitmap(game->sprite_boss, 0, 0, frame_width, sh,
                            bx - boss_size / 2, by - boss_size, boss_size, boss_size, flags);
                    }
                    else {
                        // Sprite único - desenhar completo
                        al_draw_scaled_bitmap(game->sprite_boss, 0, 0, sw, sh,
                            bx - boss_size / 2, by - boss_size, boss_size, boss_size, flags);
                    }
                }
                else {
                    // Fallback: POLVO estilizado (corpo + tentáculos)
                    // Corpo principal (cabeça do polvo)
                    al_draw_filled_circle(bx, by - boss_size / 2, boss_size / 3, al_map_rgb(150, 50, 150));
                    al_draw_circle(bx, by - boss_size / 2, boss_size / 3, al_map_rgb(200, 100, 200), 3);

                    // Olhos grandes e brilhantes
                    al_draw_filled_circle(bx - 12, by - boss_size / 2 + 5, 10, al_map_rgb(255, 255, 255));
                    al_draw_filled_circle(bx + 12, by - boss_size / 2 + 5, 10, al_map_rgb(255, 255, 255));
                    al_draw_filled_circle(bx - 10, by - boss_size / 2 + 5, 6, al_map_rgb(255, 0, 0));
                    al_draw_filled_circle(bx + 10, by - boss_size / 2 + 5, 6, al_map_rgb(255, 0, 0));

                    // Tentáculos (8 tentáculos de polvo)
                    for (int t = 0; t < 8; t++) {
                        float angle = (t / 8.0) * 6.28318 + 1.5;  // Distribuir ao redor, começando embaixo
                        int tx = bx + (int)(cos(angle) * boss_size / 4);
                        int ty = by - boss_size / 3 + (int)(sin(angle) * boss_size / 4);
                        int tentacle_len = boss_size / 2 + 10;
                        int tx_end = tx + (int)(cos(angle) * tentacle_len);
                        int ty_end = ty + (int)(sin(angle) * tentacle_len);
                        // Tentáculo grosso
                        al_draw_line(tx, ty, tx_end, ty_end, al_map_rgb(120, 40, 120), 5);
                        // Ventosas (bolinhas ao longo)
                        int mid_x = (tx + tx_end) / 2;
                        int mid_y = (ty + ty_end) / 2;
                        al_draw_filled_circle(mid_x, mid_y, 3, al_map_rgb(200, 150, 200));
                    }
                }

                // BARRA DE VIDA DO BOSS (acima dele)
                int health_bar_w = boss_size;
                int health_bar_h = 12;
                int health_bar_x = bx - health_bar_w / 2;
                int health_bar_y = by - boss_size - 25 - (boss_idx * 20);  // Offset vertical para múltiplos bosses

                // Fundo da barra
                al_draw_filled_rectangle(health_bar_x, health_bar_y,
                    health_bar_x + health_bar_w, health_bar_y + health_bar_h,
                    al_map_rgb(40, 40, 40));

                // Preenchimento (verde → amarelo → vermelho)
                double health_ratio = boss->health / boss->max_health;
                int fill_w = (int)(health_bar_w * health_ratio);
                ALLEGRO_COLOR health_color;
                if (health_ratio > 0.6) health_color = al_map_rgb(50, 255, 50);
                else if (health_ratio > 0.3) health_color = al_map_rgb(255, 255, 0);
                else health_color = al_map_rgb(255, 50, 50);

                if (fill_w > 0) {
                    al_draw_filled_rectangle(health_bar_x, health_bar_y,
                        health_bar_x + fill_w, health_bar_y + health_bar_h, health_color);
                }

                al_draw_rectangle(health_bar_x, health_bar_y,
                    health_bar_x + health_bar_w, health_bar_y + health_bar_h,
                    al_map_rgb(255, 255, 255), 2);
            }
        }

        // Compatibilidade: desenhar boss antigo se não usar array
        if (phase && phase->boss_count == 0 && phase->has_boss) {
            Boss* boss = &phase->boss;
            if (boss && boss->active) {
                int bx, by;
                world_to_screen(&game->camera, boss->x, boss->y, &bx, &by);
                int boss_size = (int)(3.0 * PIXELS_PER_UNIT);

                if (game->sprite_boss) {
                    int sw = al_get_bitmap_width(game->sprite_boss);
                    int sh = al_get_bitmap_height(game->sprite_boss);
                    int flags = (boss->move_dir < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;

                    if (sh < sw / 4) {
                        int frame_width = sw / 4;
                        al_draw_scaled_bitmap(game->sprite_boss, 0, 0, frame_width, sh,
                            bx - boss_size / 2, by - boss_size, boss_size, boss_size, flags);
                    }
                    else {
                        al_draw_scaled_bitmap(game->sprite_boss, 0, 0, sw, sh,
                            bx - boss_size / 2, by - boss_size, boss_size, boss_size, flags);
                    }
                }

                // Barra de vida
                int health_bar_w = boss_size;
                int health_bar_h = 12;
                int health_bar_x = bx - health_bar_w / 2;
                int health_bar_y = by - boss_size - 25;

                al_draw_filled_rectangle(health_bar_x, health_bar_y,
                    health_bar_x + health_bar_w, health_bar_y + health_bar_h,
                    al_map_rgb(40, 40, 40));

                double health_ratio = boss->health / boss->max_health;
                int fill_w = (int)(health_bar_w * health_ratio);
                ALLEGRO_COLOR health_color;
                if (health_ratio > 0.6) health_color = al_map_rgb(50, 255, 50);
                else if (health_ratio > 0.3) health_color = al_map_rgb(255, 255, 0);
                else health_color = al_map_rgb(255, 50, 50);

                if (fill_w > 0) {
                    al_draw_filled_rectangle(health_bar_x, health_bar_y,
                        health_bar_x + fill_w, health_bar_y + health_bar_h, health_color);
                }

                al_draw_rectangle(health_bar_x, health_bar_y,
                    health_bar_x + health_bar_w, health_bar_y + health_bar_h,
                    al_map_rgb(255, 255, 255), 2);
            }
        }

        // Desenhar projéteis do boss
        for (int i = 0; i < 5; i++) {
            if (!game->boss_arrows[i].active) continue;
            int bax, bay;
            world_to_screen(&game->camera, game->boss_arrows[i].x, game->boss_arrows[i].y, &bax, &bay);

            // Flecha do boss (diferente - roxa e invertida)
            ALLEGRO_COLOR boss_arrow_color = al_map_rgb(200, 50, 200);
            al_draw_line(bax - 15, bay, bax + 15, bay, al_map_rgb(100, 50, 100), 4);
            al_draw_filled_triangle(bax - 15, bay,
                bax - 5, bay - 6,
                bax - 5, bay + 6, boss_arrow_color);
            al_draw_filled_circle(bax, bay, 4, al_map_rgb(255, 100, 255));
        }

        // Player
        int px, py;
        world_to_screen(&game->camera, game->player.x, game->player.y, &px, &py);
        player_draw(&game->player, px, py);

        // Flecha do jogador
        if (game->arrow.active) {
            int ax, ay;
            world_to_screen(&game->camera, game->arrow.current_x, game->arrow.current_y, &ax, &ay);
            arrow_draw(&game->arrow, ax, ay);
        }

        // HUD do Boss Fight
        char text[128];

        // Aviso BOSS FIGHT
        al_draw_filled_rounded_rectangle(SCREEN_WIDTH / 2 - 150, 10, SCREEN_WIDTH / 2 + 150, 50,
            8, 8, al_map_rgba(100, 20, 100, 220));
        al_draw_text(font, al_map_rgb(255, 100, 255), SCREEN_WIDTH / 2, 20,
            ALLEGRO_ALIGN_CENTER, "*** BOSS FIGHT ***");

        // Flechas restantes
        snprintf(text, sizeof(text), "FLECHAS: %d", game->arrows_remaining);
        al_draw_text(font, al_map_rgb(255, 100, 100), 20, 20, 0, text);

        // BARRA DE VIDA DO PLAYER
        int player_health_bar_x = 20;
        int player_health_bar_y = 50;
        int player_health_bar_w = 200;
        int player_health_bar_h = 20;

        // Fundo
        al_draw_filled_rectangle(player_health_bar_x, player_health_bar_y,
            player_health_bar_x + player_health_bar_w, player_health_bar_y + player_health_bar_h,
            al_map_rgb(40, 40, 40));

        // Preenchimento baseado na vida (verde → amarelo → vermelho)
        double player_health_ratio = game->player.health / game->player.max_health;
        if (player_health_ratio < 0) player_health_ratio = 0;
        int player_fill_w = (int)(player_health_bar_w * player_health_ratio);

        ALLEGRO_COLOR player_health_color;
        if (player_health_ratio > 0.6) player_health_color = al_map_rgb(50, 255, 50);
        else if (player_health_ratio > 0.3) player_health_color = al_map_rgb(255, 255, 0);
        else player_health_color = al_map_rgb(255, 50, 50);

        if (player_fill_w > 0) {
            al_draw_filled_rectangle(player_health_bar_x, player_health_bar_y,
                player_health_bar_x + player_fill_w, player_health_bar_y + player_health_bar_h,
                player_health_color);
        }

        al_draw_rectangle(player_health_bar_x, player_health_bar_y,
            player_health_bar_x + player_health_bar_w, player_health_bar_y + player_health_bar_h,
            al_map_rgb(255, 255, 255), 2);

        // Texto da vida
        snprintf(text, sizeof(text), "VIDA: %.0f HP", game->player.health);
        al_draw_text(font, al_map_rgb(255, 255, 255), player_health_bar_x + player_health_bar_w / 2,
            player_health_bar_y + 3, ALLEGRO_ALIGN_CENTER, text);

        // Vida dos bosses (numérica) - mostrar todos
        if (phase && phase->boss_count > 0 && phase->boss_count <= 4) {
            int y_offset = 20;
            for (int i = 0; i < phase->boss_count; i++) {
                if (phase->bosses[i].active) {
                    snprintf(text, sizeof(text), "BOSS %d: %.0f / %.0f HP", i + 1,
                        phase->bosses[i].health, phase->bosses[i].max_health);
                    al_draw_text(font, al_map_rgb(255, 100, 255), SCREEN_WIDTH - 20, y_offset,
                        ALLEGRO_ALIGN_RIGHT, text);
                    y_offset += 25;
                }
            }
        }
        else if (phase->has_boss) {
            snprintf(text, sizeof(text), "BOSS: %.0f / %.0f HP", phase->boss.health, phase->boss.max_health);
            al_draw_text(font, al_map_rgb(255, 100, 255), SCREEN_WIDTH - 20, 20,
                ALLEGRO_ALIGN_RIGHT, text);
        }

        // Comandos
        draw_command_hud(game, font);
        break;
    }

    case STATE_PAUSED:
        // Fundo escurecido
        al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, 200));

        // Painel de pausa
        al_draw_filled_rounded_rectangle(340, 200, 940, 520, 12, 12, al_map_rgb(40, 40, 60));
        al_draw_rounded_rectangle(340, 200, 940, 520, 12, 12, al_map_rgb(100, 150, 255), 4);

        al_draw_text(font, al_map_rgb(100, 150, 255), SCREEN_WIDTH / 2, 240,
            ALLEGRO_ALIGN_CENTER, "=== PAUSADO ===");

        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 300,
            ALLEGRO_ALIGN_CENTER, "ENTER - Continuar");
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 340,
            ALLEGRO_ALIGN_CENTER, "C - Trocar Funcao");
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 380,
            ALLEGRO_ALIGN_CENTER, "R - Reiniciar Fase");
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 420,
            ALLEGRO_ALIGN_CENTER, "M - Menu Principal");
        al_draw_text(font, al_map_rgb(150, 150, 150), SCREEN_WIDTH / 2, 470,
            ALLEGRO_ALIGN_CENTER, "ESC - Continuar");
        break;

    case STATE_STAGE_CLEAR: {
        al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, 180));

        // TELA FINAL ÉPICA - JOGO COMPLETADO!
        if (game->current_phase == 4 && game->current_chapter >= 5) {
            // Background épico escuro
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgb(10, 5, 30));

            // Título GIGANTE
            draw_text_scaled(font, al_map_rgb(255, 215, 0), SCREEN_WIDTH / 2, 100,
                ALLEGRO_ALIGN_CENTER, 4.0, "PARABENS!");

            // Subtítulo
            draw_text_scaled(font, al_map_rgb(200, 255, 200), SCREEN_WIDTH / 2, 200,
                ALLEGRO_ALIGN_CENTER, 2.5, "VOCE COMPLETOU ARCHER!");

            // Mensagem épica
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 300,
                ALLEGRO_ALIGN_CENTER, "Voce e um mago pronto para resolver");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 340,
                ALLEGRO_ALIGN_CENTER, "os problemas matematicos ao longo");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 380,
                ALLEGRO_ALIGN_CENTER, "do nosso universo!");

            // Estatísticas finais
            al_draw_text(font, al_map_rgb(255, 215, 0), SCREEN_WIDTH / 2, 460,
                ALLEGRO_ALIGN_CENTER, "6 FUNCOES DOMINADAS");
            al_draw_text(font, al_map_rgb(255, 215, 0), SCREEN_WIDTH / 2, 490,
                ALLEGRO_ALIGN_CENTER, "TODOS OS BOSSES DERROTADOS");

            // Instruções
            al_draw_text(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, 580,
                ALLEGRO_ALIGN_CENTER, "ENTER - Menu Principal");
        }
        // Tela especial se completou o BOSS de um capítulo específico
        else if (game->current_phase == 4) {
            // BOSS DERROTADO - Tela ÉPICA!
            al_draw_filled_rounded_rectangle(290, 150, 990, 570, 15, 15, al_map_rgb(60, 40, 80));
            al_draw_rounded_rectangle(290, 150, 990, 570, 15, 15, al_map_rgb(255, 215, 0), 5);

            // Título grande
            al_draw_text(font, al_map_rgb(255, 215, 0), SCREEN_WIDTH / 2, 200,
                ALLEGRO_ALIGN_CENTER, "*** BOSS DERROTADO! ***");
            al_draw_text(font, al_map_rgb(100, 255, 100), SCREEN_WIDTH / 2, 250,
                ALLEGRO_ALIGN_CENTER, "CAPITULO COMPLETO!");

            // Nome da função dominada
            char func_text[128];
            snprintf(func_text, sizeof(func_text), "Funcao %s dominada!",
                game->chapters[game->current_chapter].name);
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 320,
                ALLEGRO_ALIGN_CENTER, func_text);

            // Estrelas decorativas
            al_draw_text(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2 - 150, 250, 0, "***");
            al_draw_text(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2 + 130, 250, 0, "***");

            // Mensagem motivacional
            if (game->current_chapter + 1 < 6) {
                al_draw_text(font, al_map_rgb(200, 200, 255), SCREEN_WIDTH / 2, 400,
                    ALLEGRO_ALIGN_CENTER, "Proximo desafio aguarda!");
                al_draw_text(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, 450,
                    ALLEGRO_ALIGN_CENTER, "ENTER - Proximo Capitulo");
            }
            else {
                al_draw_text(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2, 400,
                    ALLEGRO_ALIGN_CENTER, "VOCE DOMINOU TODAS AS FUNCOES!");
                al_draw_text(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, 450,
                    ALLEGRO_ALIGN_CENTER, "ENTER - Menu Principal");
            }
        }
        else {
            // Fase normal completada
            al_draw_filled_rounded_rectangle(390, 210, 890, 510, 10, 10, al_map_rgb(40, 80, 40));
            al_draw_rounded_rectangle(390, 210, 890, 510, 10, 10, al_map_rgb(100, 255, 100), 3);
            al_draw_text(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, 250,
                ALLEGRO_ALIGN_CENTER, "FASE CONCLUIDA!");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 340,
                ALLEGRO_ALIGN_CENTER, "Parabens!");
            al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 400,
                ALLEGRO_ALIGN_CENTER, "ENTER - Proxima");
        }
        break;
    }

    case STATE_TRY_AGAIN:
        al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, 180));
        al_draw_filled_rounded_rectangle(390, 210, 890, 510, 10, 10, al_map_rgb(80, 40, 40));
        al_draw_rounded_rectangle(390, 210, 890, 510, 10, 10, al_map_rgb(255, 100, 100), 3);
        al_draw_text(font, al_map_rgb(255, 150, 150), SCREEN_WIDTH / 2, 250,
            ALLEGRO_ALIGN_CENTER, "TENTE NOVAMENTE");
        al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 340,
            ALLEGRO_ALIGN_CENTER, "Flechas esgotadas!");
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 400,
            ALLEGRO_ALIGN_CENTER, "ENTER - Tentar Novamente");
        break;

    case STATE_SPECIAL_SELECT: {
        // Background escurecido
        al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, 200));

        // Título
        al_draw_text(font, al_map_rgb(255, 215, 0), SCREEN_WIDTH / 2, 60,
            ALLEGRO_ALIGN_CENTER, "ESCOLHA SEU PODER ESPECIAL!");

        // Desenhar 3 cartas lado a lado
        int card_w = 280;
        int card_h = 400;
        int spacing = 40;
        int total_width = (card_w * 3) + (spacing * 2);
        int start_x = (SCREEN_WIDTH - total_width) / 2;
        int card_y = 150;

        for (int i = 0; i < 3; i++) {
            int card_x = start_x + i * (card_w + spacing);
            bool selected = (i == game->selected_special_index);

            // Cor da carta
            ALLEGRO_COLOR card_color = selected ?
                al_map_rgb(100, 80, 20) : al_map_rgb(40, 40, 60);
            ALLEGRO_COLOR border_color = selected ?
                al_map_rgb(255, 215, 0) : al_map_rgb(100, 100, 150);
            int border_width = selected ? 5 : 2;

            // Desenhar carta
            al_draw_filled_rounded_rectangle(card_x, card_y, card_x + card_w, card_y + card_h,
                12, 12, card_color);
            al_draw_rounded_rectangle(card_x, card_y, card_x + card_w, card_y + card_h,
                12, 12, border_color, border_width);

            // Nome do especial
            al_draw_text(font, al_map_rgb(255, 255, 100), card_x + card_w / 2, card_y + 40,
                ALLEGRO_ALIGN_CENTER, game->available_specials[i].name);

            // Linha divisória
            al_draw_line(card_x + 20, card_y + 80, card_x + card_w - 20, card_y + 80,
                al_map_rgb(150, 150, 150), 2);

            // Descrição (quebra de linha manual)
            al_draw_text(font, al_map_rgb(200, 200, 200), card_x + card_w / 2, card_y + 120,
                ALLEGRO_ALIGN_CENTER, game->available_specials[i].description);

            // Ícone/representação visual (placeholder)
            int icon_size = 80;
            int icon_x = card_x + card_w / 2;
            int icon_y = card_y + 220;

            ALLEGRO_COLOR icon_color;
            switch (game->available_specials[i].type) {
            case SPECIAL_SUPER_SPEED:
                icon_color = al_map_rgb(255, 100, 100);
                // Desenhar símbolo de velocidade (>>)
                al_draw_text(font, icon_color, icon_x, icon_y, ALLEGRO_ALIGN_CENTER, ">>>");
                break;
            case SPECIAL_SLOW_TIME:
                icon_color = al_map_rgb(100, 100, 255);
                // Desenhar símbolo de tempo lento (relógio)
                al_draw_circle(icon_x, icon_y + 20, 30, icon_color, 4);
                al_draw_line(icon_x, icon_y + 20, icon_x, icon_y, icon_color, 3);
                al_draw_line(icon_x, icon_y + 20, icon_x + 15, icon_y + 20, icon_color, 3);
                break;
            case SPECIAL_TRIPLE_ARROW:
                icon_color = al_map_rgb(100, 255, 100);
                // Desenhar 3 flechas
                for (int j = -1; j <= 1; j++) {
                    al_draw_line(icon_x - 20 + j * 15, icon_y + 20 + j * 10,
                        icon_x + 20 + j * 15, icon_y + 20 + j * 10, icon_color, 3);
                }
                break;
            }

            // Número da carta
            char num[8];
            snprintf(num, sizeof(num), "%d", i + 1);
            al_draw_filled_circle(card_x + 30, card_y + 30, 20, border_color);
            al_draw_text(font, al_map_rgb(255, 255, 255), card_x + 30, card_y + 20,
                ALLEGRO_ALIGN_CENTER, num);
        }

        // Instruções
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, card_y + card_h + 50,
            ALLEGRO_ALIGN_CENTER, "Use SETAS <- -> para navegar | ENTER para selecionar | Teclas 1-3");
        break;
    }
    }
}

void game_handle_key_down(Game* game, int keycode) {
    switch (keycode) {
    case ALLEGRO_KEY_UP: game->key_up = true; break;
    case ALLEGRO_KEY_DOWN: game->key_down = true; break;
    case ALLEGRO_KEY_LEFT: game->key_left = true; break;
    case ALLEGRO_KEY_RIGHT: game->key_right = true; break;
    case ALLEGRO_KEY_SPACE:
        game->key_space = true;
        game->key_space_down = true;
        break;
    case ALLEGRO_KEY_R: game->key_r = true; break;
    case ALLEGRO_KEY_P: game->key_p = true; break;
    case ALLEGRO_KEY_H: game->key_h = true; break;
    case ALLEGRO_KEY_ENTER:
        // Se está editando parâmetro, confirma
        if (game->ui.editor.selected != PARAM_NONE) {
            if (game->ui.editor.selected == PARAM_M) {
                game->current_function.params.linear.m = editor_get_value(&game->ui.editor);
            }
            else if (game->ui.editor.selected == PARAM_K) {
                game->current_function.params.linear.k = editor_get_value(&game->ui.editor);
            }
            editor_init(&game->ui.editor);
        }
        else {
            game->key_enter = true;
        }
        break;
    case ALLEGRO_KEY_ESCAPE: game->key_escape = true; break;
    case ALLEGRO_KEY_LSHIFT:
    case ALLEGRO_KEY_RSHIFT: game->key_shift = true; break;
    case ALLEGRO_KEY_OPENBRACE: game->key_bracket_left = true; break;
    case ALLEGRO_KEY_CLOSEBRACE: game->key_bracket_right = true; break;

        // CONTROLES COM LETRAS - MUITO MAIS FÁCIL!
    case ALLEGRO_KEY_M: game->key_m = true; break;
    case ALLEGRO_KEY_K: game->key_k = true; break;
    case ALLEGRO_KEY_Q: game->key_q = true; break;
    case ALLEGRO_KEY_E: game->key_e = true; break;
    case ALLEGRO_KEY_C: game->key_c = true; break;

    case ALLEGRO_KEY_1:
        if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 0;
        else if (game->state == STATE_SPECIAL_SELECT) game->selected_special_index = 0;
        else editor_select_param(&game->ui.editor, PARAM_M);
        break;
    case ALLEGRO_KEY_2:
        if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 1;
        else if (game->state == STATE_SPECIAL_SELECT) game->selected_special_index = 1;
        else editor_select_param(&game->ui.editor, PARAM_K);
        break;
    case ALLEGRO_KEY_3:
        if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 2;
        else if (game->state == STATE_SPECIAL_SELECT) game->selected_special_index = 2;
        else editor_input_digit(&game->ui.editor, 3);
        break;
    case ALLEGRO_KEY_4:
        if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 3;
        else editor_input_digit(&game->ui.editor, 4);
        break;
    case ALLEGRO_KEY_5:
        if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 4;
        else editor_input_digit(&game->ui.editor, 5);
        break;
    case ALLEGRO_KEY_6:
        if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 5;
        else editor_input_digit(&game->ui.editor, 6);
        break;
    case ALLEGRO_KEY_7: editor_input_digit(&game->ui.editor, 7); break;
    case ALLEGRO_KEY_8: editor_input_digit(&game->ui.editor, 8); break;
    case ALLEGRO_KEY_9: editor_input_digit(&game->ui.editor, 9); break;
    case ALLEGRO_KEY_0:
    case ALLEGRO_KEY_PAD_0: editor_input_digit(&game->ui.editor, 0); break;
    case ALLEGRO_KEY_MINUS:
    case ALLEGRO_KEY_PAD_MINUS: editor_toggle_sign(&game->ui.editor); break;
    case ALLEGRO_KEY_BACKSPACE: editor_backspace(&game->ui.editor); break;

        // ENTER normal ou numérico para confirmar
    case ALLEGRO_KEY_PAD_ENTER:
        // Confirma o valor editado
        if (game->ui.editor.selected == PARAM_M) {
            game->current_function.params.linear.m = editor_get_value(&game->ui.editor);
        }
        else if (game->ui.editor.selected == PARAM_K) {
            game->current_function.params.linear.k = editor_get_value(&game->ui.editor);
        }
        editor_init(&game->ui.editor);
        break;
    }
}

void game_handle_key_up(Game* game, int keycode) {
    switch (keycode) {
    case ALLEGRO_KEY_UP: game->key_up = false; break;
    case ALLEGRO_KEY_DOWN: game->key_down = false; break;
    case ALLEGRO_KEY_LEFT: game->key_left = false; break;
    case ALLEGRO_KEY_RIGHT: game->key_right = false; break;
    case ALLEGRO_KEY_SPACE:
        game->key_space = false;
        game->key_space_down = false;
        break;
    case ALLEGRO_KEY_LSHIFT:
    case ALLEGRO_KEY_RSHIFT: game->key_shift = false; break;

        // CONTROLES COM LETRAS
    case ALLEGRO_KEY_M: game->key_m = false; break;
    case ALLEGRO_KEY_K: game->key_k = false; break;
    case ALLEGRO_KEY_Q: game->key_q = false; break;
    case ALLEGRO_KEY_E: game->key_e = false; break;
    case ALLEGRO_KEY_C: game->key_c = false; break;
    }
}

// ========================================================================
// MAIN
// ========================================================================

int main(int argc, char** argv) {
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }

    if (!al_init_font_addon() || !al_init_ttf_addon() || !al_init_image_addon() ||
        !al_init_primitives_addon() || !al_install_keyboard()) {
        fprintf(stderr, "Falha ao inicializar addons.\n");
        return -1;
    }

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }
    al_set_window_title(display, "Archer - Jogo de Funcoes");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        fprintf(stderr, "Falha ao criar timer.\n");
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    Game game;
    game_init(&game);
    game_load_resources(&game);

    printf("[INFO] Archer iniciado!\n");

    al_start_timer(timer);

    bool running = true;
    bool redraw = false;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            game_update(&game, 1.0 / FPS);
            redraw = true;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            game_handle_key_down(&game, event.keyboard.keycode);
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && game.state == STATE_TITLE) {
                running = false;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            game_handle_key_up(&game, event.keyboard.keycode);
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            game_draw(&game);
            al_flip_display();
        }
    }

    printf("[INFO] Encerrando...\n");
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_display(display);

    return 0;
}

