#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "../includes/structs.h" 
#include "../includes/fases.h"

#define MAX 12
static Vector2 bases[4][MAX] = {0}; 

// 🔑 DEFINIÇÕES DE CAMINHOS CORRIGIDAS PARA REFLETIR SUA ESTRUTURA DE PASTAS
#define CAMINHO_PORTAL "recursos/portal.png" // Mantenha, mas verifique se "recursos" está na pasta correta.
#define CAMINHO_OBS_HORIZONTAL "assets/imagens/mapas/obstaculo_direita.png" 
#define CAMINHO_OBS_VERTICAL_CIMA "assets/imagens/mapas/obstaculo_cima.png" 
#define CAMINHO_OBS_VERTICAL_ESQ "assets/imagens/mapas/obstaculo_esquerda.png" 
#define CAMINHO_OBS_VERTICAL_BAIXO "assets/imagens/mapas/obstaculo_baixo.png"


/* --- Funções Auxiliares Básicas --- */

int col(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

void desenharJogadorManual(jogador *j) {
    // ... (Mantido sem alteração) ...
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

    // 🔑 NOVO: CARREGA MÚLTIPLAS TEXTURAS DE OBSTÁCULOS
    Texture2D obsHorizontalTex = LoadTexture(CAMINHO_OBS_HORIZONTAL);
    Texture2D obsVerticalCimaTex = LoadTexture(CAMINHO_OBS_VERTICAL_CIMA);
    Texture2D obsVerticalEsqTex = LoadTexture(CAMINHO_OBS_VERTICAL_ESQ);
    Texture2D obsVerticalBaixoTex = LoadTexture(CAMINHO_OBS_VERTICAL_BAIXO);

    // Adiciona as texturas ao campo (apenas para ter uma referência para a limpeza)
    // 💡 OBS: O struct fases foi projetado para apenas UMA textura de obstáculo. 
    // Para simplificar a limpeza, vamos usar um array temporário e liberá-lo no `acabarFases`.
    Texture2D obsTexturas[4] = {
        obsHorizontalTex, 
        obsVerticalCimaTex, 
        obsVerticalEsqTex, 
        obsVerticalBaixoTex
    };
    
    // 🔑 CARREGA A TEXTURA DO PORTAL
    Texture2D portalTex = LoadTexture(CAMINHO_PORTAL);


    for (int p = 0; p < qtd; p++) {
        f[p].numero = p + 1;
        f[p].completo = 0;
        
        // Atribui a primeira textura carregada (apenas para manter a compatibilidade com a struct)
        if (p == 0) f[p].texturaObstaculo = obsHorizontalTex; 
        
        int n = 4 + p * 2;
        if (p == 3) n = 10;
        if (n > MAX) n = MAX;
        
        f[p].quantidadeObstaculos = n;
        f[p].obstaculos = malloc(sizeof(obstaculo) * n);
        if (!f[p].obstaculos) continue;

        f[p].posicaoinicial = (Vector2){80, H / 2.0f};
        
        // Inicializa o portal e atribui a textura
        f[p].saida = (portal){{W - 100, H / 2.0f - 50}, {60, 100}, 1, portalTex};

        float start = 250;
        float gap = (f[p].saida.posicao.x - 60 - start) / n;

        // --- Inicialização de Obstáculos ---
        for (int i = 0; i < n; i++) {
            obstaculo *o = &f[p].obstaculos[i];
            
            // tipo 0 (Horizontal) - Movimenta em Y
            // tipo 1 (Vertical/Móvel) - Movimenta em X, com variações de textura
            int tipo = i % 2; 
            
            float by = H * 0.5f;
            Vector2 tamanho;
            
            if (tipo == 0) { 
                tamanho = (Vector2){60, 20};
                if (i % 3 == 0) by -= 120;
                if (i % 3 == 2) by += 120;
                // 🔑 ATRIBUIÇÃO: Textura Horizontal para o tipo 0
                o->textura = obsHorizontalTex; 
            } else { 
                tamanho = (Vector2){40, 40};
                if (i % 4 == 1) by -= 70;
                if (i % 4 == 3) by += 70;

                // 🔑 ATRIBUIÇÃO DINÂMICA: Variamos as texturas para o tipo 1
                int obs_variation = i % 3;
                if (obs_variation == 0) o->textura = obsVerticalCimaTex;
                else if (obs_variation == 1) o->textura = obsVerticalEsqTex;
                else o->textura = obsVerticalBaixoTex;
            }

            Vector2 posicao = (Vector2){start + gap * i + gap * 0.5f, by};

            o->hitbox = (Rectangle){posicao.x, posicao.y, tamanho.x, tamanho.y};
            
            bases[p][i] = posicao;
            
            float amp = 60.0f + p * 15.0f;
            if (p == 3) amp = 140.0f;
            
            o->velocidade = (Vector2){(float)tipo, amp}; // Tipo 0 = Mov. Y, Tipo 1 = Mov. X
            o->ativo = 1;
        }

        // --- Inicialização de Plataformas (Mantida) ---
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
        if (qtd > 0) {
             // O portal é sempre o mesmo
             if (f[0].saida.textura.id != 0) UnloadTexture(f[0].saida.textura);
             
             // 🔑 LIBERA AS 4 TEXTURAS CARREGADAS UMA ÚNICA VEZ
             if (f[0].obstaculos && f[0].obstaculos[0].textura.id != 0) {
                 UnloadTexture(f[0].obstaculos[0].textura); // Referência para Horizontal
             }
             if (f[0].obstaculos && f[0].obstaculos[1].textura.id != 0) {
                 UnloadTexture(f[0].obstaculos[1].textura); // Referência para Cima
             }
             if (f[0].obstaculos && f[0].obstaculos[3].textura.id != 0) {
                 UnloadTexture(f[0].obstaculos[3].textura); // Referência para Esquerda
             }
             if (f[0].obstaculos && f[0].obstaculos[5].textura.id != 0) {
                 UnloadTexture(f[0].obstaculos[5].textura); // Referência para Baixo
             }
        }

        for (int i = 0; i < qtd; i++) { 
            free(f[i].obstaculos); 
            free(f[i].plataformas);
        }
    }
}

/* 🚀 FUNÇÃO DE ATUALIZAÇÃO 🚀 */
void atualizarFases(fases *f, jogador *j, TelaAtual *tela) {
    // ... (Mantido sem alteração) ...
    if (!f || !j) return;
    
    // Checagem de Limite e Game Over
    if (j->hitbox_jogador.x > f->saida.posicao.x) j->hitbox_jogador.x = f->saida.posicao.x;
    if (j->vida <= 0) { *tela = TELA_GAME_OVER; return; }

    int idx = (f->numero > 0) ? f->numero - 1 : 0;
    
    // CORREÇÃO: Calculamos a Posição Y no Frame ANTERIOR
    float y_anterior = j->hitbox_jogador.y - j->velocidade_vertical;
    bool playerOnPlatform = false; 

    // --- Lógica de Plataformas (Mantida) ---
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

        // LÓGICA DE POUSO SÓLIDO
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

        // o->velocidade.x == 0 -> Tipo 0 (Oscilação em Y - Obstáculo Horizontal)
        // o->velocidade.x != 0 -> Tipo 1 (Oscilação em X - Obstáculo Vertical/Móvel)
        if (o->velocidade.x == 0) np.y += osc; 
        else np.x += osc; 
        
        // 💡 Atualiza a posição da hitbox (Rectangle)
        o->hitbox.x = np.x;
        o->hitbox.y = np.y;
        
        // Limites (Aplicados à hitbox)
        if (o->hitbox.x < 150) o->hitbox.x = 150;
        if (o->hitbox.x + o->hitbox.width > f->saida.posicao.x - 10) 
            o->hitbox.x = f->saida.posicao.x - 10 - o->hitbox.width;
        
        if (o->hitbox.y < 30) o->hitbox.y = 30;
        if (o->hitbox.y + o->hitbox.height > GetScreenHeight() - 20) 
            o->hitbox.y = GetScreenHeight() - 20 - o->hitbox.height;

        // 🔑 Colisão usando o novo campo hitbox
        if (col(j->hitbox_jogador, o->hitbox)) {
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

/* --- Função de Desenho (Mantida) --- */

void desenharFase(fases *f, jogador *j) {
    // ... (Mantido sem alteração) ...
    if (!f || !j) return;

    // 1. DESENHA O PORTAL
    if (f->saida.ativo) {
        if (f->saida.textura.id != 0) {
            DrawTextureEx(
                f->saida.textura, 
                f->saida.posicao, 
                0.0f, 
                f->saida.tamanho.x / f->saida.textura.width, 
                WHITE 
            );
        } else {
            // Fallback 
            DrawRectangleRec((Rectangle){f->saida.posicao.x, f->saida.posicao.y, 60, 100}, DARKBLUE);
            DrawRectangleLines(f->saida.posicao.x, f->saida.posicao.y, 60, 100, SKYBLUE);
        }
    }

    // 2. DESENHA OS OBSTÁCULOS
    for (int i = 0; i < f->quantidadeObstaculos; i++) {
        obstaculo *o = &f->obstaculos[i];
        if (!o->ativo) continue;

        Texture2D tex = o->textura;
        Rectangle r = o->hitbox; // Usa a hitbox como destino
        
        if (tex.id != 0) {
            // 🔑 Desenha a textura PNG do obstáculo
            Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };

            // DrawTexturePro escala a textura para preencher o retângulo 'r' (hitbox)
            DrawTexturePro(
                tex, 
                source, 
                r, 
                (Vector2){ 0, 0 }, 
                0.0f, 
                WHITE
            );
        } else {
            // FALLBACK: Desenho geométrico antigo (usa o tipo para saber qual desenhar)
            if (o->velocidade.x == 0) { // Objeto tipo Horizontal (oscilação Y)
                DrawRectangleRec(r, BROWN); 
                DrawTriangle(
                    (Vector2){r.x + r.width, r.y + r.height/2}, 
                    (Vector2){r.x + r.width - 15, r.y + r.height}, 
                    (Vector2){r.x + r.width - 15, r.y}, 
                    RED
                );
            } else { // Objeto tipo Vertical (oscilação X)
                Vector2 center = { r.x + r.width/2, r.y + r.height/2 };
                DrawCircleV(center, r.width/2, ORANGE); 
                DrawCircleV(center, r.width/4, YELLOW); 
            }
        }
    }

    // 3. DESENHA AS PLATAFORMAS (Mantido o código)
    for (int i = 0; i < f->quantidadePlataformas; i++) {
        Plataforma *plat = &f->plataformas[i];
        if (!plat->isVisivel) continue;

        Color corPlataforma = (plat->tipo == TIPO_PLATAFORMA_FIXA) ? DARKGRAY : GRAY;
        
        DrawRectangleRec(plat->rect, corPlataforma);
        DrawRectangleLinesEx(plat->rect, 2, BLACK);
    }
}