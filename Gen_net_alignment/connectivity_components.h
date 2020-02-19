#pragma once
#include"structures_and_classes.h"

class connectivity_components
{
private: 
	unordered_map<int, bool> used;
	unordered_map<int, vector<int>> adjacency_list;

	void dfs(int v) {
		used[v] = true;
		for (size_t i = 0; i<adjacency_list[v].size(); ++i) {
			int to = adjacency_list[v][i];
			if (!used[to])
				dfs(to);
		}
	}

	void clear()
	{
		used.clear();
		adjacency_list.clear();
	}

	

public:
	//���������� ���������� ��������� ���������
	int find_count_comp(vector<edge>& const graph)
	{
		int result = 0;

		clear();
		//��������� ������ ���� � ������ ���������
		//���������� ����� ����, � ������� ���� ���� ����� a->b, �� ���� � ����� b->a
		for (edge ed : graph)
		{
			adjacency_list[ed.from].push_back(ed.to);
			adjacency_list[ed.to].push_back(ed.from);

			used[ed.from] = false;
			used[ed.to] = false;
		}

		for (pair<int, bool> node : used)
		{
			if (!used[node.first])
			{
				dfs(node.first);
				result++;
			}
		}

		return result;
	}

};

