#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "../includes/structs.h" 
#include "../includes/fases.h"

#define MAX 12
static Vector2 bases[4][MAX] = {0};

// 🔑 DEFINA O CAMINHO CORRETO DA SUA IMAGEM AQUI
#define CAMINHO_PORTAL "recursos/portal.png" 

/* --- Funções Auxiliares Básicas --- */

// 1. Colisão Simples (Rectangular)
int col(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

// 2. Desenho do Jogador (Visível)
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
    if (!viradoDireita && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A)) viradoDireita = 1;

    // Desenho do Boneco
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

/* --- Funções de Inicialização e Limpeza --- */

void comecarfase(fases *f, int qtd) {
    if (!f) return;
    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // 🔑 CARREGA A TEXTURA DO PORTAL
    Texture2D portalTex = LoadTexture(CAMINHO_PORTAL);
    if (portalTex.id == 0) TraceLog(LOG_WARNING, "FASES: Falha ao carregar textura do portal em %s", CAMINHO_PORTAL);


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
        
        // 💡 Inicializa o portal e atribui a textura carregada
        f[p].saida = (portal){{W - 100, H / 2.0f - 50}, {60, 100}, 1, portalTex};

        float start = 250;
        float gap = (f[p].saida.posicao.x - 60 - start) / n;

        // ... (inicialização de obstáculos e plataformas)
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

            if (p >= 1 && (i % 2 != 0)) {
                plat->tipo = (p % 2 == 1) ? TIPO_PLATAFORMA_MOVEL_X : TIPO_PLATAFORMA_MOVEL_Y;
                plat->velocidade = 1.0f + (p * 0.4f); 
                plat->direcao = 1;

                if (plat->tipo == TIPO_PLATAFORMA_MOVEL_Y) {
                    plat->limiteA = py - 80;
                    plat->limiteB = py + 40;
                } else { 
                    plat->limiteA = px - 50;
                    plat->limiteB = px + 50;
                }
            }

            px += gapX;
            py -= gapY;
            if (py < 100) py = H - 150.0f; 
        }
    }
}

void acabarFases(fases *f, int qtd) {
    if (f) {
        // 🔑 DESCARREGA A TEXTURA DO PORTAL UMA ÚNICA VEZ
        if (qtd > 0 && f[0].saida.textura.id != 0) {
             UnloadTexture(f[0].saida.textura);
        }

        for (int i = 0; i < qtd; i++) { 
            free(f[i].obstaculos); 
            free(f[i].plataformas);
        }
    }
}

/* 🚀 FUNÇÃO DE ATUALIZAÇÃO (MANTIDA) 🚀 */
void atualizarFases(fases *f, jogador *j, TelaAtual *tela) {
    if (!f || !j) return;
    
    // Checagem de Limite e Game Over
    if (j->hitbox_jogador.x > f->saida.posicao.x) j->hitbox_jogador.x = f->saida.posicao.x;
    if (j->vida <= 0) { *tela = TELA_GAME_OVER; return; }

    int idx = (f->numero > 0) ? f->numero - 1 : 0;
    
    // CORREÇÃO: Calculamos a Posição Y no Frame ANTERIOR (Anti-Tunneling).
    float y_anterior = j->hitbox_jogador.y - j->velocidade_vertical;
    bool playerOnPlatform = false; 

    // --- Lógica de Plataformas ---
    for (int i = 0; i < f->quantidadePlataformas; i++) {
        Plataforma *plat = &f->plataformas[i];
        
        // Movimento da Plataforma
        if (plat->tipo == TIPO_PLATAFORMA_MOVEL_Y) {
            plat->rect.y += plat->velocidade * plat->direcao;
            if (plat->rect.y >= plat->limiteB) plat->direcao = -1;
            if (plat->rect.y <= plat->limiteA) plat->direcao = 1;
        } else if (plat->tipo == TIPO_PLATAFORMA_MOVEL_X) {
            plat->rect.x += plat->velocidade * plat->direcao;
            if (plat->rect.x >= plat->limiteB) plat->direcao = -1;
            if (plat->rect.x <= plat->limiteA) plat->direcao = 1;
        }

        // LÓGICA DE POUSO SÓLIDO: Colisão + Caindo + Estava acima no frame anterior
        if (col(j->hitbox_jogador, plat->rect) && 
            (j->velocidade_vertical >= 0.0f) &&
            (y_anterior + j->hitbox_jogador.height <= plat->rect.y + 1.0f) ) 
        {
            // Colar e Parar:
            j->hitbox_jogador.y = plat->rect.y - j->hitbox_jogador.height;
            j->velocidade_vertical = 0.0f; 
            playerOnPlatform = true; 

            // Move o jogador junto com a plataforma
            if (plat->tipo == TIPO_PLATAFORMA_MOVEL_X) {
                j->hitbox_jogador.x += plat->velocidade * plat->direcao;
            } else if (plat->tipo == TIPO_PLATAFORMA_MOVEL_Y) {
                j->hitbox_jogador.y += plat->velocidade * plat->direcao;
            }
        }
    }
    
    // Atualiza o estado do chão
    if (playerOnPlatform) j->estaNoChao = 1;
    else if (j->velocidade_vertical > 0.0f) j->estaNoChao = 0;


    // --- Lógica de Obstáculos e Portal ---
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
        
        if (np.x < 150) np.x = 150;
        if (np.x + o->tamanho.x > f->saida.posicao.x - 10) np.x = f->saida.posicao.x - 10 - o->tamanho.x;
        
        if (np.y < 30) np.y = 30;
        if (np.y + o->tamanho.y > GetScreenHeight() - 20) np.y = GetScreenHeight() - 20 - o->tamanho.y;

        o->posicao = np;
        
        if (col(j->hitbox_jogador, (Rectangle){np.x, np.y, o->tamanho.x, o->tamanho.y})) {
            j->vida--;
            o->ativo = 0;
            if (j->vida <= 0) *tela = TELA_GAME_OVER;
        }
    }
    if (f->saida.ativo && col(j->hitbox_jogador, (Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100})) {
        f->completo = 1;
        f->saida.ativo = 0;
    }
}

/* --- Função de Desenho (MODIFICADA) --- */

void desenharFase(fases *f, jogador *j) {
    if (!f || !j) return;

    // 1. DESENHA O PORTAL
    if (f->saida.ativo) {
        
        // 🔑 NOVO: Desenha a textura do portal se ela estiver carregada
        if (f->saida.textura.id != 0) {
            DrawTextureEx(
                f->saida.textura, 
                f->saida.posicao, 
                0.0f,              
                f->saida.tamanho.x / f->saida.textura.width, // Escala X
                WHITE              
            );
        } else {
            // Fallback (Mantém o desenho do retângulo se a textura falhar)
            DrawRectangleRec((Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100}, DARKBLUE);
            DrawRectangleLines(f->saida.posicao.x, f->saida.posicao.y, 60, 100, SKYBLUE);
        }
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
            DrawCircleV(c, o->tamanho.x/2, ORANGE); 
        }
    }

    // 3. DESENHA AS PLATAFORMAS
    for (int i = 0; i < f->quantidadePlataformas; i++) {
        Plataforma *plat = &f->plataformas[i];
        if (!plat->isVisivel) continue;

        Color corPlataforma = (plat->tipo == TIPO_PLATAFORMA_FIXA) ? DARKGRAY : GRAY;
        
        DrawRectangleRec(plat->rect, corPlataforma);
        DrawRectangleLinesEx(plat->rect, 2, BLACK);
    }
}