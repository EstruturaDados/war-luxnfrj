// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.
#define MAX_TERRITORIOS 5
#define MAX_MISSOES 100
#define TAM_STRING 100
// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[TAM_STRING];
    char cor[TAM_STRING];
    int tropas;
} Territorio;

void lerLinha(char *dest, size_t tam) {
    if (fgets(dest, (int)tam, stdin)) {
        size_t n = strlen(dest);
        if (n && dest[n-1] == '\n') dest[n-1] = '\0';
        else { // se estourar o buffer, limpa resto da linha
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
        }
    } else {
        dest[0] = '\0';
    }
}

// util: limpa o '\n' que sobra após scanf
void limparBuffer(void) {
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
    Territorio mapa[MAX_TERRITORIOS];

    printf("=== Cadastro Inicial dos Territorios ===\n\n");

    // Entrada: nome, cor, tropas para cada um dos 5
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("Territorio %d/%d\n", i + 1, MAX_TERRITORIOS);

        printf("  Nome: ");
        lerLinha(mapa[i].nome, sizeof mapa[i].nome);

        printf("  Cor do Exercito: ");
        lerLinha(mapa[i].cor, sizeof mapa[i].cor);

        int ok = 0;
        while (!ok) {
            printf("  Numero de Tropas (>= 0): ");
            if (scanf("%d", &mapa[i].tropas) == 1 && mapa[i].tropas >= 0) {
                ok = 1;
            } else {
                printf("  Valor invalido. Tente novamente.\n");
            }
            limparBuffer();
        }
        printf("\n");
    }

    // Exibição organizada (tabela simples)
    printf("=== Estado Atual do Mapa ===\n");
    printf("+-----+------------------------------+------------------------------+--------+\n");
    printf("| ID  | Nome                         | Cor do Exercito             | Tropas |\n");
    printf("+-----+------------------------------+------------------------------+--------+\n");
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("| %3d | %-28.28s | %-28.28s | %6d |\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("+-----+------------------------------+------------------------------+--------+\n");

    return 0;
}