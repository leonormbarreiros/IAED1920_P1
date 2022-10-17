/*
 * Ficheiro: p1.c
 * Autor: Leonor Maria Barreiros; no. 95618
 * Descricao: programa que simula um sistema de logistica.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* **************************************************************        ************************************************************** */
/*                                                              CONSTANTES                                                              */
/* **************************************************************        ************************************************************** */

#define MAX_PRODUTOS 10000 /* no maximo 10000 produtos diferentes */
#define MAX_ENCOMENDAS 500 /* no maximo 500 encomendas */
#define MAX_PESO 200 /* peso maximo de uma encomenda */
#define MAX_CARACTERES 64 /* no maximo de caracteres da descricao (63 + '\0') */
#define FORA 0 /* estado -> auxiliar */
#define DENTRO 1 /* estado -> auxiliar */

/* **************************************************************        ************************************************************** */
/*                                                              ESTRUTURAS                                                              */
/* **************************************************************        ************************************************************** */

typedef struct produto /* caracterizacao de um produto */
{
    int identificador; /* identificador - inteiro em [0, 9999] */
    char descricao[MAX_CARACTERES]; /* descricao - string com [1,63] caracteres */
    int preco; /* preco - inteiro maior que 0 */
    int peso; /* peso - inteiro maior que 0 */
    int stock; /* quantidade em stock - inteiro maior ou igual a 0 */
}Produto;
typedef struct encomenda /* caracterizacao de uma encomenda - conjunto de produtos */
{
    int identificador; /* identificador - inteiro em [0, 499] */
    int stock; /* quantidade encomendada - inteiro maior ou igual a 0 */
}Encomenda;

/* **************************************************************        ************************************************************** */
/*                                                          VARIAVEIS GLOBAIS                                                           */
/* **************************************************************        ************************************************************** */

Produto Produtos_no_sistema[MAX_PRODUTOS]; /* vetor de produtos no sistema - no maximo 10000 */
Encomenda Encomendas_no_sistema[MAX_ENCOMENDAS][MAX_PESO]; /* vetor de encomendas no sistema - no maximo 500 com 200 produtos cada */
int id_produtos; /* numero de produtos no sistema */
int id_encomendas; /* numero de encomendas no sistema */
int id_prod_em_encomenda[MAX_ENCOMENDAS]; /* numero de produtos em cada encomenda */

/* **************************************************************        ************************************************************** */
/*                                                          PROTOTIPOS DE FUNCOES                                                       */
/* **************************************************************        ************************************************************** */

/* f. auxiliares */
int produto_no_sistema(int idp);
int produto_em_stock(int idp, int qtd);
int encomenda_no_sistema(int ide);
int peso_permitido(int ide, int idp, int qtd);
int produto_na_encomenda(int ide, int idp);
int calcula_custo(int ide, int n);
void maximo_produto(int idp);
void merge_produtos(Produto Produtos[], int esq, int drt, char parametro, int ide);
void junta_ordenados(Produto Produtos[], int esq, int meio, int drt, char parametro);
void imprime_vetor(Produto Produtos[], int no_elementos, char parametro, int ide);

/* f. principais */
void executa_a();
int executa_q();
void executa_N();
int executa_A();
int executa_r();
int executa_R();
int executa_C();
int executa_p();
int executa_E();
int executa_m();
void executa_l();
int executa_L();

/* **************************************************************        ************************************************************** */

int main(){
    
    /* VARIAVEIS LOCAIS */
    char comando; /* caracter que corresponde ao comando inserido*/

    while(1){ /* loop infinito - so sai quando houver break ou return */
        comando = getchar(); /* leitura do comando */
        switch (comando){
            case 'a':{  /* adiciona um novo produto */
                executa_a();
                break;
            }
            case 'q':{  /* adiciona stock a um produto existente */
                executa_q();
                break;
            }
            case 'N':{  /* cria uma nova encomenda */
                executa_N();
                break;
            }
            case 'A':{  /* adiciona um produto a uma encomenda */
                executa_A();
                break;
            }
            case 'r':{  /* remove stock a um produto existente */
                executa_r();
                break;
            }
            case 'R':{  /* remove um produto de uma encomenda */
                executa_R();
                break;
            }
            case 'C':{  /* calcula o custo de uma encomenda */
                executa_C();
                break;
            }
            case 'p':{  /* altera o preco de um produto existente */
                executa_p();
                break;
            }
            case 'E':{  /* retorna a descricao e quantidade de um produto numa encomenda */
                executa_E();
                break;
            }
            case 'm':{  /* retorna a encomenda em que um produto ocorre mais vezes */
                executa_m();
                break;
            }
            case 'l':{  /* lista todos os produtos existentes (ordem crescente de preco) */
                executa_l();
                break;
            }
            case 'L':{  /* lista todos os produtos de uma encomenda (ordem alfabetica) */
                executa_L();
                break;
            }
            case 'x':
                return EXIT_SUCCESS; /* termina com sucesso */
        }
        getchar(); /* ler o '\n' introduzido para executar outros comandos */
    }
    return EXIT_FAILURE; /* se chegar aqui, algo correu mal */
}

/* **************************************************************        ************************************************************** */
/*                                                              F. AUXILIARES                                                           */
/* **************************************************************        ************************************************************** */

/* funcao que diz se um produto esta ou nao no sistema */
int produto_no_sistema(int idp){ 
    /* o produto esta no sistema se a sua posicao no vetor ja tiver sido contabilizada pelo id_produtos */
    if (idp < id_produtos) return 1;
    else return 0;    
}

/* **************************************************************        ************************************************************** */

/* funcao que diz se um produto existe em stock na quantidade pretendida */
int produto_em_stock(int idp, int qtd){ 
    /* se o stock disponivel for menor que a quantidade devolve 0, senao 1 */
    if (Produtos_no_sistema[idp].stock < qtd) return 0;
    else return 1;  
}

/* **************************************************************        ************************************************************** */

/* funcao que diz se uma encomenda esta ou nao no sistema */
int encomenda_no_sistema(int ide){ 
    /* a encomenda esta no sistema se a sua posicao no vetor ja tiver sido contabilizada pelo id_encomendas */
    if (ide < id_encomendas) return 1;
    else return 0;
}

/* **************************************************************        ************************************************************** */

/* funcao que diz se a adicao do produto idp na quantidade qtd excede o peso permitido da encomenda ide */ 
int peso_permitido(int ide, int idp, int qtd){
    int i;
    int quantidade, identificador; /* variaveis auxiliares */
    int total=0; /* parametro que sera avaliado no fim */

    /* calcular o peso atual da encomenda */
    for (i=0; i<id_prod_em_encomenda[ide]; i++)
    {
        quantidade = Encomendas_no_sistema[ide][i].stock;
        identificador = Encomendas_no_sistema[ide][i].identificador;
        total += Produtos_no_sistema[identificador].peso * quantidade;
    }
    /* saber se ao adicionar o produto o peso vai ser excedido */
    total += qtd * Produtos_no_sistema[idp].peso;
    if (total > 200)
        return 0;
    else
        return 1;
}

/* **************************************************************        ************************************************************** */

/* funcao que diz se um produto se encontra numa encomenda e onde se encontra em caso afirmativo */
int produto_na_encomenda(int ide, int idp){
    int i, j =id_prod_em_encomenda[ide];

    for (i=0; i<j; i++){
        if (Encomendas_no_sistema[ide][i].identificador == idp)
            return i;   /* se encontrar o identificador devolve a localizacao do produto na encomenda */
    }
    return MAX_PESO;    /* se nao encontrar devolve um valor simbolico que o indica */
}

/* **************************************************************        ************************************************************** */

/* funcao que calcula o custo da encomenda com n produtos */
int calcula_custo(int ide, int n){
    int i, idp, qtd, valor; /* variaveis auxiliares */
    int total=0; /* valor que sera imprimido como retorno */

    for(i=0; i<n; i++){ /* para cada produto da encomenda multiplica a sua quantidade nela pelo preco */
        idp = Encomendas_no_sistema[ide][i].identificador;
        qtd = Encomendas_no_sistema[ide][i].stock;
        valor = Produtos_no_sistema[idp].preco;
        total += valor * qtd;
    }
    return total;
}

/* **************************************************************        ************************************************************** */

/* funcao que calcula a encomenda em que o produto ocorre mais vezes */
void maximo_produto(int idp){
    int resultado[2] = {0}; /* valores devolvidos */
    int i, j;
    int estado = FORA;

    for (i=0; i<id_encomendas; i++)
        for (j=0; j<id_prod_em_encomenda[i]; j++)
            /* ve se o produto esta na encomenda e se em maior quantidade do que o valor atual */
            if (Encomendas_no_sistema[i][j].identificador == idp && Encomendas_no_sistema[i][j].stock > resultado[1]){
                estado = DENTRO;    /* indica que ele esta em alguma encomenda */
                resultado[0] = i;   /* atualiza o resultado */
                resultado[1] = Encomendas_no_sistema[i][j].stock;
            }
    if (estado == DENTRO) /* se o produto estiver em alguma encomenda imprime aquela onde ele esta em maior quantidade */
        printf("Maximo produto %d %d %d.\n", idp, resultado[0], resultado[1]);
}

/* **************************************************************        ************************************************************** */

/* funcao que lista os produtos no sistema por ordem crescente de preco */
void merge_produtos(Produto Produtos[], int esq, int drt, char parametro, int ide){
    int meio; /* variavel auxiliar que indica onde de divide o vetor */
    if (esq<drt){
        meio = (esq+drt)/2; /* calcula o meio -> onde o divide */

        /* aplica o algoritmo as metades do vetor e ordena consoante o parametro */
        if (parametro == 'p'){ /* preco */
            merge_produtos(Produtos, esq, meio, 'p', 0);
            merge_produtos(Produtos, meio+1, drt, 'p', 0);
            junta_ordenados(Produtos, esq, meio, drt, 'p');
        }
        else{ /* descricao */
            merge_produtos(Produtos, esq, meio, 'd', ide);
            merge_produtos(Produtos, meio+1, drt, 'd', ide);
            junta_ordenados(Produtos, esq, meio, drt, 'd');
        }
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que ordena as metades do vetor e as junta -> imprime */
void junta_ordenados(Produto Produtos[], int esq, int meio, int drt, char parametro){
    int i, j, k; /* variaveis auxiliares */
    int lim_esq = meio-esq+1;
    int lim_drt = drt-meio; /* numero de produtos em cada sub-vetor */
    Produto Produtos_esq[MAX_PRODUTOS], Produtos_drt[MAX_PRODUTOS]; /* copias do vetor, mas ordenadas por preco */

    /* inicializar os vetores-copia - apenas com a descricao, preco e qtd - o resto nao interessa */
    for (i=0; i<lim_esq; i++){ /* vetor da primeira metade do sistema */
        Produtos_esq[i].preco = Produtos[esq+i].preco;
        Produtos_esq[i].stock = Produtos[esq+i].stock;
        strcpy(Produtos_esq[i].descricao, Produtos[esq+i].descricao);
    }
    for(j=0; j<lim_drt; j++){ /* vetor da segunda metade do sistema */
        Produtos_drt[j].preco = Produtos[meio+1+j].preco;
        Produtos_drt[j].stock = Produtos[meio+1+j].stock;
        strcpy(Produtos_drt[j].descricao, Produtos[meio+1+j].descricao);
    }

    /* inicializar o vetor-resultado com os sub-vetores ordenados */
    i=0; j=0; k=esq; /* reutilizar as variaveis */
    while (i<lim_esq && j<lim_drt){
        if ((parametro == 'p' && Produtos_esq[i].preco <= Produtos_drt[j].preco) ||
        (parametro == 'd' && strcmp(Produtos_esq[i].descricao, Produtos_drt[j].descricao) <= 0)){
            /* copia e atualiza o indice no vetor da esquerda */
            Produtos[k].preco = Produtos_esq[i].preco;
            Produtos[k].stock = Produtos_esq[i].stock;
            strcpy(Produtos[k].descricao, Produtos_esq[i].descricao);
            i++;
        }
        else{
            /* copia e atualiza o indice no vetor da direita */
            Produtos[k].preco = Produtos_drt[j].preco;
            Produtos[k].stock = Produtos_drt[j].stock;
            strcpy(Produtos[k].descricao, Produtos_drt[j].descricao);
            j++;
        }
        k++;
    }

    /* e possivel que sobrem elementos dos vetores da esquerda e da direita */
    while (i<lim_esq){
        /* copia e atualiza o indice no vetor da esquerda */
        Produtos[k].preco = Produtos_esq[i].preco;
        Produtos[k].stock = Produtos_esq[i].stock;
        strcpy(Produtos[k].descricao, Produtos_esq[i].descricao);
        i++;
        k++;   
    }
    while (j<lim_drt){
        /* copia e atualiza o indice no vetor da direita */
        Produtos[k].preco = Produtos_drt[j].preco;
        Produtos[k].stock = Produtos_drt[j].stock;
        strcpy(Produtos[k].descricao, Produtos_drt[j].descricao);
        j++;
        k++;
    }        
}

/* **************************************************************        ************************************************************** */

/* funcao que imprime um vetor ordenado de produtos - sejam todos os do sistema ou de uma encomenda */
void imprime_vetor(Produto Produtos[], int no_elementos, char parametro, int ide){
    int i;
    
    if (parametro == 'p') printf("Produtos\n"); /* corresponde aos produtos ordenados */
    else printf("Encomenda %d\n", ide); /* corresponde aos produtos de uma encomenda ordenados */
    for (i=0; i<no_elementos; i++)
        printf("* %s %d %d\n", Produtos[i].descricao, Produtos[i].preco, Produtos[i].stock); /* escreve no formato de saida */
}

/* **************************************************************        ************************************************************** */
/*                                                  F. PRINCIPAIS - EXECUCAO DOS COMANDOS                                               */
/* **************************************************************        ************************************************************** */

/* funcao que executa o comando a - adicionar novo produto ao sistema */
void executa_a(){   
    scanf(" %[^:]:%d:%d:%d", /* ler no formato de entrada */ 
    Produtos_no_sistema[id_produtos].descricao, 
    &Produtos_no_sistema[id_produtos].preco, 
    &Produtos_no_sistema[id_produtos].peso, 
    &Produtos_no_sistema[id_produtos].stock); 

    Produtos_no_sistema[id_produtos].identificador = id_produtos; /* adicionar ao sistema */
    printf("Novo produto %d.\n", id_produtos); /* escrever no formato de saida */

    id_produtos++; /* aumenta o numero de produtos no sistema */
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando q - adicionar stock a um produto */
int executa_q(){ 
    int idp, qtd; /* parametros que vao ser lidos */

    scanf(" %d:%d", &idp, &qtd); /* ler no formato de entrada */

    /* ERRO */
    if (!produto_no_sistema(idp)){ /* se o produto nao existir */
        printf("Impossivel adicionar produto %d ao stock. Produto inexistente.\n", idp);
        return 0;
    }
    else{ /* se o produto estiver no sistema, e alterado */
        Produtos_no_sistema[idp].stock += qtd;
        return 0;
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando N - criar nova encomenda */
void executa_N(){   
    printf("Nova encomenda %d.\n", id_encomendas); /* escrever no formato de saida */     
    id_encomendas++; /* aumentar o numero de encomendas no sistema */
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando A - adicionar um produto a uma encomenda */
int executa_A(){
    int ide, idp, qtd; /* parametros que vao ser lidos */
    int i; /* variavel auxiliar */

    scanf(" %d:%d:%d", &ide, &idp, &qtd); /* ler no formato de entrada */

    /* ERROS */
    if(!encomenda_no_sistema(ide)){ /* se a encomenda nao existir */
        printf("Impossivel adicionar produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return 0;
    }
    else if(!produto_no_sistema(idp)){ /* se o produto nao existir */
        printf("Impossivel adicionar produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return 0;
    }
    else if(!produto_em_stock(idp, qtd)){ /* se nao houver stock suficiente */
        printf("Impossivel adicionar produto %d a encomenda %d. Quantidade em stock insuficiente.\n", idp, ide);
        return 0;
    }
    else if(!peso_permitido(ide, idp, qtd)){ /* se a sua adicao exceder o peso maximo */
        printf("Impossivel adicionar produto %d a encomenda %d. Peso da encomenda excede o maximo de 200.\n", idp, ide);
        return 0;
    }

    /* se nao, adiciona o produto a encomenda */
    i = produto_na_encomenda(ide, idp);
    if(i >= MAX_PESO){ /* se o produto nao estiver na encomenda adiciona-o */ 
        /* adiciona a encomenda */
        i = id_prod_em_encomenda[ide];
        Encomendas_no_sistema[ide][i].identificador = idp;
        Encomendas_no_sistema[ide][i].stock = qtd;
        
        Produtos_no_sistema[idp].stock -= qtd; /* atualiza o stock no sistema */

        ++id_prod_em_encomenda[ide]; /* aumenta o numero de produtos na encomenda */
        return 0;
    }
    else{ /* se nao, atualiza a sua quantidade */
        Encomendas_no_sistema[ide][i].stock += qtd; /* atualizar a encomenda */
        Produtos_no_sistema[idp].stock -= qtd; /* atualizar o stock no sistema */
        return 0;
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando r - remover stock a um produto */
int executa_r(){   
    int idp, qtd; /* parametros que vao ser lidos */

    scanf(" %d:%d", &idp, &qtd); /* ler no formato de entrada */

    /* ERROS */
    if (!produto_no_sistema(idp)){ /* se o produto nao estiver no sistema */
        printf("Impossivel remover stock do produto %d. Produto inexistente.\n", idp);
        return 0;
    }
    else if (!produto_em_stock(idp, qtd)){ /* se o stock restante for negativo */
        printf("Impossivel remover %d unidades do produto %d do stock. Quantidade insuficiente.\n", qtd, idp);
        return 0;
    }
    else{ /* se nao altera-o, removendo a quantidade pretendida */
        Produtos_no_sistema[idp].stock -= qtd;
        return 0;
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando R - remover um produto de uma encomenda */
int executa_R(){
    int ide, idp; /* parametros que vao ser lidos */
    int i; /* variavel auxiliar */
    int j;

    scanf(" %d:%d", &ide, &idp); /* ler no formato de entrada */

    /* ERROS */
    if(!encomenda_no_sistema(ide)){ /* se a encomenda nao existir */
        printf("Impossivel remover produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return 0;
    }
    else if(!produto_no_sistema(idp)){ /* se o produto nao existir */
        printf("Impossivel remover produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return 0;
    }

    else{ /* se nao, remove o produto da encomenda, atualizando o sistema */
        i = produto_na_encomenda(ide, idp); /* saber qual a posicao do produto na encomenda */
        if(i < MAX_PESO){
            Produtos_no_sistema[idp].stock += Encomendas_no_sistema[ide][i].stock; /* atualiza o stock no sistema */

            /* os restantes produtos da encomenda ocupam o lugar do removido */
            for (j=i; j<id_prod_em_encomenda[ide]; j++)
            {
                Encomendas_no_sistema[ide][j].stock = Encomendas_no_sistema[ide][j+1].stock;
                Encomendas_no_sistema[ide][j].identificador = Encomendas_no_sistema[ide][j+1].identificador;
            }
        
            id_prod_em_encomenda[ide]--; /* indica que ha 1 produto a menos naquela encomenda */
            return 0;
        }
        else return 0; /* se o produto nao estiver na encomenda nao faz nada */
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando C - calcular o custo de uma encomenda */
int executa_C(){
    int ide; /* parametro que vai ser lido */
    int total; /* parametro de retorno */

    scanf(" %d", &ide); /* ler no formato de entrada */

    /* ERRO */
    if(!encomenda_no_sistema(ide)){ /* se a encomenda nao existir */
        printf("Impossivel calcular custo da encomenda %d. Encomenda inexistente.\n", ide);
        return 0;
    }

    else{ /* se nao, calcula o seu custo */
        total = calcula_custo(ide, id_prod_em_encomenda[ide]);
        printf("Custo da encomenda %d %d.\n", ide, total); /* escrever no formato de saida */
        return 0;
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando p - alterar o preco de um produto */
int executa_p(){
    int idp, preco; /* parametros que vao ser lidos */

    scanf(" %d:%d", &idp, &preco); /* ler no formato de entrada */

    /* ERRO */
    if(!produto_no_sistema(idp)){ /* se o produto nao existir */
        printf("Impossivel alterar preco do produto %d. Produto inexistente.\n", idp);
        return 0;
    }

    else{ /* se nao, altera o seu preco */
        Produtos_no_sistema[idp].preco = preco;
        return 0;
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando E - dizer a descricao e quantidade de um produto numa encomenda */
int executa_E(){
    int ide, idp; /* parametros que vao ser lidos */
    int i; /* variavel auxiliar */

    scanf(" %d:%d", &ide, &idp); /* ler no formato de entrada */

    /* ERROS */
    if(!encomenda_no_sistema(ide)){ /* se a encomenda nao existir */
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
        return 0;
    }
    else if(!produto_no_sistema(idp)){ /* se o produto nao existir */
        printf("Impossivel listar produto %d. Produto inexistente.\n", idp);
        return 0;
    }

    else{ /* se nao, lista a descricao e quantidade do produto na encomenda */
        i = produto_na_encomenda(ide, idp);
        if(i < MAX_PESO){ /* se o produto estiver na encomenda indica a descricao e a quantidade */
            printf("%s %d.\n", Produtos_no_sistema[idp].descricao, Encomendas_no_sistema[ide][i].stock);
            return 0;
        }
        else /* se nao estiver apenas indica a sua descricao */
        {
            printf("%s 0.\n", Produtos_no_sistema[idp].descricao);
            return 0;
        }  
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando m - dizer a encomenda em que um produto ocorre mais vezes */
int executa_m(){
    int idp; /* parametro de entrada */

    scanf(" %d", &idp); /* ler no formato de entrada */

    /* ERRO */
    if (!produto_no_sistema(idp)){ /* se o produto nao existir */
        printf("Impossivel listar maximo do produto %d. Produto inexistente.\n", idp);
        return 0;
    }

    /* se nao, lista a encomenda em que o produto ocorre mais vezes */
    else if (!id_encomendas) /* se nao houver encomendas nao faz nada */
        return 0;
    else{ /* se nao vai procurar o maximo do produto */
        maximo_produto(idp);
        return 0;
    }
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando l - listar todos os produtos por ordem crescente do preco */
void executa_l(){  
    Produto Produtos[MAX_PRODUTOS];
    int i;
    for(i=0; i<id_produtos; i++){ /* cria a copia dos produtos, apenas inicializa o que e usado -> e ela que vai ordenar */
        strcpy(Produtos[i].descricao, Produtos_no_sistema[i].descricao);
        Produtos[i].preco = Produtos_no_sistema[i].preco;
        Produtos[i].stock = Produtos_no_sistema[i].stock;
    }
    merge_produtos(Produtos, 0, id_produtos-1, 'p', 0); /* aplica o algoritmo de ordenacao */
    imprime_vetor(Produtos, id_produtos, 'p', 0); /* imprime o vetor ordenado */
}

/* **************************************************************        ************************************************************** */

/* funcao que executa o comando L - listar todos os produtos de uma encomenda por ordem alfabetica */
int executa_L(){  
    int ide; /* parametro de entrada */
    int i, idp; /* variaveis auxiliares */
    Produto produtos[MAX_PESO]; /* vetor de produtos que sera ordenado alfabeticamente */

    scanf(" %d", &ide); /* ler no formato de entrada */

    /* ERRO */
    if (!encomenda_no_sistema(ide)){ /* se a encomenda nao existir */
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
        return 0;
    }
    
    else{ /* se nao, vai criar o vetor que sera ordenado */
        for(i=0; i<id_prod_em_encomenda[ide]; i++){
            /* apenas inicializara as variaveis importantes para ordenacao e output, o resto nao interessa */
            idp = Encomendas_no_sistema[ide][i].identificador;
            produtos[i].stock = Encomendas_no_sistema[ide][i].stock;
            produtos[i].preco = Produtos_no_sistema[idp].preco;
            strcpy(produtos[i].descricao, Produtos_no_sistema[idp].descricao);
        }

        merge_produtos(produtos, 0, id_prod_em_encomenda[ide]-1, 'd', ide); /* aplica o algoritmo de ordenacao */
        imprime_vetor(produtos, id_prod_em_encomenda[ide], 'd', ide); /* imprime o vetor ordenado */
        return 0;
    }
}

/* ************************************************************************************************************************************ */
