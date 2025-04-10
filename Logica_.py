from itertools import product
## Esse código teve ajuda de llm para a correnção de bugs e para o aperfeiçoamento do programa
def substituir_operadores(expr):
    expr = expr.replace(" ", "")
    expr = expr.replace("!", "not ")
    expr = expr.replace("v", " or ")
    expr = expr.replace("^", " and ")

    while ">" in expr:
        i = expr.index(">")
        a = expr[i - 1]
        b = expr[i + 1]
        expr = expr[:i - 1] + f"(not {a} or {b})" + expr[i + 2:]

    while "=" in expr:
        i = expr.index("=")
        a = expr[i - 1]
        b = expr[i + 1]
        expr = expr[:i - 1] + f"({a} == {b})" + expr[i + 2:]

    return expr

def avaliar_expressao(expr, valores):
    expr_convertida = substituir_operadores(expr)
    for var in valores:
        expr_convertida = expr_convertida.replace(var, str(valores[var]))
    return eval(expr_convertida)

def gerar_combinacoes(variaveis):
    return list(product([0, 1], repeat=len(variaveis)))

def main():
    variaveis = []
    sentencas = []

    n = int(input("Quantas variáveis tem a proposição? "))
    m = 2  

    for i in range(n):
        var = input(f"Digite a variável {i+1}: ").strip()
        variaveis.append(var)

    print("Digite duas sentenças para comparação lógica:")
    for i in range(m):
        sent = input(f"Sentença {i+1}: ").strip()
        sentencas.append(sent)

    linhas = []
    for valores in gerar_combinacoes(variaveis):
        linha = dict(zip(variaveis, valores))
        for sent in sentencas:
            linha[sent] = int(avaliar_expressao(sent, linha))
        linhas.append(linha)

    equivalentes = all(linha[sentencas[0]] == linha[sentencas[1]] for linha in linhas)

    print("\nResultado da comparação:")
    if equivalentes:
        print("As sentenças são logicamente equivalentes.")
    else:
        print("As sentenças NÃO são logicamente equivalentes.")

if __name__ == "__main__":
    main()