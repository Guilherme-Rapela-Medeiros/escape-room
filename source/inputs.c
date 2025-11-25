#include "../includes/inputs.h"
#include "raylib.h"

// Ajuste esses valores para ficar "gostoso" de jogar
#define GRAVIDADE 0.8f
#define FORCA_PULO -18.0f

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
    
    // Aplica a gravidade sempre (puxa para baixo)
    jogador->velocidade_vertical += GRAVIDADE;
    jogador->hitbox_jogador.y += jogador->velocidade_vertical;

    // Define onde é o chão (Pode ser altura_tela ou altura_tela - 50, etc)
    int piso = altura_tela - jogador->hitbox_jogador.height;

    // Verifica se tocou no chão
    int esta_no_chao = 0;
    if (jogador->hitbox_jogador.y >= piso) {
        jogador->hitbox_jogador.y = piso; // Cola no chão
        jogador->velocidade_vertical = 0; // Para de cair
        esta_no_chao = 1;
    }

    // PULO: Só pula se apertar ESPAÇO/CIMA e estiver no chão
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && esta_no_chao) {
        jogador->velocidade_vertical = FORCA_PULO; // Impulso para cima
        moveu = 1;
    }

    // Se estiver no ar, atualiza o sprite para algo (opcional, pode manter o anterior)
    if (!esta_no_chao) {
        // Se quiser um sprite de pulo: jogador->sprite_atual = SPRITE_PULO;
        // Se não, mantém o sprite da direção ou CIMA
        jogador->sprite_atual = SPRITE_CIMA; 
    } else if (!moveu) {
        jogador->sprite_atual = SPRITE_PARADO;
    }
}