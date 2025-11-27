#ifndef RANKING_H
#define RANKING_H

#include "structs.h"

void inserirRanking(ranking **head, const char *nome, float tempo);

void salvarRanking(ranking *head, const char *filename);

void carregarRanking(ranking **head, const char *filename);

void liberarRanking(ranking **head);

#endif