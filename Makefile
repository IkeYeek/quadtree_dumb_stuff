simple_graph_run: simple_graph
	targets/simple_graph
simple_graph:
	gcc simple_graph.c quadtree.c -o targets/simple_graph -lncurses -g
test_run: test
	targets/test_quadtree
test:
	gcc quadtree_tests.c quadtree.c -o targets/test_quadtree -g

