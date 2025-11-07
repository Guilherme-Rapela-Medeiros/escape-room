#ifndef STRUCTS_H
#define STRUCTS_H

#define TAMANHO_NOME 50

#include <raylib.h>

typedef struct jogador {
    Vector2 posicao;
    int vida;
    char nome[TAMANHO_NOME];
} jogador;

typedef struct obstaculo{
    Vector2 posicao;
    Vector2 tamanho;
    Vector2 velocidade;
    bool ativo;
} obstaculo;

typedef struct portal{
    Vector2 posicao;
    Vector2 tamanho;
    bool ativo;
} portal;

typedef struct fases{
    int numero;
    obstaculo *obstaculos;
    int quantidadeObstaculos;
    portal saida;
    Vector2 posicaoinicial;
    bool completo;
} fases;

typedef struct ranking {
    char nome[TAMANHO_NOME];
    int pontos;
    int nivel;
    double tempo;
    struct ranking *prox;
} ranking;

typedef struct jogo {
    jogador jogador;
    bool FimDeJogo;
    int FaseAtual;
    fases fases[4];
    ranking *Head;
} jogo;

#endif