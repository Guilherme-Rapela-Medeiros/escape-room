#ifndef INPUTS_H
#define INPUTS_H

#include "raylib.h"
#include "structs.h"
#include <ctype.h>

void inputs_jogador_movimento(rectangle *jogador, int largura_tela, int velocidade_jogador, fases.obstaculo *obstaculos){
    if (IsKeyDown(KEY_RIGHT)){
        jogador->x += velocidade_jogador;  // posição horizontal do jogador
        if (jogador->x + jogador->width > largura_tela) {
            jogador->x = largura_tela - jogador->width;
        }
    }
}
    if (IsKeyDown(KEY_LEFT)){
        jogador->x -= velocidade_jogador;
        if (jogador->x < 0) {
            jogador->x = 0;
        }
    }

    if ((IsKeyPressed(KEY_SPACE) ) || (IsKeyDown(KEY_UP))){}
       
    if(IsKeyDown(KEY_DOWN)){}



//criado o arquivo dos inputs