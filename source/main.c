// main.c (completo, com tempo acumulativo e ranking TXT top 5)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/inputs.h"
#include "../includes/fases.h"
#include "../includes/mapa.h"
#include "../includes/ranking.h" // assume ranking.h está em includes

#define LARGURA_TELA 756
#define ALTURA_TELA 817
#define TOTAL_FASES 4
#define ARQUIVO_RANKING "ranking.txt"
#define TOP_N 5

typedef struct {
    Rectangle rect;
    const char *texto;
    bool hovered;
} Botao;

/* formata tempo float (segundos) para MM:SS.mmm */
static void formatar_tempo(float tempo, char *out, size_t outlen) {
    if (!out) return;
    int minutos = (int)(tempo / 60.0f);
    float restante = tempo - (minutos * 60);
    int segundos = (int)restante;
    int miles = (int)((restante - segundos) * 1000.0f + 0.5f);
    if (miles >= 1000) { miles = 0; segundos += 1; }
    if (segundos >= 60) { segundos = 0; minutos += 1; }
    snprintf(out, outlen, "%02d:%02d.%03d", minutos, segundos, miles);
}

/* Mantém somente os TOP_N nós (libera o resto) */
static void manter_top_n(ranking **head, int n) {
    if (!head || !*head) return;
    ranking *atual = *head;
    int count = 1;
    while (atual && atual->prox) {
        if (count == n) {
            /* libera o resto */
            ranking *rem = atual->prox;
            atual->prox = NULL;
            while (rem) {
                ranking *nx = rem->prox;
                free(rem);
                rem = nx;
            }
            return;
        }
        atual = atual->prox;
        count++;
    }
}

/* Renderiza o ranking na tela (até TOP_N) */
static void desenhar_ranking_na_tela(ranking *head) {
    int y = 140;
    int i = 1;
    ranking *it = head;
    while (it && i <= TOP_N) {
        char buf[128];
        formatar_tempo(it->tempo, buf, sizeof(buf));
        DrawText(TextFormat("%d) %s  -  %s", i, it->nome[0] ? it->nome : "(sem nome)", buf), 120, y, 20, WHITE);
        it = it->prox;
        y += 30;
        i++;
    }
}

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Escape Room");
    SetTargetFPS(60);

    // =============================
    // SISTEMA DE TEMPO
    // =============================
    float tempoTotal = 0.0f;     // acumulado entre todas as fases
    bool tempoRodando = false;   // true apenas em TELA_FASES quando contagem ativa

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

    Texture2D iniciarTexturas[4] = {iniciar1Textura, iniciar2Textura, iniciar3Textura, iniciar4Textura};

    // --- Estruturas principais ---
    jogo EscapeRoom;
    EscapeRoom.FimDeJogo = FALSE;
    EscapeRoom.FaseAtual = 0;

    // jogador
    EscapeRoom.jogador.vida = 3;
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        LARGURA_TELA / 2.0f - 25, ALTURA_TELA / 2.0f - 25, 50, 50
    };

    // ---------------------------------------
    // CARREGAR SPRITES DO JOGADOR
    // ---------------------------------------
    // Carregar sprites do jogador na ordem correta do enum
    EscapeRoom.jogador.sprites[SPRITE_PARADO]   = LoadTexture("assets/player/jogador_sprite_parado.png");
    EscapeRoom.jogador.sprites[SPRITE_ESQUERDA] = LoadTexture("assets/player/jogador_sprite_esquerda.png");
    EscapeRoom.jogador.sprites[SPRITE_DIREITA]  = LoadTexture("assets/player/jogador_sprite_direita.png");
    EscapeRoom.jogador.sprites[SPRITE_CIMA]     = LoadTexture("assets/player/jogador_sprite_cima.png");
    EscapeRoom.jogador.sprites[SPRITE_BAIXO]    = LoadTexture("assets/player/jogador_sprite_baixo.png");

    // Define direção inicial parado
    EscapeRoom.jogador.sprite_atual = SPRITE_PARADO;

    // Checar se algum arquivo não carregou
    for (int i = 0; i < 5; i++) {
        if (EscapeRoom.jogador.sprites[i].id == 0) {
            printf("ERRO: textura do jogador %d nao carregou!\n", i);
        }
    }


    // inicializa fases
    comecarfase(EscapeRoom.fases, TOTAL_FASES);

    if (EscapeRoom.fases != NULL) {
        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
    }

    TelaAtual telaAtual = TELA_MENU;   // controla a tela atual (local, passado para funções)
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

    // --- Ranking: carrega do arquivo (lista ligada) ---
    ranking *rankHead = NULL;
    carregarRanking(&rankHead, ARQUIVO_RANKING);
    // garante que só tenha TOP_N logo no início
    manter_top_n(&rankHead, TOP_N);

    // --- Variáveis para input de nome (TELA_FINAL) ---
    char nomeInput[TAMANHO_NOME] = {0};
    int nomeLen = 0;
    bool finalNomeConfirmado = false;

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
                        // zera tempo para novo jogo
                        tempoTotal = 0.0f;
                        tempoRodando = false;
                        telaAtual = TELA_JOGO; // tela "iniciar fase"
                        telaJustChanged = true;
                        break;

                    case 1: // TUTORIAL
                        telaAtual = TELA_TUTORIAL;
                        telaJustChanged = true;
                        break;

                    case 2: // RANKING -> mostra ranking carregado
                        // (já carregado no início; recarrega para garantir atualidade)
                        liberarRanking(&rankHead);
                        rankHead = NULL;
                        carregarRanking(&rankHead, ARQUIVO_RANKING);
                        manter_top_n(&rankHead, TOP_N);
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
                // COMEÇA A CONTAGEM DO TEMPO (retoma)
                tempoRodando = true;
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

            // acumula tempo apenas enquanto estiver em TELA_FASES
            if (tempoRodando) {
                tempoTotal += GetFrameTime();
            }

            if (EscapeRoom.FaseAtual < TOTAL_FASES) {
                inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA,
                                         5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);

                // *** CHAMADA CORRIGIDA: passa &telaAtual (variavel local) ***
                atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador, &telaAtual);

                if (EscapeRoom.fases[EscapeRoom.FaseAtual].completo) {
                    // pausa o tempo ao tocar no portal
                    tempoRodando = false;

                    EscapeRoom.FaseAtual++;
                    if (EscapeRoom.FaseAtual >= TOTAL_FASES) {
                        EscapeRoom.FaseAtual = TOTAL_FASES;
                        // fim do jogo: vai para TELA_FINAL e pede nome
                        telaAtual = TELA_FINAL;
                        telaJustChanged = true;
                        // prepara input
                        nomeInput[0] = '\0';
                        nomeLen = 0;
                        finalNomeConfirmado = false;
                    } else {
                        telaAtual = TELA_JOGO;
                        telaJustChanged = true;
                    }
                }
            }
        }

        // ============ TELA GAME OVER ============
        if (telaAtual == TELA_GAME_OVER) {
            // pausa o tempo por segurança
            tempoRodando = false;

            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("GAME OVER", 260, 120, 48, RED);
            DrawText("Voce ficou sem vidas.", 250, 200, 22, LIGHTGRAY);

            // Botões
            Rectangle btnMenu = { 238, 420, 200, 60 };
            Rectangle btnRetry = { 338, 420, 200, 60 };
            Vector2 mouse = GetMousePosition();

            bool hoverMenu = CheckCollisionPointRec(mouse, btnMenu);
            bool hoverRetry = CheckCollisionPointRec(mouse, btnRetry);

            Color corFundoMenu = hoverMenu ? Fade(SKYBLUE, 0.95f) : Fade(DARKGRAY, 0.8f);
            Color corFundoRetry = hoverRetry ? Fade(GREEN, 0.95f) : Fade(DARKGREEN, 0.8f);

            DrawRectangleRec(btnMenu, corFundoMenu);
            DrawRectangleLinesEx(btnMenu, 2, WHITE);
            DrawText("VOLTAR AO MENU", btnMenu.x + 16, btnMenu.y + 18, 20, BLACK);

            DrawRectangleRec(btnRetry, corFundoRetry);
            DrawRectangleLinesEx(btnRetry, 2, WHITE);
            DrawText("JOGAR NOVAMENTE", btnRetry.x + 12, btnRetry.y + 18, 20, BLACK);

            // impede clique instantâneo
            if (!telaJustChanged) {
                if ((hoverMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_M)) {
                    // Voltar ao menu: reset parcial
                    telaAtual = TELA_MENU;
                    telaJustChanged = true;
                    EscapeRoom.FaseAtual = 0;
                    EscapeRoom.jogador.vida = 3;
                    tempoTotal = 0.0f;
                    tempoRodando = false;
                    if (EscapeRoom.fases != NULL) {
                        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
                        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
                    }
                }
                if ((hoverRetry && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
                    // Jogar novamente
                    EscapeRoom.FaseAtual = 0;
                    EscapeRoom.jogador.vida = 3;
                    tempoTotal = 0.0f;
                    tempoRodando = false;
                    if (EscapeRoom.fases != NULL) {
                        EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[0].posicaoinicial.x;
                        EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[0].posicaoinicial.y;
                    }
                    telaAtual = TELA_JOGO;
                    telaJustChanged = true;
                }
            }

            EndDrawing();
            // skip the general drawing below since we already handled this frame
            telaJustChanged = false;
            continue;
        }

        // ============ TELA_FINAL (input nome e salvar) ============
        if (telaAtual == TELA_FINAL) {
            // input de texto (GetCharPressed)
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && nomeLen < TAMANHO_NOME - 1) {
                    nomeInput[nomeLen++] = (char)key;
                    nomeInput[nomeLen] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && nomeLen > 0) {
                nomeLen--;
                nomeInput[nomeLen] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                // confirma nome e salva no ranking
                if (nomeLen == 0) {
                    // nome vazio -> usa "(anonimo)"
                    strncpy(nomeInput, "(anonimo)", TAMANHO_NOME - 1);
                    nomeInput[TAMANHO_NOME - 1] = '\0';
                    nomeLen = (int)strlen(nomeInput);
                }
                // inserir ordenado pelo tempo
                inserirRanking(&rankHead, nomeInput, tempoTotal);
                // manter top N
                manter_top_n(&rankHead, TOP_N);
                // salvar em arquivo
                salvarRanking(rankHead, ARQUIVO_RANKING);
                finalNomeConfirmado = true;
            }

            // se confirmado, após salvar, mostrar ranking automaticamente
            if (finalNomeConfirmado) {
                // recarrega do arquivo para garantir consistência
                liberarRanking(&rankHead);
                rankHead = NULL;
                carregarRanking(&rankHead, ARQUIVO_RANKING);
                manter_top_n(&rankHead, TOP_N);
                telaAtual = TELA_RANKING;
                telaJustChanged = true;
            }
        }

        // =============================
        //          DESENHO (padrão)
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

                int fase = EscapeRoom.FaseAtual;
                if (fase >= 0 && fase < TOTAL_FASES && iniciarTexturas[fase].id != 0){
                    DrawTexture(iniciarTexturas[fase], 0, 0, WHITE);
                }

                int faseMostrada = EscapeRoom.FaseAtual + 1;

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

                // Mostrar tempo acumulado também na tela "iniciar fase" (formatado)
                char bufTempo[32];
                formatar_tempo(tempoTotal, bufTempo, sizeof(bufTempo));
                DrawText(TextFormat("TEMPO: %s", bufTempo), 10, 70, 20, LIGHTGRAY);

            } break;

            // -------- RANKING --------
            case TELA_RANKING: {
                ClearBackground(BLACK);
                DrawText("RANKING - TOP 5", 220, 80, 36, YELLOW);
                desenhar_ranking_na_tela(rankHead);
                DrawText("APERTE BACKSPACE PARA VOLTAR", 180, 560, 20, LIGHTGRAY);
            } break;

            // -------- FASE ATIVA --------
            case TELA_FASES: {
                // Proteção: só desenhar se o índice estiver dentro dos limites do array
                if (EscapeRoom.FaseAtual >= 0 && EscapeRoom.FaseAtual < TOTAL_FASES) {
                    desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);

                    // Desenhar sprite do jogador (dependendo da direção)
                    DrawTexture(
                        EscapeRoom.jogador.sprites[EscapeRoom.jogador.sprite_atual],
                        EscapeRoom.jogador.hitbox_jogador.x,
                        EscapeRoom.jogador.hitbox_jogador.y,
                        WHITE
                    );


                    // HUD no canto superior esquerdo: Fase, Vida, Tempo formatado MM:SS.mmm
                    DrawText(TextFormat("Fase: %d", EscapeRoom.FaseAtual + 1),
                             10, 10, 20, LIGHTGRAY);
                    DrawText(TextFormat("Vida: %d", EscapeRoom.jogador.vida),
                             10, 40, 20, LIGHTGRAY);

                    char bufTempo[32];
                    formatar_tempo(tempoTotal, bufTempo, sizeof(bufTempo));
                    DrawText(TextFormat("Tempo: %s", bufTempo), 10, 70, 20, LIGHTGRAY);
                } else {
                    // Caso excepcional: índice inválido — mostra mensagem segura ao invés de crashar
                    DrawText("Nenhuma fase ativa (indice invalido)", 80, 200, 20, RED);
                    DrawText("Pressione BACKSPACE para voltar ao menu", 80, 240, 18, LIGHTGRAY);

                    // opcional: garantir que o jogador possa voltar com BACKSPACE
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        telaAtual = TELA_MENU;
                        telaJustChanged = true;
                    }
                }
            } break;

            // -------- TELA FINAL: pedir nome e salvar --------
            case TELA_FINAL: {
                ClearBackground(BLACK);
                DrawText("PARABENS! VOCE COMPLETOU O JOGO", 160, 160, 24, YELLOW);
                char bufTempo[32];
                formatar_tempo(tempoTotal, bufTempo, sizeof(bufTempo));
                DrawText(TextFormat("SEU TEMPO: %s", bufTempo), 220, 220, 22, GREEN);

                DrawText("DIGITE SEU NOME E APERTE ENTER:", 160, 300, 20, LIGHTGRAY);
                // caixa simples
                DrawRectangle(180, 340, 400, 40, DARKGRAY);
                DrawRectangleLinesEx((Rectangle){180, 340, 400, 40}, 2, WHITE);

                // desenha o texto do nome
                DrawText(nomeInput, 190, 350, 20, WHITE);

                DrawText("BACKSPACE PARA VOLTAR AO MENU (CANCELA)", 160, 420, 18, LIGHTGRAY);
            } break;
        }

        EndDrawing();

        telaJustChanged = false;
    }

    // cleanup
    if (menuTextura.id != 0) UnloadTexture(menuTextura);
    if (tutorialTextura.id != 0) UnloadTexture(tutorialTextura);
    for (int i = 0; i < 4; i++){
        if (iniciarTexturas[i].id != 0){
            UnloadTexture(iniciarTexturas[i]);
        }
    }

    // salvar ranking atual antes de fechar (opcional)
    if (rankHead) {
        salvarRanking(rankHead, ARQUIVO_RANKING);
        liberarRanking(&rankHead);
    }

    // Unload sprites do jogador
    for (int i = 0; i < 5; i++){
    if (EscapeRoom.jogador.sprites[i].id != 0)
        UnloadTexture(EscapeRoom.jogador.sprites[i]);
    }

    acabarFases(EscapeRoom.fases, TOTAL_FASES);
    CloseWindow();
    return 0;
}