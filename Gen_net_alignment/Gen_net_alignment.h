#pragma once
#include"seq_alignment.h"
#include"connectivity_components.h"
#include"structures_and_classes.h"
#include"dropped_nodes.h"
#include<set>
#include"count_unavailable_nodes.h"

//основной принцип оценивания: цем масштабнее задача, тем менее значителен вкад каждой хорошей и плохой вещи
//чем больше требований, тем сложнее соблюсти их все
//чем больше сеть и паттерн, тем лояльнее оценивание
double score_alignment(alignment& const alig, Net& const pattern, Net& const net)
{
	double result = 0;

	fill_subst_matr();
	//просто сумма выравнивания последовательностей
	for (pair<int,vector<int>> i : alig.Nodes)
		for (int j: i.second)
			result += local_alignment_seq(pattern.Nodes[i.first].sequence, net.Nodes[j].sequence);

	//ищем количество компонент связности в подграфе
	connectivity_components cc = connectivity_components();

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		int count_cc = cc.find_count_comp(alig.Edges[i]);
		
		//TODO как учитывать количество компонент связности

		if (count_cc == 1)
			result += 2;
		else
			result = result - count_cc * 2;
	}


	count_dropped_nodes c_d_n = count_dropped_nodes();

	for (pair<int, vector<int>> vec : alig.Nodes)
	{
		int dropped_nodes = c_d_n.find_count_dropped_nodes(net, vec.second);
		//TODO количество выбитых вершин

		result = result - dropped_nodes;
	}

	count_unavailable_nodes c_u_n = count_unavailable_nodes();

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		int unavailable_nodes = c_u_n.find_count_unavailable_nodes(alig.Nodes[pattern.Edges[i].from],
			alig.Nodes[pattern.Edges[i].to],
			alig.Edges[i]);
		//TODO количество недостижимых вершин

		result = result - unavailable_nodes;
	}

	return result;

	//check! хорошо - когда между вершинами сети, выровненными с одной вершиной паттерна есть путь от одной к другой. Еще лучше, есть есть такой путь, проходящий только через выровненные вершины. Когда пути нет - плохо
	//check! хорошо - когда подграф, с которым выровнено ребро паттера, односвязно. Когда компонент связности больше одной - плохо
	//check! хорошо - когда в подграфе есть путь из всех вершин, с которыми выровнено начало ребра паттерна во все вершины, с которыми выровнен конец ребра паттерна. Чем чаще это не так - тем хуже
	//хорошо - когда подграф вырождается в путь
	//хорошо - когда в подграфе есть вершина, из которой можно добраться до всех вершин в подграфе
	//что с лишними ребрами?
}