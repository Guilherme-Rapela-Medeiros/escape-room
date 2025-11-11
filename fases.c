#include <stdlib.h>
#include <string.h>
#include "fases.h"
#include "raylib.h"

// Tamanho padrão do jogador (convenção usada no projeto)
#define JOGADOR_LARGURA 50
#define JOGADOR_ALTURA 50

// Inicializa fases com alguns obstáculos e um portal de saída
void comecarfase(fases fasesArray[], int quantidade){
	if (!fasesArray) return;

	for (int i = 0; i < quantidade; i++){
		fasesArray[i].numero = i + 1;
		// aumenta dificuldade com o número da fase
		fasesArray[i].quantidadeObstaculos = 2 + i; 
		fasesArray[i].obstaculos = malloc(sizeof(obstaculo) * fasesArray[i].quantidadeObstaculos);
		if (!fasesArray[i].obstaculos) {
			fasesArray[i].quantidadeObstaculos = 0;
			continue;
		}

		// configura posição inicial do jogador nesta fase (centro da tela)
		fasesArray[i].posicaoinicial = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

		// cria alguns obstáculos simples
		for (int j = 0; j < fasesArray[i].quantidadeObstaculos; j++){
			obstaculo *o = &fasesArray[i].obstaculos[j];
			o->tamanho = (Vector2){ 40.0f + 10.0f * j, 40.0f };
			// espalha os obstáculos pela tela
			o->posicao = (Vector2){ 50.0f + 120.0f * j + 20.0f * i, 50.0f + 80.0f * j };
			// velocidade simples para mover horizontalmente
			o->velocidade = (Vector2){ 1.0f + i * 0.2f + j * 0.1f, 0.0f };
			o->ativo = TRUE;
		}

		// define portal de saída; por padrão, ativo somente na última fase
		fasesArray[i].saida.posicao = (Vector2){ GetScreenWidth() - 80.0f, GetScreenHeight() / 2.0f };
		fasesArray[i].saida.tamanho = (Vector2){ 60.0f, 80.0f };
		fasesArray[i].saida.ativo = (i == quantidade - 1) ? TRUE : TRUE; // deixa ativo para permitir teste

		fasesArray[i].completo = FALSE;
	}
}

// libera recursos alocados nas fases
void acabarFases(fases fasesArray[], int quantidade){
	if (!fasesArray) return;
	for (int i = 0; i < quantidade; i++){
		if (fasesArray[i].obstaculos){
			free(fasesArray[i].obstaculos);
			fasesArray[i].obstaculos = NULL;
			fasesArray[i].quantidadeObstaculos = 0;
		}
	}
}

// verifica interseção entre dois retângulos
static int rectsColidem(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh){
	return (ax < bx + bw) && (ax + aw > bx) && (ay < by + bh) && (ay + ah > by);
}

// atualiza posição dos obstáculos e detecta colisões com o jogador e portal
void atualizarFases(fases *faseAtual, jogador *jogador){
	if (!faseAtual || !jogador) return;

	int largura = GetScreenWidth();
	int altura = GetScreenHeight();

	Rectangle rJogador = { jogador->posicao.x, jogador->posicao.y, JOGADOR_LARGURA, JOGADOR_ALTURA };

	// atualizar cada obstáculo
	for (int i = 0; i < faseAtual->quantidadeObstaculos; i++){
		obstaculo *o = &faseAtual->obstaculos[i];
		if (!o->ativo) continue;

		// movimenta
		o->posicao.x += o->velocidade.x;
		o->posicao.y += o->velocidade.y;

		// rebater nas bordas da tela
		if (o->posicao.x < 0 || o->posicao.x + o->tamanho.x > largura) o->velocidade.x *= -1;
		if (o->posicao.y < 0 || o->posicao.y + o->tamanho.y > altura) o->velocidade.y *= -1;

		// checar colisão com o jogador
		if (rectsColidem(o->posicao.x, o->posicao.y, o->tamanho.x, o->tamanho.y,
						 rJogador.x, rJogador.y, rJogador.width, rJogador.height)){
			// dano simples: reduz vida e desativa obstáculo temporariamente
			jogador->vida -= 1;
			o->ativo = FALSE;
		}
	}

	// checar colisão com o portal
	portal *p = &faseAtual->saida;
	if (p->ativo){
		if (rectsColidem(p->posicao.x, p->posicao.y, p->tamanho.x, p->tamanho.y,
						 rJogador.x, rJogador.y, rJogador.width, rJogador.height)){
			faseAtual->completo = TRUE;
			p->ativo = FALSE;
		}
	}
}

// desenha a fase atual: obstáculos, portal e jogador
void desenharFase(fases *faseAtual, jogador *jogador){
	if (!faseAtual || !jogador) return;

	// desenha obstáculos
	for (int i = 0; i < faseAtual->quantidadeObstaculos; i++){
		obstaculo *o = &faseAtual->obstaculos[i];
		if (!o->ativo) continue;
		DrawRectangleRec((Rectangle){ o->posicao.x, o->posicao.y, o->tamanho.x, o->tamanho.y }, RED);
	}

	// desenha portal
	if (faseAtual->saida.ativo){
		DrawRectangleRec((Rectangle){ faseAtual->saida.posicao.x, faseAtual->saida.posicao.y, faseAtual->saida.tamanho.x, faseAtual->saida.tamanho.y }, BLUE);
	}

	// desenha jogador
	DrawRectangle((int)jogador->posicao.x, (int)jogador->posicao.y, JOGADOR_LARGURA, JOGADOR_ALTURA, GREEN);

	// HUD simples
	char vidaText[32];
	sprintf(vidaText, "Vida: %d", jogador->vida);
	DrawText(vidaText, 10, 40, 20, BLACK);
}
