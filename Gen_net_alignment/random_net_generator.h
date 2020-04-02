#pragma once
#include"structures_and_classes.h"
#include<ctime>

string generate_random_seq() {
	string result;

	int length = 15 + rand() % 5;

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

// если индекс произвольный, то с циклами. Если индексы смежных только больше, то без
vector<int> generate_adjacent_nodes(int current_node, int count_of_nodes) {
	vector<int> result = vector<int>();

	int length = 1 + rand() % ((int)log2(count_of_nodes));

	for (int i = 0; i < length; ++i) {
		int index = rand() % count_of_nodes; //с циклами
		/*if (count_of_nodes == current_node + 1) {
			break;
		}
		int index = current_node + 1 + (rand() % (count_of_nodes - current_node - 1)); //без циклов*/
		if (find(result.begin(), result.end(), index) == result.end()) {
			result.push_back(index);
		}
	}
	return result;
}

Net generate_pattern(int count_of_nodes = 0) {
	Net result = Net();

	if (count_of_nodes == 0) {
		count_of_nodes = rand() % 20;
	}

	Node n;

	for (int i = 0; i < count_of_nodes; ++i) {
		string seq = generate_random_seq();
		vector<int> adjacent_nodes = vector<int>();

		if (i < count_of_nodes - 1) {
			adjacent_nodes.push_back(i + 1);
		}

		n = { i, adjacent_nodes,  seq };
		result.AddNode(n);
	}

	return result;
}

Net generate_net(int count_of_nodes = 0) {
	Net result = Net();

	if (count_of_nodes == 0) {
		count_of_nodes = rand() % 20;
	}

	Node n;
	
	for (int i = 0; i < count_of_nodes; ++i) {
		string seq = generate_random_seq();
		vector<int> adjacent_nodes = generate_adjacent_nodes(i, count_of_nodes);
	
		n = { i, adjacent_nodes,  seq };
		result.AddNode(n);
	}

	return result;
}
