#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/fases.h"

#define MAX 12
// Bases estáticas para o movimento dos obstáculos
static Vector2 bases[4][MAX] = {0};

/* --- Funções Auxiliares --- */

// Função simples de colisão
int col(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

// NOVO: Desenha o boneco do jogador manualmente (fallback)
void desenharJogadorManual(jogador *j) {
    if (!j) return;
    
    // Pegamos a posição X e Y da "alma" (hitbox) para saber onde desenhar
    float x = j->hitbox_jogador.x;
    float y = j->hitbox_jogador.y;
    float w = j->hitbox_jogador.width;
    float h = j->hitbox_jogador.height;

    // O "Pivô" do boneco é o centro da base (nos pés)
    Vector2 pe = { x + w/2, y + h }; 

    // Cores
    Color pele = BEIGE;
    Color roupa = BLUE;

    // Detecta direção (Gambiarra visual baseada no input)
    int viradoDireita = (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) ? 1 : 0;
    if (!viradoDireita && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A)) viradoDireita = 1; // Padrão direita

    // --- DESENHO DO BONECO ---
    
    // Cabeça (Flutuando acima da hitbox)
    DrawCircle(pe.x, pe.y - 45, 10, pele);

    // Corpo (Tronco)
    DrawRectangle(pe.x - 8, pe.y - 35, 16, 20, roupa);

    // Pernas (Simulando movimento se estiver andando)
    float balanco = sinf(GetTime() * 10.0f) * 5.0f;
    if (!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) balanco = 0;

    // Perna Esquerda
    DrawLineEx((Vector2){pe.x - 4, pe.y - 15}, (Vector2){pe.x - 4 + balanco, pe.y}, 4, DARKBLUE);
    // Perna Direita
    DrawLineEx((Vector2){pe.x + 4, pe.y - 15}, (Vector2){pe.x + 4 - balanco, pe.y}, 4, DARKBLUE);

    // Braços
    if (viradoDireita) {
        DrawRectangle(pe.x - 2, pe.y - 32, 12, 4, pele); // Aponta pra frente
    } else {
        DrawRectangle(pe.x - 10, pe.y - 32, 12, 4, pele); // Aponta pra tras
    }
}

/* --- Funções de Inicialização e Limpeza --- */

void comecarfase(fases *f, int qtd) {
    if (!f) return;
    int W = GetScreenWidth();
    int H = GetScreenHeight();

    for (int p = 0; p < qtd; p++) {
        f[p].numero = p + 1;
        f[p].completo = 0;

        // Define quantidade: Fase 4 tem 10, outras aumentam de 2 em 2
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
            int tipo = i % 2; // 0 = Flecha, 1 = Fogo
            
            // Define tamanho e altura base
            float by = H * 0.5f;
            
            if (tipo == 0) { // Flecha
                o->tamanho = (Vector2){60, 20};
                if (i % 3 == 0) by -= 120;
                if (i % 3 == 2) by += 120;
            } else { // Fogo
                o->tamanho = (Vector2){40, 40};
                if (i % 4 == 1) by -= 70;
                if (i % 4 == 3) by += 70;
            }

            bases[p][i] = o->posicao = (Vector2){start + gap * i + gap * 0.5f, by};
            
            // Define velocidade/amplitude
            float amp = 60.0f + p * 15.0f;
            if (p == 3) amp = 140.0f;
            
            o->velocidade = (Vector2){(float)tipo, amp};
            o->ativo = 1;
        }
    }
}

void acabarFases(fases *f, int qtd) {
    if (f) for (int i = 0; i < qtd; i++) { free(f[i].obstaculos); }
}

/* --- Funções de Jogo (Update e Draw) --- */

void atualizarFases(fases *f, jogador *j, TelaAtual *tela) {
    if (!f || !j) return;
    
    // Trava jogador no limite do portal
    if (j->hitbox_jogador.x > f->saida.posicao.x) j->hitbox_jogador.x = f->saida.posicao.x;
    
    if (j->vida <= 0) { *tela = TELA_GAME_OVER; return; }

    int idx = (f->numero > 0) ? f->numero - 1 : 0;
    
    // Velocidade: Fase 4 é fixa em 1.4, outras aumentam aos poucos
    float freq = 1.0f + idx * 0.15f;
    if (idx == 3) freq = 1.4f;

    float t = GetTime();

    for (int i = 0; i < f->quantidadeObstaculos; i++) {
        obstaculo *o = &f->obstaculos[i];
        if (!o->ativo) continue;

        float osc = sinf(t * freq + i) * o->velocidade.y;
        Vector2 np = bases[idx][i];

        if (o->velocidade.x == 0) np.x += osc; // Flecha mexe no X
        else np.y += osc;                    // Fogo mexe no Y <--- CORRIGIDO: SEM CARACTERES INVISÍVEIS

        // Limites
        if (np.x < 150) np.x = 150;
        if (np.x + o->tamanho.x > f->saida.posicao.x - 10) np.x = f->saida.posicao.x - 10 - o->tamanho.x;
        
        if (np.y < 30) np.y = 30;
        if (np.y + o->tamanho.y > GetScreenHeight() - 20) np.y = GetScreenHeight() - 20 - o->tamanho.y;

        o->posicao = np;
        
        // Colisão Player
        if (col(j->hitbox_jogador, (Rectangle){np.x, np.y, o->tamanho.x, o->tamanho.y})) {
            j->vida--;
            o->ativo = 0;
            if (j->vida <= 0) *tela = TELA_GAME_OVER;
        }
    }

    // Colisão Portal
    if (f->saida.ativo && col(j->hitbox_jogador, (Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100})) {
        f->completo = 1;
        f->saida.ativo = 0;
    }
}

void desenharFase(fases *f, jogador *j) {
    if (!f || !j) return;

    // 1. DESENHA O PORTAL
    if (f->saida.ativo) {
        DrawRectangleRec((Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100}, DARKBLUE);
        DrawRectangleLines(f->saida.posicao.x, f->saida.posicao.y, 60, 100, SKYBLUE);
    }

    // 2. DESENHA OS OBSTÁCULOS
    for (int i = 0; i < f->quantidadeObstaculos; i++) {
        obstaculo *o = &f->obstaculos[i];
        if (!o->ativo) continue;

        if (o->velocidade.x == 0) { // Flecha
            DrawRectangle(o->posicao.x, o->posicao.y + 8, o->tamanho.x - 15, 4, BROWN);
            DrawRectangle(o->posicao.x, o->posicao.y + 6, 10, 8, RED);
            Vector2 p1 = {o->posicao.x + o->tamanho.x, o->posicao.y + o->tamanho.y/2};
            Vector2 p2 = {o->posicao.x + o->tamanho.x - 15, o->posicao.y + o->tamanho.y};
            Vector2 p3 = {o->posicao.x + o->tamanho.x - 15, o->posicao.y};
            DrawTriangle(p1, p2, p3, LIGHTGRAY);
        } else { // Fogo
            Vector2 c = {o->posicao.x + o->tamanho.x/2, o->posicao.y + o->tamanho.y/2};
            DrawCircleV(c, o->tamanho.x/2, RED);
            DrawCircleV(c, o->tamanho.x/2, ORANGE); // Efeito simples
        }
    }

    // *** REMOVIDO: O CÓDIGO MANUAL DE DESENHO DO JOGADOR FOI MOVIDO PARA desenharJogadorManual() ***
    // Isso garante que o jogador será desenhado APENAS no main.c, eliminando o erro de duplicação.
    // O HUD de vida TAMBÉM NÃO DEVE ESTAR AQUI! Deve estar no main.c
}