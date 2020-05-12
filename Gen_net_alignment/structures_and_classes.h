#pragma once
#include<vector>
#include<iostream>
#include<string>
#include<unordered_map>
#include<set>
#include<queue>

//множитель штрафов (устаревший)
double fine_coefficient = 0.9;
//штрафы для растяжений
double etta = -10; //для ребер паттерна
double gamma = -15; //для вершин сети

using namespace std;

//исключение для некорректного номера вершины
class Wrong_node_ex :public exception
{
public:
	int index;
	Wrong_node_ex(const char* msg, int ind) : exception(msg)
	{
		this->index = ind;
	}
};

//структура для вершины
struct Node
{
	int index;
	vector<int> adjacent_nodes; //вектор индексов смежных вершин
	string sequence; //генетическая последовательность

	Node()
	{
		index = -1;
		adjacent_nodes = vector<int>();
		sequence = string();
	}

	Node(int index)
	{
		index = index;
		adjacent_nodes = vector<int>();
		sequence = string();
	}

	Node(int ind, vector<int> a_n, string seq)
	{
		index = ind;
		adjacent_nodes = a_n;
		sequence = seq;
	}

	void print()
	{
		cout << "index: " << index << endl;
		cout << "sequence: " << sequence << endl;
	}
};

//структура для ребра
struct edge
{
	int from, to;

    friend bool operator==(const edge& const lv, const edge& const rv)
	{
		return lv.from == rv.from && lv.to == rv.to;
	}

	void print()
	{
		cout << "from: " << from << endl;
		cout << "to: " << to << endl;
	}
};

//структура для сети
struct Net
{
	int root; //индекс корневой вершины (для линейных паттернов и деревьев)
	unordered_map<int, Node> Nodes;
	vector<edge> Edges;
	unordered_map<int, unordered_map<int, int>> New_edges; //по номерам вершин, образующим edge, возвращает индекс в основном векторе

	Net()
	{
		Edges.reserve(131072);//2^17
	}

	void AddNode(Node& const n)
	{
		if (Nodes.find(n.index) != Nodes.end())
			throw Wrong_node_ex("One network cannot contain two vertices with the same indices. Problem node: ", n.index);
		else
		{
			Nodes[n.index] = n;

			if (this->Nodes.size() == 1) {
				this->root = n.index;
			}

			for (int i = 0; i < n.adjacent_nodes.size(); i++) {
				this->Edges.push_back({ n.index, n.adjacent_nodes[i] });
				this->New_edges[n.index][n.adjacent_nodes[i]] = Edges.size() - 1;
			}
		}
	}

	void print()
	{
		cout << "root: " << root << endl;
		for (pair<int, Node> n : Nodes)
			n.second.print();
		for (int i = 0; i < Edges.size(); i++)
			Edges[i].print();
	}

	//добавляет вершине с указанным индексом ребра к указанным вершинам
	void add_edges(int node_index, vector<int> adiing_node_index_vector) {
		for (int i : adiing_node_index_vector) {
			Nodes[node_index].adjacent_nodes.push_back(i);
			Edges.push_back({ node_index, i });
		}
	}
};

//структура для выравнивания
struct alignment
{
	unordered_map<int, vector<int>> Nodes; //по индексу вершины паттерна возвращает коллекцию вершин в сети, с которыми та выровнена
	vector<vector<edge>> Edges; //по индексу ребра паттерна возвращает коллекцию ребер сети, с которыми та выровнена

	double score;

	alignment() {
		this->Edges = vector<vector<edge>>();

		this->score = INT_MIN;
	}

	alignment(int pattern_length) {
		this->Edges = vector<vector<edge>>();

		this->Edges.resize(pattern_length - 1, vector<edge>());
		this->score = INT_MIN;
	}

	friend bool operator<(const alignment& lv, const alignment& rv)
	{
		if (abs(lv.score - rv.score) > 0.000001)
			return lv.score < rv.score;

		for (pair<int,vector<int>> x : lv.Nodes)
		{
			if (rv.Nodes.count(x.first)) {
				if (x.second.size() != rv.Nodes.at(x.first).size())
					return x.second.size() < rv.Nodes.at(x.first).size();
			}
		}

		for (int i = 0; i < lv.Edges.size(); i++)
		{
			if (lv.Edges.at(i).size() != rv.Edges.at(i).size())
				return lv.Edges.at(i).size() < rv.Edges.at(i).size();
		}

		for (pair<int, vector<int>> x : lv.Nodes)
		{
			for (int i : x.second)
				if (rv.Nodes.count(x.first) > 0) {
					if (find(rv.Nodes.at(x.first).begin(), rv.Nodes.at(x.first).end(), i) == rv.Nodes.at(x.first).end())
					{
						for (int j : rv.Nodes.at(x.first))
							if (find(lv.Nodes.at(x.first).begin(), lv.Nodes.at(x.first).end(), j) == lv.Nodes.at(x.first).end())
								return i < j;
					}
				}
		}

		for (int i = 0; i < lv.Edges.size(); i++)
		{
			for (edge j : lv.Edges.at(i))
				if (find(rv.Edges.at(i).begin(), rv.Edges.at(i).end(), j) == rv.Edges.at(i).end())
				{
					for (edge k : rv.Edges.at(i))
						if (find(lv.Edges.at(i).begin(), lv.Edges.at(i).end(), k) == lv.Edges.at(i).end())
							return j.from < k.from;
				}
		}

		return false;
	}

	void print()
	{
		cout << "score: " << score << endl;
		cout << "Nodes:" << endl;

		for (pair<int, vector<int>> x : Nodes)
		{
			cout << "from: " << x.first << "; to: ";
			for (int n : x.second)
				cout << n << ",";
			cout << endl;
		}

		cout << endl << "Edges:" <<  endl;

		for (int i = 0; i < Edges.size(); i++)
		{
			cout << "__";
			cout << "edge number: " << i << " alig with:" << endl;
			for (edge e : Edges[i])
			{
				e.print();
			}
		}
		cout << "___________________________________________________________" << endl;
	}
};

//структура для хранения путей (для линейных паттернов)
struct WayStruct {
	//список вершин, образующий путь
	vector<int> way;
	//длина пути
	int length;
	//ключ -- индекс в way; значение -- номера циклов, которым принадлежит соответствующая вершина
	unordered_map<int, vector<int>> entry_points;

	WayStruct(int index1, int index2) {
		this->way = vector<int>();
		this->way.push_back(index1);
		this->way.push_back(index2);
		this->length = 2;
		this->entry_points = unordered_map<int, vector<int>>();
	}

	WayStruct() {
		this->way = vector<int>();
		this->length = 0;
		this->entry_points = unordered_map<int, vector<int>>();
	}

	WayStruct(vector<int>& const in) {
		this->way = in;
		this->length = in.size();
	}

	void addNode(int index) {
		this->way.push_back(index);
		this->length++;
	}

	void print() {
		cout << "length : " << length << endl << "way: ";
		for (int i : way) {
			cout << i << " ";
		}
		cout << endl;
		cout << "entry_points :" << endl;
		for (auto kv : entry_points) {
			cout << kv.first << ": ";
			for (int j : kv.second) {
				cout << j << " ";
			}
			cout << endl;
		}
	}

	//путь является лишним (избыточным) <=> существует вершина, которая посещается более одного раза
	//возвращает false, если путь не лишний. True, если лишний
	bool excess() {
		unordered_map<int, bool> data = unordered_map<int, bool>(); //ключ -- индекс вершины. Значение = true, если такая вершина уже встречалась

		for (int i = 0; i < way.size(); ++i) {
			if (data.count(way[i]) > 0) {
				if (i == way.size() - 1 && way[i] == way[0]) {
					continue;
				}
				return true;
			}
			else {
				data[way[i]] = true;
			}
		}
		return false;
	}


};

//объединение двух путей
WayStruct joinPath(WayStruct& const way1, WayStruct& const way2) {
	WayStruct result = WayStruct();

	result.way.insert(result.way.end(), way1.way.begin(), way1.way.end());
	if (result.way.size() > 0) {
		result.way.erase(result.way.end() - 1);
	}

	result.way.insert(result.way.end(), way2.way.begin(), way2.way.end());

	result.entry_points = unordered_map<int, vector<int>>();

	result.length = result.way.size();

	return result;
}

//структура для удобного хранения отсортированных "указателей" на WayStruct в таблицах
struct WayPriority {
	int column;
	int line;
	int index;
	int priority;
	WayStruct way;

	WayPriority(int c, int l, int i, int p, WayStruct w) {
		this->column = c;
		this->line = l;
		this->index = i;
		this->priority = p;
		this->way = w;
	}

	friend bool operator<(const WayPriority& left, const WayPriority& right) {
		if (left.priority < right.priority)
			return true;
		if (left.priority > right.priority)
			return false;

		if (left.column < right.column)
			return true;
		if (left.column > right.column)
			return false;

		if (left.line < right.line)
			return true;
		if (left.line > right.line)
			return false;

		if (left.index < right.index)
			return true;
		if (left.index > right.index)
			return false;

		if (left.way.length < right.way.length)
			return true;
		if (left.way.length > right.way.length)
			return false;

		if (left.way.entry_points.size() < right.way.entry_points.size())
			return true;
		if (left.way.entry_points.size() > right.way.entry_points.size())
			return false;

		return false;
	}
};

//преобразует линейный паттерн в путь
WayStruct from_net_to_way(Net& const net) {
	WayStruct result = WayStruct();
	Node current_node = net.Nodes[net.root];

	while (true) {
		result.addNode(current_node.index);

		if (current_node.adjacent_nodes.empty()) {
			break;
		}
		current_node = net.Nodes[*current_node.adjacent_nodes.begin()];
	}
	result.length = result.way.size();
	return result;
}

//таблица. В ячейке [i,j] находятся все пути из i в j (устаревший)
unordered_map<int, unordered_map<int, vector<WayStruct>>> path_table;

//устаревший)
vector<WayStruct> Cycles;
//хранит номера вершин в сети
set<int> nodesNumbers;
//в ячейке [i][j] путь минимальной длины из вершины i в вершину j
unordered_map<int, unordered_map<int, WayStruct>> shortest_path_table;

//алгоритм обхода графа в ширину для заполнения shortest_path_table
void bfs(int start_index, Net& const net) {
	unordered_map<int, bool> visit = unordered_map<int, bool>();
	unordered_map<int, int> who_pred = unordered_map<int, int>();//по индексу вершины возвращает индекс предшественника

	queue<int> q;
	q.push(start_index);
	shortest_path_table[start_index][start_index] = WayStruct(start_index, start_index);

	while (!q.empty()) {
		int current_index = q.front();

		for (int i : net.Nodes[current_index].adjacent_nodes) {
			if (!visit[i]) {
				q.push(i);
				visit[i] = true;
				who_pred[i] = current_index;
			}
		}

		if (current_index == start_index) {
			q.pop();
			continue;
		}

		vector<int> reverse_way = vector<int>();

		while (current_index != start_index) {
			reverse_way.push_back(current_index);

			current_index = who_pred[current_index];
		}
		reverse_way.push_back(start_index);

		vector<int> normal_way = vector<int>();

		for (int i = 0; i < reverse_way.size(); ++i) {
			normal_way.push_back(reverse_way[reverse_way.size() - (i + 1)]);
		}

		WayStruct way = WayStruct(normal_way);

		shortest_path_table[start_index][q.front()] = way;

		q.pop();
	}
}

//заполняет shortest_path_table
void fill_shortest_path_table(Net& const net) {
	nodesNumbers = set<int>();
	for (auto kv : net.Nodes) {
		nodesNumbers.insert(kv.first);
	}

	for (int i : nodesNumbers) {
		bfs(i, net);
	}
}

//хранит индекс узла в сети, с которым выровнена некоторая вершина. И оценку этого выравнивания
struct ind_score {
	int index;//индекс 
	double score;

	friend bool operator<(const ind_score& left, const ind_score& right) {
		if (abs(left.score - right.score) > 0.0001)
			return left.score < right.score;
		else
			return left.index < right.index;
	}
};

//хранит комбинацию из p чисел, кодирующих какое-то выравнивание. И оценку этого выравнивания сверху, посчитанную как сумма (для перебора с отсечением)
struct Combination {
	unordered_map<int, int> combination;
	double score;

	friend bool operator<(const Combination& left, const Combination& right) {
		if (abs(left.score - right.score) > 0.0001)
			return left.score < right.score;
		else {
			for (auto x : left.combination) {
				if (x.second != right.combination.at(x.first)) {
					return x.second < right.combination.at(x.first);
				}
			}
		}
		return false;
			
	}
};