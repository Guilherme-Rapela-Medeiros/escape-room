#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"
#include "structs.h"

// Define o tamanho padrão da matriz da fase
#define MAPA_LINHAS 12
#define MAPA_COLUNAS 16
#define TAMANHO_BLOCO 50  // Tamanho em pixels de cada bloco/célula

// Cada célula da matriz pode representar:
// 0 = vazio
// 1 = plataforma
// 2 = obstáculo
// 3 = portal
// (você pode adicionar outros códigos depois)
#define BLOCO_VAZIO 0
#define BLOCO_PLATAFORMA 1
#define BLOCO_OBSTACULO 2
#define BLOCO_PORTAL 3

// Estrutura para armazenar texturas do mapa
typedef struct {
    Texture2D plataforma;
    Texture2D obstaculo;
    Texture2D portal;
} TexturasMapa;

// Funções principais do módulo de mapa

// Carrega as texturas usadas no mapa
void carregarTexturasMapa(TexturasMapa *texturas);

// Descarrega as texturas do mapa (para liberar memória)
void descarregarTexturasMapa(TexturasMapa *texturas);

// Inicializa a matriz da fase (preenche com valores base)
void carregarMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], int numeroFase);

// Desenha o mapa na tela
void desenharMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], TexturasMapa *texturas);

// Verifica se há colisão entre o jogador e obstáculos do mapa
int verificarColisaoMapa(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador);

// Verifica se o jogador está em pé numa plataforma (colisão inferior)
int estaEmPlatafora(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador);

// Retorna verdadeiro se jogador tocou no portal (saída da fase)
int verificarPortal(int mapa[MAPA_LINHAS][MAPA_COLUNAS], Rectangle hitbox_jogador);

#endif
