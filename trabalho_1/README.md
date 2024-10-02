# Trabalho Prático 1: Convolução
## Particionamento

Parte do algoritimo é paralelizada por dados e parte é paralelizada por instrução. Se você observar o grafo abixo, as tarefas que geram multiplas tarefas indicam paralelização por instrução e as tarefas que estão anotadas como `**paralelizável**`, indicam paralelização por dados.

Existe uma tarefa para ler as dimensões da imagem que gera outra tarefa para ler as dimensões do filtro. A última tarefa gera duas outras tarefas, uma para alocar a memória para armazenar a imagem e outra para ler o endereço da imagem de destino, que por sua vez irá gerar uma tarefa para ler o seed, que irá gerar uma tarefa para gerar o filtro.

Na alocação de memória são geradas 3 sub-tarefas. Uma das tarefas é reponsável por criar uma "borda" de tamanho `f/2` (onde `f/2` é a dimensão do filtro) zerada na parte superior da imagem, uma é responsável por criar uma borda de mesmo tamanho na parte inferior e a terceira é reponsável por preencher a parte central da imagem. As três tarefas podem ser paralelizadas para cada linha dentro do seu domínio.

Umas das três novas tarefas geradas é responsável por preencher as `f/2` colunas laterais esquerdas com `0`, criando uma borda do lado esquerdo da imagem, outra das tarefas é responsável por preencher a borda do lado direito e a terceira é reponsável por armazenar o valor da imagem na posição respectiva dentro das bordas. Essas três tarefas também podem ser paralelizadas para cada coluna dentro do seu domínio.

Após a inicialização da imagem, partimos para o processamento, onde, para cada pixel da imagem, nós inicializamos uma variável local de somatório e aplicamos a convolução de forma paralela.

A convolução aplica a multiplicação e adiciona a variável somatório, paralelamente, para cada pixel do filtro. Cada thread executada paralelamente tem sua própria variável sum local e ao final é feita a redução para unificar a variável.

Depos de aplicar a convolução, nós comparamos os somatórios e tribuimos os valores maiximos e minimos locais de cada thread, e depois comparamos esses valores para atribuir os valores globais.

Em seguida, partimos pra o slvament e a exibição dos resultdos, onde geramos tarefas para desalocar a memória do filtrio, salvar o resultado da convulução em uma imagem de destino, desalocar a memória da imagem e imprimir os resultados, finalizado a execução do programa ao final desses processos.

```mermaid
flowchart TD
    A[Início] --> B(Ler dimensões x,y da imagem)
    subgraph r1 [Leitura e inicialização das variáveis]
        style r1 stroke-dasharray: 5, 5, fill:none, stroke: grey

        B --> E(Ler a dimensão f do filtro)
        E --> AD(Ler endereço de imagem de destino)
        AD --> F(Ler o Seed)
        E --> G(Alocar memória para armazenar imagem)
        G --> H(**Paralelizável:** para toda linha i = 0 a f/2, alocar memória inicializada a zero)
        G --> M(**Paralelizável:** para toda linha de i = x + f/2 a x + f, alocar memória inicializada a zero)
        I --> J(**Paralelizável:** para toda coluna de 0 a f/2 inicializar a zero)
        I --> f(**Paralelizável:** para toda coluna j = f/2 a y + f/ 2 - 1 armazenar valor da imagem na posição i, j)
        I --> L(**Paralelizável:** para toda coluna j = y + f/2 a y + f - 1 inicializar a zero)
        G --> I(**Paralelizável:** para toda linha i = x + f/2 a x + f - 1)
        F --> N(Gerar filtro)
    end
    subgraph r2 [Processamento da imagem]
        style r2 stroke-dasharray: 5, 5, fill:none, stroke: grey

        M --> O(**Paralelizável:** Inicializar variáveis locais de mínimo, máximo e somatório)
        L --> O
        f --> O
        J --> O
        H --> O
        N --> O
        O --> Q(**Paralelizável:** para cada linha i = f/2 a x + f/2 - 1 da imagem)
        Q --> S(**Paralelizável:** para cada coluna j = f/2 a j + f/2 - 1 aplicar convolução)
        S --> T(**Paralelizável:** para cada linha k no filtro e i + k na imagem)
        T --> X(**Paralelizável:** para cada coluna l no filtro e j + l na imagem aplicar multiplicação e adicionar ao somatório)
        X --> Y(Comparar somatórios locais e atribui valor aos máximos e mínimos locais)
        Y --> Z(Compara máximos e mínimos locais e atribui ao máximo e mínimo globais)
    end
    subgraph r3 [Salvamento e exibição de resultados, desalocação da memória]
        style r3 stroke-dasharray: 5, 5, fill:none, stroke: grey
        Z --> W(Imprime resultados)
        Z --> AC(**Paralelizável:** Desaloca memória para filtro)
        Z --> C(Salva resultado da convolução em imagem de destino)
        C --> AB(**Paralelizável:** Desaloca memória para imagem)
    end

    AB --> EXIT(EXIT_SUCCESS)
    AC --> EXIT
    W --> EXIT
```
## Comunicação
## Aglomeração
## Mapeamento
A atribuição de tarefas é realizada de forma dinâmica. Cada unidade de processamento deve ser alocada para um processo, e essa gestão é responsabilidade do Sistema Operacional da máquina. A expectativa é que a distribuição seja uniforme entre os elementos de processamento. Quando uma unidade de processamento fica ociosa, ela consome um bloco de tarefas do pool de processamento.
