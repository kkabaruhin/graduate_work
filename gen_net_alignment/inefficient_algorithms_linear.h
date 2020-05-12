#pragma once
#include"score_alignment.h"
#include"random_net_generator.h"
#include<fstream>

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

void fill_path_table(Net& const net) {
	path_table = unordered_map<int, unordered_map<int, vector<WayStruct>>>();
	shortest_path_table = unordered_map<int, unordered_map<int, WayStruct>>();
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
		//parallel_for_each(nodesNumbers.begin(), nodesNumbers.end(), [&](int j) {
		//	path_table[i][j] = find_cell_value(i, j, current_way, visit, net);
		//});
		for (int j : nodesNumbers) {
			path_table[i][j] = find_cell_value(i, j, current_way, visit, net);
		}
	}

	for (int i : nodesNumbers) {
		for (int j : nodesNumbers) {
			int shortest_len = INT_MAX;
			int index;

			for (int k = 0; k < path_table[i][j].size(); ++k) {
				if (path_table[i][j][k].length < shortest_len) {
					shortest_len = path_table[i][j][k].length;
					index = k;
				}
			}

			if (shortest_len == INT_MAX) {
				shortest_path_table[i][j] = WayStruct();
			}
			else {
				shortest_path_table[i][j] = path_table[i][j][index];
			}
		}
	}

}

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

//возвращает количество различных раст€жений текущего выравнивани€
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

//по имеющимс€ пути и длине паттерна подбирает оптимальное количество проходов по циклам
WayPriority find_optimize_topology(int line, int colunm, int index, WayStruct& const way, int pattern_length) {
	//учет разницы голых длин путей
	//количество необходимых и/или возможных проходов по циклам
	int path_length = way.length;
	int priority = max(way.length, pattern_length) - abs(pattern_length - path_length);

	// (pattern_length - path_length) / Cycles[ind].length -- нижн€€ оценка оптимального количества прохождений по циклу
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

alignment alignment_one_node(WayStruct& const path, WayStruct& const pattern,
	int current_node_path, int curren_node_pattern, alignment& const prev_alig,
	Net& const net, Net& const pattern_net,
	int number_of_allowable_stretch) {
	//если f == 1, то функци€ была вызвана способом 1, если 2, то спобом 2 и тд

	alignment result = alignment(pattern.length);

	if (count_of_stretching(prev_alig) > (current_node_path + curren_node_pattern) / 6.0 &&
		current_node_path + curren_node_pattern > log2(pattern.length)) {
		return result;
	}

	if (current_node_path >= path.way.size() || curren_node_pattern >= pattern.way.size()) {
		result = prev_alig;
		result.score = score_alignment_for_working_path(result, pattern_net, net, INT_MIN, gamma, etta);
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

	//I текущ€€ вершина паттерна ¬џ–ќ¬Ќ≈Ќј с текущей вершиной пути
	//1) следующие вершины путей выравниваютс€ друг с другом
	work_alig.Nodes[pattern.way[curren_node_pattern]].push_back(path.way[current_node_path]);
	alignment new_alignment = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern + 1, work_alig, net, pattern_net, number_of_allowable_stretch);
	if (result.score < new_alignment.score) {
		result = new_alignment;
	}

	work_alig.Nodes[pattern.way[curren_node_pattern]].erase(find(work_alig.Nodes[pattern.way[curren_node_pattern]].begin(),
		work_alig.Nodes[pattern.way[curren_node_pattern]].end(),
		path.way[current_node_path]));

	//2)следующа€ вершина паттерна = текуща€ вершина паттерна. —ледующа€ вершина сети без изменений
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
	//3)следующа€ вершина паттерна без изменений. Cледующа€ вершина сети = текуща€ вершина сети
	work_alig.Nodes[pattern.way[curren_node_pattern]].push_back(path.way[current_node_path]);
	new_alignments = alignment_one_node(path, pattern, current_node_path, curren_node_pattern + 1, work_alig, net, pattern_net, 3);
	if (!new_alignments.empty()) {
	result.insert(new_alignments.begin(), new_alignments.end());
	}
	work_alig.Nodes[pattern.way[curren_node_pattern]].erase(find(work_alig.Nodes[pattern.way[curren_node_pattern]].begin(),
	work_alig.Nodes[pattern.way[curren_node_pattern]].end(),
	path.way[current_node_path]));
	*/
	//II текущ€€ вершина паттерна Ќ≈ ¬џ–ќ¬Ќ≈Ќј с текущей вершиной пути

	//4) следующие вершины путей выравниваютс€ друг с другом
	//new_alignments = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern + 1, work_alig, net, pattern_net, 4);
	//if (!new_alignments.empty()) {
	//	result.insert(new_alignments.begin(), new_alignments.end());
	//}
	//5)следующа€ вершина паттерна = текуща€ вершина паттерна. —ледующа€ вершина сети без изменений
	/*if (f != 6) {
	new_alignments = alignment_one_node(path, pattern, current_node_path + 1, curren_node_pattern, work_alig, net, pattern_net, 5);
	if (!new_alignments.empty()) {
	result.insert(new_alignments.begin(), new_alignments.end());
	}
	}*/
	//6)следующа€ вершина паттерна без изменений. Cледующа€ вершина сети = текуща€ вершина сети

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
	//выровн€ть текущую вершину паттерна оптимально и рекурсивно запустить дл€ следующей
	//после этого выровн€ть текущую вершину менее оптимальными способами и дл€ всех запустить дл€ следующей вершины
	WayStruct path = path_link.way;

	WayStruct pattern_way = from_net_to_way(pattern);
	alignment start_alig = alignment(pattern.Edges.size());

	return alignment_one_node(path, pattern_way, 0, 0, start_alig, net, pattern, path.length - pattern_way.length);
	//return dynamic_realization(pattern_way, net, pattern);
}

alignment path_alignment(Net& const pattern, Net& const net) {
	alignment result = alignment(pattern.Nodes.size());
	set<WayPriority> ways = set<WayPriority>();

	//пути без циклов
	for (int i : nodesNumbers) {
		for (int j : nodesNumbers) {
			for (int k = 0; k < path_table[i][j].size(); k++) {
				if (path_table[i][j][k].length == pattern.Nodes.size()) {
					ways.insert(find_optimize_topology(i, j, k, path_table[i][j][k], pattern.Nodes.size() - 1));
					if (ways.size() > net.Nodes.size()*net.Nodes.size()*log2(net.Nodes.size())) {
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
	//prepare_table();

	vector<alignment> result = vector<alignment>();

	//parallel_for(size_t(0), patterns.size(), [&](int i) {
	//	result.push_back(path_alignment(patterns[i], net));
	//}
	//);
	for (int i = 0; i < patterns.size(); ++i) {
		result.push_back(path_alignment(patterns[i], net));
	}

	return result;
}

void start_test_alignment(int pattern_count, int pattern_len, int net_len, int seq_len) {
	Net random_net = generate_net(net_len, seq_len);
	vector<Net> random_patterns = generate_patterns_vector(pattern_count, pattern_len, seq_len);
	prepare_table();

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
	cout << endl << endl;
	for (int i = 0; i < net_len; ++i) {
		for (int j = 0; j < net_len; ++j) {
			cout << shortest_path_table[i][j].length << " ";
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

void find_average_time(int min_pattern_count, int min_pattern_len, int min_net_len, int max_pattern_count, int max_pattern_len, int max_net_len,
	int count_of_tests, int seq_len) {
	ofstream file;
	file.open("times.txt");


	for (int pattern_count = min_pattern_count; pattern_count < max_pattern_count + 1; ++pattern_count) {
		for (int pattern_len = min_pattern_len; pattern_len < max_pattern_len + 1; ++pattern_len) {
			for (int net_len = min_net_len; net_len < max_net_len + 1; ++net_len) {
				long max_time, min_time, average_time, sum_time;
				max_time = min_time = average_time = sum_time = 0;
				for (int test_index = 0; test_index < count_of_tests; ++test_index) {
					vector<Net> patterns = vector<Net>();

					for (int i = 0; i < pattern_count; ++i) {
						patterns.push_back(generate_pattern(pattern_len, seq_len));
					}

					Net net = generate_net(net_len, seq_len);

					long begin_time = clock();

					vector<alignment> result = start_algorithm(patterns, net);

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
				file << "max_time = " << min_time << "; ";
				file << "average_time = " << average_time << "; ";
				file << endl;
			}
		}
	}
	file.close();
}
