#include "raylib.h"
#include "structs.h"
#include "inputs.h"    // inclui todos os arquivos de cabeçalho necessários
#include "fases.h"

#define LARGURA_TELA 800   // define a largura e a altura da tela
#define ALTURA_TELA 600

int main(){

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");  // inicializa a janela do jogo

    SetTargetFPS(60);   // define a taxa de frames por segundo e o máximo que ele pode chegar

    jogo EscapeRoom;   // cria uma variável do tipo jogo com nosso titulo

    EscapeRoom.FimDeJogo = false;   // define o estado inicial do jogo como não terminado

    EscapeRoom.FaseAtual = 0;   // define a fase atual como a primeira fase

    EscapeRoom.jogador.posicao = (Vector2){LARGURA_TELA /2, ALTURA_TELA /2};  // define a posição inicial do jogador no centro da tela

    EscapeRoom.jogador.vida = 3;       // define a vida inicial do jogador

    comecarfase(EscapeRoom.fases, 4);   // inicializa as fases do jogo


    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo){   // loop principal do jogo

        inputs_jogador_movimento(&(Rectangle){EscapeRoom.jogador.posicao.x, EscapeRoom.jogador.posicao.y, 50, 50}, LARGURA_TELA, ALTURA_TELA, 5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);  // chama a função para processar os inputs do jogador

        atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);  // atualiza a fase atual com base nos inputs do jogador

        BeginDrawing();   // inicia o desenho na tela
        ClearBackground(RAYWHITE);   // limpa a tela com a cor branca

        desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);  // desenha a fase atual e o jogador

        DrawText("Escape Room", 10, 10, 20, LIGHTGRAY); // desenha o título do jogo na tela
        EndDrawing();   // finaliza o desenho na tela
    }

    // limpa recursos e fecha a janela após o loop principal
    acabarFases(EscapeRoom.fases, 4);  // finaliza e libera recursos das fases
    CloseWindow(); // fecha a janela do jogo

    return 0;
}