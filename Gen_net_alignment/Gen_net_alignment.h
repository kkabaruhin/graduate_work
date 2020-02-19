#pragma once
#include"seq_alignment.h"
#include"connectivity_components.h"
#include"structures_and_classes.h"
#include"dropped_nodes.h"
#include<set>
#include"count_unavailable_nodes.h"

//�������� ������� ����������: ��� ���������� ������, ��� ����� ���������� ���� ������ ������� � ������ ����
//��� ������ ����������, ��� ������� �������� �� ���
//��� ������ ���� � �������, ��� �������� ����������
double score_alignment(alignment& const alig, Net& const pattern, Net& const net)
{
	double result = 0;

	fill_subst_matr();
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

		if (count_cc == 1)
			result += 2;
		else
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

	return result;

	//check! ������ - ����� ����� ��������� ����, ������������ � ����� �������� �������� ���� ���� �� ����� � ������. ��� �����, ���� ���� ����� ����, ���������� ������ ����� ����������� �������. ����� ���� ��� - �����
	//check! ������ - ����� �������, � ������� ��������� ����� �������, ����������. ����� ��������� ��������� ������ ����� - �����
	//check! ������ - ����� � �������� ���� ���� �� ���� ������, � �������� ��������� ������ ����� �������� �� ��� �������, � �������� �������� ����� ����� ��������. ��� ���� ��� �� ��� - ��� ����
	//������ - ����� ������� ����������� � ����
	//������ - ����� � �������� ���� �������, �� ������� ����� ��������� �� ���� ������ � ��������
	//��� � ������� �������?
}