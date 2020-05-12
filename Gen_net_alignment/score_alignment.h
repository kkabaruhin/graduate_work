#pragma once
//���� ��� ������� ����������
#include"seq_alignment.h"
#include"connectivity_components.h"
#include"dropped_nodes.h"
#include"count_unavailable_nodes.h"
#include"stretching.h"

//������� ��� ������������, ��������������� ������ ��������� (�������������, ��� ����������� ���������� �� ������������)
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

	//���� ���������� ������ � �����
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

	//check! ������ - ����� ����� ��������� ����, ������������ � ����� �������� �������� ���� ���� �� ����� � ������. ��� �����, ���� ���� ����� ����, ���������� ������ ����� ����������� �������. ����� ���� ��� - �����
	//check! ������ - ����� �������, � ������� ��������� ����� �������, ����������. ����� ��������� ��������� ������ ����� - �����
	//check! ������ - ����� � �������� ���� ���� �� ���� ������, � �������� ��������� ������ ����� �������� �� ��� �������, � �������� �������� ����� ����� ��������. ��� ���� ��� �� ��� - ��� ����
	//������ - ����� ������� ����������� � ����
	//������ - ����� � �������� ���� �������, �� ������� ����� ��������� �� ���� ������ � ��������
}

//�������� ������� �������� ������ ������������
double score_alignment_for_working_path(alignment& const alig, Net& const pattern, Net& const net,
										double sigma, double gamma, double etta) {
	double result = 0;
	//������ ����� ������������ �������������������
	for (pair<int, vector<int>> i : alig.Nodes)
		for (int j : i.second)
			result += local_alignment_seq(pattern.Nodes[i.first].sequence, net.Nodes[j].sequence);

	{
		unordered_map<int, int> pattern_node_stretching, net_node_stretching, pattern_edge_stretching;

		find_stretching(alig, pattern_node_stretching, net_node_stretching, pattern_edge_stretching);

		//���������� ����� ������� �������� �� ��������� ����� ����
		for (pair<int, int> x : pattern_node_stretching)
		{
			if (x.second > 1)
				result += sigma * (x.second - 1);
		}

		//���������� ������ ���� ���� �� ��������� ������ ��������
		for (pair<int, int> x : net_node_stretching)
		{
			if (x.second > 1)
				result += gamma * (x.second - 1);
		}

		//���������� ����� �������� �� ���� � ����
		for (pair<int, int> x : pattern_edge_stretching)
		{
			if (x.second > 1)
				result += etta * (x.second - 1);
			
		}
	}

	return result;
}