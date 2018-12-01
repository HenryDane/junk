#ifndef PATCH_H
#define PATCH_H

#include <string>
#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#endif // PATCH_H

#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>

int w = 20;
int h = 10;
int stack_size = w * h;

struct tile_t {
	bool visited;

	bool n;
	bool s;
	bool e;
	bool w;
	
	bool solid;
};

struct point_t {
	int x;
	int y;
};

void set_tile(tile_t & t, bool v, bool N, bool S, bool E, bool W) {
	t.visited = v;
	t.n = N;
	t.s = S;
	t.e = E;
	t.w = W;
	
	t.solid = false;
}

void print_maze(tile_t * g) {
	//std::cout << "maze:" << std::endl;
	for (int y = 0; y < h; y++){
		std::string ln1;
		std::string ln2;
		std::string ln3;
		for (int x = 0; x < w; x++){
			ln1 += "O" + patch::to_string((g[y * w + x].n) ? " " : "#") + "O";
			ln2 += patch::to_string((g[y * w + x].e) ? " " : "#") + patch::to_string((g[y * w + x].visited) ? " " : "#") + patch::to_string((g[y * w + x].w) ? " " : "#");
			ln3 += "O" + patch::to_string((g[y * w + x].s) ? " " : "#") + "O";
		}
		std::cout << ln1 << std::endl;
		std::cout << ln2 << std::endl;
		std::cout << ln3 << std::endl;
	}
	//std::cout << "done." << std::endl;
}

void print_visited(tile_t * grid){
	//std::cout << "visited:" << std::endl;
	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){
			std::cout << grid[y * w + x].visited;
		}
		std::cout << std::endl;
	}
	//std::cout << "done.";
}

int main(int argc, char *argv[]) {
	srand(time(NULL)); // init random
	
	if (argc == 3) {
		w = strtol(argv[1], NULL, 10);
		h = strtol(argv[2], NULL, 10);
		stack_size = w * h;
	} 

	// create and init grid
	tile_t * grid = new tile_t[w * h];
	for (int i = 0; i < w * h; i++)
		set_tile(grid[i], false, false, false, false, false);

	// create and init stack
	int stk_addr = 0;
	point_t * stk = new point_t[stack_size];
	for (int i = 0; i < stack_size; i++){
		stk[i].x = 0;
		stk[i].y = 0;
	}

	// main algorithm
	int cx = rand() % w;
	int cy = rand() % h;

	// push current onto stack
	//stk_addr++; // leave {-1, -1} as first element
	stk[stk_addr++] = {cx, cy};

	int cycle = 1;
	while (true) {
		//std::cout << "cycle " << cycle << "\n";
		// set current cell as visited
		//grid[cy * w + cx].visited = true;

		// get unvisited neighbors
		int n[4] = {0, 0, 0, 0};
		if (cx + 1 < w) {
			n[0] = !grid[cy * w + (cx + 1)].visited; // east
		}
		if (cx - 1 >= 0) {
			n[1] = !grid[cy * w + (cx - 1)].visited; // west
		}
		if (cy + 1 < h) {
			n[2] = !grid[(cy + 1) * w + cx].visited; // south
		}
		if (cy - 1 >= 0) {
			n[3] = !grid[(cy - 1) * w + cx].visited; // north
		}

		if (n[0] + n[1] + n[2] + n[3] > 0) { // if 1 or more unvisited neighbors
			// choose random neighbor
			int idx = -1;
			while (idx < 0 || n[idx] == 0) {
				idx = rand() % 4;
			}

			// grid[cy * w + cx].visited = true;

			// connect current to neighbors and set neighbor as current cell
			switch (idx){
			case 0:
				grid[(cx + 1) + cy * w].e = true;
				grid[cx + cy * w].w = true;
				cx++;
				break;
			case 1:
				grid[(cx - 1) + cy * w].w = true;
				grid[cx + cy * w].e = true;
				cx--;
				break;
			case 2:
				grid[cx + (cy + 1) * w].n = true;
				grid[cx + cy * w].s = true;
				cy++;
				break;
			case 3:
				grid[cx + (cy - 1) * w].s = true;
				grid[cx + cy * w].n = true;
				cy--;
				break;
			}

			grid[cy * w + cx].visited = true;

			// push current cell to stack
			if (stk_addr + 1 < stack_size) {
				stk[stk_addr++] = {cx, cy};
			} else {
				std::cout << "failed to push to stack!" << std::endl;
				std::cout << "    " << stk_addr + 1 << " >= " << stack_size << std::endl;
			}
			
		} else { // else
			// pop stack into current cell
			point_t p = stk[stk_addr--];
			//std::cout << "reading from " << stk_addr + 1 << " found " << p.x << " , " << p.y << std::endl;
			cx = p.x;
			cy = p.y;
		}

		// if all cells visited exit
		bool flag = true;
		for (int i = 0; i < w * h; i++){
			if (!grid[i].visited) {
				flag = false;
				break;
			}
		}

		if (flag) {
			break;
		}

		/*std::cout << "=========== cycle " << cycle << " ==============" << std::endl;
		print_maze(grid);
		std::cout << std::endl;*/
		
		cycle++;
	}

	print_maze(grid);

	// cleanup memory
	delete[] grid;
	delete[] stk;

	// leave
	return true;
}
