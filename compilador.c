#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definições do analisador léxico
typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    COMENTARIO,
    AND,
    BEGIN,
    BOOLEAN,
    ELIF,
    ELSE,
    END,
    FALSE,
    FOR,
    IF,
    INTEGER,
    NOT,
    OF,
    OR,
    PROGRAM,
    READ,
    SET,
    TO,
    TRUE,
    WRITE,
    PONTO_VIRGULA,
    VIRGULA,
    PONTO,
    OP_SOMA,          // Para '+'
    OP_SUB,  
    OP_MULT,
    OP_DIV,       // Para '-'
    OP_MAIOR,         // Para '>'
    OP_MENOR,
    OP_MENORIGUAL,
    OP_MAIORIGUAL,
    OP_DIFERENTE,         // Para '<'
    OP_IGUAL,        
    DOIS_PONTOS,
    ABRE_PAR,
    FECHA_PAR,
    EOS
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    int atributo_numero;
    char atributo_ID[16];
    char comentario[256];
} TInfoAtomo;

// Variáveis globais
char *buffer;
int contaLinha = 1;
char *original_buffer;

char *msgAtomo[] = {
    "ERRO",
    "IDENTIFICADOR",
    "NUMERO",
    "COMENTARIO",
    "AND",
    "BEGIN",
    "BOOLEAN",
    "ELIF",
    "ELSE",
    "END",
    "FALSE",
    "FOR",
    "IF",
    "INTEGER",
    "NOT",
    "OF",
    "OR",
    "PROGRAM",
    "READ",
    "SET",
    "TO",
    "TRUE",
    "WRITE",
    ";",
    ",",
    ".",
    "+",        
    "-",
    "*",
    "/",         
    ">",        
    "<",
    "<=",
    ">=",
    "/= ",         
    "=",        
    ":",
    "abre_par",
    "fecha_par",
    "EOS"
};

// Protótipos de funções do analisador léxico
char* ler_arquivo(const char* nome_arquivo);
TInfoAtomo obter_atomo();
TInfoAtomo reconhece_id();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_comentario();
TInfoAtomo reconhece_relacional();

// Implementações das funções do analisador léxico
char* ler_arquivo(const char* nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(1);
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    rewind(arquivo);

    char *conteudo = (char *)malloc(tamanho_arquivo + 1);
    if (!conteudo) {
        printf("Erro ao alocar memória\n");
        exit(1);
    }

    fread(conteudo, 1, tamanho_arquivo, arquivo);
    conteudo[tamanho_arquivo] = '\0';
    fclose(arquivo);
    
    original_buffer = conteudo;
    
    return conteudo;
}

TInfoAtomo obter_atomo() {
    TInfoAtomo info_atomo;

    // consome espaços em branco, quebra de linhas, tabulação e retorno de carro
    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n')
            contaLinha++;
        buffer++;
    }

    // reconhece identificador ou número
    if (*buffer == ';') {
        info_atomo.atomo = PONTO_VIRGULA;
        buffer++;
         
    } else if (*buffer == '.') {
        info_atomo.atomo = PONTO;
        buffer++;
    } else if (*buffer == ',') {
        info_atomo.atomo = VIRGULA;
        buffer++;
    } else if (*buffer == '(') {
        info_atomo.atomo = ABRE_PAR;
        buffer++;
    } else if (*buffer == ')') {
        info_atomo.atomo = FECHA_PAR;
        buffer++;
    } else if (*buffer == '+') {
        info_atomo.atomo = OP_SOMA;
        buffer++;
    } else if (*buffer == '-') {
        info_atomo.atomo = OP_SUB;
        buffer++;
    } else if (*buffer == '*') {
        info_atomo.atomo = OP_MULT;
        buffer++;
    } else if (*buffer == '/') {
        info_atomo.atomo = OP_DIV;
        buffer++;
    } else if (*buffer == '=' || *buffer == '>' || *buffer == '<') {
        info_atomo = reconhece_relacional();
    } else if (*buffer == ':') {
        info_atomo.atomo = DOIS_PONTOS;
        buffer++;
    } else if (islower(*buffer)) { // Se for letra minúscula
        info_atomo = reconhece_id();
    } else if (*buffer == '#' || (*buffer == '{' && *(buffer + 1) == '-')) {
        info_atomo = reconhece_comentario();
    } else if (*buffer == '0' && *(buffer + 1) == 'b') { // Número binário
        info_atomo = reconhece_num();
    } else if (*buffer == 0) {
        info_atomo.atomo = EOS;
    } else {
        info_atomo.atomo = ERRO;
    }
    if (info_atomo.atomo!=COMENTARIO)
        info_atomo.linha = contaLinha;
    //info_atomo.linha = contaLinha;
    return info_atomo;
}

TInfoAtomo reconhece_relacional() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    // Verifica o operador relacional
    if (*buffer == '=') {
        info_atomo.atomo = OP_IGUAL;
        buffer++;
    } else if (*buffer == '>') {
        buffer++;
        if (*buffer == '=') {  // Verifica se é '>='
            info_atomo.atomo = OP_MAIORIGUAL;
            buffer++;
        } else {
            info_atomo.atomo = OP_MAIOR;  // Apenas '>'
        }
    } else if (*buffer == '<') {
        buffer++;
        if (*buffer == '=') {  // Verifica se é '<='
            info_atomo.atomo = OP_MENORIGUAL;
            buffer++;
        } else if (*buffer == '>') {  // Verifica se é '/='
            info_atomo.atomo = OP_DIFERENTE;
            buffer++;
        } else {
            info_atomo.atomo = OP_MENOR;  // Apenas '<'
        }
    }

    info_atomo.linha = contaLinha;
    return info_atomo;
}
// Implementar reconhece_id(), reconhece_num() e reconhece_comentario() aqui
TInfoAtomo reconhece_comentario() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = COMENTARIO;
    info_atomo.linha = contaLinha;

    if (*buffer == '#') {
        // Comentário de uma linha
        info_atomo.linha = contaLinha; 
        while (*buffer != '\n' && *buffer != 0) {
            buffer++;
        }/*
        if (*buffer == '\n') {
            contaLinha++;
            buffer++;
        }*/
    } else if (*buffer == '{' && *(buffer + 1) == '-') {
        // Comentário de várias linhas
        info_atomo.linha = contaLinha;
        buffer += 2;
        while (!(*buffer == '-' && *(buffer + 1) == '}') && *buffer != 0) {
            if (*buffer == '\n')
                contaLinha++;
            buffer++;
        }
        if (*buffer == '-' && *(buffer + 1) == '}') {
            buffer += 2;
        }
    }

    return info_atomo;
}

TInfoAtomo reconhece_num() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    // Verifica se começa com '0b'
    if (*buffer == '0' && *(buffer + 1) == 'b') {
        buffer += 2; // Pula o '0b'

        // Verifica se a sequência é válida (0 ou 1)
        int valor = 0;
        if (*buffer != '0' && *buffer != '1') {
            return info_atomo; // Se não tiver ao OP_SUB um dígito válido, retorna erro
        }

        while (*buffer == '0' || *buffer == '1') {
            valor = (valor << 1) + (*buffer - '0'); // Converte de binário para decimal
            buffer++;
        }

        info_atomo.atributo_numero = valor;
        info_atomo.atomo = NUMERO;
    }

    return info_atomo;
}

// IDENTIFICADOR -> LETRA_MINUSCULA (LETRA_MINUSCULA | DIGITO | _ )*
TInfoAtomo reconhece_id() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;
    int contaChar = 1;
    char *iniID = buffer;

    // já temos uma letra minúscula
    buffer++;

q1:
    if (islower(*buffer) || isdigit(*buffer) || *buffer == '_') {
        buffer++;
        contaChar++;
        if (contaChar > 15) {
            info_atomo.atomo = ERRO;
            return info_atomo;
        }
        goto q1;
    }

    if (isupper(*buffer))
        return info_atomo;

    strncpy(info_atomo.atributo_ID, iniID, buffer - iniID);
    info_atomo.atributo_ID[buffer - iniID] = '\0';

    // Verifica se é palavra reservada
    if (strcmp(info_atomo.atributo_ID, "and") == 0) info_atomo.atomo = AND;
    else if (strcmp(info_atomo.atributo_ID, "begin") == 0) info_atomo.atomo = BEGIN;
    else if (strcmp(info_atomo.atributo_ID, "boolean") == 0) info_atomo.atomo = BOOLEAN;
    else if (strcmp(info_atomo.atributo_ID, "elif") == 0) info_atomo.atomo = ELIF;
    else if (strcmp(info_atomo.atributo_ID, "else") == 0) info_atomo.atomo = ELSE;
    else if (strcmp(info_atomo.atributo_ID, "end") == 0) info_atomo.atomo = END;
    else if (strcmp(info_atomo.atributo_ID, "false") == 0) info_atomo.atomo = FALSE;
    else if (strcmp(info_atomo.atributo_ID, "for") == 0) info_atomo.atomo = FOR;
    else if (strcmp(info_atomo.atributo_ID, "if") == 0) info_atomo.atomo = IF;
    else if (strcmp(info_atomo.atributo_ID, "integer") == 0) info_atomo.atomo = INTEGER;
    else if (strcmp(info_atomo.atributo_ID, "not") == 0) info_atomo.atomo = NOT;
    else if (strcmp(info_atomo.atributo_ID, "of") == 0) info_atomo.atomo = OF;
    else if (strcmp(info_atomo.atributo_ID, "or") == 0) info_atomo.atomo = OR;
    else if (strcmp(info_atomo.atributo_ID, "program") == 0) info_atomo.atomo = PROGRAM;
    else if (strcmp(info_atomo.atributo_ID, "read") == 0) info_atomo.atomo = READ;
    else if (strcmp(info_atomo.atributo_ID, "set") == 0) info_atomo.atomo = SET;
    else if (strcmp(info_atomo.atributo_ID, "to") == 0) info_atomo.atomo = TO;
    else if (strcmp(info_atomo.atributo_ID, "true") == 0) info_atomo.atomo = TRUE;
    else if (strcmp(info_atomo.atributo_ID, "write") == 0) info_atomo.atomo = WRITE;
    else info_atomo.atomo = IDENTIFICADOR;

    return info_atomo;
}

TAtomo lookahead;//posteriormente sera do tipo TAtomo, declarado no ASDR
TInfoAtomo info_atomo;
void consome(TAtomo atomo);

/*/ Protótipos de funções do analisador sintático
void expressao();
void comando();
void bloco();
void declaracao_de_variaveis();
void lista_variavel();
void tipo();
void comando_atribuicao();
void comando_condicional();
void comando_repeticao();
void comando_entrada();
void comando_saida();
void comando_composto();
void programa();
*/
// Implementações das funções do analisador sintático


// Implementar as deOP_SOMA funções do analisador sintático aqui
// (Copie as implementações do seu arquivo sintatico.c)
// Funções para cada produção da gramática

/**/
void expressao(); // Prototipando expressao para uso posterior

void comando();
void bloco();
void declaracao_de_variaveis();
void lista_variavel();
void tipo();
void comando_atribuicao();
void comando_condicional();
void comando_repeticao();
void comando_entrada();
void comando_saida();
void comando_composto();



void consome(TAtomo atomo){
    if(lookahead == COMENTARIO){
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
        printf("%03d# %s\n", info_atomo.linha, msgAtomo[lookahead]);
        consome(lookahead);
    }
    else
    if(lookahead==atomo){
        info_atomo = obter_atomo();
        lookahead=info_atomo.atomo;
        if (info_atomo.atomo == IDENTIFICADOR)
            printf("%03d# %s | %s\n", info_atomo.linha, msgAtomo[lookahead], info_atomo.atributo_ID);
        else if (info_atomo.atomo == NUMERO)
            printf("%03d# %s | %d\n", info_atomo.linha, msgAtomo[lookahead], info_atomo.atributo_numero);
        else if (info_atomo.atomo == ERRO)
            printf("%03d# Erro léxico\n", info_atomo.linha);
        else
            printf("%03d# %s\n", info_atomo.linha, msgAtomo[lookahead]);


    }
    else{
        printf("#%d:Erro sintatico:esperado [%s] encontrado [%s] \n",info_atomo.linha,msgAtomo[atomo],msgAtomo[lookahead]);
        exit(0);
    }
}
// <programa> ::= program identificador “;” <bloco> “.”
void programa() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

void op_relacional () {
    if(lookahead == OP_MENOR || lookahead == OP_MAIOR || lookahead == OP_IGUAL || lookahead == OP_MAIORIGUAL || lookahead == OP_MENORIGUAL || lookahead == OP_DIFERENTE){
        consome(lookahead);
    }
}
// <bloco>::= <declaracao_de_variaveis> <comando_composto>
void bloco() {
    declaracao_de_variaveis();
    comando_composto();
}

// <declaracao_de_variaveis> ::= {<tipo> <lista_variavel> “;”}
void declaracao_de_variaveis() {
    while (lookahead == INTEGER || lookahead == BOOLEAN) {
        tipo();
        lista_variavel();
        consome(PONTO_VIRGULA);
    }
}

// <tipo> ::= integer | boolean
void tipo() {
    if (lookahead == INTEGER) {
        consome(INTEGER);
    } else if (lookahead == BOOLEAN) {
        consome(BOOLEAN);
    } 
}

// <lista_variavel> ::= identificador { “,” identificador }
void lista_variavel() {
    consome(IDENTIFICADOR);
    while (lookahead == VIRGULA) {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}

// <comando_composto> ::= begin <comando> {“;”<comando>} end
void comando_composto() {
    consome(BEGIN);
    comando();
    while (lookahead == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(END);
}

// <comando> ::= <comando_atribuicao> |
//               <comando_condicional> |
//               <comando_repeticao> |
//               <comando_entrada> |
//               <comando_saida> |
//               <comando_composto>
void comando() {
    if(lookahead == SET) {
        comando_atribuicao();
    } else if(lookahead == IF) {
        comando_condicional();
    } else if(lookahead == FOR) {
        comando_repeticao();
    } else if(lookahead == READ) {
        comando_entrada();
    } else if(lookahead == WRITE) {
        comando_saida();
    } else if(lookahead == BEGIN) {
        comando_composto();
    }
}

// <comando_atribuicao> ::= set identificador to <expressao>
void comando_atribuicao() {
    consome(SET);
    consome(IDENTIFICADOR);
    consome(TO);
    expressao();
}

// <comando_condicional> ::= if <expressao> “:” <comando> [elif <comando>]
void comando_condicional() {
    consome(IF);
    expressao();
    consome(DOIS_PONTOS);
    comando();
    if (lookahead == ELIF) {
        consome(ELIF);
        comando();
    }
}

// <comando_repeticao> ::= for identificador of <expressão> to <expressão> “:” <comando>
void comando_repeticao() {
    consome(FOR);
    consome(IDENTIFICADOR);
    consome(OF);
    expressao();
    consome(TO);
    expressao();
    consome(DOIS_PONTOS);
    comando();
}

// <comando_entrada> ::= read “(“ <lista_variavel> “)”
void comando_entrada() {
    consome(READ);
    consome(ABRE_PAR);
    lista_variavel();
    consome(FECHA_PAR);
}

// <comando_saida> ::= write “(“ <expressao> { “,” <expressao> } “)”
void comando_saida() {
    consome(WRITE);
    consome(ABRE_PAR);
    expressao();
    while (lookahead == VIRGULA) {
        consome(VIRGULA);
        expressao();
    }
    consome(FECHA_PAR);
}

void fator() {
    if(lookahead == IDENTIFICADOR || lookahead == NUMERO || lookahead == TRUE || lookahead == FALSE){ 
        consome(lookahead);
    } else if(lookahead == NOT){
        consome(NOT);
        fator();
    } else if(lookahead == ABRE_PAR){
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    }
}

void termo() {
    fator();
    while(lookahead == OP_MULT || lookahead == OP_DIV){
        consome(lookahead);
        fator();
    }
}


void expressao_simples() {
    termo();
    while(lookahead == OP_SOMA || lookahead == OP_SUB){
        consome(lookahead);
        termo();
    }
}

void expressao_relacional() {
    expressao_simples();
    if(lookahead == OP_MENOR || lookahead == OP_MAIOR || lookahead == OP_IGUAL || lookahead == OP_MAIORIGUAL || lookahead == OP_MENORIGUAL || lookahead == OP_DIFERENTE){
        op_relacional();
        expressao_simples();
    }
       
}

void expressao_logica() {
    expressao_relacional();
    while(lookahead == AND){
        consome(AND);
        expressao_relacional();
    }   
}
// Implementar a análise de expressões simples
void expressao() {
    // Implementação básica: aceita números ou identificadores por enquanto
    expressao_logica();
    while(lookahead == OR){
        consome(OR);
        expressao_logica();
    } 
    
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    buffer = ler_arquivo(argv[1]);

    info_atomo = obter_atomo();
    lookahead=info_atomo.atomo;
    printf("%03d# %s\n", info_atomo.linha, msgAtomo[lookahead]);
    programa();
    printf("%d linhas analisadas, programa sintaticamente correto\n", contaLinha);
    free(original_buffer);

    return 0;
}
