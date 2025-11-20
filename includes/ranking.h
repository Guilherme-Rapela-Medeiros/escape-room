#ifndef RANKING_H
#define RANKING_H

#include "structs.h"

// Insere um novo jogador no ranking em ordem crescente de tempo
void inserirRanking(ranking **head, const char *nome, float tempo);

// Salva o ranking em arquivo
void salvarRanking(ranking *head, const char *filename);

// Carrega ranking de arquivo
void carregarRanking(ranking **head, const char *filename);

// Libera memória da lista do ranking
void liberarRanking(ranking **head);

#endif