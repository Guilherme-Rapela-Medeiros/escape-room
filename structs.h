#ifndef STRUCTS_H
#define STRUCTS_H

#define TAMANHO_NOME 50

#include <raylib.h>

typefef struct jogador {
    Vector2 posicao;
    int vida;
    int pontuação;
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

#endif