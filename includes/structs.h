#ifndef STRUCTS_H
#define STRUCTS_H

#define TAMANHO_NOME 50

#include <raylib.h>

#define TRUE 1
#define FALSE 0

typedef enum {
    TIPO_PLATAFORMA_FIXA,
    TIPO_PLATAFORMA_MOVEL_X, // Movimento Horizontal
    TIPO_PLATAFORMA_MOVEL_Y  // Movimento Vertical
} TipoPlataforma;

typedef struct {
    Rectangle rect;          // Posição (x, y) e tamanho (width, height)
    TipoPlataforma tipo;     // FIXA, MOVEL_X ou MOVEL_Y

    // Campos de Movimento
    float limiteA;       // Ponto mínimo (X ou Y)
    float limiteB;       // Ponto máximo (X ou Y)
    float velocidade;    // Velocidade de deslocamento (ex: 2.0f)
    int direcao;         // +1 (A -> B) ou -1 (B -> A)
    int isVisivel;      // Se a plataforma deve ser desenhada
} Plataforma;

typedef enum {
    TELA_MENU,
    TELA_TUTORIAL,
    TELA_FASES,
    TELA_JOGO,
    TELA_RANKING,
    TELA_FINAL,
    TELA_GAME_OVER,
} TelaAtual;

typedef enum {
    SPRITE_PARADO = 0,
    SPRITE_ESQUERDA,
    SPRITE_DIREITA,
    SPRITE_CIMA,
    SPRITE_BAIXO,
    SPRITE_TOTAL
} PlayerSpriteID;

typedef struct jogador {
    Rectangle hitbox_jogador;
    int vida;
    char nome[TAMANHO_NOME];
    float velocidade_vertical;
    Texture2D sprites[SPRITE_TOTAL];
    Texture2D textura;
    PlayerSpriteID sprite_atual;
    int estaNoChao;
} jogador;

typedef struct obstaculo{
    // 💡 MODIFICAÇÃO: Usando Rectangle para facilitar colisão e desenho
    Rectangle hitbox; 
    
    // Vector2 posicao; // Removido
    // Vector2 tamanho; // Removido
    
    Vector2 velocidade;
    int ativo;
    
    // 💡 MODIFICAÇÃO: Campo para armazenar o PNG do obstáculo
    Texture2D textura; 
} obstaculo;

typedef struct portal{
    Vector2 posicao;
    Vector2 tamanho;
    int ativo;
    // 🔑 CAMPO MANTIDO: Para a textura do PNG
    Texture2D textura; 
} portal;

typedef struct fases{
    int numero;
    
    // Elementos de Fase
    obstaculo *obstaculos;
    int quantidadeObstaculos;
    
    // 💡 MODIFICAÇÃO: Textura padrão para os obstáculos da fase, carregada uma única vez
    Texture2D texturaObstaculo;
    
    // 💡 MODIFICAÇÃO: Array para as novas plataformas
    Plataforma *plataformas;
    int quantidadePlataformas;
    
    portal saida;
    Vector2 posicaoinicial;
    int completo;
} fases;

typedef struct ranking {
    char nome[TAMANHO_NOME];
    double tempo; 
    struct ranking *prox; 
} ranking;

typedef struct jogo {
    jogador jogador;
    int FimDeJogo;
    int telaAtual;
    int FaseAtual;
    fases fases[4];
    Texture2D mapas[4]; 
    ranking *Head;
} jogo;

#endif