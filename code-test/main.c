#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define GRAVITY 980.0f  // pixels/s² (ajustado para visual)
#define MAX_ARROWS 100

typedef struct {
    float x, y;
    float vx, vy;
    bool active;
} Arrow;

int main() {
    if (!al_init()) {
        return -1;
    }
    al_init_primitives_addon();
    al_install_mouse();

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    bool running = true;
    float dt = 1.0f / 60.0f;

    // Personagem fixo
    float player_x = 100;
    float player_y = 500;

    Arrow arrows[MAX_ARROWS];
    for (int i = 0; i < MAX_ARROWS; i++) {
        arrows[i].active = false;
    }

    while (running) {
        ALLEGRO_EVENT ev;
        while (al_get_next_event(event_queue, &ev)) {
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                running = false;
            }
            else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                // Atira flecha em direção ao clique do mouse
                float mx = ev.mouse.x;
                float my = ev.mouse.y;

                float dx = mx - player_x;
                float dy = my - player_y;

                float angle = atan2f(dy, dx);
                float speed = 500.0f; // velocidade inicial da flecha (pixels/seg)

                // Procura uma flecha inativa para ativar
                for (int i = 0; i < MAX_ARROWS; i++) {
                    if (!arrows[i].active) {
                        arrows[i].x = player_x;
                        arrows[i].y = player_y;
                        arrows[i].vx = speed * cosf(angle);
                        arrows[i].vy = speed * sinf(angle);
                        arrows[i].active = true;
                        break;
                    }
                }
            }
        }

        // Atualiza flechas
        for (int i = 0; i < MAX_ARROWS; i++) {
            if (arrows[i].active) {
                arrows[i].x += arrows[i].vx * dt;
                arrows[i].y += arrows[i].vy * dt;

                arrows[i].vy += GRAVITY * dt; // gravidade influencia vy

                // Desativa se sair da tela
                if (arrows[i].x < 0 || arrows[i].x > SCREEN_W || arrows[i].y > SCREEN_H) {
                    arrows[i].active = false;
                }
            }
        }

        // Desenha tudo
        al_clear_to_color(al_map_rgb(0, 0, 0));

        // Desenha personagem (retângulo azul)
        al_draw_filled_rectangle(player_x - 20, player_y - 40, player_x + 20, player_y, al_map_rgb(0, 0, 255));

        // Desenha flechas (linha branca)
        for (int i = 0; i < MAX_ARROWS; i++) {
            if (arrows[i].active) {
                al_draw_line(arrows[i].x, arrows[i].y, arrows[i].x - 10, arrows[i].y + 2, al_map_rgb(255, 255, 255), 2);
            }
        }

        al_flip_display();

        al_rest(dt);
    }

    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
