#include "hotel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ==========================================
// 1. FUNÇÕES UTILITÁRIAS
// ==========================================

void limpar_tela(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    while (getchar() != '\n');
}

void cabecalho(const char* titulo) {
    printf("\n==================================\n");
    printf(" %s\n", titulo);
    printf("==================================\n\n");
}

void exibir_mensagem(const char* msg) {
    printf("\n[!] %s\n", msg);
}

int ler_inteiro(void) {
    int valor;
    while (scanf("%d", &valor) != 1) {
        limpar_buffer();
        exibir_mensagem("Entrada invalida! Digite um numero inteiro.");
        printf("Tente novamente: ");
    }
    limpar_buffer();
    return valor;
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int ano_bissexto(int ano) {
	return (ano % 400 == 0) || (ano % 4 == 0 && ano % 100 != 0);
}

int data_valida(Data data){
	int dias_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,31};
	
	if(data.ano <= 0){
		return 0;
	}
	
	if(data.mes < 1 || data.mes > 12){
		return 0;
	}
	
	if(ano_bissexto(data.ano)){
		dias_mes[1] = 29;
	}
	
	if(data.dia < 1 || data.dia > dias_mes[data.mes - 1]){
		return 0;
	}
	return 1;
}

int calcular_diferenca_dias(Data inicio, Data fim) {
    struct tm tm_inicio = {0};
    struct tm tm_fim = {0};
    
    tm_inicio.tm_mday = inicio.dia;
    tm_inicio.tm_mon = inicio.mes - 1;
    tm_inicio.tm_year = inicio.ano - 1900;
    
    tm_fim.tm_mday = fim.dia;
    tm_fim.tm_mon = fim.mes - 1;
    tm_fim.tm_year = fim.ano - 1900;
    
    time_t time_inicio = mktime(&tm_inicio);
    time_t time_fim = mktime(&tm_fim);
    
    if (time_inicio == -1 || time_fim == -1) {
        exibir_mensagem("Erro: Data invalida.");
        return -1;
    }
    
    double diferenca = difftime(time_fim, time_inicio);
    return (int)(diferenca / (60 * 60 * 24));
}

// ==========================================
// 2. FUNÇÕES DE TIPO DE QUARTO
// ==========================================

int carregar_tipos_quarto(TipoQuarto** tipos) {
    FILE* arquivo = fopen("tipos_quarto.dat", "rb");
    if (!arquivo) return 0;
    
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    rewind(arquivo);
    
    int total = tamanho / sizeof(TipoQuarto);
    if (total <= 0) {
        fclose(arquivo);
        return 0;
    }
    
    *tipos = (TipoQuarto*)malloc(total * sizeof(TipoQuarto));
    if (*tipos == NULL) {
        fclose(arquivo);
        return 0;
    }
    
    if (fread(*tipos, sizeof(TipoQuarto), total, arquivo) != total) {
        free(*tipos);
        *tipos = NULL;
        fclose(arquivo);
        return 0;
    }
    
    fclose(arquivo);
    return total;
}

void salvar_tipos_quarto(TipoQuarto* tipos, int total) {
    if (tipos == NULL || total <= 0) {
        return;
    }
    
    FILE* arquivo = fopen("tipos_quarto.dat", "wb");
    if (!arquivo) {
        exibir_mensagem("Erro ao abrir arquivo para salvar tipos de quarto.");
        return;
    }
    
    fwrite(tipos, sizeof(TipoQuarto), total, arquivo);
    fclose(arquivo);
}

void listar_tipos_quarto(TipoQuarto* tipos, int total) {
    int i;
    cabecalho("TIPOS DE QUARTO DISPONIVEIS");
    for (i = 0; i < total; i++) {
        if (tipos[i].ativo) {
            printf(" %d - %s\n", i + 1, tipos[i].nome);
            printf("    Preco/dia: %.2f Kz\n", tipos[i].preco);
            printf("    Capacidade: %d pessoas\n", tipos[i].capacidade);
            printf("    Caracteristicas: %s\n\n", tipos[i].caracteristicas);
        }
    }
}

float preco_quarto(TipoQuarto* tipos, int total, int tipo) {
    if (tipo < 1 || tipo > total || !tipos[tipo-1].ativo) return 0.0f;
    return tipos[tipo-1].preco;
}

void gerenciar_tipos_quarto(TipoQuarto** tipos, int* total) {
    int opcao;
    do {
        limpar_tela();
        cabecalho("GERENCIAMENTO DE TIPOS DE QUARTO");
        printf(" 1. Adicionar tipo\n");
        printf(" 2. Editar tipo\n");
        printf(" 3. Desativar tipo\n");
        printf(" 4. Listar tipos\n");
        printf(" 0. Voltar\n");
        printf("\n Escolha: ");
        opcao = ler_inteiro();
        
        switch (opcao) {
            case 1: {
                TipoQuarto* temp = realloc(*tipos, (*total + 1) * sizeof(TipoQuarto));
                if (!temp) {
                    exibir_mensagem("Falha ao alocar memoria!");
                    break;
                }
                *tipos = temp;
                printf("Nome do novo tipo: ");
                fgets((*tipos)[*total].nome, 50, stdin);
                (*tipos)[*total].nome[strcspn((*tipos)[*total].nome, "\n")] = 0;
                
                 do{
   					printf("Preco por dia: ");
    				scanf("%f",&(*tipos)[*total].preco);
    				limpar_buffer();
    				
    				if((*tipos)[*total].preco <= 0){
        				exibir_mensagem("O preco deve ser maior que zero.");
        			}
				}while((*tipos)[*total].preco <= 0);
                 
                printf("Capacidade: ");
                (*tipos)[*total].capacidade = ler_inteiro();
                
                printf("Caracteristicas: ");
                fgets((*tipos)[*total].caracteristicas, MAX_CARACTERISTICAS, stdin);
                (*tipos)[*total].caracteristicas[strcspn((*tipos)[*total].caracteristicas, "\n")] = 0;
                
                (*tipos)[*total].ativo = 1;
                (*total)++;
                exibir_mensagem("Tipo adicionado com sucesso!");
                break;
            }
            case 2: {
                int id;
                listar_tipos_quarto(*tipos, *total);
                printf("ID do tipo a editar: ");
                id = ler_inteiro() - 1;
                if (id >= 0 && id < *total) {
                    printf("Novo nome [atual: %s]: ", (*tipos)[id].nome);
                    fgets((*tipos)[id].nome, 50, stdin);
                    (*tipos)[id].nome[strcspn((*tipos)[id].nome, "\n")] = 0;
                    
                    do {
    					printf("Novo preco [atual: %.2f]: ", (*tipos)[id].preco);
    					scanf("%f", &(*tipos)[id].preco);
    					limpar_buffer();

    					if ((*tipos)[id].preco <= 0) {
        					exibir_mensagem("Preco invalido! O valor deve ser maior que zero.");
    					}
					} while ((*tipos)[id].preco <= 0);
            
                    printf("Nova capacidade [atual: %d]: ", (*tipos)[id].capacidade);
                    (*tipos)[id].capacidade = ler_inteiro();
                    
                    printf("Novas caracteristicas [atual: %s]: ", (*tipos)[id].caracteristicas);
                    fgets((*tipos)[id].caracteristicas, MAX_CARACTERISTICAS, stdin);
                    (*tipos)[id].caracteristicas[strcspn((*tipos)[id].caracteristicas, "\n")] = 0;
                    
                    exibir_mensagem("Tipo atualizado com sucesso!");
                } else {
                    exibir_mensagem("ID invalido!");
                }
                break;
            }
            case 3: {
                int id;
                listar_tipos_quarto(*tipos, *total);
                printf("ID do tipo a desativar: ");
                id = ler_inteiro() - 1;
                if (id >= 0 && id < *total) {
                    (*tipos)[id].ativo = 0;
                    exibir_mensagem("Tipo desativado com sucesso!");
                } else {
                    exibir_mensagem("ID invalido!");
                }
                break;
            }
            case 4:
                listar_tipos_quarto(*tipos, *total);
                pausar();
                break;
            case 0:
                break;
            default:
                exibir_mensagem("Opcao invalida!");
        }
    } while (opcao != 0);
}

// ==========================================
// 3. FUNÇÕES DE RESERVA
// ==========================================

Reserva* criar_reserva(int novo_id, TipoQuarto* tipos, int total_tipos) {
    Reserva* nova = (Reserva*)malloc(sizeof(Reserva));
    if (!nova) {
        exibir_mensagem("Erro ao alocar memoria para a reserva.");
        return NULL;
    }
    
    nova->id = novo_id;
    nova->status = 1;
    nova->proximo = NULL;
    
    cabecalho("NOVA RESERVA");
    printf("ID Gerado: %d\n", novo_id);
    
    printf("Nome do cliente: ");
    fgets(nova->nome_cliente, MAX_NOME, stdin);
    nova->nome_cliente[strcspn(nova->nome_cliente, "\n")] = 0;
    
    printf("Data de entrada (dia mes ano): ");
    if (scanf("%d %d %d", &nova->data_entrada.dia, &nova->data_entrada.mes, &nova->data_entrada.ano) != 3) {
        exibir_mensagem("Formato de data invalido.");
        limpar_buffer();
        free(nova);
        return NULL;
    }
    limpar_buffer();
    
    printf("Data de saida (dia mes ano): ");
    if (scanf("%d %d %d", &nova->data_saida.dia, &nova->data_saida.mes, &nova->data_saida.ano) != 3) {
        exibir_mensagem("Formato de data invalido.");
        limpar_buffer();
        free(nova);
        return NULL;
    }
    limpar_buffer();
    
    if(!data_valida(nova->data_entrada) || !data_valida(nova->data_saida)){
    	exibir_mensagem("Formato de data invalido.");
    	free(nova);
    	return NULL;
	}

	nova->duracao_dias = calcular_diferenca_dias(nova->data_entrada, nova->data_saida);

	if(nova->duracao_dias <= 0){
    	exibir_mensagem("A data de saida deve ser posterior a entrada.");
    	free(nova);
    	return NULL;
	}
    
    listar_tipos_quarto(tipos, total_tipos);
    printf("Tipo de quarto (1-%d): ", total_tipos);
    nova->tipo_quarto = ler_inteiro();
    
    if (nova->tipo_quarto < 1 || nova->tipo_quarto > total_tipos || !tipos[nova->tipo_quarto-1].ativo) {
        exibir_mensagem("Tipo de quarto invalido!");
        free(nova);
        return NULL;
    }
    
    nova->valor_total = calcular_valor_reserva(nova, tipos, total_tipos);
    return nova;
}

float calcular_valor_reserva(Reserva* reserva, TipoQuarto* tipos, int total_tipos) {
    if (reserva->duracao_dias <= 0) {
        reserva->duracao_dias = calcular_diferenca_dias(reserva->data_entrada, reserva->data_saida);
    }
    return reserva->duracao_dias * preco_quarto(tipos, total_tipos, reserva->tipo_quarto);
}

void mostrar_reserva(Reserva* reserva, TipoQuarto* tipos, int total_tipos) {
    if (!reserva) return;
    
    printf(" ID: %d\n", reserva->id);
    printf(" Cliente: %s\n", reserva->nome_cliente);
    printf(" Entrada: %02d/%02d/%04d\n", reserva->data_entrada.dia, reserva->data_entrada.mes, reserva->data_entrada.ano);
    printf(" Saida: %02d/%02d/%04d\n", reserva->data_saida.dia, reserva->data_saida.mes, reserva->data_saida.ano);
    printf(" Duracao: %d dias\n", reserva->duracao_dias);
    printf(" Tipo quarto: %s\n", tipos[reserva->tipo_quarto-1].nome);
    printf(" Valor total: %.2f Kz\n", reserva->valor_total);
    printf(" Status: %s\n", reserva->status ? "Ativa" : "Cancelada");
}

void editar_reserva(Reserva* reserva, TipoQuarto* tipos, int total_tipos) {
    int opcao;
    do {
        limpar_tela();
        cabecalho("EDITAR RESERVA");
        mostrar_reserva(reserva, tipos, total_tipos);
        
        printf("\n 1. Editar nome\n");
        printf(" 2. Editar datas\n");
        printf(" 3. Editar tipo de quarto\n");
        printf(" 0. Voltar\n");
        printf("\n Escolha: ");
        opcao = ler_inteiro();
        switch (opcao) {
            case 1:
                printf("Novo nome: ");
                fgets(reserva->nome_cliente, MAX_NOME, stdin);
                reserva->nome_cliente[strcspn(reserva->nome_cliente, "\n")] = 0;
                exibir_mensagem("Nome atualizado!");
                break;
            case 2:
                printf("Nova data de entrada (dia mes ano): ");
                if (scanf("%d %d %d", &reserva->data_entrada.dia, &reserva->data_entrada.mes, &reserva->data_entrada.ano) != 3) {
                    exibir_mensagem("Formato invalido.");
                    limpar_buffer();
                } else {
                    limpar_buffer();
                    printf("Nova data de saida (dia mes ano): ");
                    if (scanf("%d %d %d", &reserva->data_saida.dia, &reserva->data_saida.mes, &reserva->data_saida.ano) != 3) {
                        exibir_mensagem("Formato invalido.");
                        limpar_buffer();
                    } else {
                        limpar_buffer();
                    	if(!data_valida(reserva->data_entrada) ||!data_valida(reserva->data_saida)){
    						exibir_mensagem("Datas invalidas! Nao alterado.");
						} else {

							reserva->duracao_dias = calcular_diferenca_dias(reserva->data_entrada, reserva->data_saida);

							if(reserva->duracao_dias <= 0){
    							exibir_mensagem("A data de saida deve ser posterior a entrada.");
                       		} else {
                            	reserva->valor_total = calcular_valor_reserva(reserva, tipos, total_tipos);
                            	exibir_mensagem("Datas atualizadas!");
                        	}
                    	}
                    }
                }
                break;
            case 3:
                listar_tipos_quarto(tipos, total_tipos);
                printf("Novo tipo de quarto (1-%d): ", total_tipos);
                reserva->tipo_quarto = ler_inteiro();
                if (reserva->tipo_quarto < 1 || reserva->tipo_quarto > total_tipos || !tipos[reserva->tipo_quarto-1].ativo) {
                    exibir_mensagem("Tipo de quarto invalido! Nao alterado.");
                } else {
                    reserva->valor_total = calcular_valor_reserva(reserva, tipos, total_tipos);
                    exibir_mensagem("Tipo de quarto atualizado!");
                }
                break;
            case 0:
                break;
            default:
                exibir_mensagem("Opcao invalida!");
        }
    } while (opcao != 0);
}

void cancelar_reserva(Reserva* reserva) {
    reserva->status = 0;
    exibir_mensagem("Reserva cancelada com sucesso!");
}

// ==========================================
// 4. FUNÇÕES DE TABELA HASH
// ==========================================

HashTable* criar_hash_table(void) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) return NULL;
    
    table->tamanho = TABLE_SIZE;
    table->ocupados = 0;
    table->tabela = (Reserva**)calloc(TABLE_SIZE, sizeof(Reserva*));
    if (table->tabela == NULL) {
        free(table);
        return NULL;
    }
    return table;
}

void destruir_hash_table(HashTable* table) {
    int i;
    if (!table) return;
    for (i = 0; i < table->tamanho; i++) {
        Reserva* atual = table->tabela[i];
        while (atual) {
            Reserva* proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
    }
    free(table->tabela);
    free(table);
}

void inserir_reserva(HashTable* table, Reserva* reserva) {
    if (!table || !reserva) return;
    int indice = reserva->id % table->tamanho;
    reserva->proximo = table->tabela[indice];
    table->tabela[indice] = reserva;
    table->ocupados++;
}

Reserva* buscar_reserva(HashTable* table, int id) {
    if (!table) return NULL;
    int indice = id % table->tamanho;
    Reserva* atual = table->tabela[indice];
    while (atual) {
        if (atual->id == id) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

void carregar_reservas(HashTable* table) {
    FILE* arquivo = fopen("reservas.dat", "rb");
    if (!arquivo) return;
    
    Reserva reserva;
    while (fread(&reserva, sizeof(Reserva), 1, arquivo) == 1) {
        Reserva* nova = (Reserva*)malloc(sizeof(Reserva));
        if (!nova) break;
        *nova = reserva;
        nova->proximo = NULL;
        inserir_reserva(table, nova);
    }
    fclose(arquivo);
}

void salvar_reservas(HashTable* table) {
    if (!table) return;
    FILE* arquivo = fopen("reservas.dat", "wb");
    if (!arquivo) return;
    
    int i;
    for (i = 0; i < table->tamanho; i++) {
        Reserva* atual = table->tabela[i];
        while (atual) {
            if (atual->status) {
                Reserva temp = *atual;
                temp.proximo = NULL; // Limpa ponteiro para evitar corromper o ficheiro binário
                fwrite(&temp, sizeof(Reserva), 1, arquivo);
            }
            atual = atual->proximo;
        }
    }
    fclose(arquivo);
}

// ==========================================
// 5. FUNÇÕES DE RELATÓRIO (HEAPIFY)
// ==========================================

void heapify(Reserva* array, int n, int i) {
    int maior = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;
    
    if (esq < n && array[esq].valor_total > array[maior].valor_total) {
        maior = esq;
    }
    if (dir < n && array[dir].valor_total > array[maior].valor_total) {
        maior = dir;
    }
    if (maior != i) {
        Reserva temp = array[i];
        array[i] = array[maior];
        array[maior] = temp;
        heapify(array, n, maior);
    }
}

void mostrar_resumo_reserva(Reserva* r) {
    printf("Reserva ID: %d | Cliente: %s | Total: %.2f Kz\n", r->id, r->nome_cliente, r->valor_total);
    printf("--------------------------------------------------\n");
}

void gerar_relatorio_reservas(HashTable* table) {
	
	int reservasAtivas = 0, reservasCanceladas = 0,maiorQtdCliente = 0, contagemTipos[100] = {0};
	float receitaTotal = 0;
	char clienteMaisReservas[MAX_NOME]="";

    int i, n = 0, index;
    Reserva* array = NULL;
    Reserva* atual;
    
    limpar_tela();
    cabecalho("RELATORIO DE RESERVAS POR VALOR");
    if (!table) return;
    
    for (i = 0; i < table->tamanho; i++) {
        atual = table->tabela[i];
        while (atual) {
            if (atual->status) n++;
            atual = atual->proximo;
        }
    }
    
    if (n == 0) {
        printf("Nenhuma reserva ativa encontrada.\n");
        pausar();
        return;
    }
    
    array = (Reserva*)malloc(n * sizeof(Reserva));
    if (!array) return;
    
    index = 0;
    for (i = 0; i < table->tamanho; i++) {
        atual = table->tabela[i];
        while (atual) {
            if (atual->status) {
                array[index++] = *atual;
            }
            atual = atual->proximo;
        }
    }
    
    for (i = n / 2 - 1; i >= 0; i--) {
        heapify(array, n, i);
    }
    for (i = n - 1; i > 0; i--) {
        Reserva temp = array[0];
        array[0] = array[i];
        array[i] = temp;
        heapify(array, i, 0);
    }
    
    printf("Reservas ordenadas por valor (maior primeiro):\n\n");
    for (i = n - 1; i >= 0; i--) {
        mostrar_resumo_reserva(&array[i]);
    }
    
    free(array);
    pausar();
}

// ==========================================
// 6. MENUS DO HOTEL
// ==========================================

int login_admin(void) {
    char usuario[20], senha[20];
    limpar_tela();
    cabecalho("ACESSO ADMINISTRATIVO");
    printf("Usuario: ");
    fgets(usuario, 20, stdin);
    usuario[strcspn(usuario, "\n")] = 0;
    
    printf("Senha: ");
    fgets(senha, 20, stdin);
    senha[strcspn(senha, "\n")] = 0;
    
    if (strcmp(usuario, "admin") == 0 && strcmp(senha, "admin123") == 0) {
        return 1;
    }
    exibir_mensagem("Credenciais invalidas!");
    return 0;
}

void menu_recepcao(HashTable* table, TipoQuarto* tipos, int total_tipos) {
    int opcao;
    do {
        limpar_tela();
        cabecalho("MENU RECEPCAO");
        printf(" 1. Nova Reserva\n");
        printf(" 2. Editar Reserva\n");
        printf(" 3. Cancelar Reserva\n");
        printf(" 4. Listar Reservas Ativas\n");
        printf(" 0. Voltar\n");
        printf("\n Escolha: ");
        opcao = ler_inteiro();
        switch (opcao) {
            case 1: {
                int novo_id = table->ocupados + 1001;
                Reserva* nova = criar_reserva(novo_id, tipos, total_tipos);
                if (nova) {
                    inserir_reserva(table, nova);
                    exibir_mensagem("Reserva criada com sucesso!");
                }
                pausar();
                break;
            }
            case 2: {
                int id;
                printf("ID da reserva a editar: ");
                id = ler_inteiro();
                Reserva* r = buscar_reserva(table, id);
                if (r) editar_reserva(r, tipos, total_tipos);
                else exibir_mensagem("Reserva nao encontrada!");
                pausar();
                break;
            }
            case 3: {
                int id;
                printf("ID da reserva a cancelar: ");
                id = ler_inteiro();
                Reserva* r = buscar_reserva(table, id);
                if (r) cancelar_reserva(r);
                else exibir_mensagem("Reserva nao encontrada!");
                pausar();
                break;
            }
            case 4: {
                limpar_tela();
                cabecalho("RESERVAS ATIVAS");
                int i;
                for (i = 0; i < table->tamanho; i++) {
                    Reserva* atual = table->tabela[i];
                    while (atual) {
                        if (atual->status) {
                            mostrar_reserva(atual, tipos, total_tipos);
                            printf("----------------------------\n");
                        }
                        atual = atual->proximo;
                    }
                }
                pausar();
                break;
            }
        }
    } while (opcao != 0);
}

void menu_admin(HashTable* table, TipoQuarto** tipos, int* total_tipos) {
    int opcao;
    do {
        limpar_tela();
        cabecalho("MENU ADMINISTRACAO");
        printf(" 1. Gerenciar Tipos de Quarto\n");
        printf(" 2. Relatorio de Reservas (por valor)\n");
        printf(" 0. Voltar\n");
        printf("\n Escolha: ");
        opcao = ler_inteiro();
        switch (opcao) {
            case 1:
                gerenciar_tipos_quarto(tipos, total_tipos);
                break;
            case 2:
                gerar_relatorio_reservas(table);
                break;
        }
    } while (opcao != 0);
}

void menu_principal(void) {
    HashTable* table = criar_hash_table();
    int total_tipos = 0, i;
    TipoQuarto* tipos = NULL;
    int opcao;
    
    if (!table) return;
    
    total_tipos = carregar_tipos_quarto(&tipos);
    carregar_reservas(table);
    
    if (total_tipos == 0) {
        total_tipos = 3;
        tipos = (TipoQuarto*)malloc(total_tipos * sizeof(TipoQuarto));
        for (i = 0; i < total_tipos; i++) tipos[i].ativo = 1;
        
        strcpy(tipos[0].nome, "Standard");
        tipos[0].preco = 15000.0f;
        tipos[0].capacidade = 2;
        strcpy(tipos[0].caracteristicas, "Cama de casal, Wi-Fi, TV, Banheiro");
        
        strcpy(tipos[1].nome, "Luxo");
        tipos[1].preco = 25000.0f;
        tipos[1].capacidade = 2;
        strcpy(tipos[1].caracteristicas, "Cama King, AC, Wi-Fi Premium, Frigobar");
        
        strcpy(tipos[2].nome, "Suite");
        tipos[2].preco = 50000.0f;
        tipos[2].capacidade = 4;
        strcpy(tipos[2].caracteristicas, "Dois quartos, Sala, Varanda, TV 4K");
        salvar_tipos_quarto(tipos, total_tipos);
    }
    
    do {
        limpar_tela();
        cabecalho("SISTEMA DE GERENCIAMENTO DE HOTEL");
        printf(" 1. Recepcao\n");
        printf(" 2. Administracao\n");
        printf(" 0. Sair\n");
        printf("\n Escolha: ");
        opcao = ler_inteiro();
        switch (opcao) {
            case 1:
                menu_recepcao(table, tipos, total_tipos);
                break;
            case 2:
                if (login_admin()) menu_admin(table, &tipos, &total_tipos);
                else pausar();
                break;
            case 0:
                exibir_mensagem("Salvando dados e saindo...");
                salvar_reservas(table);
                salvar_tipos_quarto(tipos, total_tipos);
                break;
        }
    } while (opcao != 0);
    
    destruir_hash_table(table);
    free(tipos);
}
