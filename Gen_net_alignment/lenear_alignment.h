#pragma once
#include"score_alignment.h"

//таблица. В ячейке [i,j] находятся все пути из i в j
unordered_map<int, unordered_map<int, vector<WayStruct>>> path_table;
vector<WayStruct> Cycles;
set<int> nodesNumbers;

void remove_extra_paths() {
	//cout << "message!!!!!!!!!!!!";
	for (auto lines : path_table) {
		for (int k : nodesNumbers) {
			if (lines.first == k) {
				continue;
			}
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

void fill_path_table(Net net) {
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
			if (i > index) {
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

		int colum = index;
		for (int index2 : nodesNumbers) {
			int line = index2;

			if (line >= colum) {
				break;
			}

			if (path_table[line][colum].empty()) {
				continue;
			}

			for (auto kv2 : path_table[index]) {
				if (kv2.second.empty()) {
					continue;
				}
				if (index == line) {
					continue;
				}
				if (colum == kv2.first) {
					continue;
				}
				for (WayStruct firstWay : path_table[line][colum]) {
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

}


/*set<alignment> path_alignment(Net pattern, Net net) {

}*/