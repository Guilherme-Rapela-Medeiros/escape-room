#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/inputs.h"
#include "../includes/fases.h"

#define LARGURA_TELA 800
#define ALTURA_TELA 600

int main() {

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");
    SetTargetFPS(60);

    // Carrega imagem do menu
    Image menuImg = LoadImage("../assets/imagens/menu.inicial.png");
    Texture2D menuTextura = LoadTextureFromImage(menuImg);
    UnloadImage(menuImg);

    int menuAtivo = TRUE;

    jogo EscapeRoom;

    EscapeRoom.FimDeJogo = FALSE;
    EscapeRoom.FaseAtual = 0;

    // --- Inicializa o jogador ---
    EscapeRoom.jogador.vida = 3;
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f, 50, 50
    };
    EscapeRoom.jogador.vel_x = 0.0f;
    EscapeRoom.jogador.vel_y = 0.0f;
    EscapeRoom.jogador.grounded = FALSE;

    // --- Inicializa as fases ---
    comecarfase(EscapeRoom.fases, 4);

    // --- Loop principal ---
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo) {

        // Menu
        if (menuAtivo) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(menuTextura, 0, 0, WHITE);
            DrawText("Pressione ENTER para começar", 200, 550, 20, WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                menuAtivo = FALSE;
            }
        } else {
            // Jogo
            inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA, 5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);
            atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

            BeginDrawing();
            ClearBackground(RAYWHITE);
            desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);
            DrawText("Escape Room", 10, 10, 20, LIGHTGRAY);
            EndDrawing();
        }
    }

    UnloadTexture(menuTextura);
    acabarFases(EscapeRoom.fases, 4);
    CloseWindow();

    return 0;
}
