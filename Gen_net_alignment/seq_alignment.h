#pragma once
//файл для функций выравнивания генетических последовательностей
#include<vector>
#include<iostream>
#include<unordered_map>
#include<algorithm>

using namespace std;

//штраф для выравнивания последовательностей за несовподение символов
double g = -2;
unordered_map<char, unordered_map<char, double>> nucl_substr_matrix; 
//выравнивание некоторых пар последовательностей иногда приходится искать несколько раз
//поэтому эта таблица хранит выравнивания, сделанные ранее в рамках текущей задачи
unordered_map<string, unordered_map<string, double>> scores_seq_alig; 

//заполнение матрицы нуклеотидов
void fill_subst_matr()
{
	auto nucleotides = { 'A', 'T', 'G', 'C' };

	for (char c1 : nucleotides)
		for (char c2 : nucleotides)
		{
			if (c1 == c2)
				nucl_substr_matrix[c1][c2] = 1;
			else
				nucl_substr_matrix[c1][c2] = g;
		}
}

//короткая запись для получения значения выравнивания двух символов
double p(char c1, char c2)
{
	return nucl_substr_matrix[c1][c2];
}

//обновляет переменные в начале работы алгорима глобального выравнивания сетей
void prepare_table() {
	fill_subst_matr();
	scores_seq_alig = unordered_map<string, unordered_map<string, double>>();
}

//если seq1 и seq2 еще не были выровнены, то необходимо явно их выровнять
//выравнивания происходит классическим способом динамического программирования с той особенностью
//что нулевые строка и столбец таблицы выравнивания заполняются нулями, в угоду спецификации задачи
double find_new_seq_alig(string& const seq1, string& const seq2)
{
	vector<vector<double>> matr;
	matr.resize(seq1.length() + 1);

	for (int i = 0; i < matr.size(); i++)
		matr[i].resize(seq2.length() + 1);

	for (int i = 0; i < matr.size(); i++)
		matr[i][0] = 0;
	for (int i = 0; i < matr[0].size(); i++)
		matr[0][i] = 0;

	for (int i = 1; i < matr.size(); i++)
		for (int j = 1; j < matr[0].size(); j++)
		{
			double max_val = LONG_MIN;

			max_val = max(max_val, matr[i - 1][j - 1] + p(seq1[i - 1], seq2[j - 1]));

			if (i < matr.size() - 1 && j < matr[0].size() - 1)
			{
				max_val = max(max_val, matr[i - 1][j] + g);
				max_val = max(max_val, matr[i][j - 1] + g);
			}
			if (i == matr.size() - 1)
			{
				max_val = max(max_val, matr[i][j - 1]);
			}
			if (j == matr[0].size() - 1)
			{
				max_val = max(max_val, matr[i - 1][j]);
			}

			matr[i][j] = max_val;
		}

	return matr[matr.size() - 1][matr[0].size() - 1];
}

//локальне сравнение последовательностей. Ищем вхождение seq2 в seq1
//если такие строки уже были выровнены, просто возвращаем запомненное значение
double local_alignment_seq(string& const seq1, string& const seq2)
{
	if (scores_seq_alig.count(seq1) > 0) {
		if (scores_seq_alig[seq1].count(seq2) > 0) {
			return scores_seq_alig[seq1][seq2];
		}
	}
	double result = find_new_seq_alig(seq1, seq2);

	scores_seq_alig[seq1][seq2] = result;
	return result;
}