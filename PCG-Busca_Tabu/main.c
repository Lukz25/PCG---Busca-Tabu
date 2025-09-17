#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/// definições de parâmetros
#define MAX_ITER 1000
#define TABU_TAM 10

#define MAXIMO_CIDADES 20

/// tipo de solução
typedef struct {
    float fluxo_trafego; /// entre 0 e 1, onde 1 = 100% congestionado
    float tamanho_rota; ///em km
} adjacencia;

typedef struct {
    /// Qual é a melhor ordem das cidades que devem ser acessadas para a rota dos ciclistas
    int caminhos[MAXIMO_CIDADES];
} solucao;

/// lista tabu
solucao lista_tabu[TABU_TAM];
int tabu_index = 0;

/// funções auxiliares
solucao gerar_solucao_aleatoria() {
    solucao s;
    int i;
    /// Inicializa a solução
    for ( i = 0; i < MAXIMO_CIDADES; i ++){
        s.caminhos[i] = i;
    }

    for ( i = 0; i < MAXIMO_CIDADES/2; i++){
        s.caminhos[i] = s.caminhos[rand()%MAXIMO_CIDADES];
    }

    return s;
}

bool criterio_parada(int iteracao) {
    return iteracao >= MAX_ITER;
}

void gerar_vizinhanca(solucao atual, solucao vizinhos[], int num_vizinhos) {

    int i,j;
    for (i = 0; i < num_vizinhos; i++) {
        /// Copia os valores do vetor atual
        for(j = 0; j < MAXIMO_CIDADES; j++){
            vizinhos[i].caminhos[j] = atual.caminhos[j];
        }
        vizinhos[i].caminhos[i] = vizinhos[i].caminhos[rand()%MAXIMO_CIDADES];
    }
}

float FUNCAO_DE_CUSTO(adjacencia m[][MAXIMO_CIDADES], solucao alvo){
    float valor = 0;
    int j;
    for(j = 0; j < MAXIMO_CIDADES; j++){
        int origem = alvo.caminhos[j], destino = alvo.caminhos[(j+1)%MAXIMO_CIDADES];
            adjacencia valor_alvo = m[origem][destino];
            /// Da enfaze em soluçoes que priorizam o menor fluxo nas rotas, com pouca influencia no tamanho da rota
            valor += (0.8*valor_alvo.fluxo_trafego + 0.2*valor_alvo.tamanho_rota);
            /// Se quiser dar enfaze nos valores de distancia, eh soh fazer isso:
            /// (0.2*valor_alvo.fluxo_trafego + 0.8*valor_alvo.tamanho_rota)
            /// Nao precisa necessariamente somar 1 os valores de pesos, mas estamos fazendo assim
            /// para fazer media ponderada
        }

    return valor;
}

solucao selecionar_melhor(solucao vizinhos[], int num_vizinhos, adjacencia m[][MAXIMO_CIDADES]) {
    solucao melhor = vizinhos[0];
    int i;
    float melhor_valor = FUNCAO_DE_CUSTO(m, melhor);
    for (i = 1; i < num_vizinhos; i++) {
        float valor = FUNCAO_DE_CUSTO(m, vizinhos[i]);
        if (valor <= melhor_valor) {
            melhor = vizinhos[i];
            melhor_valor = valor;

        }
    }
    return melhor;
}

bool esta_na_lista_tabu(solucao s, adjacencia m [][MAXIMO_CIDADES]) {
    int i;
    float valor_tabu, valor_solucao;
    valor_solucao = FUNCAO_DE_CUSTO(m, s);

    for (i = 0; i < TABU_TAM; i++) {

        valor_tabu = FUNCAO_DE_CUSTO(m, lista_tabu[i]);

        if (valor_tabu == valor_solucao) {
            return true;
        }

    }
    return false;
}

void atualizar_lista_tabu(solucao s) {
    lista_tabu[tabu_index] = s;
    tabu_index = (tabu_index + 1) % TABU_TAM;
}

/// função principal
int main() {
    int iteracao = 0;
    float valor_atual, valor_global;
    /// Inicializar a matriz de adjacencia das rotas da cidade
    /// Se isso for implementado em uma maior escala, os dados
    /// devem ser obtidos via API de GPS
    adjacencia rotas[MAXIMO_CIDADES][MAXIMO_CIDADES];
    int i, j;

    for (i = 0; i < MAXIMO_CIDADES; i ++){
        for (j = 0; j < MAXIMO_CIDADES; j ++){
            if(i == j){
                rotas[i][j].fluxo_trafego = 0;
                rotas[i][j].tamanho_rota = 0;
            }
            else{
                rotas[i][j].fluxo_trafego = rand() % 10000 /100.;
                rotas[i][j].tamanho_rota = rand() % 10000 /100.;
            }

        }
    }


    solucao solucao_atual = gerar_solucao_aleatoria();
    solucao melhor_global = solucao_atual;

    while (!criterio_parada(iteracao)) {
        solucao vizinhanca[10];
        int num_vizinhos = 10;

        gerar_vizinhanca(solucao_atual, vizinhanca, num_vizinhos);
        solucao melhor_vizinho = selecionar_melhor(vizinhanca, num_vizinhos, rotas);

        if (esta_na_lista_tabu(melhor_vizinho, rotas)) {
            continue;/// ignora solução tabu
        } else {
            solucao_atual = melhor_vizinho;

            valor_atual = FUNCAO_DE_CUSTO(rotas, solucao_atual);
            valor_global = FUNCAO_DE_CUSTO(rotas, melhor_global);

            if (valor_atual > valor_global) {
                melhor_global = solucao_atual;
            }
        }

        atualizar_lista_tabu(melhor_vizinho);
        iteracao++;
    }

    printf("melhor solucao encontrada: %.2lf\n", valor_global);
    return 0;
}
