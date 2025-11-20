#include "world.h"
#include <stdio.h>

#define COLS_PER_BG 8

// IDs de cenario
enum
{
    TILE_AR,          // invisivel
    TILE_CHAO_BG,     // "invisivel" (esta no bg)
    TILE_CHAO_PADRAO, // visivel
    TILE_CHAO_GELO,   // visivel
    TILE_PLATAFORMA,  // visivel
    TILE_AGUA         // visivel + dano
};

void world_destroy_func(World *self)
{
    for (int i = 0; i < NUM_BG; i++){
        if (self->bg_img[i]){
            al_destroy_bitmap(self->bg_img[i]);
        }
    }

    free(self->plataforms);
    free(self);
}

void world_draw_func(World *self, float camera_x){
    if (camera_x < 0) camera_x = 0;
    
    float bg_width_tela = 256 * self->escala;

    int index_inicial = (int)(camera_x / bg_width_tela);

    for (int i = 0; i < 2; i++){
        int idx = index_inicial + i;

        if (idx < NUM_BG){
            float pos_x = (idx * bg_width_tela) - camera_x;

            al_draw_scaled_bitmap(
                self->bg_img[idx],
                0, 0, 256, 144,
                pos_x, 0,
                bg_width_tela, 144 * self->escala,
                0
            );
        }
    }

    // ------ DEBUG --------
    ALLEGRO_COLOR cor_debug;
    for (int i = 0; i < self->num_plataforms; i++){
        Plataform p = self->plataforms[i];

        float tela_x = p.x - camera_x;
        float tela_y = p.y;

        if (p.type == PLAT_TYPE_BLOCK)
            cor_debug = al_map_rgb(255, 0, 0);
        else if (p.type == PLAT_TYPE_ONE_WAY)
            cor_debug = al_map_rgb(0, 255, 0);
        else
            cor_debug = al_map_rgb(0, 0, 255);
        
        al_draw_rectangle(
            tela_x, tela_y,
            tela_x + p.w, tela_y + p.h,
            cor_debug, 2
        );
    }
}

static void add_plataform(World *w, float x, float y, float width, float height, int type){
    w->num_plataforms++;

    w->plataforms = realloc(w->plataforms, w->num_plataforms * sizeof(Plataform));

    w->plataforms[w->num_plataforms - 1] = (Plataform){
        .x = x,
        .y = y,
        .w = width,
        .h = height,
        .type = type};
}

World *world_init(void)
{
    World *w;
    if (!(w = malloc(sizeof(World))))
        return NULL;

    // Variaveis basicas
    *w = (World){
        .escala = 5.0,
        .plataforms = NULL,
        .num_plataforms = 0,

        .draw = world_draw_func,
        .destroy = world_destroy_func};

    // Cenarios
    char path[50];
    for (int i = 0; i < NUM_BG; i++)
    {
        sprintf(path, "assets/bg%d.png", i);
        w->bg_img[i] = al_load_bitmap(path);

        if (!w->bg_img[i])
        {
            fprintf(stderr, "Nao foi possivel carregar o background %d\n", i);
            free(w);
            return NULL;
        }
    }

    // Plataformas
    float escala = 5.0;
    float width_plataform = 256 * escala;
    float offset_x = 0;

    // Bg0
    add_plataform(w, offset_x, 106 * escala, // Chao
                    width_plataform, 200, 
                    PLAT_TYPE_BLOCK);   
    add_plataform(w, 59 * escala, 73 * escala, // Placa
                    16 * escala, 5, 
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, 99 * escala, 50 * escala, // Telhado
                    109 * escala, 4, 
                    PLAT_TYPE_ONE_WAY);     

    offset_x += width_plataform;    
    // Bg1
    add_plataform(w, offset_x, 106 * escala, // Chao
                    width_plataform, 200, 
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (13 * escala), 80 * escala, // Plataforma
                    10 * escala, 6, 
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (61 * escala), 65 * escala, // Plataforma
                    10 * escala, 9, 
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (91 * escala), 56 * escala, // Plataforma
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (139 * escala), 65 * escala,// Plataforma
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (180 * escala), 80 * escala,// Plataforma
                    8 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    
    offset_x += width_plataform;
    //Bg2
    add_plataform(w, offset_x, 106 * escala,    // Chao
                    width_plataform, 200,
                    PLAT_TYPE_BLOCK);
    // add_plataform(w, 68 * escala, 111 * escala,
    //                 32 * escala, 9, 
    //                 PLAT_TYPE_DANGER);
    add_plataform(w, offset_x + (71 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (100 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (130 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (155 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    
    offset_x += width_plataform;
    //Bg3
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (106 * escala), 64 * escala, 
                    44 * escala, 5,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (94 * escala), 80 * escala, 
                    52 * escala, 2,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (33 * escala), 84 * escala, 
                    8 * escala, 4,
                    PLAT_TYPE_ONE_WAY);
    
    offset_x += width_plataform;
    //Bg4 (mudar dps)
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (14 * escala), 93 * escala, 
                    9 * escala, 11 * escala,
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (233 * escala), 93 * escala, 
                    9 * escala, 11 * escala,
                    PLAT_TYPE_BLOCK);

    offset_x += width_plataform;
    //Bg5
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_BLOCK);
    offset_x += width_plataform;
    //Bg6
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_BLOCK);
    return w;
}