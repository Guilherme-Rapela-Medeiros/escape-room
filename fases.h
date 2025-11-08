#ifndef FASES_H
#define FASES_H

#include "structs.h"

void comecarfase(fases fases[], int quantidade);   // inicializa as fases do jogo

void acabarFases(fases fases[], int quantidade);  // inicializa as fases do jogo

void atualizarFases(fases *faseAtual, jogador);  // atualiza os movimentos na fase atual

void desenharFase(fases *faseAtual, jogador *jogador);  // desenha os elementos da fase atual

#endif

//criado o arquivo das fases