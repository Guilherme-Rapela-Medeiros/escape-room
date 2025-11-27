#ifndef FASES_H
#define FASES_H

#include "structs.h" 

void comecarfase(fases *f, int qtd);

void acabarFases(fases *f, int qtd);

void atualizarFases(fases *f, jogador *j, TelaAtual *tela);

void desenharFase(fases *f, jogador *j);

void desenharJogadorManual(jogador *j); 

#endif