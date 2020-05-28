#include "lenear_alignment.h"
#include "tree_alignment.h"
#include "clipping_linear.h"
#include<ctime>

int main()
{
	srand(time(0));

	//start_test_trees_alignment(1, 5, 7, 20);

	//для линейного графа
	/*find_average_time_dynamic(1, 21, 5,
	1, 22, 100,
	3, 5,
	5, 20);*/

	//время для дерева
	/*find_average_time_tree(1, 3, 5,
		1, 100, 200,
		3, 5,
		10, 20);*/

	//время для ветвей и границ 
	find_average_time_clipping(1, 5, 10,
		1, 25, 100,
		3, 5,
		1000, 30);


	system("pause");
	return 0;
}