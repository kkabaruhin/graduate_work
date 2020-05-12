#include "lenear_alignment.h"
#include "tree_alignment.h"
#include "clipping_linear.h"
#include<ctime>

int main()
{
	srand(time(0));

	//время для дерева
	/*find_average_time_tree(1, 3, 5,
		1, 7, 11,
		3, 5,
		5, 20);*/

	//время для ветвей и границ 
	find_average_time_clipping(1, 2, 3,
		1, 50, 100,
		3, 5,
		5, 20);

	//для линейного графа
	/*find_average_time_dynamic(1, 2, 3,
		5, 4, 10, 
		3, 5,
		5, 20);*/


	system("pause");
	return 0;
}