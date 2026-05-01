#ifndef COMPARISONS_H
#define COMPARISONS_H

extern int comparisons;

// Função para zerar o contador de compara��es
void resetComparisons();

// Função para retornar o n�mero atual de compara��es
int getComparisons();

// Funções de comparação
int ehMaior(int a, int b);
int ehMenor(int a, int b);
int ehIgual(int a, int b);
int ehMaiorOuIgual(int a, int b);
int ehMenorOuIgual(int a, int b);
int ehDiferente(int a, int b);

#endif // COMPARISONS_H
