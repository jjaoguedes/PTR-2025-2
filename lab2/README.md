
-----

### **Instruções de Execução**

Para começar, abra o terminal na raiz do projeto.

#### **1. Compilação e Execução sem Carga**

Para compilar e executar a simulação **sem carga**, rode o seguinte comando:

```bash
make
./sim_robot
```

A simulação irá gerar os seguintes arquivos na pasta `out/`:

  * **`sim_out.tsv`**: Contém os valores de `t`, `v`, `w`, `yx` e `yy` da simulação de 20 segundos.
  * **`periods_sem_carga.csv`**: Contém a série dos períodos `T(k)` e o jitter `J(k)` da simulação sem carga.

Para plotar a trajetória do robô e visualizar outros dados da simulação, execute o seguinte comando:

```bash
python3 scripts/plot.py
```

#### **2. Compilação e Execução com Carga**

Para rodar a simulação **com carga**, execute o seguinte comando:

```bash
make load
./sim_robot --load
```

Este comando gera os seguintes arquivos na pasta `out/`:

  * **`periods_com_carga.csv`**: Contém a série dos períodos `T(k)` e o jitter `J(k)` da simulação com carga.
  * **`tabela_periodos_jitter.md`** ou **`tabela_periodos_jitter.csv`**: Esta tabela de estatísticas é gerada pelo script `gera_tabela.py` para análise do jitter.

Para gerar a tabela de jitter, rode:

```bash
python3 scripts/gera_tabela.py
```

-----

### **Estrutura dos Arquivos de Saída**

Após a execução, a pasta `out/` conterá os seguintes arquivos:

  * **`sim_out.tsv`**: Dados da simulação de 20s.
  * **`periods_sem_carga.csv`**: Períodos e jitter para a simulação sem carga.
  * **`periods_com_carga.csv`**: Períodos e jitter para a simulação com carga.
  * **`tabela_periodos_jitter.md`** ou **`tabela_periodos_jitter.csv`**: Tabela de estatísticas do jitter.

-----
