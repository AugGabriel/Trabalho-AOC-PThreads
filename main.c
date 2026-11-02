#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Mensagens de erro
#define ERRO_ALOCACAO_MEMORIA \
    "** Erro: memoria insuficiente **\n"
#define ERRO_LINHAS_COLUNAS_NAO_POSITIVO \
    "** Erro: numeros de linhas e de colunas devem ser maiores que 0 **\n"

// Alturas e larguras da matriz e dos macro blocos.
#define ALTURA_MATRIZ 1
#define LARGURA_MATRIZ 1
#define ALTURA_BLOCOS 1
#define LARGURA_BLOCOS 1


/**
 * @brief Verifica se o número informado é primo.
 * 
 * Elimina casos excepcionais (0, 1 e 2), e testa divisores até a raíz quadrada do
 * número para os casos convencionais.
 * 
 * @param entrada O número a ser testado.
 * @return 1 se o número for primo, 0 se não for.
 */
int eh_primo(int entrada) {
    if (entrada < 2) {
        return 0;
    }
    if (entrada == 2) {
        return 1;
    }

    for (int divisor = 2; divisor <= ceil(sqrt(entrada)); divisor++) {
        if (entrada % divisor == 0) {
            return 0;
        }
    }
    return 1;
}


/**
 * @brief Gera um número aleatório de 0 a 31999.
 * 
 * Gera um inteiro aleatório e retorna o resto da divisão dele por 32000.
 * 
 * @return O número aleatório gerado.
 */
int gerar_numero() {
    return rand() % 32000;
}


int **alocar_matriz_int(int linhas, int colunas) {
    // Verificação da entrada
    if (linhas < 1 || colunas < 1) {
        printf(ERRO_LINHAS_COLUNAS_NAO_POSITIVO);
        return(NULL);
    }
    
    // Alocação das linhas da matriz
    int **matriz = calloc(linhas, sizeof(int*));        // Vetor de ponteiros para int

    if (matriz == NULL) {
        printf(ERRO_ALOCACAO_MEMORIA);
        return (NULL);
    }

    // Alocação das colunas da matriz
    for (int i = 0; i < linhas; i++) {
        matriz[i] = calloc(colunas, sizeof(int));       // Vetores de valores do tipo int

        if (matriz[i] == NULL) {
            printf(ERRO_ALOCACAO_MEMORIA);
            return (NULL);
        }
    }

    // Retorno
    return (matriz);
}

void liberar_matriz_int(int linhas, int colunas, float **matriz)
{
    // Verificação das entradas
    if (matriz == NULL) {
        printf("** Erro: matriz nao definida **\n");
        return;
    }

    if (linhas < 1 || colunas < 1) {
        printf(ERRO_LINHAS_COLUNAS_NAO_POSITIVO);
        return;
    }

    // Liberação da matriz
    for(int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}


int main() {
    srand(1);

    for (int i = 0; i < 30; i++)
    {
        printf("%d\colunas", gerar_numero());
    }
    
    return 0;
}