// main.c (completo e atualizado)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/inputs.h"
#include "../includes/fases.h"
#include "../includes/mapa.h"

#define LARGURA_TELA 756
#define ALTURA_TELA 817
#define TOTAL_FASES 4

typedef struct {
    Rectangle rect;
    const char *texto;
    bool hovered;
} Botao;

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");
    SetTargetFPS(60);

    // --- Carrega imagens ---
    Image menuImg = LoadImage("assets/imagens/menu.inicial.png");
    Texture2D menuTextura = {0};
    if (menuImg.data != NULL) {
        menuTextura = LoadTextureFromImage(menuImg);
        UnloadImage(menuImg);
    }

    Image tutorialImg = LoadImage("assets/imagens/tela.tutorial.png");
    Texture2D tutorialTextura = {0};
    if (tutorialImg.data != NULL) {
        tutorialTextura = LoadTextureFromImage(tutorialImg);
        UnloadImage(tutorialImg);
    }

    Image iniciar1Img = LoadImage("assets/imagens/iniciar.fase1.png");
    Texture2D iniciar1Textura = {0};
    if (iniciar1Img.data != NULL) {
        iniciar1Textura = LoadTextureFromImage(iniciar1Img);
        UnloadImage(iniciar1Img);
    }

    Image iniciar2Img = LoadImage("assets/imagens/iniciar.fase2.png");
    Texture2D iniciar2Textura = {0};
    if (iniciar2Img.data != NULL) {
        iniciar2Textura = LoadTextureFromImage(iniciar2Img);
        UnloadImage(iniciar2Img);
    }

    Image iniciar3Img = LoadImage("assets/imagens/iniciar.fase3.png");
    Texture2D iniciar3Textura = {0};
    if (iniciar3Img.data != NULL) {
        iniciar3Textura = LoadTextureFromImage(iniciar3Img);
        UnloadImage(iniciar3Img);
    }

    Image iniciar4Img = LoadImage("assets/imagens/iniciar.fase4.png");
    Texture2D iniciar4Textura = {0};
    if (iniciar4Img.data != NULL) {
        iniciar4Textura = LoadTextureFromImage(iniciar4Img);
        UnloadImage(iniciar4Img);
    }

    // --- Estruturas principais ---
    jogo EscapeRoom;
    EscapeRoom.FimDeJogo = FALSE;
    EscapeRoom.FaseAtual = 0;

    // jogador
    EscapeRoom.jogador.vida = 3;
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        LARGURA_TELA / 2.0f - 25, ALTURA_TELA / 2.0f - 25, 50, 50
    };

    // inicializa fases
    comecarfase(EscapeRoom.fases, TOTAL_FASES);

    if (EscapeRoom.fases != NULL) {
        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
    }

    TelaAtual telaAtual = TELA_MENU;
    bool telaJustChanged = false;

    // --- Botões do menu ---
    const int nBotoes = 4;
    Botao botoes[nBotoes];

    int bw = 320;
    int bh = 48;
    int bx = (LARGURA_TELA - bw) / 2;
    int startY = 380;
    int espacamento = 64;

    botoes[0] = (Botao){ (Rectangle){bx, startY + 0*espacamento, bw, bh}, "START GAME", false };
    botoes[1] = (Botao){ (Rectangle){bx, startY + 1*espacamento, bw, bh}, "TUTORIAL", false };
    botoes[2] = (Botao){ (Rectangle){bx, startY + 2*espacamento, bw, bh}, "RANKING", false };
    botoes[3] = (Botao){ (Rectangle){bx, startY + 3*espacamento, bw, bh}, "EXIT GAME", false };

    int selecionado = 0;
    botoes[0].hovered = true;

    // =============================
    //        LOOP PRINCIPAL
    // =============================
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo) {

        // ============ MENU ============
        if (telaAtual == TELA_MENU) {

            if (IsKeyPressed(KEY_DOWN)) selecionado = (selecionado + 1) % nBotoes;
            else if (IsKeyPressed(KEY_UP)) selecionado = (selecionado - 1 + nBotoes) % nBotoes;

            Vector2 mouse = GetMousePosition();

            for (int i = 0; i < nBotoes; i++) {
                if (CheckCollisionPointRec(mouse, botoes[i].rect)) {
                    botoes[i].hovered = true;
                    selecionado = i;
                } else {
                    botoes[i].hovered = (i == selecionado);
                }
            }

            bool ativou = false;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < nBotoes; i++) {
                    if (CheckCollisionPointRec(mouse, botoes[i].rect)) {
                        selecionado = i;
                        ativou = true;
                        break;
                    }
                }
            }

            if (IsKeyPressed(KEY_ENTER) || ativou) {
                switch (selecionado) {

                    case 0: // START GAME
                        EscapeRoom.FaseAtual = 0;
                        telaAtual = TELA_JOGO; // tela "iniciar fase"
                        telaJustChanged = true;
                        break;

                    case 1: // TUTORIAL
                        telaAtual = TELA_TUTORIAL;
                        telaJustChanged = true;
                        break;

                    case 2: // RANKING
                        telaAtual = TELA_RANKING;
                        telaJustChanged = true;
                        break;

                    case 3: // EXIT
                        EscapeRoom.FimDeJogo = TRUE;
                        break;
                }
            }
        }

        // ============ TELA "INICIAR FASE" ============
        if (telaAtual == TELA_JOGO) {

            Vector2 mouse = GetMousePosition();
            Rectangle btn = { 238, 500, 280, 70 };
            bool hover = CheckCollisionPointRec(mouse, btn);

            bool clicou = false;

            if (!telaJustChanged) {
                if ((hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
                    clicou = true;
            }

            if (clicou) {
                telaAtual = TELA_FASES;
                EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.x;
                EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.y;
                telaJustChanged = true;
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                telaAtual = TELA_MENU;
                telaJustChanged = true;
            }
        }

        // ============ TUTORIAL ============
        if (telaAtual == TELA_TUTORIAL) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                telaAtual = TELA_MENU;
                telaJustChanged = true;
            }
        }

        // ============ RANKING ============
        if (telaAtual == TELA_RANKING) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                telaAtual = TELA_MENU;
                telaJustChanged = true;
            }
        }

        // ============ FASE ATIVA ============
        if (telaAtual == TELA_FASES) {

            if (EscapeRoom.FaseAtual < TOTAL_FASES) {
                inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA,
                                         5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);

                atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

                if (EscapeRoom.fases[EscapeRoom.FaseAtual].completo) {
                    EscapeRoom.FaseAtual++;
                    if (EscapeRoom.FaseAtual >= TOTAL_FASES) {
                        EscapeRoom.FimDeJogo = TRUE;
                    } else {
                        telaAtual = TELA_JOGO;
                        telaJustChanged = true;
                    }
                }
            }
        }

        // =============================
        //          DESENHO
        // =============================
        BeginDrawing();
        ClearBackground(BLACK);

        switch (telaAtual) {

            // -------- MENU --------
            case TELA_MENU: {
                if (menuTextura.id != 0) DrawTexture(menuTextura, 0, 0, WHITE);

                for (int i = 0; i < nBotoes; i++) {

                    Color corFundo = botoes[i].hovered ? Fade(SKYBLUE, 0.95f) : Fade(DARKGRAY, 0.8f);
                    Color corBorda = botoes[i].hovered ? BLUE : GRAY;

                    DrawRectangleRec(botoes[i].rect, corFundo);
                    DrawRectangleLinesEx(botoes[i].rect, 2, corBorda);

                    int fontSize = 20;
                    int tw = MeasureText(botoes[i].texto, fontSize);
                    float tx = botoes[i].rect.x + (botoes[i].rect.width - tw)/2;
                    float ty = botoes[i].rect.y + (botoes[i].rect.height - fontSize)/2 - 2;
                    DrawText(botoes[i].texto, tx, ty, fontSize, BLACK);
                }

                DrawText("Use UP/DOWN para navegar, ENTER para selecionar",
                         60, ALTURA_TELA - 50, 14, LIGHTGRAY);

            } break;

            // -------- TUTORIAL --------
            case TELA_TUTORIAL: {
                if (tutorialTextura.id != 0) DrawTexture(tutorialTextura, 0, 0, WHITE);
                DrawText("TUTORIAL - APERTE BACKSPACE PARA VOLTAR",
                         120, ALTURA_TELA - 40, 18, LIGHTGRAY);
            } break;

            // -------- INICIAR FASE --------
            case TELA_JOGO: {
                if (iniciar4Textura.id != 0) DrawTexture(iniciar4Textura, 0, 0, WHITE);

                int faseMostrada = EscapeRoom.FaseAtual + 1;

                DrawText(TextFormat("FASE %d", faseMostrada), 300, 200, 48, YELLOW);

                // Botão começar
                Rectangle btn = { 238, 500, 280, 70 };
                Vector2 mouse = GetMousePosition();
                int hover = CheckCollisionPointRec(mouse, btn);

                Color corFundo = hover ? Fade(GREEN, 0.9f) : Fade(DARKGREEN, 0.8f);
                Color corBorda = hover ? YELLOW : BLACK;

                DrawRectangleRec(btn, corFundo);
                DrawRectangleLinesEx(btn, 3, corBorda);

                int tf = 30;
                int tw = MeasureText("COMEÇAR", tf);

                DrawText("COMEÇAR", btn.x + (btn.width - tw)/2, btn.y + 18, tf, WHITE);

                DrawText("BACKSPACE PARA VOLTAR AO MENU",
                         170, 770, 18, LIGHTGRAY);

            } break;

            // -------- RANKING --------
            case TELA_RANKING: {
                DrawText("RANKING (exemplo)", 220, 80, 36, YELLOW);
                DrawText("1) JogadorA - 1000", 200, 180, 22, WHITE);
                DrawText("2) JogadorB - 800", 200, 220, 22, WHITE);
                DrawText("APERTE BACKSPACE PARA VOLTAR", 180, 560, 20, LIGHTGRAY);
            } break;

            // -------- FASE ATIVA --------
            case TELA_FASES: {
                desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

                DrawText(TextFormat("Fase: %d", EscapeRoom.FaseAtual+1),
                         10, 10, 20, LIGHTGRAY);
                DrawText(TextFormat("Vida: %d", EscapeRoom.jogador.vida),
                         10, 40, 20, LIGHTGRAY);
            } break;
        }

        EndDrawing();

        telaJustChanged = false;
    }

    // cleanup
    if (menuTextura.id != 0) UnloadTexture(menuTextura);
    if (tutorialTextura.id != 0) UnloadTexture(tutorialTextura);
    if (iniciar4Textura.id != 0) UnloadTexture(iniciar4Textura);

    acabarFases(EscapeRoom.fases, TOTAL_FASES);
    CloseWindow();
    return 0;
}
