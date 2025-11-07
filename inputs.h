#ifndef INPUTS_H
#define INPUTS_H

#include "raylib.h"
#include "structs.h"
#include <ctype.h>

void inputs_jogador_movimento(Rectangle *jogador, int largura_tela, int tamanho_tela, int velocidade_jogador, obstaculo *obstaculos){
    if (IsKeyDown(KEY_RIGHT)){
        jogador->x += velocidade_jogador;  //  muda posição horizontal do jogador
        if (jogador->x + jogador->width > largura_tela) {
            jogador->x = largura_tela - jogador->width;        
        }       // para o jogador não sair da tela para a direita
    }

    if (IsKeyDown(KEY_LEFT)){
        jogador->x -= velocidade_jogador; // muda posição horizontal do jogador
        if (jogador->x < 0) {       // verifica se o jogador saiu da tela na extremidade esquerda
            jogador->x = 0;    
        }   // para o jogador não sair da tela para a extremidade esquerda
    }

    if ((IsKeyPressed(KEY_SPACE) ) || (IsKeyDown(KEY_UP))){   // quando pressionado  a tecla de espaço ou seta para cima
        jogador->y -= velocidade_jogador;   // muda posição vertical do jogador
        if (jogador->y < 0) {
            jogador->y = 0;  //  jogador não sair da tela para cima
        }
    }
       
    if(IsKeyDown(KEY_DOWN)){
        jogador->y += velocidade_jogador;   // muda posição vertical do jogador
        if (jogador->y + jogador->height > tamanho_tela) {
            jogador->y = tamanho_tela - jogador->height;     // jogador não sair da tela para baixo
        }
    }
}

#endif

//criado o arquivo dos inputs