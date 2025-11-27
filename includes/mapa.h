#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"
#include "structs.h"

#define MAPA_LINHAS 12
#define MAPA_COLUNAS 16
#define TAMANHO_BLOCO 50  

#define BLOCO_VAZIO 0
#define BLOCO_PLATAFORMA 1
#define BLOCO_OBSTACULO 2
#define BLOCO_PORTAL 3

typedef struct {
    Texture2D plataforma;
    Texture2D obstaculo;
    Texture2D portal;
} TexturasMapa;

void carregarTexturasMapa(TexturasMapa *texturas);

void descarregarTexturasMapa(TexturasMapa *texturas);

void carregarMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], int numeroFase);

void desenharMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], TexturasMapa *texturas);

int verificarColisaoMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador);

int estaEmPlatafora(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador);

int verificarPortal(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador);

#endif
