#pragma once
//на входе: набор вершин в сети, с которыми выровнена какая-то вершина паттерна
//на выходе: выпавшие вершины, то есть такие, через которые проходит минимальный путь до ближайшей выровненной вершины с бОльшим номером, но которая не выровнена
//идея: 1)для каждой выровненной вершина находим вектор кратчайших расстояний до всех остальных выровненных вершин с помощью bfs (опять переходя к неориентированному графу)
//2)составляем из полученных векторов матрицу и в правой верхней половине, исключая нулевую диагональ, в каждой строчке ищем минимум
//3)от каждого минимума вычитаем 1, суммируем и получаем ответ

#include"structures_and_classes.h"
#include<queue>

using namespace std;

class count_dropped_nodes
{
	vector<vector<int>> matr;
	unordered_map<int, Node> new_graph;

	void initialize(int size, int cnt_nodes)
	{
		matr.clear();
		new_graph.clear();
		matr.resize(size);

		for (int i = 0; i < size; i++)
			matr.resize(size);
	}

	void fill_new_graph(unordered_map<int, Node>& const nodes)
	{
		for (pair<int, Node> n : nodes)
		{
			new_graph[n.first] = n.second;
		}

		for (pair<int, Node> n : nodes)
		{
			for (int i : n.second.adjacent_nodes)
				new_graph[i].adjacent_nodes.push_back(n.first);
		}
	}

	vector<int> find_distances(int index, vector<int>& const nodes)
	{
		int count_visited = 1;
		unordered_map<int, int> v;
		vector<int> result;
		for (pair<int, Node> n : new_graph)
		{
			v[n.first] = INT_MAX;
		}
		result.resize(nodes.size(), INT_MAX);

		result[index] = 0;
		v[nodes[index]] = 0;

		queue<int> q;

		q.push(nodes[index]);

		while (true)
		{
			if (count_visited == nodes.size())
				return result;

			if (q.empty())
			{
				cout << "something wrong" << endl;
				return result;
			}

			int current_ind = q.front();
			q.pop();

			for (int n : new_graph[current_ind].adjacent_nodes)
			{
				if (v[n] > v[current_ind] + 1)
				{
					v[n] = v[current_ind] + 1;
					q.push(n);

					for (int i = 0; i < nodes.size(); i++)
						if (nodes[i] == n)
						{
							result[i] = v[n];
							count_visited++;
							break;
						}
				}
			}
		}
	}

public:

	int find_count_dropped_nodes(Net& const net, vector<int>& const nodes)
	{
		initialize(nodes.size(), net.Nodes.size());

		int result = 0;

		fill_new_graph(net.Nodes);

		for (int i = 0; i < nodes.size(); i++)
		{
			matr[i] = find_distances(i, nodes);
		}

		for (int i = 0; i < nodes.size() - 1; i++)
		{
			int min_in_line = INT_MAX;
			for (int j = i + 1; j < nodes.size(); j++)
				min_in_line = min(min_in_line, matr[i][j]);

			result += min_in_line;
		}

		result = result - (nodes.size() - 1);

		return result;
	}
};

