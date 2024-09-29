Compilador Fase 1 : Pascal+-

Autores:
Erik Samuel Viana Hsu - 10403109
Thiago Shihan Cardoso Toma - 10400764

Como compilar o programa (considerando que o nome do programa seja compilador.c):
gcc -g -Og -Wall compilador.c -o compilador

Como executar o programa:
./compilador {nome_arquivo_txt_entrada}

Sobre o compilador:
Este compilador é constituído por um analisador léxico e sintático para reconhecer a linguagem Pascal+-. 
O código foi implementado em C e é capaz de ler um arquivo fonte, 
identificar tokens e verificar a sintaxe do código.

Estrutura do projeto:
1. Analisador Léxico: Identifica e classifica os tokens no código fonte. Os tokens suportados incluem:
- Identificadores
- Números (em binário)
- Comentários (de linha e de bloco)
- Operadores aritméticos e relacionais
- Palavras reservadas (ex.: begin, if, for, etc.)
- Delimitadores (ex.: ;, ,, .)

2. Analisador Sintático: Implementa a gramática da linguagem e valida a estrutura do código. 
As regras sintáticas implementadas incluem:

- Estrutura básica de um programa
- Declaração de variáveis
- Comandos compostos e condicionais
- Comandos de entrada e saída

Funcionamento:
1. Leitura do Arquivo
O arquivo fonte é lido completamente e armazenado em um buffer. 
O analisador léxico utiliza esse buffer para processar o conteúdo.

2. Identificação de Tokens 
A função obter_atomo percorre o buffer, ignorando espaços em branco e identificando tokens válidos. 
Dependendo do token encontrado, ele é classificado e atribuído a uma estrutura chamada TInfoAtomo.

3. Análise Sintática 
A função programa inicia a análise sintática. 
Ela consome os tokens e verifica se estão de acordo com a gramática definida. 
Em caso de erro, uma mensagem é exibida, e o programa é encerrado.

4. Exibição de Resultados 
Após a análise, o programa exibe a linha e o tipo de cada token identificado, 
além de mensagens de erro quando apropriado.
