#pragma once
//эффективный алгоритм выравнивания дерева с сетью при гамма равной нулю
//на входе: набор деревьев-паттернов, и сеть произвольного вида
//на выходе: набор оптимальных выравниваний каждого дерева с сетью

#include"score_alignment.h"
#include"random_net_generator.h"
#include<stack>

//функция для объединения текщуего выравнивания в корне с выравниванием из потомка
alignment join_trees(Net& const pattern, Net& const net, alignment& root_alig, alignment& child_alig, 
					int current_index, int child_index) {

	alignment result = root_alig;

	//добавляем в выравнивание вершины
	for (auto x : child_alig.Nodes) {
		result.Nodes[x.first] = x.second;
	}

	//добавляем в выравнивание ребра
	for (int i = 0; i < child_alig.Edges.size(); ++i) {
		if (!child_alig.Edges[i].empty()) {
			result.Edges[i] = child_alig.Edges[i];
		}
	}

	int current_u = *result.Nodes[current_index].begin();
	int child_u = *result.Nodes[child_index].begin();

	
	for (int k = 0; k < shortest_path_table[current_u][child_u].length - 1; ++k) {
		result.Edges[pattern.New_edges[current_index][child_index]].
			push_back(net.Edges[net.New_edges[shortest_path_table[current_u][child_u].way[k]]
			[shortest_path_table[current_u][child_u].way[k + 1]]]);
	}

	if (shortest_path_table[current_u][child_u].length == 0) {
		result.score = INT_MIN;
	}
	else {
		result.score = score_alignment_for_working_path(result, pattern, net, INT_MIN, 0, etta);
	}

	return result;
}

//основная функция выравнивания дерева
alignment tree_alignment(Net& const pattern, Net& const net) {
	stack<int> stack_of_nodes = stack<int>(); //стек номеров вершин паттерна для обхода в глубину
	//вершина извлекается только когда заполняется ее массив.
	//То есть каждая вершина посещается ровно два раза: один раз на спуске, и один раз на подъеме

	stack_of_nodes.push(pattern.root);
	//если visit[i] не инициализирован, то вершина i посещается впервые (на спуске)
	//если visit[i] = false, то вершина была посещена на спуске, но массив Si еще не заполнен
	//если visit[i] = true, то массив Si заполнен 
	unordered_map<int, bool> visit = unordered_map<int, bool>(); 

	unordered_map<int, unordered_map<int, alignment>> S = unordered_map<int, unordered_map<int, alignment>>();

	while (!stack_of_nodes.empty()) {
		int current_node_index = stack_of_nodes.top();
		//текущая вершина до этого не была посещена
		if (visit.count(current_node_index) == 0) {
			visit[current_node_index] = false;
			
			for (int i : pattern.Nodes[current_node_index].adjacent_nodes) {
				stack_of_nodes.push(i);
			}
			continue;
		}

		//текущая вершина до этого посещалась. Нужно заполнить ее массив

		//формируем новые выравнивания
		unordered_map<int, alignment> current_S = unordered_map<int, alignment>();
		for (int i : nodesNumbers) {
			alignment current_alignment = alignment(pattern.Nodes.size());
			current_alignment.Nodes[current_node_index].push_back(i);
			current_alignment.score = score_alignment_for_working_path(current_alignment, pattern, net, INT_MIN, 0, etta);
			current_S[i] = current_alignment;
		}

		//объединяем с выравниваниями из потомков
		for (int i : pattern.Nodes[current_node_index].adjacent_nodes) {//проходим всех потомков
			for (int j : nodesNumbers) {//проходим все узлы
				alignment best_alig = join_trees(pattern, net, current_S[j], S[i][j], current_node_index, i);
				for (int k : nodesNumbers) {//проходим все выравнивания потомка
					alignment current_alig = join_trees(pattern, net, current_S[j], S[i][k], current_node_index, i);

					if (current_alig.score > best_alig.score) {
						best_alig = current_alig;
					}
				}
				current_S[j] = best_alig;
			}

		}

		visit[current_node_index] = true;
		S[current_node_index] = current_S;

		stack_of_nodes.pop();
		
	}

	int root = pattern.root;

	alignment result = (*S[root].begin()).second;

	for (auto x : S[root]) {
		if (x.second.score > result.score) {
			result = x.second;
		}
	}

	return result;
}

vector<alignment> find_trees_alignment(vector<Net> patterns, Net net) {
	//fill_shortest_path_table(net);
	//prepare_table();

	vector<alignment> result = vector<alignment>();

	for (Net pattern : patterns) {
		result.push_back(tree_alignment(pattern, net));
	}

	return result;
}

//генерирует деревья и сеть, выравнивает и выводит данные на консоль
void start_test_trees_alignment(int pattern_count, int pattern_len, int net_len, int seq_len) {
	Net random_net = generate_net(net_len, seq_len);
	vector<Net> random_patterns = generate_trees_vector(pattern_count, pattern_len, seq_len);
	prepare_table();

	random_net.print();

	cout << endl << endl;

	for (int i = 0; i < pattern_count; i++) {
		cout << "pattern number " << i << endl;
		random_patterns[i].print();
	}

	double table_begin_clock = clock();
	fill_shortest_path_table(random_net);
	double table_end_clock = clock();

	for (int i = 0; i < net_len; ++i) {
		for (int j = 0; j < net_len; ++j) {
			cout << shortest_path_table[i][j].length << " ";
		}
		cout << endl;
	}
	double begin_time = clock();

	vector<alignment> alignments = find_trees_alignment(random_patterns, random_net);

	double end_time = clock();

	for (int i = 0; i < pattern_count; ++i) {
		cout << "Alignment " << i << " pattern" << endl;
		alignments[i].print();
	}


	cout << endl << "time table: " << table_end_clock - table_begin_clock << endl;
	cout << endl << "time alig: " << end_time - begin_time << endl;
}

//создает файл, в который записывает данные о времени работы
void find_average_time_tree(int min_pattern_count, int min_pattern_len, int min_net_len,
	int max_pattern_count, int max_pattern_len, int max_net_len,
	int delta_pattern, int delta_net,
	int count_of_tests, int seq_len) {
	ofstream file;
	file.open("times.txt");

	if (!file.is_open()) {
		cout << "eto fiasco" << endl;
	}

	for (int pattern_count = min_pattern_count; pattern_count < max_pattern_count + 1; ++pattern_count) {
		for (int pattern_len = min_pattern_len; pattern_len < max_pattern_len + 1; pattern_len += delta_pattern) {
			for (int net_len = min_net_len; net_len < max_net_len + 1; net_len += delta_net) {
				long max_time, min_time, average_time, sum_time;
				max_time = min_time = average_time = sum_time = 0;
				for (int test_index = 0; test_index < count_of_tests; ++test_index) {
					vector<Net> patterns = patterns = generate_trees_vector(pattern_count, pattern_len, seq_len);

					Net net = generate_net(net_len, seq_len);

					long begin_time = clock();

					fill_shortest_path_table(net);

					vector<alignment> result = find_trees_alignment(patterns, net);

					long end_time = clock();

					long current_time = end_time - begin_time;

					sum_time += current_time;

					max_time = max(max_time, current_time);

					if (min_time == 0) {
						min_time = current_time;
					}
					else {
						min_time = min(min_time, current_time);
					}
				}
				average_time = sum_time / count_of_tests;
				file << "pattern_count = " << pattern_count << "; ";
				file << "pattern_len = " << pattern_len << "; ";
				file << "net_len = " << net_len << "; ";
				file << "min_time = " << min_time << "; ";
				file << "max_time = " << max_time << "; ";
				file << "average_time = " << average_time << "; ";
				file << endl;
			}
			file << endl;
		}
	}
	file.close();
}