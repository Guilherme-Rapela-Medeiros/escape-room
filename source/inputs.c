#include "../includes/inputs.h"
#include "raylib.h"

void inputs_jogador_movimento(jogador *jogador, int largura_tela, int altura_tela, int velocidade_jogador, obstaculo *obstaculos) {

    int moveu = 0;

    if (IsKeyDown(KEY_RIGHT)) {
        jogador->hitbox_jogador.x += velocidade_jogador;
        if (jogador->hitbox_jogador.x + jogador->hitbox_jogador.width > largura_tela)
            jogador->hitbox_jogador.x = largura_tela - jogador->hitbox_jogador.width;

        jogador->sprite_atual = SPRITE_DIREITA;
        moveu = 1;
    }

    if (IsKeyDown(KEY_LEFT)) {
        jogador->hitbox_jogador.x -= velocidade_jogador;
        if (jogador->hitbox_jogador.x < 0)
            jogador->hitbox_jogador.x = 0;

        jogador->sprite_atual = SPRITE_ESQUERDA;
        moveu = 1;
    }

    if (IsKeyDown(KEY_UP)) {
        jogador->hitbox_jogador.y -= velocidade_jogador;
        if (jogador->hitbox_jogador.y < 0)
            jogador->hitbox_jogador.y = 0;

        jogador->sprite_atual = SPRITE_CIMA;
        moveu = 1;
    }

    if (IsKeyDown(KEY_DOWN)) {
        jogador->hitbox_jogador.y += velocidade_jogador;
        if (jogador->hitbox_jogador.y + jogador->hitbox_jogador.height > altura_tela)
            jogador->hitbox_jogador.y = altura_tela - jogador->hitbox_jogador.height;

        jogador->sprite_atual = SPRITE_BAIXO;
        moveu = 1;
    }

    if (!moveu) {
        jogador->sprite_atual = SPRITE_PARADO;
    }
}