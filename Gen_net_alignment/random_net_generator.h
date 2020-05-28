#pragma once
#include"structures_and_classes.h"
#include<ctime>

//���������� ��������� ������������ ������������������ ��������� ����� � ��������� ��������� ����������� 
string generate_random_seq(int length_in) {
	string result;

	int length = length_in + rand() % ((int)log2(length_in));

	for (int i = 0; i < length; ++i) {
		char c;
		int numb = rand() % 4;

		switch (numb)
		{
			case 0: {
				c = 'A';
				break;
			}
			case 1: {
				c = 'T';
				break;
			}
			case 2: {
				c = 'C';
				break;
			}
			case 3: {
				c = 'G';
				break;
			}

		default:
			break;
		}
		result += c;
	}

	return result;
}

//���������� ������������� ���������� ������������ ������������������ 
//�� ������ ������
string change_seq(string& const in_seq) {
	string result = in_seq;

	for (int i = 0; i < result.length(); ++i) {
		int random = rand() % 20;//�������� ����� ������ ������ ��������� �������

		if (random == 0) {
			char c;
			int numb = rand() % 4;

			switch (numb)
			{
			case 0: {
				c = 'A';
				break;
			}
			case 1: {
				c = 'T';
				break;
			}
			case 2: {
				c = 'C';
				break;
			}
			case 3: {
				c = 'G';
				break;
			}

			default:
				break;
			}

			result[i] = c;
		}
	}

	return result;
}

//��������� ������� ������� ������
vector<int> generate_adjacent_nodes(int current_node, int count_of_nodes) {
	vector<int> result = vector<int>();

	int length = 1 + rand() % ((int)log2(count_of_nodes));

	for (int i = 0; i < length; ++i) {
		int index = rand() % count_of_nodes; 
		if (find(result.begin(), result.end(), index) == result.end()) {
			result.push_back(index);
		}
	}
	return result;
}

//��������� ������� ������� ������, ���������� ������� ���� ������ ����
vector<int> generate_full_adjacent_nodes(int current_node, int count_of_nodes) {
	vector<int> result = vector<int>();

	for (int i = 0; i < count_of_nodes; ++i) {
		if (i != current_node)
			result.push_back(i);
	}

	return result;
}

//��������� ��������� ��������
Net generate_pattern(int count_of_nodes, int seq_len) {
	Net result = Net();

	if (count_of_nodes < 0) {
		count_of_nodes = rand() % 20;
	}

	Node n;

	for (int i = 0; i < count_of_nodes; ++i) {
		string seq = generate_random_seq(seq_len);
		vector<int> adjacent_nodes = vector<int>();

		if (i < count_of_nodes - 1) {
			adjacent_nodes.push_back(i + 1);
		}

		n = { i, adjacent_nodes,  seq };
		result.AddNode(n);
	}

	return result;
}

//��������� ������� �������� ���������
vector<Net> generate_patterns_vector(int count_of_patterns, int count_of_nodes, int seq_len) {
	vector<Net> result = vector<Net>();

	for (int i = 0; i < count_of_patterns; ++i) {
		result.push_back(generate_pattern(count_of_nodes, seq_len));
	}
	return result;
}

//��������� ������������ ����
Net generate_net(int count_of_nodes, int seq_len) {
	Net result = Net();

	if (count_of_nodes < 0) {
		count_of_nodes = rand() % 20;
	}

	Node n;
	
	for (int i = 0; i < count_of_nodes; ++i) {
		string seq = generate_random_seq(seq_len);
		vector<int> adjacent_nodes = generate_adjacent_nodes(i, count_of_nodes);
	
		n = { i, adjacent_nodes,  seq };
		result.AddNode(n);
	}

	return result;
}

//��������� ������ ����
Net generate_full_net(int count_of_nodes, int seq_len) {
	Net result = Net();
	Node n;

	for (int i = 0; i < count_of_nodes; ++i) {
		string seq = generate_random_seq(seq_len);
		vector<int> adjacent_nodes = generate_full_adjacent_nodes(i, count_of_nodes);

		n = { i, adjacent_nodes,  seq };
		result.AddNode(n);
	}

	return result;
}

//��������� ������������� ������
Net generate_tree(int count_of_nodes, int seq_len) {
	Net result = Net();

	int current_index = 0;//������ ����������� �������
	int child_index = 0;//������������ ������ ������������ �������

	while (current_index < count_of_nodes) {
		Node current_node;
		current_node.index = current_index;
		current_node.sequence = generate_random_seq(seq_len);

		int count_of_child = rand() % 3;
		if (current_index == child_index) {//������� ������� ��������� �� ������� ������, � ������ �������� �� ���������
			++count_of_child;
		}

		count_of_child = min(count_of_child, count_of_nodes - child_index - 1);//������ �������� ��������� �� �����, ��������

		vector<int> adjacent_nodes = vector<int>();

		for (int i = 0; i < count_of_child; ++i) {
			++child_index;
			adjacent_nodes.push_back(child_index);
		}

		current_node.adjacent_nodes = adjacent_nodes;
		result.AddNode(current_node);
		++current_index;
	}
	return result;
}

//��������� ������� ��������
vector<Net> generate_trees_vector(int count_of_trees, int count_of_nodes, int seq_len) {
	vector<Net> result = vector<Net>();

	for (int i = 0; i < count_of_trees; ++i) {
		result.push_back(generate_tree(count_of_nodes, seq_len));
	}
	return result;
}

//��������� ���� ��������� �������:
//� ����������� �������� ����������� ����� ����� � �������
//��� ���� ��� ������ ��������� ��������� � ���������� ���������
Net generate_T2_net(Net& const pattern, int count_of_nodes, int seq_len) {
	Net result = pattern;

	int count_nodes_in_pattern = result.Nodes.size();

	count_of_nodes = max(count_of_nodes, count_nodes_in_pattern);

	for (int i = 0; i < count_of_nodes; ++i) {
		vector<int> adjacent_nodes = generate_adjacent_nodes(i, count_of_nodes);
		if (i < count_nodes_in_pattern) {//������� ������ �� ��������, � �� ����� ��������� ������ �������
			result.add_edges(i, adjacent_nodes);
		}
		else {//��������� ����� �������
			Node n;
			n.index = i;
			n.adjacent_nodes = adjacent_nodes;
			n.sequence = generate_random_seq(seq_len);
			result.AddNode(n);
		}
	}

	return result;
}

//��������� ���� ��������� �������:
//������� ���������� ������� ��� ��������� ���������
//������������� ������ ������������ ������������������ � ������ �������
//��������� ����� ��������� ������� � �����
//� ��������� ������������ �����, ����������� � �������� ������� i � j, ����� ������������ � �� ������� ����, ��������� �� ����� ��������� ������
Net generate_T3_net(Net& const pattern, int count_of_nodes, int seq_len) {
	Net result = pattern;

	int count_nodes_in_pattern = result.Nodes.size();
	count_of_nodes = max(count_of_nodes, count_nodes_in_pattern);
	int count_adding_nodes = count_of_nodes - count_nodes_in_pattern; //���������� ������, ������� ����� ��� ��������
	int current_index_adding_node = count_nodes_in_pattern; //������� ������ �������, ������� ����� ��������

	for (int i = 0; i < count_nodes_in_pattern; ++i) {
		vector<int> adjacent_nodes = generate_adjacent_nodes(i, count_of_nodes);
		//������� ������ �� ��������, � �� ����� ��������� ������ ������� � �������� ������������ ������������������
		result.Nodes[i].sequence = change_seq(result.Nodes[i].sequence);

		if (!result.Nodes[i].adjacent_nodes.empty()) {
			vector<int> adjacent_nodes_vec = result.Nodes[i].adjacent_nodes;
			for (int k : adjacent_nodes_vec) {
				int random = rand() % count_nodes_in_pattern;

				if (random <= log2(count_nodes_in_pattern)) { // ��������� �������
					int count = 1 + rand() % (int)(log2(count_of_nodes));//���������� ����������� ������ �� ����� log2 �� ������ ���������� ������ � ����
					count = min(count, count_adding_nodes); //���� �� ������ �� ����� ��������� �������, �� �� ����� �� ������

					if (count > 0) {//������� ����� ����� ������� ��������� � ������� �����
						auto x = find(result.Nodes[i].adjacent_nodes.begin(), result.Nodes[i].adjacent_nodes.end(), k);
						result.Nodes[i].adjacent_nodes.erase(x);
						result.Nodes[i].adjacent_nodes.push_back(current_index_adding_node);

						result.Edges.push_back({ i, current_index_adding_node });
						result.New_edges[i][current_index_adding_node] = result.Edges.size() - 1;

						int index = result.New_edges[i][k];
						result.Edges[index] = { -1, -1 };
					}

					for (int j = 0; j < count; ++j) {
						Node n;
						n.index = current_index_adding_node;
						n.adjacent_nodes = generate_adjacent_nodes(current_index_adding_node, count_of_nodes);

						if (j < count - 1) {//����� ������� ������� ������� �� ���������
							n.adjacent_nodes.push_back(current_index_adding_node + 1);
						}
						else {//��������� ����������� ������� ����������� � �������� ���������� �� ��������
							n.adjacent_nodes.push_back(k);
						}

						n.sequence = generate_random_seq(seq_len);
						result.AddNode(n);

						current_index_adding_node++;
						count_adding_nodes--;
					}
				}
			}
		}

		result.add_edges(i, adjacent_nodes);
	}

	for (int i = 0; i < count_adding_nodes; i++) {
		Node n;
		n.index = current_index_adding_node;
		n.adjacent_nodes = generate_adjacent_nodes(current_index_adding_node, count_of_nodes);
		n.sequence = generate_random_seq(seq_len);
		result.AddNode(n);

		current_index_adding_node++;
	}

	return result;
}