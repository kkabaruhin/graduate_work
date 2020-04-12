#pragma once
#include<vector>
#include<iostream>
#include<string>
#include<unordered_map>

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

struct Net
{
	int root; //индекс корневой вершины
	unordered_map<int, Node> Nodes;
	vector<edge> Edges;
	unordered_map<int, unordered_map<int, int>> New_edges; //по номерам вершин, образующим edge, возвращает индекс в основном векторе

	Net()
	{
		//Nodes.reserve(512);
		Edges.reserve(131072);//2^17
		//New_edges.reserve(131072);
	}

	void AddNode(Node& const n)
	{
		if (Nodes.find(n.index) != Nodes.end())
			throw Wrong_node_ex("Одна сеть не может содержать две вершины с одинаковыми индексами. Проблемная вершина: ", n.index);
		else
		{
			Nodes[n.index] = n;

			if (this->Nodes.size() == 1) {
				this->root = n.index;
			}

			for (int i = 0; i < n.adjacent_nodes.size(); i++) {
				this->Edges.push_back({ n.index, n.adjacent_nodes[i] });
				this->New_edges[(*(--Edges.end())).from][(*(--Edges.end())).to] = Edges.size() - 1;
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
};

struct alignment
{
	unordered_map<int, vector<int>> Nodes; //по индексу вершины паттерна возвращает коллекцию вершин в сети, с которыми та выровнена
	vector<vector<edge>> Edges; //по индексу ребра паттерна возвращает коллекцию ребер сети, с которыми та выровнена

	double score;

	alignment(int pattern_length) {
		this->Edges = vector<vector<edge>>();

		this->Edges.resize(pattern_length, vector<edge>());
		this->score = 0;
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
			cout << "endge number: " << i << " alig with:" << endl;
			for (edge e : Edges[i])
			{
				e.print();
			}
		}
		cout << "___________________________________________________________" << endl;
	}
};

//структура для хранения путей

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

WayStruct joinPath(WayStruct& const way1, WayStruct& const way2) {
	WayStruct result = WayStruct();

	result.way.insert(result.way.end(), way1.way.begin(), way1.way.end());
	if (result.way.size() > 0) {
		result.way.erase(result.way.end() - 1);
	}

	result.way.insert(result.way.end(), way2.way.begin(), way2.way.end());

	result.entry_points = unordered_map<int, vector<int>>();

	/*for (auto point : way2.entry_points) {
		if (result.entry_points[point.first].empty()) {
			result.entry_points[point.first] = vector<int>();
		}
		//индексы смещаются и что делать с повторяемостью информации?
		result.entry_points[point.first + way1.length - 1].insert(result.entry_points[point.first].end(), point.second.begin(), point.second.end());
	}*/

	result.length = result.way.size();

	return result;
}

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

