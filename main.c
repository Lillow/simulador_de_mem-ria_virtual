#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para representar uma página
typedef struct {
    int id; // Identificador da página
    int last_used; // Último acesso para o algoritmo LRU
} Page;

// Função para encontrar o índice da página na memória
int find_page_index(Page* memory, int page_id, int mem_size) {
    for (int i = 0; i < mem_size; i++) {
        if (memory[i].id == page_id) {
            return i; // Retorna o índice da página na memória
        }
    }
    return -1; // Retorna -1 se a página não estiver na memória
}

// Algoritmo de substituição FIFO (First In First Out)
int fifo(Page* memory, int mem_size, int page_id) {
    static int oldest_page_index = 0; // Índice da página mais antiga na memória
    memory[oldest_page_index].id = page_id; // Substitui a página mais antiga
    oldest_page_index = (oldest_page_index + 1) % mem_size; // Atualiza o índice da página mais antiga
    return oldest_page_index;
}

// Algoritmo de substituição LRU (Least Recently Used)
int lru(Page* memory, int mem_size, int page_id) {
    int least_recently_used = 0;
    for (int i = 1; i < mem_size; i++) {
        if (memory[i].last_used < memory[least_recently_used].last_used) {
            least_recently_used = i; // Encontra a página menos recentemente usada
        }
    }
    memory[least_recently_used].id = page_id; // Substitui a página menos recentemente usada
    return least_recently_used;
}

// Algoritmo de substituição Random
int random_algo(int mem_size) {
    return rand() % mem_size; // Retorna um índice aleatório na memória
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: %s [algoritmo] [arquivo] [tam_pagina] [tam_memoria]\n", argv[0]);
        return 1;
    }

    // Leitura dos argumentos
    char* algoritmo = argv[1];
    char* arquivo = argv[2];
    int tam_pagina = atoi(argv[3]);
    int tam_memoria = atoi(argv[4]);

    // Verifica se os argumentos estão dentro dos limites
    if (tam_pagina < 2 || tam_pagina > 64 || tam_memoria < 128 || tam_memoria > 16384) {
        printf("Tamanho de página ou memória fora dos limites permitidos.\n");
        return 1;
    }

    // Calcula o número de páginas e alocando memória para a memória principal
    int num_paginas = tam_memoria / tam_pagina;
    Page* memory = (Page*)malloc(num_paginas * sizeof(Page));

    // Inicializa a memória principal
    for (int i = 0; i < num_paginas; i++) {
        memory[i].id = -1; // -1 indica que a página está vazia
        memory[i].last_used = 0;
    }

    // Abre o arquivo de instruções
    FILE *file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Variáveis para contar os hits, misses e faults
    int page_hits = 0;
    int page_misses = 0;
    int page_faults = 0;

    // Processamento das instruções
    int page_id;
    while (fscanf(file, "%d", &page_id) != EOF) {
        // Verifica se a página está na memória
        int page_index = find_page_index(memory, page_id, num_paginas);
        if (page_index != -1) {
            page_hits++; // Hit
            memory[page_index].last_used++; // Atualiza o último acesso para LRU
        } else {
            page_misses++; // Miss
            page_faults++; // Fault
            // Determina qual algoritmo usar para substituição de página
            int new_page_index;
            if (strcmp(algoritmo, "lru") == 0) {
                new_page_index = lru(memory, num_paginas, page_id);
            } else if (strcmp(algoritmo, "fifo") == 0) {
                new_page_index = fifo(memory, num_paginas, page_id);
            } else if (strcmp(algoritmo, "random") == 0) {
                new_page_index = random_algo(num_paginas);
            } else {
                printf("Algoritmo de substituição inválido.\n");
                return 1;
            }
            memory[new_page_index].last_used = 1; // Reinicia o último acesso para LRU
        }
    }

    // Fecha o arquivo
    fclose(file);

    // Exibe o relatório
    printf("Page hits: %d\n", page_hits);
    printf("Page misses: %d\n", page_misses);
    printf("Page faults: %d\n", page_faults);

    // Libera a memória alocada
    free(memory);

    return 0;
}
