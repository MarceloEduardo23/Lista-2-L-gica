#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(){
    printf("Verificador de consequencias logicas: \n");
    printf("Escolha o teorema: \n");
    printf("1 - Modus Pones (P -> Q, P |- Q)\n2 - Modus Tollens (P -> Q, !Q |- !P)\nDigite sua escolha: ");
    
    int escolha;
    scanf("%d", &escolha);
    getchar();

    char p[100], q[100];

    printf("Digite a proposicao p (ex: 'Esta chovendo'): ");
    fgets(p, sizeof(p), stdin);
    p[strcspn(p, "\n")] = 0;

    printf("Digite a proposicao q (ex: 'A rua esta molhada'): ");
    fgets(q, sizeof(q), stdin);
    q[strcspn(q, "\n")] = 0;

    char var;

    if(escolha == 1){
        printf("%s e verdadeiro? (s/n) ", p);
        scanf("%c", &var);
        var = tolower(var);
        
        if(var == 's'){
            printf("Conclusao: %s e verdadeiro!\n", q);
        }else{
            printf("Nao e possivel concluir nada com os valores dados.\n");
        }
    }else if(escolha == 2){
        printf("%s e falso? (s/n) ", q);
        scanf("%c", &var);
        var = tolower(var);

        if(var == 's'){
            printf("Conclusao: %s e falso!\n", p);
        }else{
            printf("Nao e possivel aplicar Modus Tollens se q nao e falso.\n");
        }
    }else{
        printf("Opcao invalida!\n");
    }

    return 0;
}