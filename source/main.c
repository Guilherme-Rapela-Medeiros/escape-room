#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/inputs.h"
#include "../includes/fases.h"

#define LARGURA_TELA 800
#define ALTURA_TELA 600

int main() {

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");
    SetTargetFPS(60);

    jogo EscapeRoom;

    EscapeRoom.FimDeJogo = false;
    EscapeRoom.FaseAtual = 0;

    // --- Inicializa o jogador ---
    EscapeRoom.jogador.vida = 3;

    // Agora o Rectangle já guarda posição e tamanho
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f, 50, 50
    };

    // --- Inicializa as fases ---
    comecarfase(EscapeRoom.fases, 4);

    // --- Loop principal ---
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo) {

        // Atualiza posição do jogador conforme inputs
        inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA, 5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);

        // Atualiza a fase atual com base no jogador
        atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);
        DrawText("Escape Room", 10, 10, 20, LIGHTGRAY);

        EndDrawing();
    }

    acabarFases(EscapeRoom.fases, 4);
    CloseWindow();

    return 0;
}
