#ifndef FASES_H
#define FASES_H

#include "structs.h"

void comecarfase(fases fases[], int quantidade);   // inicializa as fases do jogo

void acabarFases(fases fases[], int quantidade);  // finaliza e libera recursos das fases

void atualizarFases(fases *faseAtual, jogador *jogador, TelaAtual *telaAtual);

void desenharFase(fases *faseAtual, jogador *jogador);  // desenha os elementos da fase atual

#endif

//criado o arquivo das fases