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
#define ALTURA_MATRIZ 9
#define LARGURA_MATRIZ 9

#define ALTURA_BLOCOS 3
#define LARGURA_BLOCOS 3

// Número de threads
#define NUM_THREADS 9


// Variáveis globais
int cont_primos = 0;
int **matriz;


// Structs
/**
 * @brief Struct para armazenar argumentos para passar para a rotina da thread.
 */
typedef struct args_rotina {
    int x_inicio;
    int x_limite;
    int y_inicio;
    int y_limite;
} ArgsRotina;


// Funções

/**
 * @brief Cria um novo struct de argumentos para a rotina da thread.
 * 
 * Aloca espaço para novo struct e repassa os valores dos parâmetros para ele.
 * 
 * @param x_inicio A coordenada x de início do macrobloco (inclusivo);
 * @param x_limite A coordenada x limite do macrobloco (exclusivo);
 * @param y_inicio A coordenada y de início do macrobloco (inclusivo);
 * @param y_limite A coordenada y limite do macrobloco (exclusivo);
 * @return Um ponteiro para o struct recém criado.
 */
ArgsRotina *criar_args(
    int x_inicio,
    int x_limite,
    int y_inicio,
    int y_limite
) {
    ArgsRotina *args_rotina = (ArgsRotina*)malloc(sizeof(ArgsRotina*));

    args_rotina->x_inicio = x_inicio;
    args_rotina->x_limite = x_limite;
    args_rotina->y_inicio = y_inicio;
    args_rotina->y_limite = y_limite;

    return args_rotina;
}


/**
 * @brief Verifica se as dimensões da matriz e dos macroblocos são válidos.
 * 
 * Verifica se todas as dimensões são positivas, e se as dimensões dos macroblocos são menores que e divisíveis pelas dimensões da matriz.
 */
void verificar_tamanhos_matriz_macroblocos() {
    int erro_encontrado = 0;

    // Verificação de tamanhos maiores que zero
    if (ALTURA_MATRIZ <= 0) {
        printf(
            "** Erro: a altura da matriz deve ser maior que zero **\n"
        );
        erro_encontrado = 1;
    }
    if (LARGURA_MATRIZ <= 0) {
        printf(
            "** Erro: a largura da matriz deve ser maior que zero **\n"
        );
        erro_encontrado = 1;
    }

    if (ALTURA_BLOCOS <= 0) {
        printf(
            "** Erro: a altura dos macroblocos deve ser maior que zero **\n"
        );
        erro_encontrado = 1;
    }
    if (LARGURA_BLOCOS <= 0) {
        printf(
            "** Erro: a largura dos macroblocos deve ser maior que zero **\n"
        );
        erro_encontrado = 1;
    }

    if (erro_encontrado) {
        exit(EXIT_FAILURE);
    }

    // Verificação de dimensões do bloco maior que dimensões da matriz
    if (ALTURA_BLOCOS > ALTURA_MATRIZ) {
        printf(
            "** Erro: a altura dos macroblocos deve ser menor ou igual à altura da "
            "matriz**\n"
        );
        erro_encontrado = 1;
    }
    if (LARGURA_BLOCOS > LARGURA_MATRIZ) {
        printf(
            "** Erro: a largura dos macroblocos deve ser menor ou igual à largura da "
            "matriz**\n"
        );
        erro_encontrado = 1;
    }

    if (erro_encontrado) {
        exit(EXIT_FAILURE);
    }

    // Verificação de dimensões do bloco divisíveis pelas dimensões da matriz
    if (ALTURA_MATRIZ % ALTURA_BLOCOS != 0) {
        printf(
            "** Erro: a altura dos macroblocos deve ser divisível pela altura da "
            "matriz**\n"
        );
        erro_encontrado = 1;
    }
    if (LARGURA_MATRIZ % LARGURA_BLOCOS != 0) {
        printf(
            "** Erro: a largura dos macroblocos deve ser divisível pela largura da "
            "matriz**\n"
        );
        erro_encontrado = 1;
    }

    if (erro_encontrado) {
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Verifica se o número informado é primo.
 * 
 * Elimina casos excepcionais (0, 1 e 2), e testa divisores até a raíz quadrada do número para os casos convencionais.
 * 
 * @param entrada O número a ser testado.
 * @return 1 se o número for primo, 0 se não for.
 */
int eh_primo(int entrada) {
    if (entrada < 0) {
        printf(
            "** Erro: parametro para verificacao precisa ser maior ou igual a 0 **\n"
        );
        return -1;
    }

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
 * Recebe os números de linhas e de colunas, e verifica os seus valores. 
 * Depois, aloca dinamicamente a memória para a matriz de inteiros com o tamanho parametrizado, com todos os valores iniciados com zero. 
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
 * Recebe os números de linhas e de colunas da matriz, seguidos do próprio vetor da matriz, e checa os seus valores.
 * Depois, libera coluna por coluna da matriz, e ao final libera a própria matriz.
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


/**
 * @brief Gera uma matriz de números aleatórios com tamanho parametrizado.
 * 
 * Aloca uma matriz usando o auxiliar `alocar_matriz_int`, e verifica se ela foi alocada com sucesso. 
 * Depois, itera por ela, preenchendo com números inteiros aleatórios, usando o auxiliar `gerar_numero`.
 * 
 * @param linhas O número de linhas da matriz;
 * @param colunas O número de colunas da matriz;
 * @return O ponteiro para a matriz gerada.
 */
int **gerar_matriz(int linhas, int colunas) {
    int **matriz = alocar_matriz_int(linhas, colunas);

    if (matriz == NULL) {
        return NULL;
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = gerar_numero();
        }
    }

    return matriz;
}


/** 
 * @brief Conta o número de primos no macrobloco da matriz, retornando a quantidade.
 * 
 * Dadas as coordenadas de início do macrobloco (inclusivo) e as coordenadas limite (exclusivo), retorna a quantidade de números primos no macrobloco.
 * 
 * @param matriz O ponteiro para a matriz de inteiros;
 * @param x_inicio A coordenada x de início do macrobloco (inclusivo);
 * @param x_limite A coordenada x limite do macrobloco (exclusivo);
 * @param y_inicio A coordenada y de início do macrobloco (inclusivo);
 * @param y_limite A coordenada y limite do macrobloco (exclusivo);
 * @return A contagem de números primos na matriz contidos no macrobloco.
 */
int contar_primos(
    int **matriz, 
    int x_inicio, int x_limite,
    int y_inicio, int y_limite
) {
    int contador = 0;

    for (int i = x_inicio; i < x_limite; i++) {
        for (int j = y_inicio; j < y_limite; j++) {
            if (eh_primo(matriz[i][j])) {
                contador++;
            }
        }
    }

    return contador;
}


/**
 * @brief A rotina para ser passada para a thread.
 * 
 * Recebe o struct de argumentos, chama a contagem de primos e incrementa a variável global.
 * 
 * @param param O struct de argumentos da rotina, que serão passados para a função de contagem de primos.
 */
void *rotina(void *param) {
    ArgsRotina *args_rotina = (ArgsRotina*)param;

    cont_primos += contar_primos(
        matriz,
        args_rotina->x_inicio,
        args_rotina->x_limite,
        args_rotina->y_inicio,
        args_rotina->y_limite
    );

    pthread_exit(0);
}


int main() {
    verificar_tamanhos_matriz_macroblocos();
    srand(1);
    matriz = gerar_matriz(ALTURA_MATRIZ, LARGURA_MATRIZ);

    pthread_t thread;

    pthread_create(
        &thread, NULL, rotina, criar_args(0, LARGURA_MATRIZ, 0, ALTURA_MATRIZ)
    );
    pthread_join(thread, NULL);

    printf("Contagem de números primos: %d\n", cont_primos);
    
    liberar_matriz_int(ALTURA_MATRIZ, LARGURA_MATRIZ, matriz);

    return 0;
}
