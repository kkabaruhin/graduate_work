#pragma once
#include"structures_and_classes.h"

//на входе выравнивание. Ќа выходе четыре словар€. 
//¬ первом кажда€ пара это: <индекс_вершины_паттерна; количество_вершин_сети_с_кторыми_та_выровнена>
//во втором кажда€ пара это: <индекс_вершины_сети; количество_вершин_паттерна_с_кторыми_та_выровнена>
//¬ третьем кажда€ пара это: <индекс_ребра_паттерна; количество_ребер_сети_с_кторыми_то_выровнено>

void find_stretching(alignment& const alig,
	unordered_map<int, int>& pattern_node_stretching,
	unordered_map<int, int>& net_node_stretching,
	unordered_map<int, int>& pattern_edge_stretching)
{
	pattern_node_stretching = unordered_map<int, int>();
	net_node_stretching = unordered_map<int, int>();
	pattern_edge_stretching = unordered_map<int, int>();

	for (pair<int, vector<int>> x : alig.Nodes)
	{
		pattern_node_stretching[x.first] = x.second.size();

		for (int j : x.second)
			net_node_stretching[j] = net_node_stretching[j] + 1;
	}

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		pattern_edge_stretching[i] = alig.Edges[i].size();
	}
}