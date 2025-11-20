#ifndef STRUCTS_H
#define STRUCTS_H

#define TAMANHO_NOME 50

#include <raylib.h>

#define TRUE 1

#define FALSE 0

typedef enum {
    TELA_MENU,
    TELA_TUTORIAL,
    TELA_FASES,
    TELA_JOGO,
    TELA_RANKING,
    TELA_FINAL
} TelaAtual;

typedef struct jogador {
    Rectangle hitbox_jogador;
    int vida;
    char nome[TAMANHO_NOME];
} jogador;

typedef struct obstaculo{
    Vector2 posicao;
    Vector2 tamanho;
    Vector2 velocidade;
    int ativo;
} obstaculo;

typedef struct portal{
    Vector2 posicao;
    Vector2 tamanho;
    int ativo;
} portal;

typedef struct fases{
    int numero;
    obstaculo *obstaculos;
    int quantidadeObstaculos;
    portal saida;
    Vector2 posicaoinicial;
    int completo;
} fases;

typedef struct ranking {
    char nome[TAMANHO_NOME];
    double tempo;             // agora é o ÚNICO critério de rank
    struct ranking *prox;     // lista ligada
} ranking;

typedef struct jogo {
    jogador jogador;
    int FimDeJogo;
    int FaseAtual;
    fases fases[4];
    ranking *Head;
} jogo;

#endif