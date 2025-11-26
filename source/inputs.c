#include "../includes/inputs.h"
#include "raylib.h"
#include "../includes/structs.h" // Inclua structs.h para ter acesso ao campo estaNoChao

// Ajuste esses valores para ficar "gostoso" de jogar
#define GRAVIDADE 1.0f
#define FORCA_PULO -18.0f // Pulo menor (era -21.5f)

void inputs_jogador_movimento(jogador *jogador, int largura_tela, int altura_tela, int velocidade_jogador, obstaculo *obstaculos) {

    // NOTA: O parâmetro 'obstaculos' não é usado aqui, mas foi mantido na assinatura original.

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
    int piso_tela = altura_tela - jogador->hitbox_jogador.height;

    if (jogador->hitbox_jogador.y >= piso_tela) {
        jogador->hitbox_jogador.y = piso_tela; // Cola no chão
        jogador->velocidade_vertical = 0;      // Para a queda
        jogador->estaNoChao = 1;               // Marca que está no chão
    }

    // PULO: Só pula se apertar e se 'estaNoChao' for 1
    // Lembre-se: estaNoChao é setado para 1 se estiver no chão da tela OU em uma plataforma (em fases.c)
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && jogador->estaNoChao) {
        jogador->velocidade_vertical = FORCA_PULO; // Impulso para cima
        jogador->estaNoChao = 0;                   // Zera imediatamente para impedir pulo duplo
        moveu = 1;
    }

    // Se está caindo (velocidade positiva) e não está no chão da tela, 
    // zera estaNoChao para forçar a checagem de colisão em fases.c
    if (jogador->velocidade_vertical > 0.0f && jogador->hitbox_jogador.y < piso_tela) {
        jogador->estaNoChao = 0; 
    }

    if (!jogador->estaNoChao) {
        // Se estiver pulando ou caindo, mostra sprite de cima/pulo
        jogador->sprite_atual = SPRITE_CIMA; 
    } else if (!moveu) {
        jogador->sprite_atual = SPRITE_PARADO;
    }
}