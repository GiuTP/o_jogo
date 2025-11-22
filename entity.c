#include "entity.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>

static int check_overlap(float x1, float y1, float w1, float h1,
                         float x2, float y2, float w2, float h2){
    return (x1 < x2 + w2 && x1 + w1 > x2 && 
            y1 < y2 + h2 && y1 + h1 > y2);
}

static void update_collision_player(Player *p, Entity *e){
    float px = p->pos_x + (11 * p->escala);
    float py = p->pos_y + (7 * p->escala);
    float pw = 9 * p->escala;
    float ph = 18 * p->escala;

    float ex = e->x;
    float ey = e->y;
    float ew = e->w;
    float eh = e->h;

    if (check_overlap(px, py, pw, ph, ex, ey, ew, eh)){
        float pe_player = py + ph;
        float limite_stomp = ey + eh - (5.0 * p->escala);
        bool was_fall = p->vel_y > 0 || !p->on_ground;

        if (e->can_die && was_fall && p->vel_y > 0 && pe_player < limite_stomp){
            e->active = false;
            e->status = STAT_DEAD;

            p->vel_y = -10.0;
        }
        else{
            if (!p->invencivel){
                // p->vida--;
                p->invencivel = true;
                // p->tempo_invencibilidade_inicio = al_get_time();
    
                p->vel_y = -4.0;
                if (p->pos_x < e->x) p->vel_x = -5.0;
                else p->vel_x = 5.0;
            }
        }
    }
}

static void update_animation(Entity *e){
    if (e->status == STAT_DEAD) return;
}

static void update_ai_patrol(Entity *e);

static void update_ai_shark(Entity *e);

static void update_ai_bird(Entity *e);

static void entity_spawn_func(EntitiesManager *self, EntityType type, 
                                float x, float y, float range_dist){
    if (self->num_entities >= MAX_ENTITIES) return;
    
    Entity *e = &self->entities[self->num_entities];
    float scale = self->scale;

    e->type = type;
    e->active = true;

    e->x = x;
    e->y = y;

    e->start_x = x;
    e->start_y = y;

    e->end_x = x + (range_dist * scale);
    e->direction = 1;

    e->v_x = 0;
    e->v_y = 0;

    e->frame_atual = 0;
    e->tempo_animacao = 0;

    e->invincible = false;
    e->can_die = false;

    switch (type){
        case ENT_BEE:
            e->w = (27/2) * scale;
            e->h = (22/2) * scale;

            e->draw_offset_x = -1 * scale;
            e->draw_offset_y = -2 * scale;

            e->status = STAT_IDLE;

            e->spritesheet = self->spritesheets[ENT_BEE];
            e->max_frame = 3;
            break;
        case ENT_FLOWER:
            e->w = 10 * scale;
            e->h = 13 * scale;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -4 * scale;

            e->status = STAT_IDLE;

            e->spritesheet = self->spritesheets[ENT_FLOWER];
            e->max_frame = 2;
            break;
        case ENT_SHARK:
            e->w = 14 * scale;
            e->h = 26 * scale;

            e->v_y = 0;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -2 * scale;

            e->status = STAT_IDLE;

            e->spritesheet = self->spritesheets[ENT_SHARK];
            e->max_frame = 2;
            break;
        case ENT_BIRD:
            e->w = 15 * scale;
            e->h = 12 * scale;

            e->v_x = 2.0;
            e->v_y = 1.0;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -9 * scale;

            e->status = STAT_FLY;
            e->can_die = true;

            e->spritesheet = self->spritesheets[ENT_BIRD];
            e->max_frame = 3;
            break;
        case ENT_PENGUIN:
            e->w = 7 * scale;
            e->h = 11 * scale;

            e->v_x = 1.0;

            e->draw_offset_x = -12 * scale;
            e->draw_offset_y = -9 * scale;

            e->status = STAT_WALK;
            e->can_die = true;

            e->spritesheet = self->spritesheets[ENT_PENGUIN];
            e->max_frame = 3;
            break;
        case ENT_PIRANHA:
            e->w = 12 * scale;
            e->h = 8 * scale;

            e->v_x = 2.0;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -12 * scale;

            e->status = STAT_WALK;

            e->spritesheet = self->spritesheets[ENT_PIRANHA];
            e->max_frame = 2;
            break;
        case ENT_PANDA:
            e->w = 11 * scale;
            e->h = 13 * scale;

            e->v_x = 1.5;

            e->draw_offset_x = -11 * scale;
            e->draw_offset_y = -9 * scale;

            e->status = STAT_WALK;
            e->can_die = true;

            e->spritesheet = self->spritesheets[ENT_PANDA];
            e->max_frame = 7;
            break;
    }       
    
    self->num_entities++;
}

static void entity_update_func(EntitiesManager *self, Player *p){
    for (int i = 0; i < self->num_entities; i++){
        Entity *e = &self->entities[i];
        if (e->active){
            update_animation(e);
            update_collision_player(p, e);
        }
    }
}

static void entity_draw_func(EntitiesManager *self, float camera_x){
    float sclale = self->scale;

    for (int i = 0; i < self->num_entities; i++){
        Entity *e = &self->entities[i];

        if (e->active){
            int frame_calc = 0;
            if (e->max_frame > 0){
                frame_calc = (int)(al_get_time() * 6.0) % e->max_frame;
            }
            
            int row = 0;

            if (e->direction == -1) row = 1;

            if (e->type == ENT_SHARK){
                if (e->v_y > 0) row = 1;
                else row = 0;
            }

            if (e->spritesheet){
                float tela_x = (e->x - camera_x) + e->draw_offset_x;
                float tela_y = e->y + e->draw_offset_y;

                float dst_w = 32 * sclale;
                float dst_h = 32 * sclale;

                if (e->type == ENT_BEE){
                    dst_w = 16 * sclale;
                    dst_h = 16 * sclale;
                }

                al_draw_scaled_bitmap(
                    e->spritesheet,
                    frame_calc * 32, row * 32,
                    32, 32,

                    tela_x, tela_y,
                    dst_w, dst_h,
                    0
                );
            }

            al_draw_rectangle(
                e->x - camera_x, e->y,
                (e->x - camera_x) + e->w, e->y + e->h,
                al_map_rgb(255, 0, 0), 2
            );
        }
    }
}

static void entity_destroy_func(EntitiesManager *self){
    for (int i = 0; i < NUM_ENTITIES_SPRITESHEET; i++){
        al_destroy_bitmap(self->spritesheets[i]);
    }

    free(self);
}

EntitiesManager *entity_init(float game_scale){
    EntitiesManager *em;
    if (!(em = malloc(sizeof(EntitiesManager)))) return NULL;

    *em = (EntitiesManager){
        .num_entities = 0,
        .scale = game_scale,

        .spawn = entity_spawn_func,
        .update = entity_update_func,
        .draw = entity_draw_func,
        .destroy = entity_destroy_func
    };

    for (int i = 0; i < NUM_ENTITIES_SPRITESHEET; i++)
        em->spritesheets[i] = NULL;

    em->spritesheets[ENT_BEE] = al_load_bitmap("assets/abelha.png");
    em->spritesheets[ENT_FLOWER] = al_load_bitmap("assets/flor.png");
    em->spritesheets[ENT_SHARK] = al_load_bitmap("assets/tutu.png");
    em->spritesheets[ENT_BIRD] = al_load_bitmap("assets/gralha_azul.png");
    em->spritesheets[ENT_PENGUIN] = al_load_bitmap("assets/pinguim.png");
    em->spritesheets[ENT_PIRANHA] = al_load_bitmap("assets/piranha.png");
    em->spritesheets[ENT_PANDA] = al_load_bitmap("assets/panda.png");

    al_convert_mask_to_alpha(em->spritesheets[ENT_BEE], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_FLOWER], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_SHARK], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_BIRD], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_PENGUIN], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_PIRANHA], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_PANDA], al_map_rgb(105, 255, 88));

    return em;
}