#ifndef INPUTS_H
#define INPUTS_H

#include "structs.h"

// Função responsável pelo movimento do jogador
void inputs_jogador_movimento(jogador *jogador, int largura_tela, int altura_tela, int velocidade_jogador, obstaculo *obstaculos);

#endif