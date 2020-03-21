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
	unordered_map<int, Node> Nodes;
	vector<edge> Edges;
	//unordered_map<int, int> display; //Отображение, позволяющее по оригинальному индексу вершины найти соответствующий индекс в net
									 //необходимо, чтобы индексы вершин сами по себе удовлетворяли лишь одному требования: не повторялись
									 //допустимы вершины с отрицательными индексами, идущие в призвольном порядке и т.д.

	Net()
	{
		Nodes.reserve(512);
		Edges.reserve(131072);//2^17
	}

	void AddNode(Node& const n)
	{
		if (Nodes.find(n.index) != Nodes.end())
			throw Wrong_node_ex("Одна сеть не может содержать две вершины с одинаковыми индексами. Проблемная вершина: ", n.index);
		else
		{
			Nodes[n.index] = n;

			for (int i = 0; i < n.adjacent_nodes.size(); i++)
				this->Edges.push_back({ n.index, n.adjacent_nodes[i] });
		}
	}

	void print()
	{
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

	friend bool operator<(const alignment&  lv, const alignment& rv)
	{
		if (abs(lv.score - rv.score) > 0.000001)
			return lv.score < rv.score;

		for (pair<int,vector<int>> x : lv.Nodes)
		{
			if (x.second.size() != rv.Nodes.at(x.first).size())
				return x.second.size() < rv.Nodes.at(x.first).size();
		}

		for (int i = 0; i < lv.Edges.size(); i++)
		{
			if (lv.Edges.at(i).size() != rv.Edges.at(i).size())
				return lv.Edges.at(i).size() < rv.Edges.at(i).size();
		}

		for (pair<int, vector<int>> x : lv.Nodes)
		{
			for (int i : x.second)
				if (find(rv.Nodes.at(x.first).begin(), rv.Nodes.at(x.first).end(), i) == rv.Nodes.at(x.first).end())
				{
					for (int j : rv.Nodes.at(x.first))
						if (find(lv.Nodes.at(x.first).begin(), lv.Nodes.at(x.first).end(), j) == lv.Nodes.at(x.first).end())
							return i < j;
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
			cout << "from: " << i << " to:" << endl;
			for (edge e : Edges[i])
			{
				e.print();
			}
			cout << "___________";
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
		way = vector<int>();
		way.push_back(index1);
		way.push_back(index2);
		length = 2;
	}

	WayStruct() {
		this->way = vector<int>();
		this->length = 0;
		this->entry_points = unordered_map<int, vector<int>>();
	}

	void print() {

	}

	//путь является лишним (избыточным) <=> существует вершина, которая посещается более одного раза
	//возвращает false, если путь не лишний. True, если лишний
	bool excess() {
		unordered_map<int, bool> data = unordered_map<int, bool>(); //ключ -- индекс вершины. Значение = true, если такая вершина уже встречалась

		for (int i = 0; i < way.size(); ++i) {
			if (data.count(way[i]) > 0) {
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