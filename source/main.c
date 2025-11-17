// main.c (corrigido)
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

typedef struct {
    Rectangle rect;
    const char *texto;
    bool hovered;
} Botao;

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");
    SetTargetFPS(60);

    // Carrega imagem do menu (verifica se carregou)
    Image menuImg = LoadImage("assets/imagens/menu_inicial.png");
    Texture2D menuTextura = {0};
    if (menuImg.data == NULL) {
        printf("Aviso: menu_inicial.png nao carregou (verifique o caminho).\n");
    } else {
        menuTextura = LoadTextureFromImage(menuImg);
        UnloadImage(menuImg);
    }

    // Carrega tutorial
    Image tutorialImg = LoadImage("assets/imagens/tela_tutorial.png");
    Texture2D tutorialTextura = {0};
    if (tutorialImg.data == NULL) {
        printf("Aviso: tela_tutorial.png nao carregou (verifique o caminho).\n");
    } else {
        tutorialTextura = LoadTextureFromImage(tutorialImg);
        UnloadImage(tutorialImg);
    }

    // --- Inicializa jogo/estruturas ---
    jogo EscapeRoom;
    EscapeRoom.FimDeJogo = FALSE;
    EscapeRoom.FaseAtual = 0;

    // --- Inicializa o jogador ---
    EscapeRoom.jogador.vida = 3;
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        LARGURA_TELA / 2.0f - 25.0f, ALTURA_TELA / 2.0f - 25.0f, 50, 50
    };

    // --- Inicializa as fases ---
    comecarfase(EscapeRoom.fases, 4);

    // Proteção: garanta que exista ao menos uma fase antes de usar posicaoinicial
    if (EscapeRoom.fases != NULL) {
        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
    }

    // Controle de telas (usa enum TelaAtual em includes/structs.h)
    TelaAtual telaAtual = TELA_MENU;

    // --- Cria botões ---
    const int nBotoes = 4;
    Botao botoes[nBotoes];
    int bw = 320;   // largura do botão
    int bh = 48;    // altura do botão
    int bx = (LARGURA_TELA - bw) / 2;
    int startY = 380;
    int espacamento = 64;

    botoes[0].rect = (Rectangle){ bx, startY + 0 * espacamento, bw, bh };
    botoes[0].texto = "START GAME";
    botoes[1].rect = (Rectangle){ bx, startY + 1 * espacamento, bw, bh };
    botoes[1].texto = "TUTORIAL";
    botoes[2].rect = (Rectangle){ bx, startY + 2 * espacamento, bw, bh };
    botoes[2].texto = "RANKING";
    botoes[3].rect = (Rectangle){ bx, startY + 3 * espacamento, bw, bh };
    botoes[3].texto = "EXIT GAME";

    for (int i = 0; i < nBotoes; i++) botoes[i].hovered = false;
    int selecionado = 0;
    botoes[selecionado].hovered = true;

    // Loop principal
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo) {

        // Atualização de inputs / navegação
        if (telaAtual == TELA_MENU) {
            if (IsKeyPressed(KEY_DOWN)) selecionado = (selecionado + 1) % nBotoes;
            else if (IsKeyPressed(KEY_UP)) selecionado = (selecionado - 1 + nBotoes) % nBotoes;

            Vector2 mouse = GetMousePosition();
            bool algumHover = false;
            for (int i = 0; i < nBotoes; i++) {
                if (CheckCollisionPointRec(mouse, botoes[i].rect)) {
                    botoes[i].hovered = true;
                    selecionado = i;
                    algumHover = true;
                } else {
                    botoes[i].hovered = (i == selecionado);
                }
            }

            bool ativouBotao = false;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < nBotoes; i++) {
                    if (CheckCollisionPointRec(mouse, botoes[i].rect)) {
                        selecionado = i;
                        ativouBotao = true;
                        break;
                    }
                }
            }

            if (IsKeyPressed(KEY_ENTER) || ativouBotao) {
                switch (selecionado) {
                    case 0: telaAtual = TELA_TUTORIAL; break;
                    case 1: telaAtual = TELA_TUTORIAL; break;
                    case 2: telaAtual = TELA_RANKING; break;
                    case 3: EscapeRoom.FimDeJogo = TRUE; break;
                }
            }
        }

        // Lógica do jogo somente em TELA_FASES
        if (telaAtual == TELA_FASES) {
            // segurança: checar limites de fases
            if (EscapeRoom.FaseAtual < 0) EscapeRoom.FaseAtual = 0;
            if (EscapeRoom.FaseAtual >= 4) EscapeRoom.FimDeJogo = TRUE;
            else {
                inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA, 5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);
                atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

                if (EscapeRoom.fases[EscapeRoom.FaseAtual].completo) {
                    EscapeRoom.FaseAtual++;
                    if (EscapeRoom.FaseAtual >= 4) EscapeRoom.FimDeJogo = TRUE;
                    else {
                        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.x;
                        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.y;
                    }
                }
            }
        }

        // --- DESENHO: apenas UM BeginDrawing / EndDrawing por frame ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (telaAtual) {
            case TELA_MENU: {
                ClearBackground(BLACK);
                if (menuTextura.id != 0) DrawTexture(menuTextura, 0, 0, WHITE);

                for (int i = 0; i < nBotoes; i++) {
                    Color corFundo = botoes[i].hovered ? Fade(SKYBLUE, 0.95f) : Fade(DARKGRAY, 0.8f);
                    Color corBorda = botoes[i].hovered ? BLUE : GRAY;
                    DrawRectangleRec(botoes[i].rect, corFundo);
                    DrawRectangleLinesEx(botoes[i].rect, 2, corBorda);

                    int fontSize = 20;
                    int textWidth = MeasureText(botoes[i].texto, fontSize);
                    float tx = botoes[i].rect.x + (botoes[i].rect.width - textWidth) / 2.0f;
                    float ty = botoes[i].rect.y + (botoes[i].rect.height - fontSize) / 2.0f - 2;
                    DrawText(botoes[i].texto, (int)tx, (int)ty, fontSize, BLACK);
                }

                DrawText("Use UP/DOWN para navegar, ENTER para selecionar, clique também funciona", 60, ALTURA_TELA - 50, 14, LIGHTGRAY);
            } break;

            case TELA_TUTORIAL: {
                ClearBackground(BLACK);
                if (tutorialTextura.id != 0) DrawTexture(tutorialTextura, 0, 0, WHITE);
                DrawText("APERTE ENTER PARA COMECAR", 180, 750, 24, GREEN);
                DrawText("BACKSPACE PARA VOLTAR", 220, 780, 20, LIGHTGRAY);

                // Entradas para trocar de tela (coloque fora do Begin/End se preferir; aqui é ok)
                if (IsKeyPressed(KEY_ENTER)) {
                    EscapeRoom.FaseAtual = 0;
                    EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
                    EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
                    telaAtual = TELA_FASES;
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    telaAtual = TELA_MENU;
                }
            } break;

            case TELA_RANKING: {
                ClearBackground(BLACK);
                DrawText("RANKING (exemplo)", 220, 80, 36, YELLOW);
                DrawText("1) JogadorA - 1000", 200, 180, 22, WHITE);
                DrawText("2) JogadorB - 800", 200, 220, 22, WHITE);
                DrawText("APERTE BACKSPACE PARA VOLTAR", 180, 560, 20, LIGHTGRAY);

                if (IsKeyPressed(KEY_BACKSPACE)) {
                    telaAtual = TELA_MENU;
                }
            } break;

            case TELA_FASES: {
                ClearBackground(RAYWHITE);
                // Proteção: checar limites antes de desenhar a fase
                if (EscapeRoom.FaseAtual >= 0 && EscapeRoom.FaseAtual < 4) {
                    desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);
                    DrawText(TextFormat("Fase: %d", EscapeRoom.FaseAtual + 1), 10, 10, 20, LIGHTGRAY);
                    DrawText(TextFormat("Vida: %d", EscapeRoom.jogador.vida), 10, 40, 20, LIGHTGRAY);
                } else {
                    DrawText("Erro: Fase invalida", 10, 10, 20, RED);
                }
            } break;

            default:
                telaAtual = TELA_MENU;
                break;
        }

        EndDrawing();
    } // while

    // Cleanup
    if (menuTextura.id != 0) UnloadTexture(menuTextura);
    if (tutorialTextura.id != 0) UnloadTexture(tutorialTextura);

    acabarFases(EscapeRoom.fases, 4);
    CloseWindow();

    return 0;
}
