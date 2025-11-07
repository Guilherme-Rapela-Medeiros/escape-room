#ifndef INPUTS_H
#define INPUTS_H

#include "raylib.h"
#include "structs.h"
#include <ctype.h>

void inputs_jogador_movimento(Rectangle *jogador, int largura_tela, int tamanho_tela, int velocidade_jogador, obstaculo *obstaculos){
    if (IsKeyDown(KEY_RIGHT)){
        jogador->x += velocidade_jogador;  // posição horizontal do jogador
        if (jogador->x + jogador->width > largura_tela) {
            jogador->x = largura_tela - jogador->width;
        }
    }

    if (IsKeyDown(KEY_LEFT)){
        jogador->x -= velocidade_jogador;
        if (jogador->x < 0) {
            jogador->x = 0;
        }
    }

    if ((IsKeyPressed(KEY_SPACE) ) || (IsKeyDown(KEY_UP))){
        jogador->y -= velocidade_jogador;
        if (jogador->y < 0) {
            jogador->y = 0;
        }
    }
       
    if(IsKeyDown(KEY_DOWN)){
        jogador->y += velocidade_jogador;
        if (jogador->y + jogador->height > tamanho_tela) {
            jogador->y = tamanho_tela - jogador->height;
        }
    }
}

#endif

//criado o arquivo dos inputs