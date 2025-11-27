#include "../includes/inputs.h"
#include "raylib.h"
#include "../includes/structs.h" 


#define GRAVIDADE 1.0f      
#define FORCA_PULO -18.0f 

void inputs_jogador_movimento(jogador *jogador, int largura_tela, int altura_tela, int velocidade_jogador, obstaculo *obstaculos) {

    int moveu = 0;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        jogador->hitbox_jogador.x += velocidade_jogador;
        
        if (jogador->hitbox_jogador.x + jogador->hitbox_jogador.width > largura_tela)
            jogador->hitbox_jogador.x = largura_tela - jogador->hitbox_jogador.width;

        jogador->sprite_atual = SPRITE_DIREITA;
        moveu = 1;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        jogador->hitbox_jogador.x -= velocidade_jogador;
        
        if (jogador->hitbox_jogador.x < 0)
            jogador->hitbox_jogador.x = 0;

        jogador->sprite_atual = SPRITE_ESQUERDA;
        moveu = 1;
    }
    
    jogador->velocidade_vertical += GRAVIDADE;
    jogador->hitbox_jogador.y += jogador->velocidade_vertical;

    int piso_tela = altura_tela - jogador->hitbox_jogador.height;

    if (jogador->hitbox_jogador.y >= piso_tela) {
        jogador->hitbox_jogador.y = piso_tela; 
        jogador->velocidade_vertical = 0;      
        jogador->estaNoChao = 1;              
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && jogador->estaNoChao) {
        jogador->velocidade_vertical = FORCA_PULO; 
        jogador->estaNoChao = 0;                   
        moveu = 1;
    }

    if (jogador->velocidade_vertical > 0.0f && jogador->hitbox_jogador.y < piso_tela) {
        jogador->estaNoChao = 0; 
    }

    if (!jogador->estaNoChao) {
        jogador->sprite_atual = SPRITE_CIMA; 
    } else if (!moveu) {
        jogador->sprite_atual = SPRITE_PARADO;
    }
}