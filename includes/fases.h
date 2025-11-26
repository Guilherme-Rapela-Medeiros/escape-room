#ifndef FASES_H
#define FASES_H

#include "structs.h" // Garante que 'fases' e 'jogador' estão definidos

void comecarfase(fases *f, int qtd);

void acabarFases(fases *f, int qtd);

void atualizarFases(fases *f, jogador *j, TelaAtual *tela);

void desenharFase(fases *f, jogador *j);

// *** LINHA ADICIONADA PARA RESOLVER O WARNING ***
void desenharJogadorManual(jogador *j); 

#endif