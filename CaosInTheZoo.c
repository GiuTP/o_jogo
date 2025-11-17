#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "player.h"

#define X_SCREEN 1280
#define Y_SCREEN 720

#define INIT_TEST(test, description) \
    do { \
        if (!(test)){ \
            fprintf(stderr, "Nao foi possivel inicializar o/a %s\n", (description)); \
            exit(1); \
        } \
    } while(0)

int main(){
    // Primeiras inicializacoes
    INIT_TEST(al_init(), "allegro");
    INIT_TEST(al_install_keyboard(), "telado");
    INIT_TEST(al_init_image_addon(), "font addon");
    INIT_TEST(al_init_image_addon(), "image addon");

    // Inicializacoes das variaveis basicas
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    INIT_TEST(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue(); 
    INIT_TEST(queue, "fila de eventos");

    ALLEGRO_DISPLAY *display = al_create_display(X_SCREEN, Y_SCREEN);
    INIT_TEST(display, "display");

    ALLEGRO_FONT *font = al_create_builtin_font();
    INIT_TEST(font, "fonte");

    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP | ALLEGRO_ALPHA_TEST);
    al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
    
    ALLEGRO_BITMAP *spritesheet_player_giuliano = al_load_bitmap("assets/giu.png");
    INIT_TEST(spritesheet_player_giuliano, "giu.png");

    al_set_target_bitmap(spritesheet_player_giuliano);
    al_convert_mask_to_alpha(spritesheet_player_giuliano, al_map_rgb(105, 255, 88));
    al_set_target_bitmap(al_get_backbuffer(display));

    // Registrando as fontes da fila de evento 
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Player *giuliano = player_init(spritesheet_player_giuliano);
    INIT_TEST(giuliano, "jogador");

    // Variaveis de controle
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT ev;
    ALLEGRO_KEYBOARD_STATE ks;

    al_start_timer(timer);
    while(!done){
        al_wait_for_event(queue, &ev);

        switch (ev.type){
            case ALLEGRO_EVENT_TIMER:
                // ----- LÓGICA -----
                al_get_keyboard_state(&ks);
                giuliano->update(giuliano, &ks); // CHAMA O UPDATE
                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    done = true;
                }
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break; 
        }

        // ----- DESENHO -----
        if (redraw && al_is_event_queue_empty(queue)){
            al_clear_to_color(al_map_rgb(255, 255, 255)); // Fundo cinza escuro
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, "TESTE - Aperte ESC para sair");
            
            giuliano->draw(giuliano); // CHAMA O DRAW

            al_flip_display();
            redraw = false;
        }
        // al_wait_for_event(queue, &ev);

        // switch (ev.type){
        //     case ALLEGRO_EVENT_TIMER :
        //         redraw = true;
        //         break;
        //     case ALLEGRO_EVENT_KEY_DOWN:
        //     case ALLEGRO_EVENT_DISPLAY_CLOSE:
        //         done = true;
        //         break; 
        // }

        // if (done)
        //     break;
        
        // if (redraw && al_is_event_queue_empty(queue)){
        //     al_clear_to_color(al_map_rgb(0, 0, 0));

        //     al_draw_text(font, al_map_rgb(50, 120, 200), 500, 500 , 0, "Hello World!");
        //     al_draw_bitmap(spritesheet_player_giuliano, 0, 0, 0);

        //     al_flip_display();
        //     redraw = false;
            
        // }
    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_shutdown_font_addon();
    al_destroy_bitmap(spritesheet_player_giuliano);

    return 0;
}