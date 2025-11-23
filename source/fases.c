#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "../includes/structs.h"
#include "../includes/fases.h"

#define MAX 12
static Vector2 bases[4][MAX] = {0};

// Função simples de colisão
int col(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

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
        else np.y += osc;                      // Fogo mexe no Y

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

    if (f->saida.ativo) {
        DrawRectangleRec((Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100}, BLUE);
        DrawRectangleLines(f->saida.posicao.x, f->saida.posicao.y, 60, 100, DARKBLUE);
    }

    for (int i = 0; i < f->quantidadeObstaculos; i++) {
        obstaculo *o = &f->obstaculos[i];
        if (!o->ativo) continue;

        if (o->velocidade.x == 0) { // Desenha Flecha
            DrawRectangle(o->posicao.x, o->posicao.y + 5, o->tamanho.x - 15, o->tamanho.y - 10, ORANGE);
            Vector2 p1 = {o->posicao.x + o->tamanho.x, o->posicao.y + o->tamanho.y / 2};
            Vector2 p2 = {o->posicao.x + o->tamanho.x - 15, o->posicao.y + o->tamanho.y};
            Vector2 p3 = {o->posicao.x + o->tamanho.x - 15, o->posicao.y};
            DrawTriangle(p1, p2, p3, ORANGE);
        } else { // Desenha Fogo
            Vector2 c = {o->posicao.x + o->tamanho.x / 2, o->posicao.y + o->tamanho.y / 2};
            DrawCircleV(c, o->tamanho.x / 2, RED);
            DrawCircleLines(c.x, c.y, o->tamanho.x / 2, YELLOW);
        }
    }

    DrawRectangleRec(j->hitbox_jogador, GREEN);
    DrawRectangleLinesEx(j->hitbox_jogador, 2, DARKGREEN);
    DrawText(TextFormat("Vida: %d", j->vida), 10, 40, 20, BLACK);
}