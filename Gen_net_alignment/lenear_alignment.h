#pragma once
//эффективный алгоритм фыравнивани€ линейного графа при условии гамма равной нулю

#include"score_alignment.h"
#include<math.h>
#include"random_net_generator.h"
#include<algorithm>
#include<fstream>
#include<ppl.h>
using namespace concurrency;


//основной алгоритм
alignment dynamic_realization(WayStruct& const pattern, Net& const net, Net& const pattern_net) {
	unordered_map<int, unordered_map<int, alignment>> current_table = unordered_map<int, unordered_map<int, alignment>>();
	unordered_map<int, unordered_map<int, alignment>> prev_table;

	//инициализаци€ Q(2)
	for (int i : nodesNumbers) {
		for (int j : nodesNumbers) {
			alignment alig = alignment(pattern.length);

			alig.Nodes[pattern.way[0]].push_back(i);
			alig.Nodes[pattern.way[1]].push_back(j);

			for (int k = 0; k < shortest_path_table[i][j].length - 1; ++k) {
				alig.Edges[pattern_net.New_edges[pattern.way[0]][pattern.way[1]]].push_back(net.Edges[net.New_edges[shortest_path_table[i][j].way[k]]
																												   [shortest_path_table[i][j].way[k + 1]]]);
			}

			//не существует пути от i к j
			if (shortest_path_table[i][j].length == 0) {
				alig.score = INT_MIN;
			}
			else {
				alig.score = score_alignment_for_working_path(alig, pattern_net, net, INT_MIN, 0, etta);
			}

			current_table[i][j] = alig;
		}
	}

	//строим таблицы Qr
	for (int r = 2; r < pattern.length; ++r) {
		prev_table = current_table;
		current_table.clear();
		for (int i : nodesNumbers) {//i -- индекс узла, с которым выравниваетс€ перва€ вершина
			for (int j : nodesNumbers) {//j -- индекс узла, с которым выравниваетс€ последн€€ на данный момент вершина
				alignment best_alig = alignment();
				bool f = false;
				for (int k : nodesNumbers) {//индекс узла, с которым была выровнена предыдуща€ вершина
					alignment alig = prev_table[i][k];

					alig.Nodes[pattern.way[r]].push_back(j);

					for (int q = 0; q < shortest_path_table[k][j].length - 1; ++q) {
						alig.Edges[pattern_net.New_edges[pattern.way[r-1]][pattern.way[r]]].push_back(net.Edges[net.New_edges[shortest_path_table[k][j].way[q]]
																															 [shortest_path_table[k][j].way[q + 1]]]);
					}

					//не существует пути от i к j
					if (shortest_path_table[i][j].length == 0 || shortest_path_table[k][j].length == 0) {
						alig.score = INT_MIN;
					}
					else {
						alig.score = score_alignment_for_working_path(alig, pattern_net, net, INT_MIN, 0, etta);
					}

					if (alig.score > best_alig.score || !f) {
						best_alig = alig;
						f = true;
					}
				}

				current_table[i][j] = best_alig;
			}
		}

		/*{
			cout << "Q" << r << endl;
			for (int i : nodesNumbers) {
				for (int j : nodesNumbers) {
					cout << current_table[i][j].score << " ";
				}
				cout << endl;
			}
		}*/
	}

	alignment result = alignment(pattern.length);

	for (int i :nodesNumbers) {
		for (int j : nodesNumbers) {
			if (current_table[i][j].score > result.score) {
				result = current_table[i][j];
			}
		}
	}
	return result;
}

//подготовка
vector<alignment> start_dynamic_algor(vector<Net> patterns, Net net) {
	vector<alignment> result = vector<alignment>();
	//fill_shortest_path_table(net);
	prepare_table();

	for (int i = 0; i < patterns.size(); ++i) {
		WayStruct pattern_way = from_net_to_way(patterns[i]);
		result.push_back(dynamic_realization(pattern_way, net, patterns[i]));
	}
	return result;
}

//генерирует паттерны и сеть, выравнивает и выводит данные на консоль
void start_test_dynamic_alignment(int pattern_count, int pattern_len, int net_len, int seq_len) {
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
	fill_shortest_path_table(random_net);
	double table_end_clock = clock();

	for (int i = 0; i < net_len; ++i) {
		for (int j = 0; j < net_len; ++j) {
			cout << shortest_path_table[i][j].length << " ";
		}
		cout << endl;
	}
	double begin_time = clock();

	vector<alignment> alignments = start_dynamic_algor(random_patterns, random_net);

	double end_time = clock();

	for (int i = 0; i < pattern_count; ++i) {
		cout << "Alignment " << i << " pattern" << endl;
		alignments[i].print();
	}


	cout << endl << "time table: " << table_end_clock - table_begin_clock << endl;
	cout << endl << "time alig: " << end_time - begin_time << endl;
}

//заполн€ет в файл данные о времени работы алгоритма на данных разной размерности
void find_average_time_dynamic(int min_pattern_count, int min_pattern_len, int min_net_len, 
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
					vector<Net> patterns = vector<Net>();

					for (int i = 0; i < pattern_count; ++i) {
						patterns.push_back(generate_pattern(pattern_len, seq_len));
					}

					Net net = generate_net(net_len, seq_len);

					long begin_time = clock();

					fill_shortest_path_table(net);

					vector<alignment> result = start_dynamic_algor(patterns, net);

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