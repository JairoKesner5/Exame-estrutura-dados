#ifndef HOTEL_H
#define HOTEL_H

#define MAX_NOME 100
#define MAX_CARACTERISTICAS 200
#define TABLE_SIZE 101

// ==========================================
// ESTRUTURAS DO SISTEMA
// ==========================================

typedef struct Data {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct TipoQuarto {
    char nome[50];
    float preco;
    int capacidade;
    char caracteristicas[MAX_CARACTERISTICAS];
    int ativo;
} TipoQuarto;

typedef struct Reserva {
    int id;
    char nome_cliente[MAX_NOME];
    Data data_entrada;
    Data data_saida;
    int tipo_quarto;
    float valor_total;
    int status; // 1 = Ativa, 0 = Cancelada
    int duracao_dias;
    struct Reserva* proximo;
} Reserva;

typedef struct HashTable {
    Reserva** tabela;
    int tamanho;
    int ocupados;
} HashTable;

// ==========================================
// PROTÓTIPOS DAS FUNÇÕES
// ==========================================

// Funções Utilitárias (util)
void limpar_tela(void);
void pausar(void);
void cabecalho(const char* titulo);
void exibir_mensagem(const char* msg);
int ler_inteiro(void);
void limpar_buffer(void);
int ano_bissexto(int ano);
int data_valida(Data data);
int calcular_diferenca_dias(Data inicio, Data fim);

// Funções de Tipo de Quarto (tipo_quarto)
int carregar_tipos_quarto(TipoQuarto** tipos);
void salvar_tipos_quarto(TipoQuarto* tipos, int total);
void listar_tipos_quarto(TipoQuarto* tipos, int total);
float preco_quarto(TipoQuarto* tipos, int total, int tipo);
void gerenciar_tipos_quarto(TipoQuarto** tipos, int* total);

// Funções de Reserva (reserva)
Reserva* criar_reserva(int novo_id, TipoQuarto* tipos, int total_tipos);
float calcular_valor_reserva(Reserva* reserva, TipoQuarto* tipos, int total_tipos);
void mostrar_reserva(Reserva* reserva, TipoQuarto* tipos, int total_tipos);
void editar_reserva(Reserva* reserva, TipoQuarto* tipos, int total_tipos);
void cancelar_reserva(Reserva* reserva);

// Funções de Tabela Hash (hashtable)
HashTable* criar_hash_table(void);
void destruir_hash_table(HashTable* table);
void inserir_reserva(HashTable* table, Reserva* reserva);
Reserva* buscar_reserva(HashTable* table, int id);
void carregar_reservas(HashTable* table);
void salvar_reservas(HashTable* table);

// Funções de Relatório (relatorio)
void heapify(Reserva* array, int n, int i);
void mostrar_resumo_reserva(Reserva* r);
void gerar_relatorio_reservas(HashTable* table);

// Funções de Menus e Fluxo Principal (hotel)
int login_admin(void);
void menu_recepcao(HashTable* table, TipoQuarto* tipos, int total_tipos);
void menu_admin(HashTable* table, TipoQuarto** tipos, int* total_tipos);
void menu_principal(void);

#endif // HOTEL_H
