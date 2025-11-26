// inputs.c

#include "../includes/inputs.h"
#include "raylib.h"
#include "../includes/structs.h" 

// Ajuste de JOGABILIDADE
// Mantida baixa para facilitar a colisão com plataformas
#define GRAVIDADE 0.9f 
// 💡 AJUSTADO: Aumentado sutilmente para -17.0f.
#define FORCA_PULO -17.0f 
#define VELOCIDADE_H 5 // Velocidade de movimento horizontal padrao

void inputs_jogador_movimento(jogador *jogador, int largura_tela, int altura_tela, int velocidade_jogador, obstaculo *obstaculos) {

    int moveu = 0;

    // -----------------------------------------------------------------
    // 1. MOVIMENTAÇÃO HORIZONTAL (Esquerda / Direita)
    // -----------------------------------------------------------------
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        jogador->hitbox_jogador.x += velocidade_jogador;
        
        // Limite da tela direita
        if (jogador->hitbox_jogador.x + jogador->hitbox_jogador.width > largura_tela)
            jogador->hitbox_jogador.x = largura_tela - jogador->hitbox_jogador.width;

        jogador->sprite_atual = SPRITE_DIREITA;
        moveu = 1;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        jogador->hitbox_jogador.x -= velocidade_jogador;
        
        // Limite da tela esquerda
        if (jogador->hitbox_jogador.x < 0)
            jogador->hitbox_jogador.x = 0;

        jogador->sprite_atual = SPRITE_ESQUERDA;
        moveu = 1;
    }

    // -----------------------------------------------------------------
    // 2. FÍSICA VERTICAL (Gravidade e Pulo)
    // -----------------------------------------------------------------
    
    // Aplica a gravidade (aceleração) e move o jogador
    jogador->velocidade_vertical += GRAVIDADE;
    jogador->hitbox_jogador.y += jogador->velocidade_vertical;

    // Colisão com o CHÃO DA TELA
    int piso_tela = altura_tela - (int)jogador->hitbox_jogador.height;

    if (jogador->hitbox_jogador.y >= piso_tela) {
        jogador->hitbox_jogador.y = piso_tela;
        jogador->velocidade_vertical = 0;
        jogador->estaNoChao = 1; 
    }

    // PULO: Aplica a força negativa (para cima)
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && jogador->estaNoChao) {
        // Valor final ajustado: -17.0f
        jogador->velocidade_vertical = FORCA_PULO; 
        jogador->estaNoChao = 0; 
        moveu = 1;
    }

    // Se está caindo e não está no chão da tela, zera estaNoChao para forçar a checagem em fases.c
    if (jogador->velocidade_vertical > 0.0f && jogador->hitbox_jogador.y < piso_tela) {
        jogador->estaNoChao = 0; 
    }

    // Gerenciamento do Sprite (Pulo/Parado)
    if (!jogador->estaNoChao) {
        jogador->sprite_atual = SPRITE_CIMA; 
    } else if (!moveu) {
        jogador->sprite_atual = SPRITE_PARADO;
    }
}