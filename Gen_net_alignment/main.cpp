#include "exhaustive_search.h"
#include "lenear_alignment.h"
#include<ctime>
#include"random_net_generator.h"

int main()
{
	srand(time(0));
	double begin_time = clock();
	Net net = Net();
	Net pattern = Net();
	Node n;

	n = { 1,{ 2 }, "AAAA" };
	net.AddNode(n);
	n = { 2,{ 4 }, "CCCC" };
	net.AddNode(n);
	n = { 3,{ 4 }, "TTTT" };
	net.AddNode(n);
	n = { 4,{ 5 }, "TTTT" };
	net.AddNode(n);
	n = { 5,{ 6, 7 }, "CCCC" };
	net.AddNode(n);
	n = { 6,{ }, "TTTT" };
	net.AddNode(n);
	n = { 7,{}, "TTTT" };
	net.AddNode(n);

	n = { 1,{ 2 }, "AAAA" };
	pattern.AddNode(n);
	n = { 2,{ 3 }, "CCCC" };
	pattern.AddNode(n);
	n = { 3,{}, "TTTT" };
	pattern.AddNode(n);

	Net random_net = generate_net(10);
	Net random_pattern = generate_pattern(4);

	random_net.print();

	cout << endl << endl;

	random_pattern.print();
	cout << endl << endl;

	set<alignment> alignments = path_alignment(random_pattern, random_net);

	double end_time = clock();

	for (alignment alig : alignments) {
		alig.print();
	}

	cout << endl << "time: " << end_time - begin_time << endl;


	system("pause");
	return 0;
}

//проверка таблицы
/*n = { 1, {2}, "AAAA" };
net.AddNode(n);
n = { 2, {4}, "CCCC" };
net.AddNode(n);
n = { 3, {4}, "TTTT" };
net.AddNode(n);
n = { 4, {5}, "TTTT" };
net.AddNode(n);
n = { 5, {6, 7}, "CCCC" };
net.AddNode(n);
n = { 6, {4}, "TTTT" };
net.AddNode(n);
n = { 7, {}, "TTTT" };
net.AddNode(n);

fill_path_table(net);

for (auto i : net.Nodes) {
for (auto j : net.Nodes) {
if (path_table[i.first].find(j.first) != path_table[i.first].end()) {
if (path_table[i.first][j.first].size() > 0) {
for (int k = 0; k < path_table[i.first][j.first].size(); k++) {
cout << path_table[i.first][j.first][k].length << ",";
}
cout << "  ";
}
else {
cout << 0 << "  ";
}
}
else {
cout << 0 << "  ";
}
}
cout << endl;
}

cout << endl;

for (WayStruct way : path_table[2][5]) {
way.print();
}*/


/*alignment alig;

alig.Nodes = unordered_map<int, vector<int>>();
alig.Nodes[1] = vector<int>();
alig.Nodes[2] = vector<int>();
alig.Nodes[1].push_back(1);
alig.Nodes[2].push_back(2);

alig.Edges = vector<vector<edge>>();

edge e = net.Edges[0];

alig.Edges.push_back({ e });

alig.score = score_alignment(alig, pattern, net);

//================================================================
alignment alig2;

alig2.Nodes = unordered_map<int, vector<int>>();
alig2.Nodes[1] = vector<int>();
alig2.Nodes[2] = vector<int>();
alig2.Nodes[1].push_back(2);
alig2.Nodes[2].push_back(3);

alig2.Edges = vector<vector<edge>>();

edge e2 = net.Edges[1];

alig2.Edges.push_back({ e2 });

alig2.score = score_alignment(alig2, pattern, net);

cout << alig.score << endl << alig2.score << endl;

set<alignment> s;

s.insert(alig);
s.insert(alig2);

cout << s.size() << endl << endl;

for (alignment al : s)
{
cout << al.score << endl;
}*/

/*Net net;

Node n = { 1,{ 2, 3 }, "A" };

net.AddNode(n);

n = { 2, {4}, "B" };
net.AddNode(n);
n = { 3,{ 5 }, "C" };
net.AddNode(n);
n = { 4,{ 5,6,7 }, "D" };
net.AddNode(n);
n = { 5,{ 7 }, "E" };
net.AddNode(n);
n = { 6,{  }, "F" };
net.AddNode(n);
n = { 7,{ 6, 8 }, "G" };
net.AddNode(n);
n = { 8,{ 9 }, "H" };
net.AddNode(n);
n = { 9,{ 10 }, "I" };
net.AddNode(n);
n = { 10,{ 6 }, "J" };
net.AddNode(n);

net.print();

cout << endl << endl;

count_dropped_nodes c_d_n;

vector<int> nodes;

nodes = { 1,2,3, 6, 7};

cout << c_d_n.find_count_dropped_nodes(net, nodes) << endl;

nodes = { 1,2,3, 4, 6 };

cout << c_d_n.find_count_dropped_nodes(net, nodes) << endl;

nodes = { 7,8,6,10,9};

cout << c_d_n.find_count_dropped_nodes(net, nodes) << endl;*/


/*count_unavailable_nodes c_u_n;

vector<int> from = { 2, 4, 6 };

vector<int> to = { 12, -1, 13 };

vector<edge> edges;

edges.push_back({ 1,2 });
edges.push_back({ 1,3 });
edges.push_back({ 2,4 });
edges.push_back({ 3,5 });
edges.push_back({ 4,5 });
edges.push_back({ 4,6 });
edges.push_back({ 5,7 });
edges.push_back({ 6,8 });
edges.push_back({ 6,7 });
edges.push_back({ 7,9 });
edges.push_back({ 9,10 });
edges.push_back({ 10,11 });

int res = c_u_n.find_count_unavailable_nodes(from, to, edges);

cout << res << endl;*/