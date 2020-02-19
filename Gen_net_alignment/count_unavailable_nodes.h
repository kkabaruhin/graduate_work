#pragma once
//�� ����� 
//1)������ �������� ������, � ������� ��������� ������ �����
//2)������ �������� ������, � �������� �������� ����� �����
//3)����� ����� ����, � �������� ��������� ����� ��������
//�� ������ ���������� ��� (A, B) ������ �����, ��� A �� ������� �������, B �� �������, � � ��������,
//������������ ������� �� �������� ������, ��� ���� �� A � B, ���� ������ ������ � ����������� �����
//��������: bfs �� ������ ������� ������� �������

#include"structures_and_classes.h"
#include<queue>

class count_unavailable_nodes
{
	unordered_map<int, bool> used;
	unordered_map<int, Node> new_graph;

	void fill_new_graph(vector<edge>& const edges)
	{
		for (edge e : edges)
		{
			new_graph[e.from] = Node(e.from);
			new_graph[e.to] = Node(e.to);
		}

		for (edge e : edges)
		{
			new_graph[e.from].adjacent_nodes.push_back(e.to);
		}
	}

	bool contains(vector<int>& const to, int elem)
	{
		for (int i : to)
		{
			if (i == elem)
				return true;
		}

		return false;
	}

	int find_count(vector<int>& const to, int start)
	{
		for (pair<int, bool> i : used)
		{
			used[i.first] = false;
		}

		queue<int> q;
		q.push(start);

		int visited = 0;

		while (!q.empty())
		{
			int current_node = q.front();
			q.pop();

			if (used[current_node])
				continue;

			used[current_node] = true;

			if (contains(to, current_node))
				visited++;

			if (visited == to.size())
				return visited;

			for (int n : new_graph[current_node].adjacent_nodes)
			{
				if (!used[n])
					q.push(n);
			}
		}

		return visited;
	}


public:

	int find_count_unavailable_nodes(vector<int>& const from, vector<int>& const to, vector<edge>& const edges)
	{
		int result = 0;
		fill_new_graph(edges);
		
		for (pair<int, Node> i : new_graph)
		{
			used[i.first] = false;
		}

		for (int i : from)
		{
			result += find_count(to, i);
		}

		result = (from.size() * to.size()) - result;

		return result;
	}
};