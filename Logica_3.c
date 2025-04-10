#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>

typedef enum {VAR, NOT, AND, OR, IMPLIES, IFF} NODETYPE;

typedef struct Node{
    NODETYPE tipo;
    char var;
    struct Node *left;
    struct Node *right;
}NODE;

NODE *criar_var(char var){
    NODE *novo = (NODE*)malloc(sizeof(NODE));
    novo->tipo = VAR;
    novo->var = var;
    novo->left = novo->right = NULL;
    return novo;
}

NODE *criar_unario(NODETYPE tipo, NODE *expressao){
    NODE *novo = (NODE*)malloc(sizeof(NODE));
    novo->tipo = tipo;
    novo->left = expressao;
    novo->right = NULL;
    return novo;
}

NODE *criar_binario(NODETYPE tipo, NODE *left, NODE *right){
    NODE *novo = (NODE*)malloc(sizeof(NODE));
    novo->tipo = tipo;
    novo->left = left;
    novo->right = right;
    return novo;
}

const char *entrada;
char cabeca;

void next(){
    do{
        cabeca = *entrada++;
    }while(isspace(cabeca));
}

NODE *parse_expr();

NODE *parse_atom(){
    if(cabeca == '('){
        next();
        NODE *expr = parse_expr();
        if(cabeca == ')'){
            next();
        }
        return expr;
    }else if(cabeca == '!'){
        next();
        return criar_unario(NOT, parse_atom());
    }else if(isalpha(cabeca)){
        char var = cabeca;
        next();
        return criar_var(var);
    }
    return NULL;
}

NODE *parse_and_or(){
    NODE *left = parse_atom();
    while(cabeca == '^' || cabeca == 'V'){
        NODETYPE op = (cabeca == '^') ? AND : OR;
        next();
        NODE *right = parse_atom();
        left = criar_binario(op, left, right); 
    }
    return left;
}

NODE *parse_implies() {
    NODE *left = parse_and_or();
    while (cabeca == '-') {
        if(*entrada != '>') return NULL;
        entrada++;
        next();
        NODE *right = parse_implies();
        left = criar_binario(IMPLIES, left, right);
    }
    return left;
}

NODE *parse_iff() {
    NODE *left = parse_implies();
    while (cabeca == '<') {
        if(*entrada != '-') return NULL;
        entrada++;
        if(*entrada != '>') return NULL;
        entrada++;
        next();
        NODE *right = parse_iff();
        left = criar_binario(IFF, left, right);
    }
    return left;
}

NODE *parse_expr(){
    return parse_iff();
}

NODE *eliminar_implicacoes(NODE *expressao) {
    if (!expressao) return NULL;
    if (expressao->tipo == IMPLIES) {
        return criar_binario(OR,
            criar_unario(NOT, eliminar_implicacoes(expressao->left)),
            eliminar_implicacoes(expressao->right));
    } else if (expressao->tipo == IFF) {
        NODE *a = eliminar_implicacoes(expressao->left);
        NODE *b = eliminar_implicacoes(expressao->right);
        NODE *ab = criar_binario(OR, criar_unario(NOT, a), b);
        NODE *ba = criar_binario(OR, criar_unario(NOT, b), a);
        return criar_binario(AND, ab, ba);
    } else if (expressao->tipo == NOT) {
        return criar_unario(NOT, eliminar_implicacoes(expressao->left));
    } else if (expressao->tipo == AND || expressao->tipo == OR) {
        return criar_binario(expressao->tipo,
            eliminar_implicacoes(expressao->left),
            eliminar_implicacoes(expressao->right));
    } else {
        return expressao;
    }
}

NODE *aplicar_de_morgan(NODE *expr) {
    if (!expr) return NULL;
    if (expr->tipo == NOT) {
        NODE *sub = expr->left;
        if (sub->tipo == NOT)
            return aplicar_de_morgan(sub->left);
        else if (sub->tipo == AND)
            return criar_binario(OR,
                aplicar_de_morgan(criar_unario(NOT, sub->left)),
                aplicar_de_morgan(criar_unario(NOT, sub->right)));
        else if (sub->tipo == OR)
            return criar_binario(AND,
                aplicar_de_morgan(criar_unario(NOT, sub->left)),
                aplicar_de_morgan(criar_unario(NOT, sub->right)));
        else
            return criar_unario(NOT, aplicar_de_morgan(sub));
    } else if (expr->tipo == AND || expr->tipo == OR) {
        return criar_binario(expr->tipo,
            aplicar_de_morgan(expr->left),
            aplicar_de_morgan(expr->right));
    } else {
        return expr;
    }
}

NODE *distribuir_or(NODE *expr) {
    if (!expr) return NULL;
    if (expr->tipo == OR) {
        NODE *a = distribuir_or(expr->left);
        NODE *b = distribuir_or(expr->right);
        if (a->tipo == AND)
            return criar_binario(AND,
                distribuir_or(criar_binario(OR, a->left, b)),
                distribuir_or(criar_binario(OR, a->right, b)));
        else if (b->tipo == AND)
            return criar_binario(AND,
                distribuir_or(criar_binario(OR, a, b->left)),
                distribuir_or(criar_binario(OR, a, b->right)));
        else
            return criar_binario(OR, a, b);
    } else if (expr->tipo == AND) {
        return criar_binario(AND,
            distribuir_or(expr->left),
            distribuir_or(expr->right));
    } else {
        return expr;
    }
}

NODE *distribuir_and(NODE *expr) {
    if (!expr) return NULL;
    if (expr->tipo == AND) {
        NODE *a = distribuir_and(expr->left);
        NODE *b = distribuir_and(expr->right);
        if (a->tipo == OR)
            return criar_binario(OR,
                distribuir_and(criar_binario(AND, a->left, b)),
                distribuir_and(criar_binario(AND, a->right, b)));
        else if (b->tipo == OR)
            return criar_binario(OR,
                distribuir_and(criar_binario(AND, a, b->left)),
                distribuir_and(criar_binario(AND, a, b->right)));
        else
            return criar_binario(AND, a, b);
    } else if (expr->tipo == OR) {
        return criar_binario(OR,
            distribuir_and(expr->left),
            distribuir_and(expr->right));
    } else {
        return expr;
    }
}

void imprimir(NODE *expr) {
    if (!expr) return;

    switch (expr->tipo) {
        case VAR:
            printf("%c", expr->var);
            break;
        case NOT:
            printf("!");
            imprimir(expr->left);
            break;
        case AND:
            printf("(");
            imprimir(expr->left);
            printf(" ^ ");
            imprimir(expr->right);
            printf(")");
            break;
        case OR:
            printf("(");
            imprimir(expr->left);
            printf(" V ");
            imprimir(expr->right);
            printf(")");
            break;
        case IMPLIES:
            printf("(");
            imprimir(expr->left);
            printf(" -> ");
            imprimir(expr->right);
            printf(")");
            break;
        case IFF:
            printf("(");
            imprimir(expr->left);
            printf(" <-> ");
            imprimir(expr->right);
            printf(")");
            break;
    }
}

void liberar_arvore(NODE *raiz) {
    if (!raiz) return;
    liberar_arvore(raiz->left);
    liberar_arvore(raiz->right);
    free(raiz);
}

int main(){
    char entrada1[256];
    printf("Conversor de Formulas Proposicionais\n");
    printf("Operadores suportados: !, ^, V, ->, <->\n");

    while (1) {
        printf("\nDigite a formula (ou 'sair'): ");
        fgets(entrada1, sizeof(entrada1), stdin);
        entrada1[strcspn(entrada1, "\n")] = 0;

        if (strcmp(entrada1, "sair") == 0) break;

        entrada = entrada1;
        next();
        NODE *expr = parse_expr();

        if(!expr) {
            printf("Erro ao parsear a formula!\n");
            continue;
        }

        printf("\nFormula original: ");
        imprimir(expr);

        NODE *fnc = distribuir_or(aplicar_de_morgan(eliminar_implicacoes(expr)));
        printf("\nFNC: ");
        imprimir(fnc);

        NODE *fnd = distribuir_and(aplicar_de_morgan(eliminar_implicacoes(expr)));
        printf("\nFND: ");
        imprimir(fnd);

        liberar_arvore(expr);
        liberar_arvore(fnc);
        liberar_arvore(fnd);

        printf("\n");
    }

    return 0;
}