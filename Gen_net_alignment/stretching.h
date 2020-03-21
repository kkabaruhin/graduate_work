#pragma once
#include"structures_and_classes.h"

//�� ����� ������������. �� ������ ������ �������. 
//� ������ ������ ���� ���: <������_�������_��������; ����������_������_����_�_�������_��_���������>
//�� ������ ������ ���� ���: <������_�������_����; ����������_������_��������_�_�������_��_���������>
//� ������ ������ ���� ���: <������_�����_��������; ����������_�����_����_�_�������_��_���������>

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