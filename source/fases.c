#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "../includes/structs.h" 
#include "../includes/fases.h"

#define MAX 12

static Vector2 bases[4][MAX] = {0};

// Função simples de colisão (MANTIDO)
int col(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

void desenharJogadorManual(jogador *j) {
    if (!j) return;
    
    float x = j->hitbox_jogador.x;
    float y = j->hitbox_jogador.y;
    float w = j->hitbox_jogador.width;
    float h = j->hitbox_jogador.height;

    Vector2 pe = { x + w/2, y + h }; 

    Color pele = BEIGE;
    Color roupa = BLUE;

    int viradoDireita = (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) ? 1 : 0;
    if (!viradoDireita && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A)) viradoDireita = 1; // Padrão direita

    DrawCircle(pe.x, pe.y - 45, 10, pele);

    DrawRectangle(pe.x - 8, pe.y - 35, 16, 20, roupa);

    float balanco = sinf(GetTime() * 10.0f) * 5.0f;
    if (!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) balanco = 0;

    DrawLineEx((Vector2){pe.x - 4, pe.y - 15}, (Vector2){pe.x - 4 + balanco, pe.y}, 4, DARKBLUE);

    DrawLineEx((Vector2){pe.x + 4, pe.y - 15}, (Vector2){pe.x + 4 - balanco, pe.y}, 4, DARKBLUE);

    if (viradoDireita) {
        DrawRectangle(pe.x - 2, pe.y - 32, 12, 4, pele); 
    } else {
        DrawRectangle(pe.x - 10, pe.y - 32, 12, 4, pele); 
    }
}

void comecarfase(fases *f, int qtd) {
    if (!f) return;
    int W = GetScreenWidth();
    int H = GetScreenHeight();

    for (int p = 0; p < qtd; p++) {
        f[p].numero = p + 1;
        f[p].completo = 0;

        int n = 4 + p * 2;
        if (p == 3) n = 10;
        if (n > MAX) n = MAX;
        
        f[p].quantidadeObstaculos = n;
        f[p].obstaculos = malloc(sizeof(obstaculo) * n);
        if (!f[p].obstaculos) continue;

        f[p].posicaoinicial = (Vector2){80, H / 2.0f};
        f[p].saida = (portal){{W - 100, H / 2.0f - 50}, {60, 100}, 1};

        float start = 250;
        float gap = (f[p].saida.posicao.x - 60 - start) / n;

        for (int i = 0; i < n; i++) {
            obstaculo *o = &f[p].obstaculos[i];
            int tipo = i % 2; 
            
            float by = H * 0.5f;
            
            if (tipo == 0) { 
                o->tamanho = (Vector2){60, 20};
                if (i % 3 == 0) by -= 120;
                if (i % 3 == 2) by += 120;
            } else { 
                o->tamanho = (Vector2){40, 40};
                if (i % 4 == 1) by -= 70;
                if (i % 4 == 3) by += 70;
            }

            bases[p][i] = o->posicao = (Vector2){start + gap * i + gap * 0.5f, by};
            
            float amp = 60.0f + p * 15.0f;
            if (p == 3) amp = 140.0f;
            
            o->velocidade = (Vector2){(float)tipo, amp};
            o->ativo = 1;
        }

        int nPlat = 5 + p * 2; 
        if (nPlat > MAX) nPlat = MAX; 
        
        f[p].quantidadePlataformas = nPlat;
        f[p].plataformas = malloc(sizeof(Plataforma) * nPlat);
        if (!f[p].plataformas) continue;

        float px = 80.0f; 
        float py = H - 150.0f; 
        float gapX = (W - 200.0f) / nPlat; 
        float gapY = 80.0f; 
        float platW = 80.0f;
        float platH = 20.0f;

        for (int i = 0; i < nPlat; i++) {
            Plataforma *plat = &f[p].plataformas[i];
            
            plat->rect = (Rectangle){px, py, platW, platH};
            plat->isVisivel = true;
            plat->velocidade = 0.0f;
            plat->tipo = TIPO_PLATAFORMA_FIXA;

            // Dificuldade Progressiva: Plataformas Móveis a partir da Fase 2 (p >= 1)
            if (p >= 1 && (i % 2 != 0)) { // Plataformas ímpares (i=1, 3, 5...) se movem
                // Fase 2 e 4 (p=1, 3) movem X. Fase 3 (p=2) move Y.
                plat->tipo = (p % 2 == 1) ? TIPO_PLATAFORMA_MOVEL_X : TIPO_PLATAFORMA_MOVEL_Y;
                plat->velocidade = 1.0f + (p * 0.4f); // Velocidade aumenta com a fase
                plat->direcao = 1;

                if (plat->tipo == TIPO_PLATAFORMA_MOVEL_Y) {
                    plat->limiteA = py - 80;
                    plat->limiteB = py + 40;
                } else { // MOVEL_X
                    plat->limiteA = px - 50;
                    plat->limiteB = px + 50;
                }
            }

            // Próximo degrau (Escadinha)
            px += gapX;
            py -= gapY;
            if (py < 100) py = H - 150.0f; 
        }
    }
}

// MODIFICADA: Adiciona liberação das plataformas
void acabarFases(fases *f, int qtd) {
    if (f) for (int i = 0; i < qtd; i++) { 
        free(f[i].obstaculos); 
        // 💡 NOVO: Libera a memória das Plataformas
        free(f[i].plataformas);
    }
}

/* --- Funções de Jogo (Update e Draw) --- */

// MODIFICADA: Adiciona lógica de movimento e colisão de plataformas
void atualizarFases(fases *f, jogador *j, TelaAtual *tela) {
    if (!f || !j) return;
    
    // Trava jogador no limite do portal
    if (j->hitbox_jogador.x > f->saida.posicao.x) j->hitbox_jogador.x = f->saida.posicao.x;
    
    if (j->vida <= 0) { *tela = TELA_GAME_OVER; return; }

    int idx = (f->numero > 0) ? f->numero - 1 : 0;
    
    float oldY = j->hitbox_jogador.y; // Posição Y do jogador antes da colisão/movimento
    bool playerOnPlatform = false; // Flag para rastrear se o jogador está em alguma plataforma

    // ==========================================================
    // 1. ATUALIZAÇÃO E COLISÃO DAS PLATAFORMAS (NOVA LÓGICA)
    // ==========================================================
    for (int i = 0; i < f->quantidadePlataformas; i++) {
        Plataforma *plat = &f->plataformas[i];
        
        // Movimento
        if (plat->tipo == TIPO_PLATAFORMA_MOVEL_Y) {
            plat->rect.y += plat->velocidade * plat->direcao;
            if (plat->rect.y >= plat->limiteB) plat->direcao = -1;
            if (plat->rect.y <= plat->limiteA) plat->direcao = 1;
        } else if (plat->tipo == TIPO_PLATAFORMA_MOVEL_X) {
            plat->rect.x += plat->velocidade * plat->direcao;
            if (plat->rect.x >= plat->limiteB) plat->direcao = -1;
            if (plat->rect.x <= plat->limiteA) plat->direcao = 1;
        }

        // Colisão com o Jogador (Só se o jogador estiver caindo ou parado e colidindo por cima)
        if (col(j->hitbox_jogador, plat->rect) && 
            (j->hitbox_jogador.y + j->hitbox_jogador.height <= plat->rect.y + plat->rect.height * 0.5f) && 
            (j->velocidade_vertical >= 0)) 
        {
            // Reposiciona o jogador na superfície
            j->hitbox_jogador.y = plat->rect.y - j->hitbox_jogador.height;
            j->velocidade_vertical = 0.0f; // Para a queda
            // j->estaNoChao deve ser setado no inputs.c, mas garantimos aqui que ele está pousado
            playerOnPlatform = true; 

            // 💡 CRÍTICO: Move o jogador junto com a plataforma
            if (plat->tipo == TIPO_PLATAFORMA_MOVEL_X) {
                j->hitbox_jogador.x += plat->velocidade * plat->direcao;
            } else if (plat->tipo == TIPO_PLATAFORMA_MOVEL_Y) {
                // Se a plataforma move Y, o jogador se move junto verticalmente
                j->hitbox_jogador.y += plat->velocidade * plat->direcao;
            }
        }
    }
    
    // Se o jogador estava em uma plataforma, garantimos que ele está "no chão" (para poder pular)
    if (playerOnPlatform) j->estaNoChao = 1;

    // ----------------------------------------------------------
    // SEU CÓDIGO ORIGINAL DE ATUALIZAÇÃO E COLISÃO DE OBSTÁCULOS
    // (MANTIDO)
    // ----------------------------------------------------------
    float freq = 1.0f + idx * 0.15f;
    if (idx == 3) freq = 1.4f;

    float t = GetTime();

    for (int i = 0; i < f->quantidadeObstaculos; i++) {
        obstaculo *o = &f->obstaculos[i];
        if (!o->ativo) continue;

        float osc = sinf(t * freq + i) * o->velocidade.y;
        Vector2 np = bases[idx][i];

        if (o->velocidade.x == 0) np.x += osc; 
        else np.y += osc; 
        
        // Limites (MANTIDO)
        if (np.x < 150) np.x = 150;
        if (np.x + o->tamanho.x > f->saida.posicao.x - 10) np.x = f->saida.posicao.x - 10 - o->tamanho.x;
        
        if (np.y < 30) np.y = 30;
        if (np.y + o->tamanho.y > GetScreenHeight() - 20) np.y = GetScreenHeight() - 20 - o->tamanho.y;

        o->posicao = np;
        
        // Colisão Player (MANTIDO)
        if (col(j->hitbox_jogador, (Rectangle){np.x, np.y, o->tamanho.x, o->tamanho.y})) {
            j->vida--;
            o->ativo = 0;
            if (j->vida <= 0) *tela = TELA_GAME_OVER;
        }
    }
    // ----------------------------------------------------------

    // Colisão Portal (MANTIDO)
    if (f->saida.ativo && col(j->hitbox_jogador, (Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100})) {
        f->completo = 1;
        f->saida.ativo = 0;
    }
}

// MODIFICADA: Adiciona o desenho das plataformas e melhora a bola de fogo
void desenharFase(fases *f, jogador *j) {
    if (!f || !j) return;

    // 1. DESENHA O PORTAL (MANTIDO)
    if (f->saida.ativo) {
        DrawRectangleRec((Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100}, DARKBLUE);
        DrawRectangleLines(f->saida.posicao.x, f->saida.posicao.y, 60, 100, SKYBLUE);
    }

    // 2. DESENHA OS OBSTÁCULOS (MODIFICADO: MELHORIA DA BOLA DE FOGO)
    float t = GetTime();
    for (int i = 0; i < f->quantidadeObstaculos; i++) {
        obstaculo *o = &f->obstaculos[i];
        if (!o->ativo) continue;

        if (o->velocidade.x == 0) { // Flecha (MANTIDO)
            DrawRectangle(o->posicao.x, o->posicao.y + 8, o->tamanho.x - 15, 4, BROWN);
            DrawRectangle(o->posicao.x, o->posicao.y + 6, 10, 8, RED);
            Vector2 p1 = {o->posicao.x + o->tamanho.x, o->posicao.y + o->tamanho.y/2};
            Vector2 p2 = {o->posicao.x + o->tamanho.x - 15, o->posicao.y + o->tamanho.y};
            Vector2 p3 = {o->posicao.x + o->tamanho.x - 15, o->posicao.y};
            DrawTriangle(p1, p2, p3, LIGHTGRAY);
        } else { // Fogo (MELHORADO)
            
            Vector2 c = {o->posicao.x + o->tamanho.x/2, o->posicao.y + o->tamanho.y/2};
            float raio_base = o->tamanho.x / 2.0f;
            
            // Cria um fator de pulsação suave (realismo)
            // sinf(t * 12.0f) gera uma oscilação rápida entre -1 e 1
            float pulso = 1.0f + sinf(t * 12.0f + i * 0.5f) * 0.15f; 
            float raio = raio_base * pulso; 

            // --- Camadas Concéntricas para Efeito de Calor/Textura ---

            // 1. Camada Externa (Glow/Calor - Vermelho com Opacidade)
            // Usamos Fade para simular a neblina de calor e a borda suave.
            DrawCircleV(c, raio * 1.05f, Fade(RED, 0.4f)); 

            // 2. Camada de Fogo Vermelho/Laranja (Corpo Externo)
            DrawCircleV(c, raio * 0.9f, (Color){255, 128, 0, 255}); // Laranja/Vermelho (customizado)

            // 3. Camada de Fogo Laranja (Corpo Principal)
            DrawCircleV(c, raio * 0.7f, ORANGE); 

            // 4. Camada de Fogo Amarelo (Intenso)
            DrawCircleV(c, raio * 0.5f, YELLOW); 
            
            // 5. Núcleo (Branco - O ponto mais quente)
            DrawCircleV(c, raio * 0.3f, WHITE); 
        }
    }
    
    // ==========================================================
    // 💡 NOVO: 3. DESENHA AS PLATAFORMAS (cor do mapa)
    // ==========================================================
    for (int i = 0; i < f->quantidadePlataformas; i++) {
        Plataforma *plat = &f->plataformas[i];
        if (!plat->isVisivel) continue;

        // Cor escura, como a do mapa, para camuflar.
        Color corPlataforma = (plat->tipo == TIPO_PLATAFORMA_FIXA) ? DARKGRAY : GRAY;
        
        DrawRectangleRec(plat->rect, corPlataforma);
        DrawRectangleLinesEx(plat->rect, 2, BLACK); // Borda para destaque
    }
}