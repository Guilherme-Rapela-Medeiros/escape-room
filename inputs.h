#ifndef INPUTS_H
#define INPUTS_H

#include "raylib.h"
#include "structs.h"
#include <ctype.h>

void inputs_jogador_movimento(jogador *jogador, int largura_tela, int tamanho_tela, int velocidade_jogador, obstaculo *obstaculos){
    if (IsKeyDown(KEY_RIGHT)){
        jogador-> hitbox_jogador.x += velocidade_jogador;  //  muda posição horizontal do jogador
        if (jogador-> hitbox_jogador.x + jogador->hitbox_jogador.width > largura_tela) {
            jogador->hitbox_jogador.x = largura_tela - jogador->hitbox_jogador.width;        
        }       // para o jogador não sair da tela para a direita
    }

    if (IsKeyDown(KEY_LEFT)){
        jogador-> hitbox_jogador.x -= velocidade_jogador; // muda posição horizontal do jogador
        if (jogador->hitbox_jogador.x < 0) {       // verifica se o jogador saiu da tela na extremidade esquerda
            jogador->hitbox_jogador.x = 0;    
        }   // para o jogador não sair da tela para a extremidade esquerda
    }

    if ((IsKeyPressed(KEY_SPACE) ) || (IsKeyPressed(KEY_UP))){   // quando pressionado  a tecla de espaço ou seta para cima
        jogador->hitbox_jogador.y -= velocidade_jogador;   // muda posição vertical do jogador
        if (jogador->hitbox_jogador.y < 0) {
            jogador->hitbox_jogador.y = 0;  //  jogador não sair da tela para cima
        }
    }
       
    if(IsKeyDown(KEY_DOWN)){
        jogador->hitbox_jogador.y += velocidade_jogador;   // muda posição vertical do jogador
        if (jogador->hitbox_jogador.y + jogador->hitbox_jogador.height > tamanho_tela) {
            jogador->hitbox_jogador.y = tamanho_tela - jogador->hitbox_jogador.height;     // jogador não sair da tela para baixo
        }
    }
}

#endif

//criado o arquivo dos inputs