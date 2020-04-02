#pragma once
#include"seq_alignment.h"
#include"connectivity_components.h"
#include"structures_and_classes.h"
#include"dropped_nodes.h"
#include<set>
#include"count_unavailable_nodes.h"
#include"stretching.h"

//�������� ������� ����������: ��� ���������� ������, ��� ����� ���������� ���� ������ ������� � ������ ����
//��� ������ ����������, ��� ������� �������� �� ���
//��� ������ ���� � �������, ��� �������� ����������
double score_alignment(alignment& const alig, Net& const pattern, Net& const net)
{
	double result = 0;

	//������ ����� ������������ �������������������
	for (pair<int,vector<int>> i : alig.Nodes)
		for (int j: i.second)
			result += local_alignment_seq(pattern.Nodes[i.first].sequence, net.Nodes[j].sequence);

	//���� ���������� ��������� ��������� � ��������
	connectivity_components cc = connectivity_components();

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		int count_cc = cc.find_count_comp(alig.Edges[i]);
		
		//TODO ��� ��������� ���������� ��������� ���������

		if (count_cc == 0)
			result += -99;

		if (count_cc == 1)
			result += 2;

		if (count_cc > 1)
			result = result - count_cc * 2;
	}
	
	count_dropped_nodes c_d_n = count_dropped_nodes();

	for (pair<int, vector<int>> vec : alig.Nodes)
	{
		int dropped_nodes = c_d_n.find_count_dropped_nodes(net, vec.second);
		//TODO ���������� ������� ������

		result = result - dropped_nodes;
	}

	count_unavailable_nodes c_u_n = count_unavailable_nodes();

	for (int i = 0; i < alig.Edges.size(); i++)
	{
		int unavailable_nodes = c_u_n.find_count_unavailable_nodes(alig.Nodes[pattern.Edges[i].from],
			alig.Nodes[pattern.Edges[i].to],
			alig.Edges[i]);
		//TODO ���������� ������������ ������

		result = result - unavailable_nodes;
	}

	//���� ���������� ������ � �����
	{
		unordered_map<int, int> pattern_node_stretching, net_node_stretching, pattern_edge_stretching;

		find_stretching(alig, pattern_node_stretching, net_node_stretching, pattern_edge_stretching);

		for (pair<int, int> x : pattern_node_stretching)
		{
			if (x.second == 0)
				result -= 2;
			if (x.second > 1)
				result -= x.second;
		}

		for (pair<int, int> x : net_node_stretching)
		{
			if (x.second == 0)
				result -= 2;
			if (x.second > 1)
				result -= x.second;
		}

		for (pair<int, int> x : pattern_edge_stretching)
		{
			if (x.second == 0)
				result -= 2;
			if (x.second > 1)
				result -= x.second;
		}
	}

	return result;

	//check! ������ - ����� ����� ��������� ����, ������������ � ����� �������� �������� ���� ���� �� ����� � ������. ��� �����, ���� ���� ����� ����, ���������� ������ ����� ����������� �������. ����� ���� ��� - �����
	//check! ������ - ����� �������, � ������� ��������� ����� �������, ����������. ����� ��������� ��������� ������ ����� - �����
	//check! ������ - ����� � �������� ���� ���� �� ���� ������, � �������� ��������� ������ ����� �������� �� ��� �������, � �������� �������� ����� ����� ��������. ��� ���� ��� �� ��� - ��� ����
	//������ - ����� ������� ����������� � ����
	//������ - ����� � �������� ���� �������, �� ������� ����� ��������� �� ���� ������ � ��������
}

double score_alignment_for_path(alignment& const alig, Net& const pattern, Net& const net) {
	double result = 0;
	//������ ����� ������������ �������������������
	for (pair<int, vector<int>> i : alig.Nodes)
		for (int j : i.second)
			result += local_alignment_seq(pattern.Nodes[i.first].sequence, net.Nodes[j].sequence);

	{
		unordered_map<int, int> pattern_node_stretching, net_node_stretching, pattern_edge_stretching;

		find_stretching(alig, pattern_node_stretching, net_node_stretching, pattern_edge_stretching);

		for (pair<int, int> x : pattern_node_stretching)
		{
			if (x.second == 0)
				result -= 2;
			if (x.second > 1)
				result -= x.second;
		}

		for (pair<int, int> x : net_node_stretching)
		{
			if (x.second == 0)
				result -= 2;
			if (x.second > 1)
				result -= x.second;
		}

		for (pair<int, int> x : pattern_edge_stretching)
		{
			if (x.second == 0)
				result -= 2;
			if (x.second > 1)
				result -= x.second;
		}
	}

	return result;
}