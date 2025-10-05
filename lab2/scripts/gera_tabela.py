import pandas as pd
from pathlib import Path

SEM = "out/periods_sem_carga.csv"
COM = "out/periods_com_carga.csv"

def load_and_clean(path):
    df = pd.read_csv(path)
    # Garantir colunas esperadas
    expected = {"k", "t_wall(s)", "T(s)", "J(s)"}
    if not expected.issubset(df.columns):
        raise ValueError(f"{path} não contém as colunas {expected}")

    # Descartar a primeira linha (k == 0), que costuma ter período atípico
    df = df[df["k"] > 0].reset_index(drop=True)
    return df

def stats(series):
    return {
        "Média": series.mean(),
        "Variância": series.var(ddof=0),   # variância populacional
        "Desvio padrão": series.std(ddof=0),
        "Mínimo": series.min(),
        "Máximo": series.max(),
    }

def format_float(x):
    # Ajuste aqui a precisão/format que preferir
    return f"{x:.9f}"

def main():
    # Verifica existência dos arquivos
    for f in (SEM, COM):
        if not Path(f).exists():
            raise FileNotFoundError(
                f"Arquivo '{f}' não encontrado. Rode o simulador para gerá-lo."
            )

    df_sem = load_and_clean(SEM)
    df_com = load_and_clean(COM)

    # Calcula estatísticas para T e J, sem/ com carga
    s_T_sem = stats(df_sem["T(s)"])
    s_T_com = stats(df_com["T(s)"])
    s_J_sem = stats(df_sem["J(s)"])
    s_J_com = stats(df_com["J(s)"])

    # Monta DataFrame final com colunas nas quatro combinações
    tabela = pd.DataFrame({
        "T(k) sem carga": s_T_sem,
        "T(k) com carga": s_T_com,
        "J(k) sem carga": s_J_sem,
        "J(k) com carga": s_J_com,
    })

    # Ordena linhas numa ordem amigável
    ordem = ["Média", "Variância", "Desvio padrão", "Mínimo", "Máximo"]
    tabela = tabela.loc[ordem]

    # Imprime Markdown no terminal
    # (com números já formatados)
    tabela_fmt = tabela.applymap(format_float)
    md = ["| Estatística | T(k) sem carga | T(k) com carga | J(k) sem carga | J(k) com carga |",
          "|-------------|-----------------|----------------|-----------------|----------------|"]
    for idx, row in tabela_fmt.iterrows():
        md.append(f"| {idx} | {row['T(k) sem carga']} | {row['T(k) com carga']} | {row['J(k) sem carga']} | {row['J(k) com carga']} |")
    md_text = "\n".join(md)
    print(md_text)

    # Salva Markdown e CSV
    Path("out/tabela_periodos_jitter.md").write_text(md_text, encoding="utf-8")
    tabela.to_csv("out/tabela_periodos_jitter.csv", index=True, encoding="utf-8")

    print("\nArquivos gerados:")
    print(" - tabela_periodos_jitter.md")
    print(" - tabela_periodos_jitter.csv")

if __name__ == "__main__":
    main()
