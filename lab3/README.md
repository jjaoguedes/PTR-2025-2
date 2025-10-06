# Laboratório 3 — Simulação de Sistemas (Non-RT)

---

## Estrutura de Diretórios

```
lab3/
├── inc/            # Arquivos de cabeçalho (.h)
├── lib/            # Diretório reservado para bibliotecas futuras
├── obj/            # Arquivos objeto (.o) gerados na compilação
├── out/            # Logs e resultados (trajetórias, tempos)
├── scripts/        # Scripts em Python para análise e gráficos
├── src/            # Códigos-fonte principais (.c)
└── Makefile        # Arquivo de automação de compilação
```

---

## Compilação

No diretório raiz do projeto, execute:

```bash
make
```

Isso criará o executável `./lab3` e os diretórios necessários (`obj/`, `out/`, `scripts/`).

Para limpar todos os arquivos gerados:

```bash
make clean
```

---

## Execução da Simulação

### Modo normal (sem carga de CPU)

```bash
./lab3
```

- Executa a simulação por **20 segundos**.  
- Gera:
  - `out/traj.csv` → trajetória (saídas e referências)  
  - `out/perf_noload.csv` → tempos de execução e jitter por tarefa  

### Modo com carga de CPU

```bash
./lab3 --load
```

- Adiciona uma thread que gera carga de processamento.  
- Gera `out/perf_load.csv` com os tempos sob carga.

---

## Geração de Gráficos

Após a execução, pode ser usado o script Python para visualizar as trajetórias:

```bash
cd scripts
python3 plot_traj.py
```

Isso exibirá o gráfico de comparação entre as saídas do robô e as referências desejadas:

- Linhas contínuas → saídas simuladas (`y₁`, `y₂`)  
- Linhas tracejadas → referências (`x_ref`, `y_ref`)

---

## Análise Estatística de Desempenho

Para gerar a tabela comparativa de tempos (sem carga × com carga):

```bash
cd scripts
python3 stats_table.py
```

O script processa `out/perf_noload.csv` e `out/perf_load.csv`, e salva o arquivo:

```
out/stats.csv
```

Este arquivo contém média, variância, desvio padrão, valores máximos e mínimos dos tempos de execução e jitter de cada tarefa.

