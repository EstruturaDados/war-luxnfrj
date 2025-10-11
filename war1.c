// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
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
// Compilar:  gcc -std=c99 -Wall -Wextra -O2 war.c -o war
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Constantes Globais ---
#define MAX_TERRITORIOS 5
#define MAX_MISSOES 100
#define TAM_STRING 100

// --- Tipos/Enums ---

typedef enum {
    COR_VERMELHO = 0,
    COR_AZUL,
    COR_VERDE,
    COR_AMARELO,
    COR_NEUTRO
} Cor;

typedef struct {
    char nome[TAM_STRING];
    Cor dono;
    int tropas;
} Territorio;

typedef enum {
    MISSAO_CONQUISTAR_N = 1,   // dados: nTerritorios
    MISSAO_DESTRUIR_COR  = 2,  // dados: corAlvo
    MISSAO_DOMINAR_TODOS = 3   // sem dados
} TipoMissao;

typedef struct {
    TipoMissao tipo;
    int dado; // nTerritorios para CONQUISTAR_N, ou Cor alvo para DESTRUIR_COR
} Missao;

// --- Protótipos das Funções ---
// Setup e memória
Territorio* alocarMapa(size_t qtd);
void inicializarTerritorios(Territorio* mapa, size_t qtd);
void liberarMemoria(Territorio** mapa);

// Interface com o usuário
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio* mapa, size_t qtd);
void exibirMissao(const Missao* m);

// Lógica principal do jogo
void faseDeAtaque(Territorio* mapa, size_t qtd, Cor jogador);
int simularAtaque(Territorio* mapa, size_t qtd, int origem, int destino);
Missao sortearMissao(void);
int verificarVitoria(const Territorio* mapa, size_t qtd, const Missao* m, Cor jogador);

// Utilitárias
void limparBufferEntrada(void);
const char* nomeCor(Cor c);
int lerInteiro(const char* prompt, int min, int max);

// --- Função Principal (main) ---
int main(void) {
    srand((unsigned)time(NULL));

    // 1) Configuração inicial
    size_t qtd = MAX_TERRITORIOS;
    Territorio* mapa = alocarMapa(qtd);
    if (!mapa) {
        fprintf(stderr, "Falha ao alocar mapa.\n");
        return 1;
    }
    inicializarTerritorios(mapa, qtd);

    Cor jogador = COR_AZUL; // “eu” jogo de azul :)
    Missao missao = sortearMissao();

    // 2) Loop principal
    int opcao;
    int venceu = 0;
    do {
        puts("\n================== ESTADO ATUAL ==================");
        exibirMapa(mapa, qtd);
        puts("==================================================");
        puts("SUA MISSAO:");
        exibirMissao(&missao);
        puts("==================================================");

        exibirMenuPrincipal();
        opcao = lerInteiro("Escolha: ", 0, 2);

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, qtd, jogador);
                break;
            case 2:
                venceu = verificarVitoria(mapa, qtd, &missao, jogador);
                if (venceu) {
                    puts("\n>>> VOCE CUMPRIU A MISSAO! GG, cria. <<<");
                } else {
                    puts("\nAinda nao cumpriu a missao. Continuuuuua!");
                }
                break;
            case 0:
                puts("\nEncerrando o jogo. Valeu!");
                break;
            default:
                puts("Opcao invalida.");
                break;
        }

        if (!venceu && opcao != 0) {
            puts("\nPressione ENTER para continuar...");
            limparBufferEntrada();
            getchar();
        }
    } while (opcao != 0 && !venceu);

    // 3) Limpeza
    liberarMemoria(&mapa);
    return 0;
}

// ============================================================================
// Implementação das Funções
// ============================================================================

// alocarMapa()
// Aloca dinamicamente o vetor de territorios usando calloc.
Territorio* alocarMapa(size_t qtd) {
    return (Territorio*)calloc(qtd, sizeof(Territorio));
}

// inicializarTerritorios()
// Preenche dados iniciais: nomes, donos e tropas.
// Obs: design simples: alguns territorios ja com jogador e inimigos neutros.
void inicializarTerritorios(Territorio* mapa, size_t qtd) {
    if (!mapa || qtd < 5) return;

    snprintf(mapa[0].nome, TAM_STRING, "Artemis");
    mapa[0].dono = COR_AZUL;
    mapa[0].tropas = 5;

    snprintf(mapa[1].nome, TAM_STRING, "Borealis");
    mapa[1].dono = COR_NEUTRO;
    mapa[1].tropas = 3;

    snprintf(mapa[2].nome, TAM_STRING, "Cetus");
    mapa[2].dono = COR_VERMELHO;
    mapa[2].tropas = 4;

    snprintf(mapa[3].nome, TAM_STRING, "Draco");
    mapa[3].dono = COR_NEUTRO;
    mapa[3].tropas = 2;

    snprintf(mapa[4].nome, TAM_STRING, "Erebus");
    mapa[4].dono = COR_VERDE;
    mapa[4].tropas = 4;
}

// liberarMemoria()
// Libera e zera ponteiro para evitar uso apos free.
void liberarMemoria(Territorio** mapa) {
    if (mapa && *mapa) {
        free(*mapa);
        *mapa = NULL;
    }
}

// exibirMenuPrincipal()
// Menu de acoes disponiveis.
void exibirMenuPrincipal(void) {
    puts("\n---------------- MENU ----------------");
    puts("1 - Fase de ATAQUE");
    puts("2 - Verificar VITORIA");
    puts("0 - Sair");
    puts("-------------------------------------");
}

// exibirMapa()
// Tabela amigavel; apenas leitura (const).
void exibirMapa(const Territorio* mapa, size_t qtd) {
    if (!mapa) return;
    printf("%-3s | %-12s | %-10s | %-6s\n", "ID", "Territorio", "Dono", "Tropas");
    puts("-----------------------------------------------");
    for (size_t i = 0; i < qtd; ++i) {
        printf("%-3zu | %-12s | %-10s | %-6d\n",
               i, mapa[i].nome, nomeCor(mapa[i].dono), mapa[i].tropas);
    }
}

// exibirMissao()
// Tradução legível da missão sorteada.
void exibirMissao(const Missao* m) {
    if (!m) return;
    switch (m->tipo) {
        case MISSAO_CONQUISTAR_N:
            printf("Conquistar pelo menos %d territorios.\n", m->dado);
            break;
        case MISSAO_DESTRUIR_COR:
            printf("Destruir o exercito da cor %s (reduzir todos os territorios dessa cor a zero tropas, ou conquistar).\n",
                   nomeCor((Cor)m->dado));
            break;
        case MISSAO_DOMINAR_TODOS:
            puts("Dominar TODOS os territorios do mapa.");
            break;
        default:
            puts("Missao invalida.");
    }
}

// faseDeAtaque()
// Interface para um ataque: escolhe origem (seu) e destino (nao seu).
void faseDeAtaque(Territorio* mapa, size_t qtd, Cor jogador) {
    if (!mapa) return;

    puts("\n--- FASE DE ATAQUE ---");
    exibirMapa(mapa, qtd);

    int origem = lerInteiro("ID do territorio de ORIGEM (seu): ", 0, (int)qtd - 1);
    int destino = lerInteiro("ID do territorio de DESTINO (inimigo): ", 0, (int)qtd - 1);

    int resultado = simularAtaque(mapa, qtd, origem, destino);
    if (resultado == 1) {
        puts("Ataque executado.");
    } else {
        puts("Ataque nao ocorreu (entrada invalida ou condicoes insuficientes).");
    }

    // Observacao: sem movimento livre; so 1 tropa min obrigatoria ao conquistar.
}

// simularAtaque()
// Mecânica simples: 1 dado atacante (1..6) vs 1 dado defensor (1..6).
// Regras minimas: origem precisa ser do jogador e ter >1 tropa; destino nao pode ser do jogador.
// Se atacante ganha: defensor perde 1 tropa; se tropas do destino chegam a 0, territorio muda de dono e
//  move 1 tropa do atacante para ocupar.
// Se defensor ganha ou empata: atacante perde 1 tropa.
int simularAtaque(Territorio* mapa, size_t qtd, int origem, int destino) {
    if (!mapa) return 0;
    if (origem < 0 || destino < 0 || (size_t)origem >= qtd || (size_t)destino >= qtd) return 0;
    if (origem == destino) return 0;

    // Validacoes basicas
    if (mapa[origem].dono == mapa[destino].dono) {
        puts("Nao faz sentido atacar um territorio da mesma cor.");
        return 0;
    }
    if (mapa[origem].tropas <= 1) {
        puts("Origem precisa ter mais de 1 tropa para atacar.");
        return 0;
    }
    if (mapa[destino].tropas <= 0) {
        puts("Destino ja esta vazio (inconsistencia).");
        return 0;
    }

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;
    printf("Dados: Atacante=%d  x  Defensor=%d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        // defensor perde 1
        mapa[destino].tropas -= 1;
        puts("Atacante venceu a rolagem! Defensor perde 1 tropa.");
        if (mapa[destino].tropas <= 0) {
            // conquista
            puts("Territorio conquistado! Movendo 1 tropa para ocupar.");
            mapa[destino].dono = mapa[origem].dono;
            mapa[destino].tropas = 1;
            mapa[origem].tropas -= 1; // moveu 1 tropa
        }
    } else {
        // defensor vence em empate ou maior
        mapa[origem].tropas -= 1;
        puts("Defensor segurou! Atacante perde 1 tropa.");
    }

    return 1;
}

// sortearMissao()
// Gera uma missão simples aleatória, com fallback para evitar "destruir" cor inexistente.
Missao sortearMissao(void) {
    Missao m;
    int r = (rand() % 3) + 1; // 1..3
    m.tipo = (TipoMissao)r;
    switch (m.tipo) {
        case MISSAO_CONQUISTAR_N:
            m.dado = 3; // alvo razoável para 5 territorios
            break;
        case MISSAO_DESTRUIR_COR: {
            // Evitar sortear NEUTRO
            int coresValidas[] = { COR_VERMELHO, COR_VERDE, COR_AMARELO };
            m.dado = coresValidas[rand() % 3];
            break;
        }
        case MISSAO_DOMINAR_TODOS:
            m.dado = 0;
            break;
        default:
            // fallback
            m.tipo = MISSAO_CONQUISTAR_N;
            m.dado = 3;
            break;
    }
    return m;
}

// verificarVitoria()
// Implementa regras para os tres tipos de missão.
int verificarVitoria(const Territorio* mapa, size_t qtd, const Missao* m, Cor jogador) {
    if (!mapa || !m) return 0;

    switch (m->tipo) {
        case MISSAO_CONQUISTAR_N: {
            int meus = 0;
            for (size_t i = 0; i < qtd; ++i)
                if (mapa[i].dono == jogador) meus++;
            return meus >= m->dado;
        }
        case MISSAO_DESTRUIR_COR: {
            Cor alvo = (Cor)m->dado;
            int existeAlvo = 0;
            for (size_t i = 0; i < qtd; ++i) {
                if (mapa[i].dono == alvo && mapa[i].tropas > 0) {
                    existeAlvo = 1;
                    break;
                }
            }
            // Vitória se não restar nenhum território com a cor alvo
            return !existeAlvo;
        }
        case MISSAO_DOMINAR_TODOS: {
            for (size_t i = 0; i < qtd; ++i)
                if (mapa[i].dono != jogador) return 0;
            return 1;
        }
        default:
            return 0;
    }
}

// limparBufferEntrada()
// Dribla sujeira do stdin quando misturamos scanf e getchar.
void limparBufferEntrada(void) {
    int c;
    // consome ate \n ou EOF
    while ((c = getchar()) != '\n' && c != EOF) { /* no-op */ }
}

// nomeCor()
// Mapinha de enum -> string (const char* imutavel).
const char* nomeCor(Cor c) {
    switch (c) {
        case COR_VERMELHO: return "Vermelho";
        case COR_AZUL:     return "Azul";
        case COR_VERDE:    return "Verde";
        case COR_AMARELO:  return "Amarelo";
        case COR_NEUTRO:   return "Neutro";
        default:           return "Desconhecida";
    }
}

// lerInteiro()
// Input robusto com limites [min, max]; lida com lixo no buffer.
int lerInteiro(const char* prompt, int min, int max) {
    int v;
    int ok = 0;
    do {
        printf("%s", prompt);
        int lidos = scanf("%d", &v);
        if (lidos == 1 && v >= min && v <= max) {
            ok = 1;
        } else {
            puts("Entrada invalida.");
        }
        limparBufferEntrada();
    } while (!ok);
    return v;
}
