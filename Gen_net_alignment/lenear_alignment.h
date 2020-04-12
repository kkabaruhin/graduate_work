#pragma once
#include"score_alignment.h"
#include<math.h>
#include<ctime>
#include"random_net_generator.h"
#include<algorithm>

#include<ppl.h>
using namespace concurrency;

//таблица. В ячейке [i,j] находятся все пути из i в j
unordered_map<int, unordered_map<int, vector<WayStruct>>> path_table;
vector<WayStruct> Cycles;
set<int> nodesNumbers;

//множитель штрафов
double fine_coefficient = 0.9;

unordered_map<int, vector<int>> find_entry_points(int line, int column, int way_index) {
	unordered_map<int, vector<int>> result = unordered_map<int, vector<int>>();

	WayStruct current_way = path_table[line][column][way_index];
	int previous_index = current_way.length - 1;

	for (int i = 0; i < current_way.length; ++i) {
		for (int j = 0; j < Cycles.size(); ++j) {
			if (find(Cycles[j].way.begin(), Cycles[j].way.end(), current_way.way[i]) != Cycles[j].way.end()) {
				result[i].push_back(j);
				if (find(result[previous_index].begin(), result[previous_index].end(), j) != result[previous_index].end()) {
					result[previous_index].erase(find(result[previous_index].begin(), result[previous_index].end(), j));
				}
			}
		}
		previous_index = i;
	}

	return result;
}

void add_entry_points() {
	for (int i : nodesNumbers) {
		for (int j : nodesNumbers) {
			if (!path_table[i][j].empty()) {
				for (int k = 0; k < path_table[i][j].size(); k++) {
					path_table[i][j][k].entry_points = find_entry_points(i, j, k);
				}
			}
		}
	}
}

void remove_extra_paths() {
	for (auto lines : path_table) {
		for (int k : nodesNumbers) {
			if (path_table[lines.first][k].empty()) {
				continue;
			}
			for (int i = 0; i < path_table[lines.first][k].size(); ++i) {
				if (path_table[lines.first][k][i].excess()) {
					path_table[lines.first][k].erase(path_table[lines.first][k].begin() + i);
					--i;
				}
			}
		}
	}
}

/*void fill_path_table(Net net) {
	path_table = unordered_map<int, unordered_map<int, vector<WayStruct>>>();
	Cycles = vector<WayStruct>();
	bool find_new_cyrcle = false;

	nodesNumbers = set<int>();
	for (auto kv : net.Nodes) {
		nodesNumbers.insert(kv.first);
	}

	for (int index : nodesNumbers) { //проходим по каждой строке
		path_table[index] = unordered_map<int, vector<WayStruct>>();
		vector<int> new_ways = vector<int>(); //список индексов столбцов в которых появились новые пути в текущей строке

		find_new_cyrcle = false;

		for (int i : net.Nodes[index].adjacent_nodes) { //обрабатываем ячейки, в которые записываем новые пути из двух вершин
			pair<int, int> indices = pair<int, int>(index, i);

			path_table[indices.first][indices.second] = vector<WayStruct>();
			path_table[indices.first][indices.second].push_back(WayStruct(indices.first, indices.second));
			new_ways.push_back(indices.second);
		}

		//если есть путь из A в B, а из B путь в C, то из A есть путь в C
		for (int i : new_ways) { //проходим по тем строкам таблицы, в которые ведут новые пути из kv.first
			if (i >= index) {
				continue;
			}

			for (pair<int, vector<WayStruct>> cell : path_table[i]) {
				for (WayStruct way : cell.second) {
					WayStruct newWay = joinPath(*path_table[index][i].begin(), way);
					int j = cell.first;
					if (path_table[index][j].empty()) {
						path_table[index][j] = vector<WayStruct>();
					}
					path_table[index][j].push_back(newWay);
					if (index == j) //найден цикл 
					{
						find_new_cyrcle = true;
						Cycles.push_back(newWay);
					}
				}
				
			}
		}
		//к этому моменту текущая строка заполнена


		//далее проходим по всем строкам
		//в тех строках, в которых ячейка с указанным номером столбца не пустая, записываются новые пути из только что заполненной строки

		int column = index;
		for (int index2 : nodesNumbers) {
			int line = index2;

			if (line >= column) {
				break;
			}

			if (path_table[line][column].empty()) {
				continue;
			}

			for (auto kv2 : path_table[index]) {
				if (kv2.second.empty()) {
					continue;
				}
				if (index == line) {
					continue;
				}
				if (column == kv2.first) {
					continue;
				}
				for (WayStruct firstWay : path_table[line][column]) {
					if (path_table[line][kv2.first].empty()) {
						path_table[line][kv2.first] = vector<WayStruct>();
					}
					for (WayStruct secondWay : path_table[index][kv2.first]) {
						WayStruct newWay = joinPath(firstWay, secondWay);
						path_table[line][kv2.first].push_back(newWay);
					}
					
				}
			}
			
		}
		if (find_new_cyrcle) {
			remove_extra_paths();
		}
	}

	add_entry_points();

}*/

vector<WayStruct> find_cell_value(int start, int finish, vector<int> current_way, unordered_map<int, bool> visit, Net& const net) {
	vector<WayStruct> result = vector<WayStruct>();
	int current_node = current_way[current_way.size() - 1];

	if (current_node == finish && current_way.size() > 1) {//найден новый путь
		result.push_back(WayStruct(current_way));
		return result;
	}

	for (int i : net.Nodes[current_node].adjacent_nodes) {
		if (!visit[i] || (i == finish && start == finish)) {
			visit[i] = true;
			current_way.push_back(i);
			
			vector<WayStruct> new_ways = find_cell_value(start, finish, current_way, visit, net);

			visit[i] = false;
			current_way.erase(--current_way.end());
			
			for (WayStruct w : new_ways) {
				result.push_back(w);
			}
		}
	}
	return result;
}

void fill_path_table(Net& const net) {
	path_table = unordered_map<int, unordered_map<int, vector<WayStruct>>>();
	Cycles = vector<WayStruct>();

	nodesNumbers = set<int>();
	for (auto kv : net.Nodes) {
		nodesNumbers.insert(kv.first);
	}

	vector<WayStruct> cell = vector<WayStruct>();
	unordered_map<int, bool> visit = unordered_map<int, bool>();
	vector<int> current_way = vector<int>();

	//поиск всех путей из вершины i в вершину j
	for (int i : nodesNumbers) {
		visit.clear();
		current_way.clear();
		visit[i] = true;
		current_way.push_back(i);
		parallel_for_each(nodesNumbers.begin(), nodesNumbers.end(), [&](int j) {
			path_table[i][j] = find_cell_value(i, j, current_way, visit, net);
		});
	}
	
}

//возвращает количество различных растяжений текущего выравнивания
int count_of_stretching(alignment& const alig) {
	unordered_map<int, int> pattern_node_stretching, net_node_stretching, pattern_edge_stretching;

	find_stretching(alig, pattern_node_stretching, net_node_stretching, pattern_edge_stretching);
	int result = 0;

	for (pair<int, int> x : pattern_node_stretching)
	{
		if (x.second > 1)
			result += x.second;
	}

	for (pair<int, int> x : net_node_stretching)
	{
		if (x.second > 1)
			result += x.second;
	}

	for (pair<int, int> x : pattern_edge_stretching)
	{
		if (x.second > 1)
			result += x.second;
	}
	return result;
}

//по имеющимся пути и длине паттерна подбирает оптимальное количество проходов по циклам
WayPriority find_optimize_topology(int line, int colunm, int index, WayStruct& const way, int pattern_length) {
	//учет разницы голых длин путей
	//количество необходимых и/или возможных проходов по циклам
	int path_length = way.length;
	int priority = max(way.length, pattern_length) - abs(pattern_length - path_length);

	// (pattern_length - path_length) / Cycles[ind].length -- нижняя оценка оптимального количества прохождений по циклу
	// (pattern_length - (path_length + Cycles[ind].length * cout_of_laps)) -- разница в длинах путей после прохождений по циклу
	/*if (pattern_length > path_length) {
		for (auto kv : way.entry_points) {
			for (int ind : kv.second) {
				int cout_of_laps = (pattern_length - path_length) / Cycles[ind].length;
				int length_difference = (pattern_length - (path_length + Cycles[ind].length * cout_of_laps));
				priority = max(priority, pattern_length - (cout_of_laps + length_difference));

				cout_of_laps++;
				length_difference = abs(pattern_length - (path_length + Cycles[ind].length * cout_of_laps));
				priority = max(priority, pattern_length - (cout_of_laps + length_difference));
			}
		}
	}*/
	WayPriority result = WayPriority(line, colunm, index, priority, way);
	return result;
}

WayStruct from_net_to_way( Net& const net) {
	WayStruct result = WayStruct();
	Node current_node = net.Nodes[net.root];

	while (true) {
		result.addNode(current_node.index);

		if (current_node.adjacent_nodes.empty()) {
			break;
		}
		current_node = net.Nodes[*current_node.adjacent_nodes.begin()];
	}
	result.length = result.way.size() - 1;
	return result;
}

alignment alignment_one_node(WayStruct& const path, WayStruct& const pattern,
	int current_node_path, int curren_node_pattern, alignment& const prev_alig,
	Net& const net, Net& const pattern_net,
	int number_of_allowable_stretch) {
	//если f == 1, то функция была вызвана способом 1, если 2, то спобом 2 и тд

	alignment result = alignment(pattern.length);

	if (count_of_stretching(prev_alig) > (current_node_path + curren_node_pattern) / 6.0 &&
		current_node_path + curren_node_pattern > log2(pattern.length)) {
		return result;
	}

	if (current_node_path >= path.way.size() || curren_node_pattern >= pattern.way.size()) {
		result = prev_alig;
		result.score = score_alignment_for_path(result, pattern_net, net);
		return result;
	}

	alignment work_alig = prev_alig;


	if (current_node_path > 0 && curren_node_pattern > 0) {
		edge pattern_edge = edge();
		pattern_edge.from = pattern.way[curren_node_pattern - 1];
		pattern_edge.to = pattern.way[curren_node_pattern];

		edge net_edge = edge();
		net_edge.from = path.way[current_node_path - 1];
		net_edge.to = path.way[current_node_path];

		int index_pattern = pattern_net.New_edges[pattern_edge.from][pattern_edge.to];
		edge edge_net = net.Edges[net.New_edges[net_edge.from][net_edge.to]];

		work_alig.Edges[index_pattern].push_back(edge_net);
	}

	//I текущяя вершина паттерна ВЫРОВНЕНА с текущей вершиной пути
	//1) следующие вершины путей выравниваются друг с другом
	work_alig.Nodes[pattern.way[curren_node_pattern]].push_back(path.way[current_node_path]);
	alignment new_alignment = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern + 1, work_alig, net, pattern_net, number_of_allowable_stretch);
	if (result.score < new_alignment.score) {
		result = new_alignment;
	}

	work_alig.Nodes[pattern.way[curren_node_pattern]].erase(find(work_alig.Nodes[pattern.way[curren_node_pattern]].begin(),
		work_alig.Nodes[pattern.way[curren_node_pattern]].end(),
		path.way[current_node_path]));

	//2)следующая вершина паттерна = текущая вершина паттерна. Следующая вершина сети без изменений
	if (number_of_allowable_stretch > 0) {
		work_alig.Nodes[pattern.way[curren_node_pattern]].push_back(path.way[current_node_path]);
		new_alignment = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern, work_alig, net, pattern_net, number_of_allowable_stretch - 1);
		if (result.score < new_alignment.score) {
			result = new_alignment;
		}
		work_alig.Nodes[pattern.way[curren_node_pattern]].erase(find(work_alig.Nodes[pattern.way[curren_node_pattern]].begin(),
			work_alig.Nodes[pattern.way[curren_node_pattern]].end(),
			path.way[current_node_path]));
	}
	/*
	//3)следующая вершина паттерна без изменений. Cледующая вершина сети = текущая вершина сети
	work_alig.Nodes[pattern.way[curren_node_pattern]].push_back(path.way[current_node_path]);
	new_alignments = alignment_one_node(path, pattern, current_node_path, curren_node_pattern + 1, work_alig, net, pattern_net, 3);
	if (!new_alignments.empty()) {
		result.insert(new_alignments.begin(), new_alignments.end());
	}
	work_alig.Nodes[pattern.way[curren_node_pattern]].erase(find(work_alig.Nodes[pattern.way[curren_node_pattern]].begin(),
																 work_alig.Nodes[pattern.way[curren_node_pattern]].end(),
																 path.way[current_node_path]));
*/
	//II текущяя вершина паттерна НЕ ВЫРОВНЕНА с текущей вершиной пути

	//4) следующие вершины путей выравниваются друг с другом
    //new_alignments = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern + 1, work_alig, net, pattern_net, 4);
	//if (!new_alignments.empty()) {
	//	result.insert(new_alignments.begin(), new_alignments.end());
	//}
	//5)следующая вершина паттерна = текущая вершина паттерна. Следующая вершина сети без изменений
	/*if (f != 6) {
		new_alignments = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern, work_alig, net, pattern_net, 5);
		if (!new_alignments.empty()) {
			result.insert(new_alignments.begin(), new_alignments.end());
		}
	}*/
	//6)следующая вершина паттерна без изменений. Cледующая вершина сети = текущая вершина сети

		/*new_alignments = alignment_one_node(path, pattern, current_node_path, curren_node_pattern + 1, work_alig, net, pattern_net, 6);
		if (!new_alignments.empty()) {
			result.insert(new_alignments.begin(), new_alignments.end());
		}*/

	return result;
}

/*void fill_one_line_in_table(int line_index, int current_pattern_node_index,
	unordered_map<int, unordered_map<int, double>>& table, 
	WayStruct& const path, WayStruct& const pattern_way,
	Net& const net, Net& const pattern,
	int number_of_allowable_stretch) {

	for (int i = line_index; i < line_index + number_of_allowable_stretch; ++i) {
		table[line_index][i] = table[line_index][i - 1] * fine_coefficient +
			local_alignment_seq(net.Nodes[path.way[i]].sequence, pattern.Nodes[pattern_way.way[current_pattern_node_index]].sequence) * pow(fine_coefficient, i - line_index);
	}
}

int find_index_max_in_line(int line, int number_of_allowable_stretch, unordered_map<int, unordered_map<int, double>>& const table) {
	int result = line;

	for (int i = line; i < line + number_of_allowable_stretch; ++i) {
		if (table[line][i] > table[line][result])
			result = i;
	}
}

alignment new_alignment_path(WayStruct& const path, WayStruct& const pattern_way,
	Net& const net, Net& const pattern) {
	alignment result = alignment(pattern_way.length);
	int number_of_allowable_stretch = path.length - pattern_way.length;

	unordered_map<int, unordered_map<int, double>> prev_table = unordered_map<int, unordered_map<int, double>>();
	unordered_map<int, unordered_map<int, double>> current_table = unordered_map<int, unordered_map<int, double>>();

	for (int current_pattern_index = 0; current_pattern_index < pattern_way.length; current_pattern_index++) {

		fill_one_line_in_table(current_pattern_index, current_pattern_index, current_table, path, pattern_way, net, pattern, number_of_allowable_stretch);
		int index_max_in_line = find_index_max_in_line(current_pattern_index, number_of_allowable_stretch, current_table);

	}
}*/

//по заданному пути и паттерну возвращает оптимальное выравнивание с этим путем
alignment find_best_alignment(WayPriority& const path_link, Net& const net, Net& const pattern) {
	//выровнять текущую вершину паттерна оптимально и рекурсивно запустить для следующей
	//после этого выровнять текущую вершину менее оптимальными способами и для всех запустить для следующей вершины
	WayStruct path = path_link.way;

	WayStruct pattern_way = from_net_to_way(pattern);
	alignment start_alig = alignment(pattern.Edges.size());

	return alignment_one_node(path, pattern_way, 0, 0, start_alig, net, pattern, path.length - pattern_way.length);
}

alignment path_alignment(Net& const pattern, Net& const net) {
	alignment result = alignment(pattern.Nodes.size());
	set<WayPriority> ways = set<WayPriority>();

	//пути без циклов
	for (int i : nodesNumbers) {
		for (int j : nodesNumbers) {
			for (int k = 0; k < path_table[i][j].size(); k++) {
				if (path_table[i][j][k].length >= pattern.Nodes.size()) {
					ways.insert(find_optimize_topology(i, j, k, path_table[i][j][k], pattern.Nodes.size() - 1));
					if (ways.size() > net.Nodes.size()*log2(net.Nodes.size())) {
						ways.erase(ways.begin());
					}
				}
			}
		}
	}

	for (WayPriority w : ways) {
		alignment new_alig = find_best_alignment(w, net, pattern);
		if (result.score < new_alig.score) {
			result = new_alig;
		}

	}

	return result;
}

vector<alignment> start_algorithm(vector<Net>& const patterns, Net& const net) {
	//fill_path_table(net);
	prepare_table();

	vector<alignment> result = vector<alignment>();

	parallel_for(size_t(0), patterns.size(), [&](int i) {
		result.push_back(path_alignment(patterns[i], net));
	}
	);
	//for (int i = 0; i < patterns.size(); ++i) {
	//	result.push_back(path_alignment(patterns[i], net));
	//}

	return result;
}

void start_test_alignment(int pattern_count, int pattern_len, int net_len) {
	Net random_net = generate_net(net_len);
	vector<Net> random_patterns = generate_patterns_vector(pattern_count, pattern_len);

	random_net.print();

	cout << endl << endl;

	for (int i = 0; i < pattern_count; i++) {
		cout << "pattern number " << i << endl;
		random_patterns[i].print();
	}
	
	double table_begin_clock = clock();
	fill_path_table(random_net);
	double table_end_clock = clock();

	for (int i = 0; i < net_len; ++i) {
		for (int j = 0; j < net_len; ++j) {
			cout << path_table[i][j].size() << " ";
		}
		cout << endl;
	}
	double begin_time = clock();

	vector<alignment> alignments = start_algorithm(random_patterns, random_net);

	double end_time = clock();

	for (int i = 0; i < pattern_count; ++i) {
		cout << "Alignment " << i << " pattern" << endl;
		alignments[i].print();
	}


	cout << endl << "time table: " << table_end_clock - table_begin_clock << endl;
	cout << endl << "time alig: " << end_time - begin_time << endl;
}