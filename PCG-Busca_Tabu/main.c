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
    /// Qual é a melhor ordem das cidades que devme ser acessadas para a roda dos ciclistas
    int caminhos[MAXIMO_CIDADES];
    float valor;
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

bool esta_na_lista_tabu(solucao s) {
    int i;
    for (i = 0; i < TABU_TAM; i++) {
        if (lista_tabu[i].valor == s.valor) {
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
        int num_vizinhos;

        gerar_vizinhanca(solucao_atual, vizinhanca, num_vizinhos);
        solucao melhor_vizinho = selecionar_melhor(vizinhanca, num_vizinhos, rotas);

        if (esta_na_lista_tabu(melhor_vizinho)) {
            continue;/// ignora solução tabu
        } else {
            solucao_atual = melhor_vizinho;
            if (solucao_atual.valor > melhor_global.valor) {
                melhor_global = solucao_atual;
            }
        }

        atualizar_lista_tabu(melhor_vizinho);
        iteracao++;
    }

    printf("melhor solução encontrada: %d\n", melhor_global.valor);
    return 0;
}
