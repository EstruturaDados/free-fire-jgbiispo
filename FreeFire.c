#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/* Código da Ilha – Edição Free Fire
   Nível: Mestre
   Gerenciamento avançado de mochila com ordenação e busca binária.
*/

#define MAX_ITENS 10
#define MAX_NOME  50
#define MAX_TIPO  30

typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int quantidade;
    int prioridade; // 1 (baixa) a 5 (alta)
} Item;

typedef enum {
    ORDENAR_POR_NOME = 1,
    ORDENAR_POR_TIPO = 2,
    ORDENAR_POR_PRIORIDADE = 3
} CriterioOrdenacao;

/* Estado global simples da "mochila" */
static Item mochila[MAX_ITENS];
static int numItens = 0;
static bool ordenadaPorNome = false;  // controle para busca binária

/* ---------- Utilidades ---------- */

static void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}

static void chomp(char *s) {
    size_t n = strlen(s);
    if (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[n-1] = '\0';
}

static int stricmp_pt(const char *a, const char *b) { // case-insensitive simples
    unsigned char ca, cb;
    while (*a && *b) {
        ca = (unsigned char) tolower((unsigned char)*a);
        cb = (unsigned char) tolower((unsigned char)*b);
        if (ca != cb) return (int)ca - (int)cb;
        a++; b++;
    }
    return (int)(unsigned char)tolower((unsigned char)*a) -
           (int)(unsigned char)tolower((unsigned char)*b);
}

static void limparTela(void) {
    // Simples: imprime várias linhas em branco.
    for (int i = 0; i < 30; i++) puts("");
}

/* ---------- Exibição ---------- */

static void listarItens(void) {
    if (numItens == 0) {
        puts("Mochila vazia.");
        return;
    }
    printf("+----+-------------------------------+----------------------+-----------+------------+\n");
    printf("| #  | Nome                          | Tipo                 | Quantidade| Prioridade |\n");
    printf("+----+-------------------------------+----------------------+-----------+------------+\n");
    for (int i = 0; i < numItens; i++) {
        printf("| %2d | %-29s | %-20s | %9d | %10d |\n",
               i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
    }
    printf("+----+-------------------------------+----------------------+-----------+------------+\n");
}

static void exibirMenu(void) {
    printf("======== GERENCIAMENTO DA MOCHILA ========\n");
    printf("Itens: %d/%d | Ordenada por nome: %s\n",
           numItens, MAX_ITENS, ordenadaPorNome ? "SIM" : "NAO");
    puts("------------------------------------------");
    puts("1. Adicionar um item");
    puts("2. Remover um item");
    puts("3. Listar todos os itens");
    puts("4. Ordenar os itens por criterio (nome, tipo, prioridade)");
    puts("5. Realizar busca binaria por nome");
    puts("0. Sair");
    puts("------------------------------------------");
    printf("Escolha: ");
}

/* ---------- Ordenação (Insertion Sort) ---------- */

static int comparar(const Item *a, const Item *b, CriterioOrdenacao crit) {
    switch (crit) {
        case ORDENAR_POR_NOME:
            return stricmp_pt(a->nome, b->nome); // A-Z
        case ORDENAR_POR_TIPO:
            return stricmp_pt(a->tipo, b->tipo); // A-Z
        case ORDENAR_POR_PRIORIDADE:
            // Maior prioridade primeiro (descendente)
            if (a->prioridade != b->prioridade) return b->prioridade - a->prioridade;
            // Desempate por nome
            return stricmp_pt(a->nome, b->nome);
        default:
            return 0;
    }
}

static long insertionSort(CriterioOrdenacao crit) {
    long comparacoes = 0;
    for (int i = 1; i < numItens; i++) {
        Item chave = mochila[i];
        int j = i - 1;
        // Move enquanto elemento[j] > chave conforme critério
        while (j >= 0) {
            comparacoes++;
            if (comparar(&mochila[j], &chave, crit) > 0) {
                mochila[j + 1] = mochila[j];
                j--;
            } else {
                break;
            }
        }
        mochila[j + 1] = chave;
    }
    // Controle da flag de ordenação por nome
    ordenadaPorNome = (crit == ORDENAR_POR_NOME);
    return comparacoes;
}

static void menuDeOrdenacao(void) {
    if (numItens == 0) {
        puts("Nada para ordenar. A mochila esta vazia.");
        return;
    }
    puts("Escolha o criterio de ordenacao:");
    puts("1. Nome (A-Z)");
    puts("2. Tipo (A-Z)");
    puts("3. Prioridade (5 -> 1)");
    printf("Criterio: ");
    int c;
    if (scanf("%d", &c) != 1) { limparBufferEntrada(); puts("Entrada invalida."); return; }
    limparBufferEntrada();

    CriterioOrdenacao crit;
    if (c == 1) crit = ORDENAR_POR_NOME;
    else if (c == 2) crit = ORDENAR_POR_TIPO;
    else if (c == 3) crit = ORDENAR_POR_PRIORIDADE;
    else { puts("Opcao invalida."); return; }

    long cmp = insertionSort(crit);
    printf("Itens ordenados. Comparacoes realizadas: %ld\n", cmp);
}

/* ---------- Inserir / Remover ---------- */

static void inserirItem(void) {
    if (numItens >= MAX_ITENS) {
        puts("Mochila cheia! Remova algo antes de adicionar novo item.");
        return;
    }

    Item novo;
    memset(&novo, 0, sizeof(novo));

    printf("Nome do item: ");
    if (!fgets(novo.nome, sizeof(novo.nome), stdin)) return;
    chomp(novo.nome);
    if (novo.nome[0] == '\0') { puts("Nome nao pode ser vazio."); return; }

    printf("Tipo do item: ");
    if (!fgets(novo.tipo, sizeof(novo.tipo), stdin)) return;
    chomp(novo.tipo);
    if (novo.tipo[0] == '\0') { puts("Tipo nao pode ser vazio."); return; }

    printf("Quantidade (inteiro >= 0): ");
    if (scanf("%d", &novo.quantidade) != 1 || novo.quantidade < 0) {
        limparBufferEntrada();
        puts("Quantidade invalida.");
        return;
    }

    printf("Prioridade (1 a 5): ");
    if (scanf("%d", &novo.prioridade) != 1 || novo.prioridade < 1 || novo.prioridade > 5) {
        limparBufferEntrada();
        puts("Prioridade invalida.");
        return;
    }
    limparBufferEntrada();

    mochila[numItens++] = novo;
    ordenadaPorNome = false; // perdeu a ordenacao por nome
    puts("Item adicionado com sucesso!");
}

static void removerItem(void) {
    if (numItens == 0) { puts("Mochila vazia."); return; }

    char alvo[MAX_NOME];
    printf("Digite o nome exato do item a remover: ");
    if (!fgets(alvo, sizeof(alvo), stdin)) return;
    chomp(alvo);

    int idx = -1;
    for (int i = 0; i < numItens; i++) {
        if (stricmp_pt(mochila[i].nome, alvo) == 0) { idx = i; break; }
    }

    if (idx == -1) {
        puts("Item nao encontrado.");
        return;
    }

    // Compacta o vetor
    for (int i = idx; i < numItens - 1; i++) {
        mochila[i] = mochila[i + 1];
    }
    numItens--;
    puts("Item removido.");
    // A remocao nao garante manter a ordenacao por nome (elementos mudaram de posicao)
    ordenadaPorNome = false;
}

/* ---------- Busca Binária (por nome) ---------- */

static void buscaBinariaPorNome(void) {
    if (numItens == 0) { puts("Mochila vazia."); return; }
    if (!ordenadaPorNome) {
        puts("A mochila precisa estar ORDENADA POR NOME para usar a busca binaria.");
        puts("Va ao menu de ordenacao e escolha 'Nome (A-Z)'.");
        return;
    }

    char chave[MAX_NOME];
    printf("Nome a buscar: ");
    if (!fgets(chave, sizeof(chave), stdin)) return;
    chomp(chave);

    int ini = 0, fim = numItens - 1;
    long comparacoes = 0;
    while (ini <= fim) {
        int meio = ini + (fim - ini) / 2;
        int cmp = stricmp_pt(mochila[meio].nome, chave);
        comparacoes++;
        if (cmp == 0) {
            printf("Encontrado!\n");
            printf("Nome: %s | Tipo: %s | Quantidade: %d | Prioridade: %d\n",
                   mochila[meio].nome, mochila[meio].tipo, mochila[meio].quantidade, mochila[meio].prioridade);
            printf("Comparacoes realizadas: %ld\n", comparacoes);
            return;
        } else if (cmp < 0) {
            ini = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    printf("Item '%s' nao encontrado. Comparacoes realizadas: %ld\n", chave, comparacoes);
}

/* ---------- Main ---------- */

int main(void) {
    int opcao;
    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            puts("Entrada invalida.");
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                limparTela();
                inserirItem();
                break;
            case 2:
                limparTela();
                removerItem();
                break;
            case 3:
                limparTela();
                listarItens();
                break;
            case 4:
                limparTela();
                menuDeOrdenacao();
                break;
            case 5:
                limparTela();
                buscaBinariaPorNome();
                break;
            case 0:
                puts("Saindo... Boa fuga da ilha!");
                break;
            default:
                puts("Opcao invalida.");
        }
        if (opcao != 0) {
            puts("\nPressione ENTER para continuar...");
            getchar();
            limparTela();
        }
    } while (opcao != 0);

    return 0;
}
