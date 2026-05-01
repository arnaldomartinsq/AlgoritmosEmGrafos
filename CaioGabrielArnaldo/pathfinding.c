#include "pathfinding.h"
#include "comparisons.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// --------------------------- Estruturas ---------------------------

typedef struct {
    int id;
    int distancia;      // G - custo do início até esse nó
    int heuristica;     // H - estimativa até o destino (heurística)
    int custoTotal;     // F = G + H
} HeapNode;

typedef struct {
    HeapNode* dados;
    int tamanho;
    int capacidade;
} MinHeap;

typedef struct {
    int id;
    int antecessor;
    int distancia;
} DijkstraInfo;

// --------------------------- Funções da Heap ---------------------------

// Cria uma heap mínima com capacidade especificada
MinHeap* criarHeap(int capacidade) {
    MinHeap* heap = malloc(sizeof(MinHeap));
    heap->dados = malloc(sizeof(HeapNode) * capacidade);
    heap->tamanho = 0;
    heap->capacidade = capacidade;
    return heap;
}

// Troca dois nós na heap
void trocar(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

// Move um nó para cima na heap para manter a propriedade de heap mínima
void subir(MinHeap* heap, int idx) {
    while (ehMaior(idx, 0)) {
        int pai = (idx - 1) / 2;
        if (ehMenor(heap->dados[idx].custoTotal, heap->dados[pai].custoTotal)) {
            trocar(&heap->dados[idx], &heap->dados[pai]);
            idx = pai;
        } else {
            break;
        }
    }
}

// Move um nó para baixo na heap para manter a propriedade de heap mínima
void descer(MinHeap* heap, int idx) {
    while (true) {
        int menor = idx;
        int esq = 2 * idx + 1;
        int dir = 2 * idx + 2;

        int custoAtual = heap->dados[idx].custoTotal;
        int custoEsq = (ehMenor(esq, heap->tamanho)) ? heap->dados[esq].custoTotal : 1000001;
        int custoDir = (ehMenor(dir, heap->tamanho)) ? heap->dados[dir].custoTotal : 1000001;

        if (ehMenor(custoEsq, custoAtual)) menor = esq;
        if (ehMenor(custoDir, heap->dados[menor].custoTotal)) menor = dir;

        if (ehIgual(menor, idx)) break;

        trocar(&heap->dados[idx], &heap->dados[menor]);
        idx = menor;
    }
}

// Insere um novo nó na heap
void inserirHeap(MinHeap* heap, int id, int distancia, int heuristica) {
    HeapNode novo;
    novo.id = id;
    novo.distancia = distancia;
    novo.heuristica = heuristica;
    novo.custoTotal = distancia + heuristica;

    heap->dados[heap->tamanho] = novo;
    subir(heap, heap->tamanho);
    heap->tamanho++;
}

// Remove e retorna o nó com menor custo total
HeapNode extrairMinimo(MinHeap* heap) {
    HeapNode raiz = heap->dados[0];
    heap->dados[0] = heap->dados[--heap->tamanho];
    descer(heap, 0);
    return raiz;
}

// Verifica se a heap está vazia
int heapVazio(MinHeap* heap) {
    return ehIgual(heap->tamanho, 0);
}

// --------------------------- Heurística de Manhattan ---------------------------

int calcularHeuristica(int atual, int fim, int numCols) {
    int linhaAtual = atual / numCols;
    int colAtual = atual % numCols;
    int linhaFim = fim / numCols;
    int colFim = fim % numCols;
    return abs(linhaAtual - linhaFim) + abs(colAtual - colFim);
}

// --------------------------- Função Principal: A* ---------------------------

int findPath(int** adjMatrix, int numNos, int inicio, int fim, Noh** caminho, int* visitados) {
    Noh* caminho_local = NULL;
    DijkstraInfo* infoNos = malloc(sizeof(DijkstraInfo) * numNos);
    MinHeap* heap = criarHeap(numNos);
    bool* fechado = calloc(numNos, sizeof(bool));
    int ordem = 0;

    for (int i = 0; ehMenor(i, numNos); i++) {
        infoNos[i].id = i;
        infoNos[i].distancia = ehIgual(i, inicio) ? 0 : 1000000;
        infoNos[i].antecessor = -1;
        visitados[i] = -1;
    }

    int numCols = (int)sqrt(numNos); // Supondo matriz quadrada
    inserirHeap(heap, inicio, 0, calcularHeuristica(inicio, fim, numCols));

    while (!heapVazio(heap)) {
        HeapNode atual = extrairMinimo(heap);

        if (fechado[atual.id]) continue;
        fechado[atual.id] = true;
        visitados[atual.id] = ordem++;

        if (ehIgual(atual.id, fim)) break;

        for (int v = 0; ehMenor(v, numNos); v++) {
            int peso = adjMatrix[atual.id][v];
            if (ehMaior(peso, 0) && !fechado[v]) {
                int novaDist = infoNos[atual.id].distancia + peso;
                if (ehMenor(novaDist, infoNos[v].distancia)) {
                    infoNos[v].distancia = novaDist;
                    infoNos[v].antecessor = atual.id;
                    int heuristica = calcularHeuristica(v, fim, numCols);
                    inserirHeap(heap, v, novaDist, heuristica);
                }
            }
        }
    }

    if (ehMenor(infoNos[fim].distancia, 1000000)) {
        inserirCaminhoDijkstra(&caminho_local, infoNos, fim);
        *caminho = caminho_local;
        free(infoNos);
        free(heap->dados);
        free(heap);
        free(fechado);
        return 1;
    }

    free(infoNos);
    free(heap->dados);
    free(heap);
    free(fechado);
    return 0;
}

// Constrói o caminho mínimo a partir do vetor de antecessores
void inserirCaminhoDijkstra(Noh** caminho, DijkstraInfo* infoNos, int fim) {
    int i = fim;
    while (!ehIgual(i, -1)) {
        adicionarNoh(caminho, i);
        i = infoNos[i].antecessor;
    }
}
