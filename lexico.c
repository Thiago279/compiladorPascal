/*
Implementacao do mini analisador lexico
Compile com:
gcc -g -Og -Wall compilador.c -o compilador
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// definicao de tipo
typedef enum{
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
    EOS
} TAtomo;

typedef struct{
    TAtomo atomo;
    int linha;
    int atributo_numero; // Modificado de float para int
    char atributo_ID[16];
    char comentario[256];
} TInfoAtomo;

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
    "EOS"
};

// variavel global para o analisador lexico
char *buffer;
int contaLinha = 1;
char *original_buffer;

// Função para ler o arquivo fonte
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
    
    // Set the original_buffer
    original_buffer = conteudo;
    
    return conteudo;
}


// declaracao da funcao
TInfoAtomo obter_atomo(); // irá integrar com a Analisador Sintatico
TInfoAtomo reconhece_id();
TInfoAtomo reconhece_num();
TInfoAtomo reconhece_comentario();

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    // Lê o arquivo fonte para o buffer
    buffer = ler_arquivo(argv[1]);

    TInfoAtomo info_atomo;
    do {
        info_atomo = obter_atomo();
        if (info_atomo.atomo == IDENTIFICADOR)
            printf("%03d# %s | %s\n", info_atomo.linha, msgAtomo[info_atomo.atomo], info_atomo.atributo_ID);
        else if (info_atomo.atomo == NUMERO)
            printf("%03d# %s | %d\n", info_atomo.linha, msgAtomo[info_atomo.atomo], info_atomo.atributo_numero);
        else if (info_atomo.atomo == COMENTARIO)
            printf("%03d# %s | %s\n", info_atomo.linha, msgAtomo[info_atomo.atomo], info_atomo.comentario);
        else
            printf("%03d# %s\n", info_atomo.linha, msgAtomo[info_atomo.atomo]);

    } while (info_atomo.atomo != EOS && info_atomo.atomo != ERRO);

    printf("fim da analise lexica\n");

    // Libera a memória alocada
    free(original_buffer);
    buffer = NULL;
    original_buffer = NULL;
    return 0;
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
    if (islower(*buffer)) { // Se for letra minúscula
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

    info_atomo.linha = contaLinha;
    return info_atomo;
}

// As funções `reconhece_id`, `reconhece_num` e `reconhece_comentario` permanecem inalteradas

TInfoAtomo reconhece_comentario() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = COMENTARIO;

    char *inicioComentario = buffer;

    if (*buffer == '#') {
        // Comentário de uma linha (começando com '#')
        while (*buffer != '\n' && *buffer != 0) {
            buffer++;
        }
        // Copia o comentário para a estrutura
        strncpy(info_atomo.comentario, inicioComentario, buffer - inicioComentario);
        info_atomo.comentario[buffer - inicioComentario] = '\0';

        if (*buffer == '\n') {
            contaLinha++;
            buffer++;
        }
    } else if (*buffer == '{' && *(buffer + 1) == '-') {
        // Comentário de várias linhas (começando com '{-')
        buffer += 2; // Pula os caracteres '{-'

        while (!(*buffer == '-' && *(buffer + 1) == '}') && *buffer != 0) {
            if (*buffer == '\n')
                contaLinha++;
            buffer++;
        }

        if (*buffer == '-' && *(buffer + 1) == '}') {
            buffer += 2; // Pula os caracteres '-}'
        }

        // Copia o comentário para a estrutura
        strncpy(info_atomo.comentario, inicioComentario, buffer - inicioComentario);
        info_atomo.comentario[buffer - inicioComentario] = '\0';
    } else {
        info_atomo.atomo = ERRO;
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
            return info_atomo; // Se não tiver ao menos um dígito válido, retorna erro
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
