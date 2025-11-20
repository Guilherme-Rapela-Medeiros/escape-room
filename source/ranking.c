#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/ranking.h"

// Insere em ordem crescente de tempo
void inserirRanking(ranking **head, const char *nome, float tempo) {
    ranking *novo = malloc(sizeof(ranking));
    if (!novo) return;

    strcpy(novo->nome, nome);
    novo->tempo = tempo;
    novo->prox = NULL;

    // Inserção no começo da lista
    if (*head == NULL || tempo < (*head)->tempo) {
        novo->prox = *head;
        *head = novo;
        return;
    }

    // Inserção no meio ou final
    ranking *aux = *head;
    while (aux->prox != NULL && aux->prox->tempo < tempo) {
        aux = aux->prox;
    }

    novo->prox = aux->prox;
    aux->prox = novo;
}

// Salvar ranking em arquivo texto
void salvarRanking(ranking *head, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    ranking *aux = head;
    while (aux) {
        fprintf(fp, "%s %.2f\n", aux->nome, aux->tempo);
        aux = aux->prox;
    }

    fclose(fp);
}

// Carregar ranking de arquivo
void carregarRanking(ranking **head, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;

    char nome[TAMANHO_NOME];
    float tempo;

    while (fscanf(fp, "%s %f", nome, &tempo) == 2) {
        inserirRanking(head, nome, tempo);
    }

    fclose(fp);
}

// Liberar lista encadeada
void liberarRanking(ranking **head) {
    ranking *aux;

    while (*head) {
        aux = *head;
        *head = (*head)->prox;
        free(aux);
    }
}