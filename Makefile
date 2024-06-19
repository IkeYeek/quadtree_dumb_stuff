terrain_destruction_run_dbg: terrain_destruction
	gdb targets/terrain_destruction
terrain_destruction_run: terrain_destruction
	targets/terrain_destruction
terrain_destruction:
	gcc terrain_destruction.c quadtree.c vector.c -o targets/terrain_destruction -lncurses -ggdb3 -lm -fsanitize=address
simple_graph_run: simple_graph
	targets/simple_graph
simple_graph:
	gcc simple_graph.c quadtree.c vector.c -o targets/simple_graph -lncurses -g
test_run: test
	targets/test_quadtree
test:
	gcc quadtree_tests.c quadtree.c -o targets/test_quadtree -g

