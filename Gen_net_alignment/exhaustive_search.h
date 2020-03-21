#pragma once
#include"structures_and_classes.h"
#include"score_alignment.h"
#include<set>

class exhaustive_search_class
{
	//два вектора для того, чтобы в произвольном порядке хранить индексы вершин паттерна и сети
	vector<int> display_pattern_nodes;
	vector<int> display_net_nodes;

	//m - кол-во вершин в паттерне. n - кол-во вершин в сети. q, k - кол-во ребер в паттерне и сети соответственно
	alignment from_bool_to_alig(vector<bool> transposition, int m, int n, int q, int k, Net& const pattern, Net& const net)
	{
		alignment result;

		result.Edges = vector<vector<edge>>();

		for (int i = 0; i < q; i++)
			result.Edges.push_back(vector<edge>());

		for (int i = 0; i < m; i++)
			for (int j = 0; j < n; j++)
				if (transposition[i*n + j] == true)
				{
					if (result.Nodes[display_pattern_nodes[i]].empty())
						result.Nodes[display_pattern_nodes[i]] = vector<int>();

					result.Nodes[display_pattern_nodes[i]].push_back(display_net_nodes[j]);
				}

		for (int i = 0; i < q; i++)
			for (int j = 0; j < k; j++)
				if (transposition[m*n + i * k + j] == true)
				{
					result.Edges[i].push_back(net.Edges[j]);
				}

		result.score = score_alignment(result, pattern, net);

		return result;
	}

	bool next(vector<bool>& transposition)
	{
		int i = 0;

		while (i < transposition.size())
		{
			if (transposition[i] == false)
			{
				transposition[i] = true;
				return true;
			}
			else
			{
				transposition[i] = false;
				i++;
			}
		}

		return false;
	}

public:
	set<alignment> exhaustive_search(Net& const pattern, Net& const net)
	{
		int m, n, q, k;
		m = pattern.Nodes.size();
		n = net.Nodes.size();
		q = pattern.Edges.size();
		k = net.Edges.size();
		
		for (pair<int, Node> n : pattern.Nodes)
			display_pattern_nodes.push_back(n.second.index);

		for (pair<int, Node> n : net.Nodes)
			display_net_nodes.push_back(n.second.index);

		vector<bool> transposition(m*n*q*k, false);
		set<alignment> result;

		while (true)
		{
			result.insert(from_bool_to_alig(transposition, m, n, q, k, pattern, net));

			if (result.size() > 20)
			{
				result.erase(result.begin());
			}

			if (!next(transposition))
				return result;
		}
	}
};