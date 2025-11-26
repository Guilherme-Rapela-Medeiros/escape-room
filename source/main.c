// source/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/inputs.h"
#include "../includes/fases.h" // Necessário para desenharJogadorManual
#include "../includes/mapa.h"
#include "../includes/ranking.h" 

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

/* --- Funções Auxiliares de Tempo e Ranking --- */

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
            /* libera o resto da lista */
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
        
        // Cores alternadas para facilitar leitura
        Color corTexto = (i == 1) ? YELLOW : WHITE; 
        
        DrawText(TextFormat("%d. %s", i, it->nome[0] ? it->nome : "(sem nome)"), 120, y, 20, corTexto);
        DrawText(buf, 500, y, 20, corTexto); // Alinha o tempo à direita
        
        it = it->prox;
        y += 35;
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
    bool tempoRodando = false;   // true apenas em TELA_FASES

    // =============================
    // CARREGAMENTO DE IMAGENS (MODO SEGURO)
    // =============================
    
    // 1. Menu Inicial
    Image menuImg = LoadImage("assets/imagens/menu.inicial.png");
    Texture2D menuTextura = {0};
    if (menuImg.data != NULL) {
        menuTextura = LoadTextureFromImage(menuImg);
        UnloadImage(menuImg);
    }

    // 2. Tutorial
    Image tutorialImg = LoadImage("assets/imagens/tela.tutorial.png");
    Texture2D tutorialTextura = {0};
    if (tutorialImg.data != NULL) {
        tutorialTextura = LoadTextureFromImage(tutorialImg);
        UnloadImage(tutorialImg);
    }

    // 3. Telas de Início de Fase
    Texture2D iniciarTexturas[4] = {0};
    const char* pathsIniciar[4] = {
        "assets/imagens/iniciar.fase1.png",
        "assets/imagens/iniciar.fase2.png",
        "assets/imagens/iniciar.fase3.png",
        "assets/imagens/iniciar.fase4.png"
    };

    for(int i=0; i<4; i++) {
        Image tempImg = LoadImage(pathsIniciar[i]);
        if(tempImg.data != NULL) {
            iniciarTexturas[i] = LoadTextureFromImage(tempImg);
            UnloadImage(tempImg);
        }
    }

    // =============================
    // 4. Mapas das fases
    // =============================
    jogo EscapeRoom;


    const char* pathsMapas[4] = {
        "assets/imagens/mapas/mapa_fase1.png",
        "assets/imagens/mapas/mapa_fase2.png",
        "assets/imagens/mapas/mapa_fase3.png",
        "assets/imagens/mapas/mapa_fase4.png"
    };

    for (int i = 0; i < 4; i++) {
        Image tempMapa = LoadImage(pathsMapas[i]);
        EscapeRoom.mapas[i] = (tempMapa.data != NULL)
            ? LoadTextureFromImage(tempMapa)
            : (Texture2D){0};

        if (tempMapa.data != NULL)
            UnloadImage(tempMapa);

        if (EscapeRoom.mapas[i].id == 0) {
            printf("ERRO: mapa %d nao carregou!\n", i + 1);
        }
        EscapeRoom.fases[i].mapaTextura = EscapeRoom.mapas[i];
    }

    //inicialização da imagem do portal

    Image imgPortal = LoadImage("assets/imagens/imagem_portal.png");

    for (int i = 0; i < 4; i++) {
        EscapeRoom.fases[i].saida.sprite = LoadTextureFromImage(imgPortal);
    }

    UnloadImage(imgPortal);



    // =============================
    // INICIALIZAÇÃO DO JOGO
    // =============================
    EscapeRoom.FimDeJogo = FALSE;
    EscapeRoom.FaseAtual = 0;

    // Configuração Inicial do Jogador
    EscapeRoom.jogador.vida = 3;
    EscapeRoom.jogador.hitbox_jogador = (Rectangle){
        LARGURA_TELA / 2.0f - 25, ALTURA_TELA / 2.0f - 25, 50, 50
    };

    // ---------------------------------------
    // CARREGAR SPRITES DO JOGADOR (MODO SEGURO)
    // ---------------------------------------
    // Array auxiliar com os caminhos para facilitar o loop
    const char* playerPaths[5];
    playerPaths[SPRITE_PARADO]   = "assets/player/jogador_sprite_parado.png";
    playerPaths[SPRITE_ESQUERDA] = "assets/player/jogador_sprite_esquerda.png";
    playerPaths[SPRITE_DIREITA]  = "assets/player/jogador_sprite_direita.png";
    playerPaths[SPRITE_CIMA]     = "assets/player/jogador_sprite_cima.png";
    playerPaths[SPRITE_BAIXO]    = "assets/player/jogador_sprite_baixo.png";

    for (int i = 0; i < 5; i++) {
        Image pImg = LoadImage(playerPaths[i]);
        if (pImg.data != NULL) {
            EscapeRoom.jogador.sprites[i] = LoadTextureFromImage(pImg);
            UnloadImage(pImg);
        } else {
            // *** MUDANÇA AQUI ***
            // Se falhar, DEIXAMOS a textura com id=0 (seu valor inicial)
            // Isso permite que a lógica de desenho use o fallback manual.
            printf("AVISO: Falha ao carregar sprite do jogador: %s. Usando fallback manual.\n", playerPaths[i]);
            EscapeRoom.jogador.sprites[i] = (Texture2D){0}; // Garante id=0 se o LoadImage falhar
        }
    }
    
    EscapeRoom.jogador.sprite_atual = SPRITE_PARADO;

    // Inicializa estrutura das fases
    comecarfase(EscapeRoom.fases, TOTAL_FASES);

    // Posiciona jogador na primeira fase
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

    // --- Ranking: carrega do arquivo ---
    ranking *rankHead = NULL;
    carregarRanking(&rankHead, ARQUIVO_RANKING);
    manter_top_n(&rankHead, TOP_N);

    // --- Variáveis para Input de Nome ---
    char nomeInput[TAMANHO_NOME] = {0};
    int nomeLen = 0;
    bool finalNomeConfirmado = false;

    // =============================
    //        LOOP PRINCIPAL
    // =============================
    while (!WindowShouldClose() && !EscapeRoom.FimDeJogo) {

        // ============ LOGICA: MENU ============
        if (telaAtual == TELA_MENU) {

            if (IsKeyPressed(KEY_DOWN)) selecionado = (selecionado + 1) % nBotoes;
            else if (IsKeyPressed(KEY_UP)) selecionado = (selecionado - 1 + nBotoes) % nBotoes;

            Vector2 mouse = GetMousePosition();
            bool ativou = false;

            for (int i = 0; i < nBotoes; i++) {
                if (CheckCollisionPointRec(mouse, botoes[i].rect)) {
                    botoes[i].hovered = true;
                    selecionado = i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ativou = true;
                } else {
                    botoes[i].hovered = (i == selecionado);
                }
            }

            if (IsKeyPressed(KEY_ENTER) || ativou) {
                switch (selecionado) {
                    case 0: // START
                        EscapeRoom.FaseAtual = 0;
                        tempoTotal = 0.0f;
                        tempoRodando = false;
                        telaAtual = TELA_JOGO; // Vai para tela de "Iniciar Fase 1"
                        telaJustChanged = true;
                        break;
                    case 1: // TUTORIAL
                        telaAtual = TELA_TUTORIAL;
                        telaJustChanged = true;
                        break;
                    case 2: // RANKING
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

        // ============ LOGICA: TELA "INICIAR FASE" ============
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
                tempoRodando = true; // TIME START!
                telaAtual = TELA_FASES;
                
                // Reseta posição do player para o inicio da fase atual
                if (EscapeRoom.fases != NULL) {
                    EscapeRoom.jogador.hitbox_jogador.x = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.x;
                    EscapeRoom.jogador.hitbox_jogador.y = EscapeRoom.fases[EscapeRoom.FaseAtual].posicaoinicial.y;
                }
                telaJustChanged = true;
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                telaAtual = TELA_MENU;
                telaJustChanged = true;
            }
        }

        // ============ LOGICA: RETORNOS SIMPLES ============
        if (telaAtual == TELA_TUTORIAL || telaAtual == TELA_RANKING) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                telaAtual = TELA_MENU;
                telaJustChanged = true;
            }
        }

        // ============ LOGICA: FASE ATIVA (JOGO ROLANDO) ============
        if (telaAtual == TELA_FASES) {

            // Acumula tempo
            if (tempoRodando) {
                tempoTotal += GetFrameTime();
            }

            if (EscapeRoom.FaseAtual < TOTAL_FASES) {
                inputs_jogador_movimento(&EscapeRoom.jogador, LARGURA_TELA, ALTURA_TELA,
                                         5, EscapeRoom.fases[EscapeRoom.FaseAtual].obstaculos);

                // Passa &telaAtual para permitir Game Over dentro da função
                atualizarFases(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador, &telaAtual);

                // Checa se completou a fase
                if (EscapeRoom.fases[EscapeRoom.FaseAtual].completo) {
                    tempoRodando = false; // Pausa o tempo
                    EscapeRoom.FaseAtual++;
                    
                    if (EscapeRoom.FaseAtual >= TOTAL_FASES) {
                        EscapeRoom.FaseAtual = TOTAL_FASES;
                        telaAtual = TELA_FINAL; // Venceu o jogo
                        
                        // Prepara input de nome
                        nomeInput[0] = '\0';
                        nomeLen = 0;
                        finalNomeConfirmado = false;
                    } else {
                        telaAtual = TELA_JOGO; // Próxima tela de "Iniciar Fase X"
                    }
                    telaJustChanged = true;
                }
            }
        }

        // ============ LOGICA: GAME OVER ============
        if (telaAtual == TELA_GAME_OVER) {
            tempoRodando = false;
            
            // Botões do Game Over
            int larguraBtn = 200;
            Rectangle btnMenu = { (LARGURA_TELA/2 - larguraBtn - 12), 460, larguraBtn, 60 };
            Rectangle btnRetry = { (LARGURA_TELA/2 + 12), 460, larguraBtn, 60 };
            
            Vector2 mouse = GetMousePosition();
            bool hoverMenu = CheckCollisionPointRec(mouse, btnMenu);
            bool hoverRetry = CheckCollisionPointRec(mouse, btnRetry);

            if (!telaJustChanged) {
                if ((hoverMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_M)) {
                    telaAtual = TELA_MENU;
                    telaJustChanged = true;
                    // Reset parcial
                    EscapeRoom.FaseAtual = 0;
                    EscapeRoom.jogador.vida = 3;
                    tempoTotal = 0.0f;
                }
                if ((hoverRetry && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
                    telaAtual = TELA_JOGO; // Reinicia da fase 1
                    telaJustChanged = true;
                    EscapeRoom.FaseAtual = 0;
                    EscapeRoom.jogador.vida = 3;
                    tempoTotal = 0.0f;
                }
            }
        }

        // ============ LOGICA: TELA FINAL (INPUT NOME) ============
        if (telaAtual == TELA_FINAL) {
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
                if (nomeLen == 0) strcpy(nomeInput, "(anonimo)");
                
                // Salvar ranking
                inserirRanking(&rankHead, nomeInput, tempoTotal);
                manter_top_n(&rankHead, TOP_N);
                salvarRanking(rankHead, ARQUIVO_RANKING);
                
                finalNomeConfirmado = true;
            }
            
            // Se confirmou, vai ver o ranking
            if (finalNomeConfirmado) {
                liberarRanking(&rankHead);
                rankHead = NULL;
                carregarRanking(&rankHead, ARQUIVO_RANKING);
                manter_top_n(&rankHead, TOP_N);
                telaAtual = TELA_RANKING;
                telaJustChanged = true;
            }
        }

        // =============================
        //          DESENHO
        // =============================
        BeginDrawing();
        ClearBackground(BLACK);

        switch (telaAtual) {
            case TELA_MENU:
                if (menuTextura.id != 0) DrawTexture(menuTextura, 0, 0, WHITE);
                
                for (int i = 0; i < nBotoes; i++) {
                    Color corFundo = botoes[i].hovered ? Fade(SKYBLUE, 0.95f) : Fade(DARKGRAY, 0.8f);
                    Color corBorda = botoes[i].hovered ? BLUE : GRAY;
                    DrawRectangleRec(botoes[i].rect, corFundo);
                    DrawRectangleLinesEx(botoes[i].rect, 2, corBorda);
                    
                    int fs = 20;
                    int tw = MeasureText(botoes[i].texto, fs);
                    DrawText(botoes[i].texto, 
                             botoes[i].rect.x + (botoes[i].rect.width - tw)/2, 
                             botoes[i].rect.y + (botoes[i].rect.height - fs)/2 - 2, 
                             fs, BLACK);
                }
                DrawText("Use SETAS e ENTER para navegar", 60, ALTURA_TELA - 50, 14, LIGHTGRAY);
                break;

            case TELA_TUTORIAL:
                if (tutorialTextura.id != 0) DrawTexture(tutorialTextura, 0, 0, WHITE);
                DrawText("BACKSPACE PARA VOLTAR", 120, ALTURA_TELA - 40, 18, LIGHTGRAY);
                break;

            case TELA_JOGO: { // Tela de Início de Fase
                int f = EscapeRoom.FaseAtual;
                if (f >= 0 && f < 4 && iniciarTexturas[f].id != 0) {
                    DrawTexture(iniciarTexturas[f], 0, 0, WHITE);
                }
                
                // Botão "Começar"
                Rectangle btn = { 238, 500, 280, 70 };
                Vector2 m = GetMousePosition();
                bool h = CheckCollisionPointRec(m, btn);
                DrawRectangleRec(btn, h ? Fade(GREEN, 0.9f) : Fade(DARKGREEN, 0.8f));
                DrawRectangleLinesEx(btn, 3, h ? YELLOW : BLACK);
                
                int tw = MeasureText("COMEÇAR", 30);
                DrawText("COMEÇAR", btn.x + (btn.width - tw)/2, btn.y + 18, 30, WHITE);
                
                char bufTempo[32];
                formatar_tempo(tempoTotal, bufTempo, sizeof(bufTempo));
                DrawText(TextFormat("TEMPO ACUMULADO: %s", bufTempo), 10, 70, 20, LIGHTGRAY);
            } break;

            case TELA_RANKING:
                DrawText("RANKING - TOP 5", 220, 80, 36, YELLOW);
                desenhar_ranking_na_tela(rankHead);
                DrawText("BACKSPACE PARA VOLTAR", 180, 600, 20, LIGHTGRAY);
                break;

            case TELA_FASES:
                if (EscapeRoom.FaseAtual < TOTAL_FASES) {
                    // Desenhar o mapa da fase atual
                    Texture2D mapaAtual = EscapeRoom.mapas[EscapeRoom.FaseAtual];

                    if (mapaAtual.id != 0) {
                        DrawTexture(mapaAtual, 0, 0, WHITE);
                    }

                    desenharFase(&EscapeRoom.fases[EscapeRoom.FaseAtual], &EscapeRoom.jogador);
                    
                    // --- MUDANÇA AQUI: Lógica de Fallback para o Jogador ---
                    Texture2D spriteAtual = EscapeRoom.jogador.sprites[EscapeRoom.jogador.sprite_atual];
                    
                    if (spriteAtual.id != 0) {
                        // 1. Desenha o Sprite PNG (se carregou corretamente)
                        DrawTexture(
                            spriteAtual,
                            (int)EscapeRoom.jogador.hitbox_jogador.x,
                            (int)EscapeRoom.jogador.hitbox_jogador.y,
                            WHITE
                        );
                    } else {
                        // 2. Desenha o Boneco Manual (fallback se o PNG falhou)
                        desenharJogadorManual(&EscapeRoom.jogador);
                    }
                    // --------------------------------------------------------

                    // HUD
                    DrawText(TextFormat("Fase: %d", EscapeRoom.FaseAtual + 1), 10, 10, 20, LIGHTGRAY);
                    DrawText(TextFormat("Vida: %d", EscapeRoom.jogador.vida), 10, 40, 20, LIGHTGRAY);
                    
                    char bufT[32];
                    formatar_tempo(tempoTotal, bufT, sizeof(bufT));
                    DrawText(TextFormat("Tempo: %s", bufT), 10, 70, 20, LIGHTGRAY);
                }
                break;
            
            case TELA_GAME_OVER: {
                int larguraBtn = 200;
                Rectangle btnMenu = { (LARGURA_TELA/2 - larguraBtn - 12), 460, larguraBtn, 60 };
                Rectangle btnRetry = { (LARGURA_TELA/2 + 12), 460, larguraBtn, 60 };
                Vector2 m = GetMousePosition();
                
                DrawText("GAME OVER", (LARGURA_TELA - MeasureText("GAME OVER", 48))/2, 120, 48, RED);
                DrawText("Voce ficou sem vidas.", (LARGURA_TELA - MeasureText("Voce ficou sem vidas.", 22))/2, 220, 22, LIGHTGRAY);
                
                bool hm = CheckCollisionPointRec(m, btnMenu);
                DrawRectangleRec(btnMenu, hm ? Fade(SKYBLUE, 0.95f) : Fade(DARKGRAY, 0.8f));
                DrawRectangleLinesEx(btnMenu, 2, WHITE);
                DrawText("VOLTAR AO MENU", btnMenu.x + 20, btnMenu.y + 20, 20, BLACK); // Ajuste fino centralizado visualmente
                
                bool hr = CheckCollisionPointRec(m, btnRetry);
                DrawRectangleRec(btnRetry, hr ? Fade(GREEN, 0.95f) : Fade(DARKGREEN, 0.8f));
                DrawRectangleLinesEx(btnRetry, 2, WHITE);
                DrawText("JOGAR NOVAMENTE", btnRetry.x + 10, btnRetry.y + 20, 20, BLACK);
            } break;

            case TELA_FINAL: {
                DrawText("PARABENS! VOCE COMPLETOU O JOGO", 160, 160, 24, YELLOW);
                char bufT[32];
                formatar_tempo(tempoTotal, bufT, sizeof(bufT));
                DrawText(TextFormat("SEU TEMPO: %s", bufT), 220, 220, 22, GREEN);
                
                DrawText("DIGITE SEU NOME:", 160, 300, 20, LIGHTGRAY);
                DrawRectangle(180, 340, 400, 40, DARKGRAY);
                DrawRectangleLinesEx((Rectangle){180, 340, 400, 40}, 2, WHITE);
                DrawText(nomeInput, 190, 350, 20, WHITE);
                
                DrawText("ENTER para confirmar", 160, 400, 18, LIGHTGRAY);
            } break;
        }

        EndDrawing();
        telaJustChanged = false;
    }

    // =============================
    // LIMPEZA DE MEMÓRIA
    // =============================
    if (menuTextura.id != 0) UnloadTexture(menuTextura);
    if (tutorialTextura.id != 0) UnloadTexture(tutorialTextura);
    
    for (int i = 0; i < 4; i++) {
        if (iniciarTexturas[i].id != 0) UnloadTexture(iniciarTexturas[i]);
    }

    for (int i = 0; i < 4; i++) {
        UnloadTexture(EscapeRoom.fases[i].saida.sprite);
    }


    for (int i = 0; i < 4; i++) {
        if (EscapeRoom.mapas[i].id != 0) {
            UnloadTexture(EscapeRoom.mapas[i]);
        }
    }


    for (int i = 0; i < 5; i++) {
        if (EscapeRoom.jogador.sprites[i].id != 0) UnloadTexture(EscapeRoom.jogador.sprites[i]);
    }

    if (rankHead) {
        salvarRanking(rankHead, ARQUIVO_RANKING);
        liberarRanking(&rankHead);
    }

    acabarFases(EscapeRoom.fases, TOTAL_FASES);
    CloseWindow();
    return 0;
}