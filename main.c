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


/**
 * @brief Cria uma matriz de inteiros com tamanho parametrizado.
 * 
 * Recebe os números de linhas e de colunas, e verifica os seus valores. Depois, aloca
 * dinamicamente a memória para a matriz de inteiros com o tamanho parametrizado, com
 * todos os valores iniciados com zero. 
 * Cada linha é um ponteiro que aponta para uma coluna.
 * 
 * @param linhas O número de linhas da matriz;
 * @param colunas O número de colunas da matriz;
 * @return O ponteiro da matriz.
 * 
 * @authors Flávio Giraldeli e Gabriel Augusto.
 */
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


/**
 * @brief Libera a memória de uma matriz de inteiros.
 * 
 * Recebe os números de linhas e de colunas da matriz, seguidos do próprio vetor da
 * matriz, e checa os seus valores. Depois, libera coluna por coluna da matriz, e ao
 * final libera a própria matriz.
 * 
 * @param linhas O número de linhas da matriz;
 * @param colunas O número de colunas da matriz;
 * @param O ponteiro da matriz.
 * 
 * @authors Flávio Giraldeli e Gabriel Augusto.
 */
void liberar_matriz_int(int linhas, int colunas, int **matriz)
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
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}


int main() {
    srand(1);

    int linhas = 5, colunas = 5;
    int **matriz = alocar_matriz_int(linhas, colunas);

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
    
    liberar_matriz_int(linhas, colunas, matriz);

    return 0;
}