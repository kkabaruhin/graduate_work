#include "exhaustive_search.h"


int main()
{
	Net net;
	Net pattern;

	Node n = { 1, {2}, "AA" };
	pattern.AddNode(n);
	n = { 2, {}, "CC" };
	pattern.AddNode(n);

	n = { 1, {2}, "AA" };
	net.AddNode(n);
	n = { 2, {3}, "CC" };
	net.AddNode(n);
	n = { 3, {}, "TT" };
	net.AddNode(n);

	exhaustive_search_class esc;

	set<alignment> result = esc.exhaustive_search(pattern, net);

	for (alignment al : result)
	{
		al.print();
	}

	system("pause");
	return 0;
}

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