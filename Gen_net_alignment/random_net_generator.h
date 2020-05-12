#pragma once
#include"structures_and_classes.h"
#include<ctime>

//генерирует случайную генетическую последовательность указанной длины с указанным случайным отклонением 
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

//возвращает незначительно измененную генетическую последовательность 
//не меняет длинну
string change_seq(string& const in_seq) {
	string result = in_seq;

	for (int i = 0; i < result.length(); ++i) {
		int random = rand() % 20;//изменена будет только каждая двадцатая позиция

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

//генерация вектора смежных вершин
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

//генерация вектора смежных вершин, содержащий индексы всех вершин сети
vector<int> generate_full_adjacent_nodes(int current_node, int count_of_nodes) {
	vector<int> result = vector<int>();

	for (int i = 0; i < count_of_nodes; ++i) {
		if (i != current_node)
			result.push_back(i);
	}

	return result;
}

//генерация линейного паттерна
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

//генерация вектора линейных паттернов
vector<Net> generate_patterns_vector(int count_of_patterns, int count_of_nodes, int seq_len) {
	vector<Net> result = vector<Net>();

	for (int i = 0; i < count_of_patterns; ++i) {
		result.push_back(generate_pattern(count_of_nodes, seq_len));
	}
	return result;
}

//генерация произвольной сети
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

//генерация полной сети
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

//генерация произвольного дерева
Net generate_tree(int count_of_nodes, int seq_len) {
	Net result = Net();

	int current_index = 0;//индекс добавляемой вершины
	int child_index = 0;//максимальный индекс добавленного потомка

	while (current_index < count_of_nodes) {
		Node current_node;
		current_node.index = current_index;
		current_node.sequence = generate_random_seq(seq_len);

		int count_of_child = rand() % 3;
		if (current_index == child_index) {//текущая вершина последняя на текущем уровне, и больше потомков не добалвено
			++count_of_child;
		}

		count_of_child = min(count_of_child, count_of_nodes - child_index - 1);//больше потомков добавлять не нужно, возможно

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

//генерация вектора деревьев
vector<Net> generate_trees_vector(int count_of_trees, int count_of_nodes, int seq_len) {
	vector<Net> result = vector<Net>();

	for (int i = 0; i < count_of_trees; ++i) {
		result.push_back(generate_tree(count_of_nodes, seq_len));
	}
	return result;
}

//формирует сети следующим образом:
//к полученному паттерну добавляются новые ребра и вершины
//при этом сам костяк полностью совпадает с присланным паттерном
Net generate_T2_net(Net& const pattern, int count_of_nodes, int seq_len) {
	Net result = pattern;

	int count_nodes_in_pattern = result.Nodes.size();

	for (int i = 0; i < count_of_nodes; ++i) {
		vector<int> adjacent_nodes = generate_adjacent_nodes(i, count_of_nodes);
		if (i < count_nodes_in_pattern) {//вершина пришла из паттерна, и ее нужно дополнить новыми ребрами
			result.add_edges(i, adjacent_nodes);
		}
		else {//добавляем новую вершину
			Node n;
			n.index = i;
			n.adjacent_nodes = adjacent_nodes;
			n.sequence = generate_random_seq(seq_len);
			result.AddNode(n);
		}
	}

	return result;
}

//формирует сети следующим образом:
//берется присланный паттерн без изменения топологии
//незначительно меняем генетические последовательности в каждой вершине
//добавляем новые случайные вершины и ребра
//с некоторой вероятностью ребро, соединяющее в паттерне вершины i и j, может превратиться в не длинный путь, состоящий из новых случайных вершин
Net generate_T3_net(Net& const pattern, int count_of_nodes, int seq_len) {
	Net result = pattern;

	int count_nodes_in_pattern = result.Nodes.size();
	int count_adding_nodes = count_of_nodes - count_nodes_in_pattern; //количество вершин, которых нужно еще добавить
	int current_index_adding_node = count_nodes_in_pattern; //текущий индекс вершины, которую нужно добавить

	for (int i = 0; i < count_nodes_in_pattern; ++i) {
		vector<int> adjacent_nodes = generate_adjacent_nodes(i, count_of_nodes);
		//вершина пришла из паттерна, и ее нужно дополнить новыми ребрами и изменить генетическую последовательность
		result.Nodes[i].sequence = change_seq(result.Nodes[i].sequence);

		if (i < count_nodes_in_pattern - 1) {
			int random = rand() % 10; //с вероятностью 0.1 между двумя вершинами паттерна появятся новые вершнины

			if (random == 0) { // вставляем вершины
				int count = 1 + rand() % (int)(log2(count_of_nodes));//количество вставляемых вершин не более log2 от общего количества вершин в сети
				count = min(count, count_adding_nodes); //если мы больше можем вставлять вершины, то мы этого не делаем

				if (count > 0) {//удаляем связь между старыми вершинами и создаем новую
					auto x = find(result.Nodes[i].adjacent_nodes.begin(), result.Nodes[i].adjacent_nodes.end(), i + 1);
					result.Nodes[i].adjacent_nodes.erase(x);
					result.Nodes[i].adjacent_nodes.push_back(current_index_adding_node);

					result.Edges.push_back({ i, current_index_adding_node });
					result.New_edges[i][current_index_adding_node] = result.Edges.size() - 1;

					int index = result.New_edges[i][i + 1];
					result.Edges[index] = { -1, -1 };
				}

				for (int j = 0; j < count; ++j) {
					Node n;
					n.index = current_index_adding_node;
					n.adjacent_nodes = generate_adjacent_nodes(current_index_adding_node, count_of_nodes);
					
					if (j < count - 1) {//нужно связать текущую вершину со следующей
						n.adjacent_nodes.push_back(current_index_adding_node + 1);
					}
					else {//последняя добавляемая вершина соединяется с вершиной назначения из паттерна
						n.adjacent_nodes.push_back(i + 1);
					}

					n.sequence = generate_random_seq(seq_len);
					result.AddNode(n);

					current_index_adding_node++;
					count_adding_nodes--;
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