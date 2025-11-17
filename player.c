#include "player.h"
#include <stdio.h>
#include <allegro5/allegro_primitives.h>

// Constantes Internas
#define PLAYER_VIDA_INICIAL 3
// #define PLAYER_TEMPO_INVENCIBILIDADE 2.0
#define PLAYER_ANIMACAO_DELAY 0.15
#define PLAYER_WALK_START_FRAME 0
#define PLAYER_WALK_NUM_FRAME 3
#define GRAVITY 0.5
#define JUMP_FORCE -12.0
#define FAKE_FLOOR 500              // Apenas para teste
#define LEVEL_BORDER_LEFT 0
#define LEVEL_BORDER_RIGHT 1280
#define PLAYER_HITBOX_WIDTH 9
#define PLAYER_HITBOX_OFFSET 11

void player_update_func(Player *self, ALLEGRO_KEYBOARD_STATE *ks){
    self->vel_x = 0;
    self->andando = false;
    self->duck = false;

    
    
    self->vel_y += GRAVITY;
    
    if ((al_key_down(ks, ALLEGRO_KEY_J)) && (self->on_ground)){
        self->vel_y = JUMP_FORCE;
        self->on_ground = false;
    }
    
    self->pos_y += self->vel_y;
    
    if (self->pos_y >= FAKE_FLOOR){
        self->pos_y = FAKE_FLOOR;
        self->vel_y = 0;
        self->on_ground = true;
    }
    
    if (al_key_down(ks, ALLEGRO_KEY_S) && (self->on_ground)){
        self->duck = true;
    }
    
    if(!self->duck){
        if (al_key_down(ks, ALLEGRO_KEY_A)){
            self->vel_x = -PLAYER_SPEED;
            self->sprite_y_origem = 1 * PLAYER_HEIGHT_FRAME;
            self->andando = true;
        }
        if (al_key_down(ks, ALLEGRO_KEY_D)){
            self->vel_x = PLAYER_SPEED;
            self->sprite_y_origem = 0 * PLAYER_HEIGHT_FRAME;
            self->andando = true;
        }
    }


    self->pos_x += self->vel_x;
    float escala = self->escala;
    float visual_offset_x = PLAYER_HITBOX_OFFSET * escala;
    float visual_largura = PLAYER_HITBOX_WIDTH * escala;

    float visual_left_edge = self->pos_x + visual_offset_x;
    float visual_right_edge = self->pos_x + visual_offset_x + visual_largura;

    if (visual_left_edge <= LEVEL_BORDER_LEFT){
        self->pos_x = LEVEL_BORDER_LEFT - visual_offset_x;
    }

    if (visual_right_edge >= LEVEL_BORDER_RIGHT){
        self->pos_x = LEVEL_BORDER_RIGHT - visual_largura - visual_offset_x;
    }

    
    
    double tempo_atual = al_get_time();
    
    if (!self->on_ground){
        self->frame_atual = 4;
    } 
    else if (self->duck) {
        self->frame_atual = 3;
    } 
    else if (self->andando) {
            if (tempo_atual - self->tempo_animacao >= PLAYER_ANIMACAO_DELAY){
                self->frame_atual++;

                int ultimo_frame_andar = PLAYER_WALK_START_FRAME + PLAYER_WALK_NUM_FRAME;
                if (self->frame_atual >= ultimo_frame_andar){
                    self->frame_atual = PLAYER_WALK_START_FRAME;
                }

                self->tempo_animacao = tempo_atual;
            } 
    }
    else {
        self->frame_atual = 0;
    }
    
    // if (self->invencivel){
    //     if (tempo_atual - self->tempo_invencibilidade_inicio > PLAYER_TEMPO_INVENCIBILIDADE){
    //         self->invencivel = false;
    //     }
    // }
}

void player_draw_func(Player *self){
    int sprite_x_origem = self->frame_atual * PLAYER_WIDTH_FRAME;

    al_draw_scaled_bitmap(
        self->spritesheet,
        sprite_x_origem,
        self->sprite_y_origem,

        PLAYER_WIDTH_FRAME,
        PLAYER_HEIGHT_FRAME,

        self->pos_x,
        self->pos_y,

        PLAYER_WIDTH_FRAME * self->escala,
        PLAYER_HEIGHT_FRAME * self->escala,

        0
    );
}

// void player_take_damege_fung(Player *self){

// }

void player_destroy_func(Player *self){

}

Player *player_init(ALLEGRO_BITMAP *spritesheet){
    Player *p;
    if(!(p = malloc(sizeof(p)))) return NULL;
    
    *p = (Player){
        // Variáveis de localização e movimento
        .pos_x = 500,
        .pos_y = FAKE_FLOOR,    // mudar depois
        .vel_x = 0,
        .vel_y = 0,
        
        // Variáveis de estado
        .on_ground = true,
        .duck = false,
        .andando = false,
        
        // Sprites
        .frame_atual = 0,
        .sprite_y_origem = 0,
        .escala = 5.0,
        .spritesheet = spritesheet,

        // ""Métodos"" do player
        .update = player_update_func,
        .draw = player_draw_func,
        .destroy = player_destroy_func,
    };

    return p;
}