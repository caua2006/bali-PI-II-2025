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
 
 #define SCREEN_WIDTH 1280
 #define SCREEN_HEIGHT 720
 #define FPS 60.0
 #define PIXELS_PER_UNIT 32.0
 
  // Tamanhos de fonte padronizados (escala sobre builtin 8x8)
 #define FONT_SIZE_TITLE 4.0      // Título principal (32px)
 #define FONT_SIZE_SUBTITLE 2.0    // Subtítulos (16px)
 #define FONT_SIZE_LARGE 2.5      // Texto grande/importante (20px)
 #define FONT_SIZE_MEDIUM 2.2     // Texto médio (18px)
 #define FONT_SIZE_NORMAL 1.8     // Texto normal/HUD (14px)
 #define FONT_SIZE_CONTROLS 1.5   // Menu de controles (12px)
 #define FONT_SIZE_SMALL 1.2      // Texto pequeno (10px)
 
  // --- CONFIG DE ASSETS (fácil de trocar depois) ---
 static const char* ASSET_BG_LINEAR = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/linear.jpeg";
 static const char* ASSET_BG_EXPO = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/bg/exponecial.jpg";
 static const char* ASSET_PLAYER_IDLE = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/player-idle.png";   // 256x64 (4x 64x64)
 static const char* ASSET_PLAYER_WALK = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/player-walk.png";   // 256x64 (4x 64x64)
 static const char* ASSET_PLAYER_SHOOT = "C:/Users/renan/source/repos/BALI-PI/x64/Debug/assets/sprites/player-shoot.png";  // 384x64 (6x 64x64) ou 256x128 (4x2)
 
 // ========================================================================
 // TIPOS E ESTRUTURAS
 // ========================================================================
 
 // Estados do jogo
 typedef enum {
     STATE_TITLE,
     STATE_CHAPTER_SELECT,
     STATE_CHAPTER_INTRO,
     STATE_CHAPTER_TEST,
     STATE_PLAY,
     STATE_STAGE_CLEAR,
     STATE_TRY_AGAIN
 } GameState;
 
 // Tipos de função
 typedef enum {
     FN_LINEAR,
     FN_QUADRATIC
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
 
 typedef struct {
     FunctionType type;
     union {
         LinearParams linear;
         QuadraticParams quadratic;
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
     double tmax;
     double current_x;
     double current_y;
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
 } Phase;
 
 // Capítulo
 typedef struct {
     const char* name;
     FunctionType function_type;
     ALLEGRO_COLOR theme_color;
     Phase test;
     Phase phases[3];
 } Chapter;
 
 // Editor de parâmetros
 typedef enum {
     PARAM_NONE,
     PARAM_M,
     PARAM_K
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
 
 // Progresso do jogo (desbloqueio de capítulos)
 typedef struct {
     int unlocked_chapters;  // Quantos capítulos desbloqueados (1-6)
     bool chapters_completed[6];  // Quais foram 100% completados
 } Progress;
 
 // Jogo completo
 typedef struct {
     GameState state;
     int current_chapter;
     int current_phase;
     int arrows_remaining;
 
     Progress progress;
 
     Player player;
     Arrow arrow;
     MathFunction current_function;
     Camera camera;
     UI ui;
 
     bool key_up, key_down, key_space, key_left, key_right;
     bool key_space_down, was_space_down;  // Para detectar soltar
     bool key_r, key_p, key_h;
     bool key_enter, key_escape;
     bool key_shift;
     bool key_m, key_k, key_q, key_e;  // Controles com letras
     bool key_bracket_left, key_bracket_right;  // [ e ]
 
     bool show_max_hint;
     double hint_timer;
     int chapter_menu_selection;
 
     Chapter chapters[6];
 
     // Backgrounds dos capítulos
     ALLEGRO_BITMAP* bg_linear;
     ALLEGRO_BITMAP* bg_expo;
 } Game;
 
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
     default:
         return 0.0;
     }
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
     p->y_base = -8.0;  // Alinhado com a plataforma visual
     p->y = p->y_base;
     p->x_min = -18.0;  // Aumentado: permite andar em quase toda a tela
     p->x_max = 18.0;   // Aumentado: permite andar em quase toda a tela
     p->move_speed = 5.0;
     p->state = PLAYER_IDLE;
 
     // Animação
     p->facing_right = true;
     p->frame = 0;
     p->frames_idle = 4;
     p->frames_walk = 4;
     p->frames_shoot = 6;
     p->anim_fps = 12.0;
     p->frame_acc = 0.0;
 
     // Idle breathing/sway
     p->idle_timer = 0.0;
     p->idle_sway_offset = 0.0;
 
     // Layout alternativo shoot (2x4 grid)
     p->shoot_cols = 4;
     p->shoot_rows = 2;
     p->shoot_frames_total = 8;
 
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
 
     // Y sempre no chão
     p->y = p->y_base;
 
     // Animação
     p->frame_acc += dt;
     double frame_time = 1.0 / p->anim_fps;
 
     if (p->frame_acc >= frame_time) {
         p->frame_acc -= frame_time;
 
         switch (p->state) {
         case PLAYER_IDLE:
             p->frame = (p->frame + 1) % p->frames_idle;
             break;
         case PLAYER_WALK:
             p->frame = (p->frame + 1) % p->frames_walk;
             break;
         case PLAYER_SHOOT:
             // Não-loop: para no último frame
             if (p->frame < p->frames_shoot - 1) {
                 p->frame++;
             }
             break;
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
     int flags = p->facing_right ? 0 : ALLEGRO_FLIP_HORIZONTAL;
 
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
 
         // Shoot: verificar layout especial (1x6 ou 2x4)
         if (p->state == PLAYER_SHOOT && bmp_w == 384 && bmp_h == 64) {
             // Layout antigo 1x6 (384x64)
             frame_width_src = 64;
             frame_height_src = 64;
             visual_width = 96;
             visual_height = 96;
         }
         else if (p->state == PLAYER_SHOOT && p->shoot_cols > 1) {
             // Layout 2x4 ou outro grid
             draw_grid_frame(current_sprite, p->frame, p->shoot_cols, p->shoot_rows,
                 screen_x - visual_width / 2, screen_y - visual_height,
                 visual_width, visual_height, flags);
             return;
         }
 
         // Idle e Walk: desenhar frame da linha horizontal
         int sx = p->frame * frame_width_src;
 
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
     arrow->vx = 8.0;
     arrow->tmax = 5.0;
     arrow->current_x = 0.0;
     arrow->current_y = 0.0;
     arrow->sprite = NULL;
 }
 
 void arrow_shoot(Arrow* arrow, double x0, double y0, double vx) {
     arrow->active = true;
     arrow->t = 0.0;
     arrow->x0 = x0;
     arrow->y0 = y0;
     arrow->vx = vx;
     arrow->current_x = x0;
     arrow->current_y = y0;
 }
 
 void arrow_update(Arrow* arrow, double dt, const MathFunction* fn) {
     if (!arrow->active) return;
 
     arrow->t += dt;
     arrow->current_x = arrow->x0 + arrow->vx * arrow->t;
 
     // CORRIGIDO: usar distância percorrida, não posição absoluta
     double distance = arrow->vx * arrow->t;
     arrow->current_y = arrow->y0 + fn_eval(fn, fabs(distance));
 
     if (arrow->t > arrow->tmax || arrow->current_x > 25.0 || arrow->current_x < -25.0) {
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
         double y = y0 + fn_eval(fn, fabs(distance)) * (vx > 0 ? 1 : 1);  // Sempre para cima
 
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
 
 // ========================================================================
 // FUNÇÕES - NÍVEIS
 // ========================================================================
 
 void level_init_chapters(Chapter* chapters) {
     // CAPÍTULO 0: LINEAR
     chapters[0].name = "Linear";
     chapters[0].function_type = FN_LINEAR;
     chapters[0].theme_color = al_map_rgb(0, 170, 220);
 
     // TEST
     chapters[0].test.arrows_allowed = 5;
     chapters[0].test.epsilon_px = 16.0;
     chapters[0].test.color_tint = TINT_BRONZE;
     chapters[0].test.collectible_count = 4;
     chapters[0].test.items[0] = (Collectible){ 6.0, 0.0, 0.40, COLLECT_KEY, false };
     chapters[0].test.items[1] = (Collectible){ 10.0, 2.0, 0.40, COLLECT_KEY, false };
     chapters[0].test.items[2] = (Collectible){ 14.0, 4.0, 0.40, COLLECT_KEY, false };
     chapters[0].test.items[3] = (Collectible){ 8.0, 1.0, 0.35, COLLECT_APPLE, false };
 
     // FASE 1
     chapters[0].phases[0].arrows_allowed = 3;
     chapters[0].phases[0].epsilon_px = 14.0;
     chapters[0].phases[0].color_tint = TINT_BRONZE;
     chapters[0].phases[0].collectible_count = 3;
     chapters[0].phases[0].items[0] = (Collectible){ 8.0, 1.2, 0.40, COLLECT_KEY, false };
     chapters[0].phases[0].items[1] = (Collectible){ 10.0, 2.2, 0.40, COLLECT_KEY, false };
     chapters[0].phases[0].items[2] = (Collectible){ 12.0, 3.2, 0.40, COLLECT_KEY, false };
 
     // FASE 2
     chapters[0].phases[1].arrows_allowed = 3;
     chapters[0].phases[1].epsilon_px = 12.0;
     chapters[0].phases[1].color_tint = TINT_SILVER;
     chapters[0].phases[1].collectible_count = 4;
     chapters[0].phases[1].items[0] = (Collectible){ 9.0, 1.0, 0.40, COLLECT_KEY, false };
     chapters[0].phases[1].items[1] = (Collectible){ 11.0, 2.2, 0.40, COLLECT_KEY, false };
     chapters[0].phases[1].items[2] = (Collectible){ 13.0, 3.2, 0.40, COLLECT_KEY, false };
     chapters[0].phases[1].items[3] = (Collectible){ 10.0, 1.5, 0.35, COLLECT_APPLE, false };
 
     // FASE 3
     chapters[0].phases[2].arrows_allowed = 3;
     chapters[0].phases[2].epsilon_px = 10.0;
     chapters[0].phases[2].color_tint = TINT_GOLD;
     chapters[0].phases[2].collectible_count = 4;
     chapters[0].phases[2].items[0] = (Collectible){ 9.5, 0.8, 0.40, COLLECT_KEY, false };
     chapters[0].phases[2].items[1] = (Collectible){ 11.5, 2.0, 0.40, COLLECT_KEY, false };
     chapters[0].phases[2].items[2] = (Collectible){ 13.5, 3.1, 0.40, COLLECT_KEY, false };
     chapters[0].phases[2].items[3] = (Collectible){ 12.0, 2.5, 0.35, COLLECT_APPLE_GOLD, false };
 
     // Copiar estrutura para outros capítulos (simplificado)
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
             chapters[i].theme_color = al_map_rgb(255, 180, 0);
             break;
         case 3:
             chapters[i].name = "Logaritmica";
             chapters[i].theme_color = al_map_rgb(0, 200, 100);
             break;
         case 4:
             chapters[i].name = "Racional";
             chapters[i].theme_color = al_map_rgb(200, 100, 50);
             break;
         case 5:
             chapters[i].name = "Trigonometrica";
             chapters[i].theme_color = al_map_rgb(255, 100, 150);
             break;
         }
     }
 }
 
 Phase* get_current_phase(Game* game) {
     if (game->current_phase == 0) {
         return &game->chapters[game->current_chapter].test;
     }
     return &game->chapters[game->current_chapter].phases[game->current_phase - 1];
 }
 
 void phase_reset_collectibles(Phase* phase) {
     for (int i = 0; i < phase->collectible_count; i++) {
         phase->items[i].collected = false;
     }
 }
 
 bool collectible_check(Collectible* c, double arrow_x, double arrow_y, double epsilon) {
     if (c->collected) return false;
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
 // FUNÇÕES AUXILIARES - UI
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
 
 // Inicializar parâmetros da função baseado no tipo
 void init_function_params(MathFunction* fn) {
     switch (fn->type) {
     case FN_LINEAR:
         fn->params.linear.m = 1.0;
         fn->params.linear.k = 0.0;
         break;
     case FN_QUADRATIC:
         fn->params.quadratic.a = 0.1;   // Parábola suave
         fn->params.quadratic.h = 0.0;   // Vértice em x=0
         fn->params.quadratic.kq = 0.0;  // Vértice em y=0
         break;
     default:
         break;
     }
 }
 
 // ========================================================================
 // FUNÇÕES - JOGO PRINCIPAL
 // ========================================================================
 
 void game_init(Game* game) {
     game->state = STATE_TITLE;
     game->current_chapter = 0;
     game->current_phase = 0;
     game->arrows_remaining = 5;
 
     // Inicializar sistema de progressão
     game->progress.unlocked_chapters = 1;  // Começa com apenas Cap 1 (Linear)
     for (int i = 0; i < 6; i++) {
         game->progress.chapters_completed[i] = false;
     }
 
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
     game->current_function.params.quadratic.a = 0.1;
     game->current_function.params.quadratic.h = 0.0;
     game->current_function.params.quadratic.kq = 0.0;
 
     game->key_up = game->key_down = game->key_space = false;
     game->key_left = game->key_right = false;
     game->key_space_down = game->was_space_down = false;
     game->key_r = game->key_p = game->key_h = false;
     game->key_enter = game->key_escape = false;
     game->key_shift = false;
     game->key_m = game->key_k = game->key_q = game->key_e = false;
     game->key_bracket_left = game->key_bracket_right = false;
 
     game->bg_linear = NULL;
     game->bg_expo = NULL;
 
     game->show_max_hint = false;
     game->hint_timer = 0.0;
     game->chapter_menu_selection = 0;
 
     level_init_chapters(game->chapters);
 }
 
 void game_load_resources(Game* game) {
     game->ui.font_title = al_create_builtin_font();
     game->ui.font_main = al_create_builtin_font();
     game->ui.font_small = al_create_builtin_font();
 
     // Carregar sprites do player
     printf("[DEBUG] Tentando carregar: %s\n", ASSET_PLAYER_IDLE);
     game->player.sprite_idle = al_load_bitmap(ASSET_PLAYER_IDLE);
     if (!game->player.sprite_idle) {
         printf("[WARN] Sprite não encontrado: %s (usando fallback)\n", ASSET_PLAYER_IDLE);
         printf("[DEBUG] Erro do Allegro: %d\n", al_get_errno());
     }
     else {
         int w = al_get_bitmap_width(game->player.sprite_idle);
         int h = al_get_bitmap_height(game->player.sprite_idle);
 
         // IDLE deve ser SEMPRE 1 frame (imagem fixa, não anima!)
         // Se sprite for quadrado ou próximo, usar apenas primeiro frame
         if (w <= h * 1.2) {
             // Quadrado ou quase: 1 frame único
             game->player.frames_idle = 1;
             printf("[INFO] Idle sprite: %dx%d, usando 1 frame FIXO (sem animação)\n", w, h);
         }
         else {
             // Se muito largo, pode ter múltiplos frames, mas vamos usar só o primeiro
             game->player.frames_idle = 1;
             printf("[INFO] Idle sprite: %dx%d, usando apenas PRIMEIRO frame (idle fixo)\n", w, h);
         }
     }
 
     printf("[DEBUG] Tentando carregar: %s\n", ASSET_PLAYER_WALK);
     game->player.sprite_walk = al_load_bitmap(ASSET_PLAYER_WALK);
     if (!game->player.sprite_walk) {
         printf("[WARN] Sprite não encontrado: %s (usando fallback)\n", ASSET_PLAYER_WALK);
         printf("[DEBUG] Erro do Allegro: %d\n", al_get_errno());
     }
     else {
         int w = al_get_bitmap_width(game->player.sprite_walk);
         int h = al_get_bitmap_height(game->player.sprite_walk);
 
         // Walk: detectar frames automaticamente
         if (w > h) {
             // Spritesheet horizontal: dividir largura pela altura
             game->player.frames_walk = w / h;
             printf("[INFO] Walk sprite: %dx%d, detectados %d frames horizontais\n", w, h, game->player.frames_walk);
         }
         else {
             // Quadrado ou vertical: assume 4 frames padrão
             game->player.frames_walk = 4;
             printf("[INFO] Walk sprite: %dx%d, assumindo %d frames\n", w, h, game->player.frames_walk);
         }
     }
 
     printf("[DEBUG] Tentando carregar: %s\n", ASSET_PLAYER_SHOOT);
     game->player.sprite_shoot = al_load_bitmap(ASSET_PLAYER_SHOOT);
     if (!game->player.sprite_shoot) {
         printf("[WARN] Sprite não encontrado: %s (usando fallback)\n", ASSET_PLAYER_SHOOT);
         printf("[DEBUG] Erro do Allegro: %d\n", al_get_errno());
     }
     else {
         printf("[SUCCESS] Sprite shoot carregado!\n");
         // Autodetectar layout do shoot
         int w = al_get_bitmap_width(game->player.sprite_shoot);
         int h = al_get_bitmap_height(game->player.sprite_shoot);
         if (w == 384 && h == 64) {
             printf("[INFO] Shoot sprite: layout 1x6 detectado\n");
             game->player.frames_shoot = 6;
         }
         else {
             printf("[INFO] Shoot sprite: usando grid %dx%d (%d frames)\n",
                 game->player.shoot_cols, game->player.shoot_rows,
                 game->player.shoot_frames_total);
             game->player.frames_shoot = game->player.shoot_frames_total;
         }
     }
 
     // Carregar backgrounds
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
 }
 
 void game_start_phase(Game* game, int phase_index) {
     game->current_phase = phase_index;
     Phase* phase = get_current_phase(game);
     game->arrows_remaining = phase->arrows_allowed;
     phase_reset_collectibles(phase);
     game->player.y = 0.0;
     arrow_init(&game->arrow);
 
     if (phase_index == 0) {
         game->ui.special.value = 0.0;
         game->state = STATE_CHAPTER_TEST;
     }
     else {
         game->state = STATE_PLAY;
     }
 }
 
 void game_update(Game* game, double dt) {
     Phase* phase = get_current_phase(game);
 
     switch (game->state) {
     case STATE_TITLE:
         if (game->key_enter) {
             // Começa direto no primeiro capítulo desbloqueado
             game->current_chapter = 0;
             game->current_function.type = game->chapters[game->current_chapter].function_type;
             init_function_params(&game->current_function);  // Inicializa parâmetros
             game->state = STATE_CHAPTER_INTRO;
             game->key_enter = false;
         }
         break;
 
     case STATE_CHAPTER_SELECT:
         if (game->key_enter) {
             game->current_chapter = game->chapter_menu_selection;
             game->current_function.type = game->chapters[game->current_chapter].function_type;
             init_function_params(&game->current_function);  // Inicializa parâmetros
             game->state = STATE_CHAPTER_INTRO;
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
         player_update(&game->player, dt, game->key_left, game->key_right);
 
         // Sistema de tiro: segurar = puxar, soltar = disparar
         bool space_pressed_now = game->key_space_down && !game->was_space_down;
         bool space_released_now = !game->key_space_down && game->was_space_down;
 
         if (space_pressed_now) {
             // Começou a segurar: entra em SHOOT
             game->player.state = PLAYER_SHOOT;
             game->player.frame = 0;
             game->player.frame_acc = 0.0;
         }
 
         if (space_released_now) {
             // Soltou: dispara se possível
             if (!game->arrow.active && game->arrows_remaining > 0) {
                 double vx = game->player.facing_right ? 8.0 : -8.0;
                 // Flecha sai do peito, não do pé (+1.5 unidades do mundo)
                 arrow_shoot(&game->arrow, game->player.x, game->player.y + 1.5, vx);
                 game->arrows_remaining--;
             }
             // Volta ao idle
             game->player.state = PLAYER_IDLE;
             game->player.frame = 0;
         }
 
         game->was_space_down = game->key_space_down;
 
         if (game->arrow.active) {
             arrow_update(&game->arrow, dt, &game->current_function);
 
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
             game->ui.preview_enabled = !game->ui.preview_enabled;
             game->key_p = false;
         }
 
         if (game->key_r) {
             game_start_phase(game, game->current_phase);
             game->key_r = false;
         }
 
         if (game->key_h && special_bar_can_use(&game->ui.special)) {
             special_bar_use(&game->ui.special);
             game->show_max_hint = true;
             game->hint_timer = 0.0;
             game->key_h = false;
         }
 
         // CONTROLES COM LETRAS - MUITO MAIS FÁCIL!
         // Tecla M: seleciona inclinação
         if (game->key_m) {
             game->ui.editor.selected = PARAM_M;
             game->key_m = false;
         }
         // Tecla K: seleciona intercepto
         if (game->key_k) {
             game->ui.editor.selected = PARAM_K;
             game->key_k = false;
         }
         // Tecla Q: diminui valor
         if (game->key_q) {
             if (game->ui.editor.selected == PARAM_M) {
                 game->current_function.params.linear.m -= 0.1;
             }
             else if (game->ui.editor.selected == PARAM_K) {
                 game->current_function.params.linear.k -= 0.1;
             }
             game->key_q = false;
         }
         // Tecla E: aumenta valor
         if (game->key_e) {
             if (game->ui.editor.selected == PARAM_M) {
                 game->current_function.params.linear.m += 0.1;
             }
             else if (game->ui.editor.selected == PARAM_K) {
                 game->current_function.params.linear.k += 0.1;
             }
             game->key_e = false;
         }
 
         int keys = phase_count_keys_collected(phase);
         if (keys >= 3) {
             if (game->arrows_remaining == (phase->arrows_allowed - 1)) {
                 special_bar_add(&game->ui.special, 0.40);
             }
             game->state = STATE_STAGE_CLEAR;
         }
 
         if (game->arrows_remaining <= 0 && !game->arrow.active && keys < 3) {
             game->state = STATE_TRY_AGAIN;
         }
         break;
 
     case STATE_STAGE_CLEAR:
         if (game->key_enter) {
             if (game->current_phase < 3) {
                 // Próxima fase do mesmo capítulo
                 game_start_phase(game, game->current_phase + 1);
             }
             else {
                 // Completou todas as 3 fases! Marcar capítulo como completo
                 game->progress.chapters_completed[game->current_chapter] = true;
 
                 // Desbloquear próximo capítulo se existir
                 if (game->current_chapter < 5 && game->progress.unlocked_chapters <= game->current_chapter + 1) {
                     game->progress.unlocked_chapters = game->current_chapter + 2;
                     printf("[INFO] Capítulo %d desbloqueado!\n", game->current_chapter + 2);
                 }
 
                 // Vai para seleção de capítulos
                 game->state = STATE_CHAPTER_SELECT;
             }
             game->key_enter = false;
         }
         if (game->key_escape) {
             game->state = STATE_CHAPTER_SELECT;
             game->key_escape = false;
         }
         break;
 
     case STATE_TRY_AGAIN:
         if (game->key_enter) {
             game_start_phase(game, game->current_phase);
             game->key_enter = false;
         }
         if (game->key_escape) {
             game->state = STATE_CHAPTER_SELECT;
             game->key_escape = false;
         }
         break;
     }
 
     if (game->show_max_hint) {
         game->hint_timer += dt;
         if (game->hint_timer > 5.0) {
             game->show_max_hint = false;
         }
     }
 }
 
 void game_draw(Game* game) {
     al_clear_to_color(al_map_rgb(20, 20, 40));
     Phase* phase = get_current_phase(game);
     Chapter* ch = &game->chapters[game->current_chapter];
     ALLEGRO_FONT* font = game->ui.font_main;
 
     switch (game->state) {
     case STATE_TITLE:
         // Título principal
         draw_text_scaled(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2, 120,
             ALLEGRO_ALIGN_CENTER, FONT_SIZE_TITLE, "ARCHER");
         // Subtítulo
         draw_text_scaled(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 280,
             ALLEGRO_ALIGN_CENTER, FONT_SIZE_SUBTITLE, "Jogo de Funcoes Matematicas");
         // Instruções
         draw_text_scaled(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, 400,
             ALLEGRO_ALIGN_CENTER, FONT_SIZE_MEDIUM, "ENTER - Jogar");
         draw_text_scaled(font, al_map_rgb(255, 150, 150), SCREEN_WIDTH / 2, 450,
             ALLEGRO_ALIGN_CENTER, FONT_SIZE_MEDIUM, "ESC - Sair");
         break;
 
     case STATE_CHAPTER_SELECT:
         al_draw_text(font, al_map_rgb(255, 255, 100), SCREEN_WIDTH / 2, 50,
             ALLEGRO_ALIGN_CENTER, "SELECIONE O PROXIMO CAPITULO");
 
         // Mostrar apenas capítulos desbloqueados
         for (int i = 0; i < game->progress.unlocked_chapters; i++) {
             bool selected = (i == game->chapter_menu_selection);
             bool completed = game->progress.chapters_completed[i];
 
             // Cor: amarelo se selecionado, verde se completo, cinza se não
             ALLEGRO_COLOR color;
             if (selected) {
                 color = al_map_rgb(255, 255, 100);  // Amarelo
             }
             else if (completed) {
                 color = al_map_rgb(100, 255, 100);  // Verde
             }
             else {
                 color = al_map_rgb(200, 200, 200);  // Cinza claro
             }
 
             char text[64];
             if (completed) {
                 snprintf(text, sizeof(text), "%d. %s [COMPLETO]", i + 1, game->chapters[i].name);
             }
             else {
                 snprintf(text, sizeof(text), "%d. %s", i + 1, game->chapters[i].name);
             }
 
             al_draw_text(font, color, SCREEN_WIDTH / 2, 150 + i * 50,
                 ALLEGRO_ALIGN_CENTER, text);
         }
 
         // Instruções
         char instructions[128];
         snprintf(instructions, sizeof(instructions),
             "Use 1-%d para selecionar | ENTER para jogar | ESC para sair",
             game->progress.unlocked_chapters);
         al_draw_text(font, al_map_rgb(150, 150, 150), SCREEN_WIDTH / 2, 500,
             ALLEGRO_ALIGN_CENTER, instructions);
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
         // Background - usar imagem ou fallback
         ALLEGRO_BITMAP* current_bg = NULL;
         if (game->current_chapter == 0) {
             current_bg = game->bg_linear;
         }
         else if (game->current_chapter == 1) {
             current_bg = game->bg_expo;
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
 
         // Preview - linha da função (SÓ quando mirando)
         if (game->ui.preview_enabled || game->key_shift) {
             // Cor do preview: amarelo brilhante com transparência
             ALLEGRO_COLOR preview_color = al_map_rgba(255, 255, 0, 180);
             double vx_preview = game->player.facing_right ? 8.0 : -8.0;
             // Preview sai do peito (meio do corpo, não do pé)
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
 
         // Coletáveis
         for (int i = 0; i < phase->collectible_count; i++) {
             Collectible* c = &phase->items[i];
             if (c->collected) continue;
             int cx, cy;
             world_to_screen(&game->camera, c->x, c->y, &cx, &cy);
             ALLEGRO_COLOR color;
             if (c->type == COLLECT_KEY) color = al_map_rgb(255, 255, 100);
             else if (c->type == COLLECT_APPLE_GOLD) color = al_map_rgb(255, 200, 50);
             else color = al_map_rgb(255, 50, 50);
             int radius = (int)(c->radius * PIXELS_PER_UNIT);
             al_draw_filled_circle(cx, cy, radius, color);
             al_draw_circle(cx, cy, radius, al_map_rgb(200, 200, 200), 2);
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
 
         // HUD com cores vibrantes
         char text[128];
         snprintf(text, sizeof(text), "FLECHAS: %d", game->arrows_remaining);
         draw_text_scaled(font, al_map_rgb(100, 255, 255), 20, 20, 0, FONT_SIZE_NORMAL, text);  // Ciano
 
         int keys = phase_count_keys_collected(phase);
         snprintf(text, sizeof(text), "CHAVES: %d / 3", keys);
         draw_text_scaled(font, al_map_rgb(255, 220, 0), 20, 50, 0, FONT_SIZE_NORMAL, text);  // Amarelo ouro
 
         // Exibir equação baseado no tipo
         if (game->current_function.type == FN_LINEAR) {
             snprintf(text, sizeof(text), "y = %.2fx %+.2f",
                 game->current_function.params.linear.m,
                 game->current_function.params.linear.k);
             draw_text_scaled(font, al_map_rgb(200, 255, 200), 20, SCREEN_HEIGHT - 110, 0, FONT_SIZE_LARGE, text);
 
             if (game->ui.editor.selected == PARAM_M) {
                 al_draw_text(font, al_map_rgb(255, 255, 100), 20, SCREEN_HEIGHT - 75, 0,
                     "Editando: m (inclinacao)");
                 char buf[64];
                 snprintf(buf, sizeof(buf), "Valor: %s%s",
                     game->ui.editor.is_negative ? "-" : "",
                     game->ui.editor.buffer);
                 al_draw_text(font, al_map_rgb(255, 200, 100), 20, SCREEN_HEIGHT - 55, 0, buf);
             }
             else if (game->ui.editor.selected == PARAM_K) {
                 al_draw_text(font, al_map_rgb(255, 255, 100), 20, SCREEN_HEIGHT - 75, 0,
                     "Editando: k (intercepto)");
                 char buf[64];
                 snprintf(buf, sizeof(buf), "Valor: %s%s",
                     game->ui.editor.is_negative ? "-" : "",
                     game->ui.editor.buffer);
                 al_draw_text(font, al_map_rgb(255, 200, 100), 20, SCREEN_HEIGHT - 55, 0, buf);
             }
         }
         else if (game->current_function.type == FN_QUADRATIC) {
             // Mostrar equação quadrática: y = a(x-h)² + k
             snprintf(text, sizeof(text), "y = %.2f(x%+.2f)^2 %+.2f",
                 game->current_function.params.quadratic.a,
                 -game->current_function.params.quadratic.h,
                 game->current_function.params.quadratic.kq);
             draw_text_scaled(font, al_map_rgb(200, 200, 255), 20, SCREEN_HEIGHT - 110, 0, FONT_SIZE_LARGE, text);
 
             al_draw_text(font, al_map_rgb(255, 200, 200), 20, SCREEN_HEIGHT - 75, 0,
                 "Quadratica: parametros fixos (a=0.1, h=0, k=0)");
         }
 
         // Barra especial
         int bar_x = SCREEN_WIDTH - 220;
         int bar_y = 50;
         int bar_w = 200;
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
 
         // Menu de controles
         draw_text_scaled(font, al_map_rgb(180, 180, 180), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40,
             ALLEGRO_ALIGN_CENTER, FONT_SIZE_CONTROLS, "[<-/->]=mover [ESPACO]=segurar/soltar [M]=m [K]=k [Q]=diminuir [E]=aumentar [H]=especial");
         break;
     }
 
     case STATE_STAGE_CLEAR:
         al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, 180));
         al_draw_filled_rounded_rectangle(390, 210, 890, 510, 10, 10, al_map_rgb(40, 80, 40));
         al_draw_rounded_rectangle(390, 210, 890, 510, 10, 10, al_map_rgb(100, 255, 100), 3);
         al_draw_text(font, al_map_rgb(150, 255, 150), SCREEN_WIDTH / 2, 250,
             ALLEGRO_ALIGN_CENTER, "FASE CONCLUIDA!");
         al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 340,
             ALLEGRO_ALIGN_CENTER, "Parabens!");
         al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, 400,
             ALLEGRO_ALIGN_CENTER, "ENTER - Proxima");
         break;
 
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
 
     case ALLEGRO_KEY_1:
         if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 0;
         else editor_input_digit(&game->ui.editor, 1);
         break;
     case ALLEGRO_KEY_2:
         if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 1;
         else editor_input_digit(&game->ui.editor, 2);
         break;
     case ALLEGRO_KEY_3:
         if (game->state == STATE_CHAPTER_SELECT) game->chapter_menu_selection = 2;
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
 
 