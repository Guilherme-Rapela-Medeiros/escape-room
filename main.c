#include "raylib.h"
#include "structs.h"
#include "inputs.h"    // inclui todos os arquivos de cabeçalho necessários
#include "fases.h"

#define LARGURA_TELA 800   // define a largura e a altura da tela
#define ALTURA_TELA 600

int main(){

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");  // inicializa a janela do jogo

    SetTargetFPS(60);   // define a taxa de frames por segundo e o máximo que ele pode chegar

    jogo EscapeRoom;   // cria uma variável do tipo jogo com nosso título

    EscapeRoom.FimDeJogo = false;   // define o estado inicial do jogo como não terminado
    EscapeRoom.FaseAtual = 0;       // define a fase atual como a primeira fase

    // --- Inicializa o jogador ---
    EscapeRoom.jogador.posicao = (Vector2){LARGURA_TELA / 2, ALTURA_TELA / 2};  // posição inicial
    EscapeRoom.jogador.vida = 3;       // vida inicial

    // --- Define o tamanho da hitbox do jogador ---
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        EscapeRoom.jogador.posicao.x, 
        EscapeRoom.jogador.posicao.y, 
        50, 50   // largura e altura do jogador
    };

    // --- Inicializa as fases ---
    comecarfase(EscapeRoom.fases, 4);   // inicializa as fases do jogo

    // --- Loop principal do jogo ---
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo){   

        // Atualiza posição do jogador conforme inputs
        inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA, 5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);

        // Atualiza a fase atual com base no estado do jogador
        atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

        // Atualiza a hitbox com base na posição atual 
        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.jogador.posicao.x;
        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.jogador.posicao.y;

        // Desenho
        BeginDrawing();
        ClearBackground(RAYWHITE);

        desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);
        DrawText("Escape Room", 10, 10, 20, LIGHTGRAY);

        EndDrawing();
    }

    // --- Finalização ---
    acabarFases(EscapeRoom.fases, 4);  // finaliza e libera recursos das fases
    CloseWindow(); // fecha a janela do jogo

    return 0;
}
