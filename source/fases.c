#include <stdlib.h>
#include "fases.h"
#include "raylib.h"

// Tamanho padrão do jogador
#define JOGADOR_LARGURA 50
#define JOGADOR_ALTURA 50

// Função auxiliar: verifica colisão entre dois retângulos
static bool colidem(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width) && (a.x + a.width > b.x) &&
           (a.y < b.y + b.height) && (a.y + a.height > b.y);
}

// Inicializa as fases do jogo
void comecarfase(fases fasesArray[], int quantidade) {
    if (!fasesArray) return;

    for (int i = 0; i < quantidade; i++) {
        fasesArray[i].numero = i + 1;
        fasesArray[i].quantidadeObstaculos = 2 + i;
        fasesArray[i].completo = false;

        // Aloca memória para os obstáculos
        fasesArray[i].obstaculos = malloc(sizeof(obstaculo) * fasesArray[i].quantidadeObstaculos);
        if (!fasesArray[i].obstaculos) continue;

        // Posição inicial do jogador nesta fase
        fasesArray[i].posicaoinicial = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

        // Cria obstáculos
        for (int j = 0; j < fasesArray[i].quantidadeObstaculos; j++) {
            obstaculo *o = &fasesArray[i].obstaculos[j];
            o->tamanho = (Vector2){ 40.0f + 10.0f * j, 40.0f };
            o->posicao = (Vector2){ 50.0f + 120.0f * j + 20.0f * i, 50.0f + 80.0f * j };
            o->velocidade = (Vector2){ 1.0f + i * 0.2f + j * 0.1f, 0.0f };
            o->ativo = true;
        }

        // Define portal da fase
        fasesArray[i].saida = (portal){
            .posicao = { GetScreenWidth() - 80.0f, GetScreenHeight() / 2.0f },
            .tamanho = { 60.0f, 80.0f },
            .ativo = true
        };
    }
}

// Libera memória das fases
void acabarFases(fases fasesArray[], int quantidade) {
    if (!fasesArray) return;

    for (int i = 0; i < quantidade; i++) {
        free(fasesArray[i].obstaculos);
        fasesArray[i].obstaculos = NULL;
        fasesArray[i].quantidadeObstaculos = 0;
    }
}

// Atualiza a lógica da fase (movimentos e colisões)
void atualizarFases(fases *fase, jogador *jogador) {
    if (!fase || !jogador) return;

    Rectangle rJogador = jogador->hitbox_jogador;

    for (int i = 0; i < fase->quantidadeObstaculos; i++) {
        obstaculo *o = &fase->obstaculos[i];
        if (!o->ativo) continue;

        // Move o obstáculo
        o->posicao.x += o->velocidade.x;
        o->posicao.y += o->velocidade.y;

        // Rebater nas bordas
        if (o->posicao.x < 0 || o->posicao.x + o->tamanho.x > GetScreenWidth())
            o->velocidade.x *= -1;
        if (o->posicao.y < 0 || o->posicao.y + o->tamanho.y > GetScreenHeight())
            o->velocidade.y *= -1;

        // Colisão com o jogador
        Rectangle rObst = { o->posicao.x, o->posicao.y, o->tamanho.x, o->tamanho.y };
        if (colidem(rJogador, rObst)) {
            jogador->vida -= 1;
            o->ativo = false;
        }
    }

    // Colisão com o portal
    portal *p = &fase->saida;
    Rectangle rPortal = { p->posicao.x, p->posicao.y, p->tamanho.x, p->tamanho.y };
    if (p->ativo && colidem(rJogador, rPortal)) {
        fase->completo = true;
        p->ativo = false;
    }
}

// Desenha a fase atual
void desenharFase(fases *fase, jogador *jogador) {
    if (!fase || !jogador) return;

    // Obstáculos
    for (int i = 0; i < fase->quantidadeObstaculos; i++) {
        obstaculo *o = &fase->obstaculos[i];
        if (o->ativo)
            DrawRectangleV(o->posicao, o->tamanho, RED);
    }

    // Portal
    if (fase->saida.ativo)
        DrawRectangleV(fase->saida.posicao, fase->saida.tamanho, BLUE);

    // Jogador
    DrawRectangle((int)jogador->hitbox_jogador.x, (int)jogador->hitbox_jogador.y, (int)jogador->hitbox_jogador.width, (int)jogador->hitbox_jogador.height, GREEN);

    // HUD simples
    DrawText(TextFormat("Vida: %d", jogador->vida), 10, 40, 20, BLACK);
}