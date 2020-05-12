#pragma once
//файл для функций оценивания
#include"seq_alignment.h"
#include"connectivity_components.h"
#include"dropped_nodes.h"
#include"count_unavailable_nodes.h"
#include"stretching.h"

//функция для выравниваний, сгенерированных полным перебором (неэффективная, для эффективных алгоритмов не используется)
double score_alignment(alignment& const alig, Net& const pattern, Net& const net)
{
	double result = 0;

	//просто сумма выравнивания последовательностей
	for (pair<int,vector<int>> i : alig.Nodes)
		for (int j: i.second)
			result += local_alignment_seq(pattern.Nodes[i.first].sequence, net.Nodes[j].sequence);

	//ищем количество компонент связности в подграфе
	connectivity_components cc = connectivity_components();

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		int count_cc = cc.find_count_comp(alig.Edges[i]);
		
		if (count_cc == 0)
			result *= 0.001;

		if (count_cc == 1)
			result *= 1.1;

		if (count_cc > 1)
			result *= pow(0.9, count_cc);
	}
	
	count_dropped_nodes c_d_n = count_dropped_nodes();

	for (pair<int, vector<int>> vec : alig.Nodes)
	{
		int dropped_nodes = c_d_n.find_count_dropped_nodes(net, vec.second);

		result *= pow(0.97, dropped_nodes);
	}

	count_unavailable_nodes c_u_n = count_unavailable_nodes();

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		int unavailable_nodes = c_u_n.find_count_unavailable_nodes(alig.Nodes[pattern.Edges[i].from],
			alig.Nodes[pattern.Edges[i].to],
			alig.Edges[i]);

		result *= pow(0.95, unavailable_nodes);
	}

	//ищем растяжения вершин и ребер
	{
		unordered_map<int, int> pattern_node_stretching, net_node_stretching, pattern_edge_stretching;

		find_stretching(alig, pattern_node_stretching, net_node_stretching, pattern_edge_stretching);

		for (pair<int, int> x : pattern_node_stretching)
		{
			if (x.second == 0)
				result *= 0.93;
			if (x.second > 1)
				result *= pow(0.99, x.second);
		}

		for (pair<int, int> x : net_node_stretching)
		{
			if (x.second > 1)
				result *= pow(0.99, x.second);
		}

		for (pair<int, int> x : pattern_edge_stretching)
		{
			if (x.second == 0)
				result *= 0.8;
			if (x.second > 1)
				result *= pow(0.99, x.second);
		}
	}

	return result;

	//check! хорошо - когда между вершинами сети, выровненными с одной вершиной паттерна есть путь от одной к другой. Еще лучше, есть есть такой путь, проходящий только через выровненные вершины. Когда пути нет - плохо
	//check! хорошо - когда подграф, с которым выровнено ребро паттера, односвязно. Когда компонент связности больше одной - плохо
	//check! хорошо - когда в подграфе есть путь из всех вершин, с которыми выровнено начало ребра паттерна во все вершины, с которыми выровнен конец ребра паттерна. Чем чаще это не так - тем хуже
	//хорошо - когда подграф вырождается в путь
	//хорошо - когда в подграфе есть вершина, из которой можно добраться до всех вершин в подграфе
}

//основная функция подсчета оценки выравнивания
double score_alignment_for_working_path(alignment& const alig, Net& const pattern, Net& const net,
										double sigma, double gamma, double etta) {
	double result = 0;
	//просто сумма выравнивания последовательностей
	for (pair<int, vector<int>> i : alig.Nodes)
		for (int j : i.second)
			result += local_alignment_seq(pattern.Nodes[i.first].sequence, net.Nodes[j].sequence);

	{
		unordered_map<int, int> pattern_node_stretching, net_node_stretching, pattern_edge_stretching;

		find_stretching(alig, pattern_node_stretching, net_node_stretching, pattern_edge_stretching);

		//растяжения одной вершины паттерна на несколько узлов сети
		for (pair<int, int> x : pattern_node_stretching)
		{
			if (x.second > 1)
				result += sigma * (x.second - 1);
		}

		//растяжение одного узла сети на несколько вершин паттерна
		for (pair<int, int> x : net_node_stretching)
		{
			if (x.second > 1)
				result += gamma * (x.second - 1);
		}

		//растяжение ребра паттерна не путь в сети
		for (pair<int, int> x : pattern_edge_stretching)
		{
			if (x.second > 1)
				result += etta * (x.second - 1);
			
		}
	}

	return result;
}