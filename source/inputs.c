#include "../includes/inputs.h"
#include "raylib.h"

void inputs_jogador_movimento(jogador *jogador, int largura_tela, int altura_tela, int velocidade_jogador, obstaculo *obstaculos) {

    // Movimento para a direita
    if (IsKeyDown(KEY_RIGHT)) {
        jogador->hitbox_jogador.x += velocidade_jogador;
        if (jogador->hitbox_jogador.x + jogador->hitbox_jogador.width > largura_tela) {
            jogador->hitbox_jogador.x = largura_tela - jogador->hitbox_jogador.width;
        }
    }

    // Movimento para a esquerda
    if (IsKeyDown(KEY_LEFT)) {
        jogador->hitbox_jogador.x -= velocidade_jogador;
        if (jogador->hitbox_jogador.x < 0) {
            jogador->hitbox_jogador.x = 0;
        }
    }

    // Movimento para cima (pulo)
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) {
        jogador->hitbox_jogador.y -= velocidade_jogador;
        if (jogador->hitbox_jogador.y < 0) {
            jogador->hitbox_jogador.y = 0;
        }
    }

    // Movimento para baixo
    if (IsKeyDown(KEY_DOWN)) {
        jogador->hitbox_jogador.y += velocidade_jogador;
        if (jogador->hitbox_jogador.y + jogador->hitbox_jogador.height > altura_tela) {
            jogador->hitbox_jogador.y = altura_tela - jogador->hitbox_jogador.height;
        }
    }
}