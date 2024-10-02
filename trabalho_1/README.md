# Trabalho Prático 1: Convolução
## Particionamento

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
