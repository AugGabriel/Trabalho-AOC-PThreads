#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Mensagens de erro
#define ERRO_ALOCACAO_MEMORIA \
    "** Erro: memoria insuficiente **\n"
#define ERRO_LINHAS_COLUNAS_NAO_POSITIVO \
    "** Erro: numeros de linhas e de colunas devem ser maiores que 0 **\n"


// Alturas e larguras da matriz e dos macro blocos.
#define ALTURA_MATRIZ 7000
#define LARGURA_MATRIZ 7000

#define ALTURA_BLOCOS 100
#define LARGURA_BLOCOS 100


// Macros function-like

/**
 * @brief Gera um número aleatório de 0 a 31999.
 * 
 * Gera um inteiro aleatório e retorna o resto da divisão dele por 32000.
 */
#define GERAR_NUMERO (rand() % 32000)


// Gerenciamento das threads
#define MAX_THREADS 80

pthread_mutex_t mutex_cont_primos;
pthread_mutex_t mutex_blocos_restantes;


// Variáveis globais
int **matriz;
int cont_primos;
int blocos_restantes;


// Funções

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
 * @brief Inicializa/reinicia as variáveis globais que podem sofrer alteração durante a execução.
 * 
 * Uma vez que algumas variáveis globais podem sofrer alteração pelas duas execuções, essa função seta os valores corretos para essas variáveis
 * antes da execução serial, e entre a serial e a paralela.
 * 
 * @param total_blocos O total de macroblocos que será usado, que também será o número inicial de blocos restantes.
 */
void inicializar_variaveis_globais(int total_blocos) {
    cont_primos = 0;
    blocos_restantes = total_blocos;
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
        exit(EXIT_FAILURE);
    }

    if (entrada < 2) {
        return 0;
    }
    if (entrada == 2) {
        return 1;
    }

    int raiz_entrada = ceil(sqrt(entrada));

    for (int divisor = 2; divisor <= raiz_entrada; divisor++) {
        if (entrada % divisor == 0) {
            return 0;
        }
    }
    return 1;
}


/**
 * @brief Cria uma matriz de inteiros com tamanho parametrizado.
 * 
 * Recebe os números de linhas e de colunas, e verifica os seus valores. 
 * Depois, aloca dinamicamente a memória para a matriz de inteiros com o tamanho parametrizado. 
 * Internamente, há apenas uma linha que contém os ponteiros para cada coluna.
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
        exit(EXIT_FAILURE);
    }
    
    // Alocação da linha de colunas da matriz
    int **matriz = malloc(linhas * sizeof(int*));        // Vetor de ponteiros para int

    if (matriz == NULL) {
        printf(ERRO_ALOCACAO_MEMORIA);
        exit(EXIT_FAILURE);
    }

    // Alocação das colunas da matriz
    for (int i = 0; i < linhas; i++) {
        matriz[i] = malloc(colunas * sizeof(int));       // Vetores de valores do tipo int

        if (matriz[i] == NULL) {
            printf(ERRO_ALOCACAO_MEMORIA);
            exit(EXIT_FAILURE);
        }
    }

    // Retorno
    return (matriz);
}


/**
 * @brief Libera a memória de uma matriz de inteiros.
 * 
 * Recebe os números de linhas e de colunas da matriz, seguidos do próprio vetor da matriz, e checa os seus valores.
 * Depois, libera coluna por coluna da matriz, e ao final libera a linha de colunas da matriz.
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
        exit(EXIT_FAILURE);;
    }

    if (linhas < 1 || colunas < 1) {
        printf(ERRO_LINHAS_COLUNAS_NAO_POSITIVO);
        exit(EXIT_FAILURE);;
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
 * Aloca uma matriz e inicializa suas células com números inteiros aleatórios de 0 a 31999.
 * 
 * @param linhas O número de linhas da matriz;
 * @param colunas O número de colunas da matriz;
 * @return O ponteiro para a matriz gerada.
 */
int **gerar_matriz(int linhas, int colunas) {
    int **matriz = alocar_matriz_int(linhas, colunas);

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = GERAR_NUMERO;
        }
    }

    return matriz;
}


/** 
 * @brief Conta o número de primos no macrobloco da matriz, retornando a quantidade.
 * 
 * Dadas as posições de início e as posições limite do macrobloco, retorna a quantidade de números primos contidos nele.
 * 
 * @param matriz O ponteiro para a matriz de inteiros;
 * @param i_inicio A posição i de início do macrobloco (inclusivo);
 * @param i_limite A posição i limite do macrobloco (exclusivo);
 * @param j_inicio A posição j de início do macrobloco (inclusivo);
 * @param j_limite A posição j limite do macrobloco (exclusivo);
 * @return A contagem de números primos na matriz contidos no macrobloco.
 */
int contar_primos(
    int **matriz, 
    int i_inicio, int i_limite,
    int j_inicio, int j_limite
) {
    if (i_inicio < 0 || i_limite < 0 || j_inicio < 0 || j_limite < 0) {
        printf(
            "Todas as entradas de inteiro da função `contar_primos` precisam ser"
            " positivas. Valores: \n"
        );
        printf("Posição inicial: %d, %d\n", i_inicio, j_inicio);
        printf("Posição limite: %d, %d\n", i_limite, j_limite);

        exit(EXIT_FAILURE);
    }

    int contador = 0;

    for (int i = i_inicio; i < i_limite; i++) {
        for (int j = j_inicio; j < j_limite; j++) {
            if (eh_primo(matriz[i][j])) {
                contador++;
            }
        }
    }

    return contador;
}


/**
 * @brief Rotina executada por cada thread. Enquanto houver blocos não contabilizados, ela designa um bloco para si mesma, e conta a quantidade de primos nele.
 *
 * Enquanto ainda há trabalho para executar, a função designa para si mesma um macrobloco, 
 * recebendo o seu índice enquanto decrementa a quantidade de blocos disponíveis.
 * 
 * Depois, a função calcula as coordenadas inicial e final do bloco, chama a função auxiliar para contar a quantidade de números primos no macrobloco, 
 * e incrementa o contador de primos global.
 * 
 * Os macroblocos são indexados em ordem de linha: primeiro por linha, depois por coluna, seguindo o seguinte modelo:
 *
 * ┌───┬───┐
 * │ 1 │ 2 │
 * ├───┼───┤
 * │ 3 │ 4 │
 * └───┴───┘
 */
void *rotina(void *param) {
    // Quantidade de macroblocos que cabem horizontalmente (um do lado do outro) na matriz
    int quant_blocos_horizontal = LARGURA_MATRIZ / LARGURA_BLOCOS;

    while (1) {
        // Acesso à variável global `blocos_restantes`
        pthread_mutex_lock(&mutex_blocos_restantes);

        if (blocos_restantes <= 0) {
            pthread_mutex_unlock(&mutex_blocos_restantes);
            break;
        }

        int indice_bloco = --blocos_restantes;
        pthread_mutex_unlock(&mutex_blocos_restantes);

        // Nenhum acesso a variáveis globais        
        int index_bloco_eixo_y = indice_bloco / quant_blocos_horizontal;     // Índex do bloco no eixo X
        int index_bloco_eixo_x = indice_bloco % quant_blocos_horizontal;     // Índex do bloco no eixo Y
        
        int i_inicio = index_bloco_eixo_y * ALTURA_BLOCOS;
        int i_fim = i_inicio + ALTURA_BLOCOS;
        
        int j_inicio = index_bloco_eixo_x * LARGURA_BLOCOS;
        int j_fim = j_inicio + LARGURA_BLOCOS;

        int primos_bloco = contar_primos(
            matriz,
            i_inicio,
            i_fim,
            j_inicio,
            j_fim
        );
        
        // Acesso à variável global `cont_primos`
        pthread_mutex_lock(&mutex_cont_primos);
        cont_primos += primos_bloco;
        pthread_mutex_unlock(&mutex_cont_primos);
    }
        
    pthread_exit(0);
}


int main() {
    // ===== Inicializações =====
    verificar_tamanhos_matriz_macroblocos();
    srand(1);

    matriz = gerar_matriz(ALTURA_MATRIZ, LARGURA_MATRIZ);

    int total_blocos = \
        (ALTURA_MATRIZ * LARGURA_MATRIZ) \
        / (ALTURA_BLOCOS * LARGURA_BLOCOS);

    inicializar_variaveis_globais(total_blocos);

    pthread_mutex_init(&mutex_cont_primos, NULL);
    pthread_mutex_init(&mutex_blocos_restantes, NULL);

    // ===== Processamento serial =====
    struct timespec inicio_serial, fim_serial;

    clock_gettime(CLOCK_MONOTONIC, &inicio_serial);
    cont_primos += contar_primos(matriz, 0, ALTURA_MATRIZ, 0, LARGURA_MATRIZ);
    clock_gettime(CLOCK_MONOTONIC, &fim_serial);

    printf("Contagem serial de números primos: %d\n", cont_primos);

    // ===== Reinicialização de variáveis globais =====
    inicializar_variaveis_globais(total_blocos);
    
    // ===== Processamento paralelo =====
    pthread_t threads[MAX_THREADS];

    struct timespec inicio_paralelo, fim_paralelo;
    clock_gettime(CLOCK_MONOTONIC, &inicio_paralelo);
    
    for (int i = 0; i < MAX_THREADS && i < total_blocos; i++) {
        pthread_create(&(threads[i]), NULL, rotina, NULL);
    }
    
    for (int i = 0; i < MAX_THREADS && i < total_blocos; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &fim_paralelo);
    printf("Contagem paralela de números primos: %d\n", cont_primos);

    // ===== Resultados =====
    double tempo_serial = \
        (fim_serial.tv_sec - inicio_serial.tv_sec)
        + (fim_serial.tv_nsec - inicio_serial.tv_nsec)
        / 1e9;

    double tempo_paralelo = \
        (fim_paralelo.tv_sec - inicio_paralelo.tv_sec)
        + (fim_paralelo.tv_nsec - inicio_paralelo.tv_nsec) 
        / 1e9;

    printf("\nTempo serial: %.4f\n", tempo_serial);
    printf("Tempo paralelo: %.4f\n", tempo_paralelo);
    printf("Speedup: %.2fx\n", tempo_serial / tempo_paralelo);
    
    // ===== Liberações =====
    liberar_matriz_int(ALTURA_MATRIZ, LARGURA_MATRIZ, matriz);

    pthread_mutex_destroy(&mutex_cont_primos);
    pthread_mutex_destroy(&mutex_blocos_restantes);

    // ===== Retorno =====
    return 0;
}
