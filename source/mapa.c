#include "../includes/mapa.h"
#include "raylib.h"
#include "../includes/structs.h"
#include <stdio.h>


// Legenda:
// 0 = vazio
// 1 = plataforma
// 2 = obstáculo
// 3 = portal (saída)

// FASE 1: Fácil - poucas plataformas, layout simples
int mapa_fase_1[MAPA_LINHAS][MAPA_COLUNAS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// FASE 2: Médio - mais plataformas flutuantes, 1 obstáculo
int mapa_fase_2[MAPA_LINHAS][MAPA_COLUNAS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// FASE 3: Difícil - muitas plataformas, 2 obstáculos
int mapa_fase_3[MAPA_LINHAS][MAPA_COLUNAS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// FASE 4: Desafio final - layout complexo, 3 obstáculos

int mapa_fase_4[MAPA_LINHAS][MAPA_COLUNAS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
 
 void carregarMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], int numeroFase) {
    if (!mapa) return;

    int (*fonte)[MAPA_COLUNAS] = NULL;

    switch (numeroFase) {
        case 1:
            fonte = mapa_fase_1;
            printf("[MAPA] Fase 1 carregada\n");
            break;
        case 2:
            fonte = mapa_fase_2;
            printf("[MAPA] Fase 2 carregada\n");
            break;
        case 3:
            fonte = mapa_fase_3;
            printf("[MAPA] Fase 3 carregada\n");
            break;
        case 4:
            fonte = mapa_fase_4;
            printf("[MAPA] Fase 4 carregada\n");
            break;
        default:
            fonte = mapa_fase_1;
            printf("[MAPA] Fase inválida, carregando fase 1\n");
            break;
    }

    for (int i = 0; i < MAPA_LINHAS; i++) {
        for (int j = 0; j < MAPA_COLUNAS; j++) {
            mapa[i][j] = fonte[i][j];
        }
    }
}

//* Desenha o mapa na tela com cores diferentes para cada tipo de bloco

void desenharMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], TexturasMapa *texturas) {
    if (!mapa) return;

    for (int linha = 0; linha < MAPA_LINHAS; linha++) {
        for (int coluna = 0; coluna < MAPA_COLUNAS; coluna++) {
            int bloco = mapa[linha][coluna];
            float x = coluna * TAMANHO_BLOCO;
            float y = linha * TAMANHO_BLOCO;

            switch (bloco) {
                case BLOCO_PLATAFORMA:
                    DrawRectangle((int)x, (int)y, TAMANHO_BLOCO, TAMANHO_BLOCO, YELLOW);
                    DrawRectangleLines((int)x, (int)y, TAMANHO_BLOCO, TAMANHO_BLOCO, ORANGE);
                    break;

                case BLOCO_OBSTACULO:
                    DrawRectangle((int)x, (int)y, TAMANHO_BLOCO, TAMANHO_BLOCO, RED);
                    DrawRectangleLines((int)x, (int)y, TAMANHO_BLOCO, TAMANHO_BLOCO, MAROON);
                    DrawLine((int)x, (int)y, (int)(x + TAMANHO_BLOCO), (int)(y + TAMANHO_BLOCO), MAROON);
                    DrawLine((int)(x + TAMANHO_BLOCO), (int)y, (int)x, (int)(y + TAMANHO_BLOCO), MAROON);
                    break;

                case BLOCO_PORTAL:
                    DrawRectangle((int)x, (int)y, TAMANHO_BLOCO, TAMANHO_BLOCO, BLUE);
                    DrawRectangleLines((int)x, (int)y, TAMANHO_BLOCO, TAMANHO_BLOCO, DARKBLUE);
                    DrawCircle((int)(x + TAMANHO_BLOCO / 2), (int)(y + TAMANHO_BLOCO / 2), TAMANHO_BLOCO / 4, SKYBLUE);
                    break;

                case BLOCO_VAZIO:
                default:
                    // Não desenha nada
                    break;
            }
        }
    }
}

int verificarColisaoMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador) {
    if (!mapa) return 0;

    for (int linha = 0; linha < MAPA_LINHAS; linha++) {
        for (int coluna = 0; coluna < MAPA_COLUNAS; coluna++) {
            if (mapa[linha][coluna] == BLOCO_OBSTACULO) {
                Rectangle bloco_rect = {
                    coluna * TAMANHO_BLOCO,
                    linha * TAMANHO_BLOCO,
                    TAMANHO_BLOCO,
                    TAMANHO_BLOCO
                };

                if (CheckCollisionRecs(hitbox_jogador, bloco_rect)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int estaEmPlatafora(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador) {
    if (!mapa) return 0;

    Rectangle pernas = {
        hitbox_jogador.x + 5,
        hitbox_jogador.y + hitbox_jogador.height - 5,
        hitbox_jogador.width - 10,
        10
    };

    for (int linha = 0; linha < MAPA_LINHAS; linha++) {
        for (int coluna = 0; coluna < MAPA_COLUNAS; coluna++) {
            int bloco = mapa[linha][coluna];

            if (bloco == BLOCO_PLATAFORMA || bloco == BLOCO_OBSTACULO) {
                Rectangle bloco_rect = {
                    coluna * TAMANHO_BLOCO,
                    linha * TAMANHO_BLOCO,
                    TAMANHO_BLOCO,
                    TAMANHO_BLOCO
                };

                if (CheckCollisionRecs(pernas, bloco_rect)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int verificarPortal(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador) {
    if (!mapa) return 0;

    for (int linha = 0; linha < MAPA_LINHAS; linha++) {
        for (int coluna = 0; coluna < MAPA_COLUNAS; coluna++) {
            if (mapa[linha][coluna] == BLOCO_PORTAL) {
                Rectangle portal_rect = {
                    coluna * TAMANHO_BLOCO,
                    linha * TAMANHO_BLOCO,
                    TAMANHO_BLOCO,
                    TAMANHO_BLOCO
                };

                if (CheckCollisionRecs(hitbox_jogador, portal_rect)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}
