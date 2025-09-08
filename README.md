# Guia de Uso dos Testes `testMatrix.txt` e `testIntegral.txt` no Projeto `lab1`

**Projeto PTR–2025–2**  
Data: *28/08/2025*

---

## Visão Geral

Este documento descreve como executar testes do projeto `lab1` utilizando os arquivos `testMatrix.txt` e `testIntegral.txt`, localizados no diretório raiz do projeto (`lab1/`). Cada um desses arquivos contém um *programa de teste* que deve ser **colado dentro de `src/main.c`** para compilar e rodar os testes com o `Makefile` fornecido.

---

## Como Executar o Teste de `Matrix`

1. Abra o arquivo `lab1/testMatrix.txt` no editor.
2. Selecione **todo** o conteúdo de `testMatrix.txt` e copie.
3. Abra `lab1/src/main.c`. Apague o conteúdo atual de `main.c` e **cole** o conteúdo copiado de `testMatrix.txt`.
4. Salve o arquivo.
5. No terminal, dentro do diretório `lab1`, execute:

make
./lab1

## Como Executar o Teste de Integral

1. Abra o arquivo lab1/testIntegral.txt.
2. Copie todo o seu conteúdo.
3. Abra lab1/src/main.c, substitua o conteúdo atual colando o texto de testIntegral.txt.
4. Salve o arquivo.
5. No terminal, dentro do diretório `lab1`, execute:

make
./lab1
