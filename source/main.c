// main.c
#include <stdio.h>
#include <stdlib.h>
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

    // Carrega imagem do menu
    Image menuImg = LoadImage("assets/imagens/menu.inicial.png");
    Texture2D menuTextura = LoadTextureFromImage(menuImg);
    UnloadImage(menuImg);

    Image tutorialImg = LoadImage("assets/imagens/tela.tutorial.png");
    Texture2D tutorialTextura = LoadTextureFromImage(tutorialImg);
    UnloadImage(tutorialImg);


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

    // Ajusta posição do jogador para a fase inicial (caso comecarfase tenha definido)
    EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
    EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;

    // Controle de telas (usa enum TelaAtual em includes/structs.h)
    TelaAtual telaAtual = TELA_MENU;

    // --- Cria botões (em "escada") ---
    const int nBotoes = 4;
    Botao botoes[nBotoes];
    int bw = 320;   // largura do botão
    int bh = 48;    // altura do botão
    int bx = (LARGURA_TELA - bw) / 2;
    int startY = 380;   // ajuste vertical inicial
    int espacamento = 64; // espaçamento entre botões

    botoes[0].rect = (Rectangle){ bx, startY + 0 * espacamento, bw, bh };
    botoes[0].texto = "START GAME";

    botoes[1].rect = (Rectangle){ bx, startY + 1 * espacamento, bw, bh };
    botoes[1].texto = "TUTORIAL";

    botoes[2].rect = (Rectangle){ bx, startY + 2 * espacamento, bw, bh };
    botoes[2].texto = "RANKING";

    botoes[3].rect = (Rectangle){ bx, startY + 3 * espacamento, bw, bh };
    botoes[3].texto = "EXIT GAME";

    for (int i = 0; i < nBotoes; i++) botoes[i].hovered = false;

    int selecionado = 0; // index do botão selecionado por teclado
    // Garante seleção inicial visível
    for (int i = 0; i < nBotoes; i++) botoes[i].hovered = (i == selecionado);

    // Loop principal
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo) {

        // --- AtualIZAÇÃO INPUT / NAVEGAÇÃO DOS BOTOES (apenas quando estiver no MENU) ---
        if (telaAtual == TELA_MENU) {
            // Navegação por teclado
            if (IsKeyPressed(KEY_DOWN)) {
                selecionado = (selecionado + 1) % nBotoes;
            } else if (IsKeyPressed(KEY_UP)) {
                selecionado = (selecionado - 1 + nBotoes) % nBotoes;
            }

            // Hover com mouse e clique
            Vector2 mouse = GetMousePosition();
            bool algumHover = false;
            for (int i = 0; i < nBotoes; i++) {
                if (CheckCollisionPointRec(mouse, botoes[i].rect)) {
                    botoes[i].hovered = true;
                    selecionado = i; // atualiza seleção quando o mouse passa por cima
                    algumHover = true;
                } else {
                    botoes[i].hovered = (i == selecionado); // mantém seleção caso não haja hover
                }
            }

            // Clique do mouse
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

            // Confirmar com ENTER ou clique
            if (IsKeyPressed(KEY_ENTER) || ativouBotao) {
                switch (selecionado) {
                    case 0: // START -> vai para TUTORIAL primeiro (conforme pedido)
                        telaAtual = TELA_TUTORIAL;
                        break;
                    case 1: // TUTORIAL (direto)
                        telaAtual = TELA_TUTORIAL;
                        break;
                    case 2: // RANKING
                        telaAtual = TELA_RANKING;
                        break;
                    case 3: // EXIT GAME
                        EscapeRoom.FimDeJogo = TRUE;
                        break;
                }
            }
        }

        // --- LÓGICA DO JOGO: executada somente em TELA_JOGO ---
        if (telaAtual == TELA_FASES) {
            // Atualizações do jogo (entradas e lógica)
            inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA, 5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);
            atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

            // Verifica se a fase terminou e troca de fase
            if (EscapeRoom.fases[EscapeRoom.FaseAtual].completo) {
                EscapeRoom.FaseAtual++;

                if (EscapeRoom.FaseAtual >= 4) {
                    // Fim do jogo após terminar todas as fases
                    EscapeRoom.FimDeJogo = TRUE;
                } else {
                    // Reseta posição do jogador para a próxima fase
                    EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.x;
                    EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.y;
                }
            }
        }

        // --- DESENHO ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (telaAtual) {

            // TELA MENU
            case TELA_MENU: {
                ClearBackground(BLACK);
                DrawTexture(menuTextura, 0, 0, WHITE);

                // Desenha botões
                for (int i = 0; i < nBotoes; i++) {
                    Color corFundo = botoes[i].hovered ? Fade(SKYBLUE, 0.95f) : Fade(DARKGRAY, 0.8f);
                    Color corBorda = botoes[i].hovered ? BLUE : GRAY;

                    DrawRectangleRec(botoes[i].rect, corFundo);
                    DrawRectangleLinesEx(botoes[i].rect, 2, corBorda);

                    // Centraliza texto
                    int fontSize = 20;
                    int textWidth = MeasureText(botoes[i].texto, fontSize);
                    float tx = botoes[i].rect.x + (botoes[i].rect.width - textWidth) / 2.0f;
                    float ty = botoes[i].rect.y + (botoes[i].rect.height - fontSize) / 2.0f - 2;
                    DrawText(botoes[i].texto, (int)tx, (int)ty, fontSize, BLACK);
                }

                DrawText("Use UP/DOWN para navegar, ENTER para selecionar, clique também funciona", 60, ALTURA_TELA - 50, 14, LIGHTGRAY);
            } break;

            // TELA TUTORIAL
            case TELA_TUTORIAL: {
                BeginDrawing();
                ClearBackground(BLACK);

                // Desenha o PNG da tela de tutorial
                DrawTexture(tutorialTextura, 0, 0, WHITE);

                // Instrução para começar o jogo
                DrawText("APERTE ENTER PARA COMECAR", 180, 750, 24, GREEN);

                // Sair para o menu
                DrawText("BACKSPACE PARA VOLTAR", 220, 780, 20, LIGHTGRAY);

                EndDrawing();

                // Entrar no jogo
                if (IsKeyPressed(KEY_ENTER)) {
                    EscapeRoom.FaseAtual = 0;
                    EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
                    EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
                    telaAtual = TELA_FASES;
                }

                // Voltar ao menu
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    telaAtual = TELA_MENU;
                }
            } break;

            // TELA RANKING (simples; voltar com BACKSPACE)
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

            // TELA_FASES (o JOGO em si)
            case TELA_FASES: {
                ClearBackground(RAYWHITE);

                // Desenha fase atual e HUD (usando suas funções)
                desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

                DrawText(TextFormat("Fase: %d", EscapeRoom.FaseAtual + 1), 10, 10, 20, LIGHTGRAY);
                DrawText(TextFormat("Vida: %d", EscapeRoom.jogador.vida), 10, 40, 20, LIGHTGRAY);
            } break;

            default:
                // Segurança: volta para o menu se estado inválido
                telaAtual = TELA_MENU;
                break;
        } // switch telas

        EndDrawing();
    } // while main loop

    // Cleanup
    
    UnloadTexture(menuTextura);
    UnloadTexture(tutorialTextura);
    acabarFases(EscapeRoom.fases, 4);
    CloseWindow();

    return 0;
}
