#pragma once
//����� ������ � ������

#include"score_alignment.h"
#include"random_net_generator.h"
#include<iostream>
#include<fstream>

//���������� ������������ �� �������
alignment generate_alig(WayStruct& const pattern_way,  Net& const pattern, Net& const net, Combination current_comb) {
	alignment result = alignment(pattern.Nodes.size());
	for (auto x : current_comb.combination) { //������������� �������
		result.Nodes[x.first].push_back(x.second);
	}

	bool f = true; //���� f == true, ��� ������, ��� ���� ����� i � j � �������� ���� �����, �� �� ���� ���� ���� �� ������ i � ����� j

	for (int i = 0; i < pattern_way.way.size() - 1; i++) {
		int current_v = pattern_way.way[i];
		int next_v = pattern_way.way[i+1];

		int current_u = *result.Nodes[current_v].begin();
		int next_u = *result.Nodes[next_v].begin();

		for (int k = 0; k < shortest_path_table[current_u][next_u].length - 1; ++k) {//���������� �����
			result.Edges[pattern.New_edges[current_v][next_v]].
				push_back(net.Edges[net.New_edges[shortest_path_table[current_u][next_u].way[k]]
					[shortest_path_table[current_u][next_u].way[k + 1]]]);
		}

		if (shortest_path_table[current_u][next_u].length == 0) {
			result.score = INT_MIN;
			f = false;
		}
		
	}

	if (f) {
		result.score = score_alignment_for_working_path(result, pattern, net, INT_MIN, gamma, etta);
	}

	return result;
}

//��������� ������� � ����������
alignment clipping(WayStruct pattern_way, Net& const pattern, Net& const net) {
	//��������� ��������������� ������� ��� ������ �������
	//queues[i].begin() ������ ������ ����, � ������� ����� ����� ������������� ������� i, � ���� �������� ������������ 
	unordered_map<int, vector<ind_score>> queues = unordered_map<int, vector<ind_score>>();

	//whats_your_index[i][j] �������� ������ �������� � queues[i], ������� ����� j
	unordered_map<int, unordered_map<int, int>> whats_your_index = unordered_map<int, unordered_map<int, int>>();

	for (auto p : pattern_way.way) { //�������� queues � �����������
		queues[p] = vector<ind_score>();
		for (int n : nodesNumbers) {
			ind_score i_s;
			i_s.index = n;
			i_s.score = local_alignment_seq(pattern.Nodes[p].sequence, net.Nodes[n].sequence);
			queues[p].push_back(i_s);
		}
		sort(queues[p].begin(), queues[p].end());
		for (int i = 0; i < queues[p].size() / 2; ++i) {
			ind_score x = queues[p][i];
			queues[p][i] = queues[p][queues[p].size() - 1 - i];
			queues[p][queues[p].size() - 1 - i] = x;

			whats_your_index[p][queues[p][i].index] = i;

			whats_your_index[p][x.index] = queues[p].size() - 1 - i;
		}
	}

	//��������� ���������� � ������������ ������� ������
	Combination best_comb;
	best_comb.score = 0;
	for (auto p : pattern.Nodes) {
		best_comb.combination[p.first] = (*queues[p.first].begin()).index;
		best_comb.score += (*queues[p.first].begin()).score;
	}

	//���������, ���������� ��� ����������, ������� ���� ������������ ��� ��������� ������������
	set<Combination> working_comb = set<Combination>();

	working_comb.insert(best_comb);

	//���������� ����������� ������������ �� ������������ �������
	alignment best_alig = generate_alig(pattern_way, pattern, net, best_comb);

	while (true) {
		bool end = true; //end == true, ����� �� ������� ������������ �� ����� ����� ������� � ��� �������� ��� ���� �����������

		Combination current_best_comb;
		current_best_comb.score = INT_MIN;

		//���� ����� ���������� � ������������ �������
		//����� ���������� -- ��� ���� �� ������ ����������, � ������� ������� � ����� �� �������� ������� �� ���� �����
		for (auto x : working_comb) {
			for (auto p : pattern_way.way) {
				Combination current_comb = x;
				int index_in_queue = whats_your_index[p][current_comb.combination[p]];

				if (index_in_queue >= queues[p].size() - 1) {//������ ������� ��� ������ �������
					continue;
				}

				//��������� ����� �������
				current_comb.score -= queues[p][index_in_queue].score;

				current_comb.combination[p] = queues[p][index_in_queue + 1].index;
				current_comb.score += queues[p][index_in_queue + 1].score;

				if (current_best_comb < current_comb) {
					if (working_comb.find(current_comb) == working_comb.end()) {
						current_best_comb = current_comb;
						end = false;
					}
				}
			}
		}

		if (end) {//�� ��������� ��� ��������
			return best_alig;
		}

		if (best_alig.score > current_best_comb.score) {//best_alig �������� ����������� �������������
			return best_alig;
		}

		alignment current_alig = generate_alig(pattern_way, pattern, net, current_best_comb);

		if (best_alig.score < current_alig.score) {
			best_alig = current_alig;
		}

		working_comb.insert(current_best_comb);
	}
}

//������ ������� � ����������. �������� ��� ��������� ��������
vector<alignment> find_clipping(vector<Net> patterns, Net net) {
	vector<alignment> result = vector<alignment>();

	//fill_shortest_path_table(net);
	//prepare_table();

	for (int i = 0; i < patterns.size(); i++) {
		WayStruct pattern_way = from_net_to_way(patterns[i]);
		result.push_back(clipping(pattern_way, patterns[i], net));
	}

	return result;
}

//���������� ��������� ����� ��������� � ����, ������� �� � ������� ������� ��������
void start_test_find_clipping(int pattern_count, int pattern_len, int net_len, int seq_len) {
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

	cout << "shortest_path_table:" << endl;

	for (int i = 0; i < net_len; ++i) {
		for (int j = 0; j < net_len; ++j) {
			cout << shortest_path_table[i][j].length << " ";
		}
		cout << endl;
	}
	double begin_time = clock();

	vector<alignment> alignments = find_clipping(random_patterns, random_net);

	double end_time = clock();

	for (int i = 0; i < pattern_count; ++i) {
		cout << "Alignment " << i << " pattern" << endl;
		alignments[i].print();
	}


	cout << endl << "time table: " << table_end_clock - table_begin_clock << endl;
	cout << endl << "time alig: " << end_time - begin_time << endl;
}

//������� ���� � ������� � �������� ������ ��������� ��������
void find_average_time_clipping(int min_pattern_count, int min_pattern_len, int min_net_len,
	int max_pattern_count, int max_pattern_len, int max_net_len,
	int delta_pattern, int delta_net,
	int count_of_tests, int seq_len) {
	ofstream file;
	file.open("times.txt");

	if (!file.is_open()) {
		cout << "eto fiasco" << endl;
	}

	ofstream file2;
	file2.open("times2.txt");

	if (!file2.is_open()) {
		cout << "eto fiasco" << endl;
	}

	ofstream categories_file;
	categories_file.open("categories.txt");

	if (!categories_file.is_open()) {
		cout << "eto fiasco" << endl;
	}

	vector<long long> times = vector<long long>();

	//���������� ����� min � max ������� �� ������ ������ ������ � ������ ����� �������� � ���� �� ���� ���������
	//categories[i] �������� ���������� ������, ������������� ��������� i
	int count_of_categories = 30;
	vector<int> categories = vector<int>(count_of_categories, 0);

	prepare_table();

	for (int pattern_count = min_pattern_count; pattern_count < max_pattern_count + 1; ++pattern_count) {
		for (int pattern_len = min_pattern_len; pattern_len < max_pattern_len + 1; pattern_len += delta_pattern) {
			for (int net_len = max(min_net_len, pattern_len); net_len < max_net_len + 1; net_len += delta_net) {
				long long max_time, min_time, average_time, sum_time;
				max_time = min_time = average_time = sum_time = 0;
				file2 << "pattern_len = " << pattern_len << "; ";
				file2 << "net_len = " << net_len << "; " << endl;
				for (int test_index = 1; test_index < count_of_tests + 1; ++test_index) {
					vector<Net> patterns = generate_patterns_vector(pattern_count, pattern_len, seq_len);

					Net net = generate_net(net_len, seq_len);

					long begin_time = clock();

					fill_shortest_path_table(net);

					vector<alignment> result = find_clipping(patterns, net);

					long end_time = clock();

					long current_time = end_time - begin_time;

					sum_time += current_time;

					if (max_time < current_time) {
						max_time = current_time;
					}

					if (min_time == 0) {
						min_time = current_time;
					}
					else {
						if (min_time > current_time)
							min_time = current_time;
					}

					average_time = sum_time / test_index;
					file << "min_time = " << min_time << "; ";
					file << "max_time = " << max_time << "; ";
					file << "average_time = " << average_time << "; ";
					file << "current_time = " << current_time << "; ";
					file << endl;

					file2 << current_time << endl;
					times.push_back(current_time);
				}
				average_time = sum_time / count_of_tests;
				file << "pattern_count = " << pattern_count << "; ";
				file << "pattern_len = " << pattern_len << "; ";
				file << "net_len = " << net_len << "; ";
				file << "min_time = " << min_time << "; ";
				file << "max_time = " << max_time << "; ";
				file << "average_time = " << average_time << "; ";
				file << "end" << endl;

				file2 << "end" << endl;

				long long delta = ((max_time - min_time) / count_of_categories) + 1;

				for (int i = 0; i < count_of_tests; i++) {
					long long current_time = times[i] - min_time;
					int current_categories = current_time / delta;
					current_categories = min(current_categories, count_of_categories - 1);
					categories[current_categories]++;
				}

				categories_file << "pattern_count = " << pattern_count << "; ";
				categories_file << "pattern_len = " << pattern_len << "; " << endl;

				for (int i = 0; i < count_of_categories; i++) {
					categories_file << "from " << min_time + delta * i << ";";
					categories_file << "to " << min_time + delta * (i + 1) << ";";
					categories_file << "count = " << categories[i] << endl;
				}
			}
			file << endl;
		}
	}
	file.close();
	file2.close();
	categories_file.close();
}