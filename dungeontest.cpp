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

#include <iostream> // std::cout 
#include <cstdlib> // rand()
#include <time.h> // time()
#include <math.h> // sin/cos
#include <cstring> // memcpy

#define PI 3.14159265359

const int param_n_rooms = 50;
const int param_radius = 20; 
const int param_height = 100;
const int param_width = 100;

int global_uuid_idx = 0;

struct room_t {
	int id_self;
	float x;
	float y;
	int w;
	int h;
	bool fixed;
	
	// neighbors
	int n1;
	int n2;
	int n3;
	
	
	void get_center(int &xf, int &yf){
		xf = (x + w) / 2;
		yf = (y + h) / 2;
	}
};

struct graph_node_t {
	int id_self;
	int id_target;
};

struct graph_edge_t {
	int id_self;
	int id_target_a;
	int id_target_b;
	float distance;
};

struct tile_t {
	bool n;
	bool e;
	bool s;
	bool w;
	
	bool used;
};

double rand_n(void) { return (rand() / ((double) RAND_MAX)); }

double rand_normal(double mean, double stddev) {//Box muller method
    static double n2 = 0.0;
    static int n2_cached = 0;
    if (!n2_cached)
    {
        double x, y, r;
        do
        {
            x = 2.0*rand()/RAND_MAX - 1;
            y = 2.0*rand()/RAND_MAX - 1;

            r = x*x + y*y;
        }
        while (r == 0.0 || r > 1.0);
        {
            double d = sqrt(-2.0*log(r)/r);
            double n1 = x*d;
            n2 = y*d;
            double result = n1*stddev + mean;
            n2_cached = 1;
            return result;
        }
    }
    else
    {
        n2_cached = 0;
        return n2*stddev + mean;
    }
}

int main() {
	srand(time(NULL)); // init random
	
	int x_min = -1;
	int y_min = -1;

	// generate list of n rooms within a circle of radius r using normal distrib for size
	room_t* rooms = new room_t[param_n_rooms];
	for (int i = 0; i < param_n_rooms; i++){
		// pick x, y inside radius
		float t = 2 * PI * rand_n();
		float u = rand_n() + rand_n();
		float r = ((u > 1) ? 2 - u : u);
		
		rooms[i].x = param_radius * r * cos(t);
		rooms[i].y = param_radius * r * sin(t);
		
		// pick w, h from distrib
		rooms[i].w = rand_normal(10, 1.5);
		rooms[i].h = rand_normal(10, 1.5);
		
		// give it an id
		rooms[i].id_self = global_uuid_idx++; 
		
		rooms[i].fixed = false;
		rooms[i].n1 = -1;
		rooms[i].n2 = -1;
		rooms[i].n3 = -1;
		
		// update grid bounds
		if (rooms[i].x < x_min) x_min = rooms[i].x;
		if (rooms[i].y < y_min) y_min = rooms[i].y;
	}
	
	// seperate all rooms from each other (seperation steering alg)
	bool seperated = false;
	int cycles = 1;
	while (!seperated) {
		// pick a random room
		int idx = rand() % param_n_rooms;
		while (rooms[idx].fixed) {
			idx = rand() % param_n_rooms;
		}
		
		// pick random direction (float, float)
		float dx = rand_n() * ((rand() % 2) ? -1 : 1);
		float dy = rand_n() * ((rand() % 2) ? -1 : 1);
		
		// for all unfixed rooms
		for (int i = 0; i < param_n_rooms; i++){		
			// wile it overlaps that room)
			if (i == idx) continue;
			
			while (!(rooms[idx].x > rooms[i].x + rooms[i].w ||
				rooms[i].x > rooms[idx].x + rooms[idx].w ||
				rooms[idx].y > rooms[i].y + rooms[i].h ||
				rooms[i].y > rooms[idx].y + rooms[idx].h)) {
				// move it in direction until is does`
				rooms[idx].x += dx;
				rooms[idx].y += dy;
				
				//std::cout << rooms[idx].x << "," << rooms[idx].y << " | " << dx << "," << dy << " | " << i << " | " << cycles << "\r";
				//std::cout << "[" << rooms[idx].x << "," << rooms[idx].y << "," << rooms[idx].w << "," << rooms[idx].h << "] ";
				//std::cout << "[" << rooms[i].x << "," << rooms[i].y << "," << rooms[i].w << "," << rooms[i].h << "] ";
				//std::cout << "i:" << i << " c:" << cycles << "           \r";
			}		
		}
		
		// mark it as fixed
		rooms[idx].fixed = true;
			
		// if no unfixed rooms
		bool escape = true;
		for (int i = 0; i < param_n_rooms; i++){
			if (!rooms[i].fixed) {
				// algorithm complete
				escape = false;
				break;
			}
		}

		if (escape) break;
		
		cycles++;
	}
	
	int num_main_rooms = 0;
	room_t* main_rooms = new room_t[param_n_rooms];
	for (int i = 0; i < param_n_rooms; i++) {
		// snap to grid
		rooms[i].x = round(rooms[i].x);
		rooms[i].y = round(rooms[i].y);
	
		// add by threshold
		if (rooms[i].w > 8 && rooms[i].h > 8) 
			main_rooms[num_main_rooms++] = rooms[i];
	}
	
	{ // array resize hack
		room_t* n = new room_t[num_main_rooms];
		memcpy(n, main_rooms, num_main_rooms * sizeof(room_t));
		delete[] main_rooms;
		main_rooms = n;
	}
	
	// link rooms together
	int top_link_idx = 0;
	graph_edge_t * links = new graph_edge_t[num_main_rooms * num_main_rooms]; // i hope this is enough
	for (int i = 0; i < num_main_rooms; i++){
		// if room i has no free slots skip it
		if (main_rooms[i].n1 != -1 &&
			main_rooms[i].n2 != -1 && 
			main_rooms[i].n3 != -1) continue;
			
		int id1 = main_rooms[i].n1; float d1 = -1;
		int id2 = main_rooms[i].n2; float d2 = -1;
		//int n3 = rooms[i].n3; float d3 = -1;
		
		for (int j = 0; j < param_n_rooms; j++){
			if (j == i) continue; // this would be buggy
			
			// if room j has no free slots skip it
			if (main_rooms[j].n1 != -1 &&
				main_rooms[j].n2 != -1 && 
				main_rooms[j].n3 != -1) continue;
				
			float dist = sqrt( pow(main_rooms[i].x - main_rooms[j].x, 2) * pow(main_rooms[i].y - main_rooms[j].y, 2) );
			
			if (dist < d1) {
				d1 = dist;
				id1 = j;
			}	
		}
		
		// assign link for id1
		if (main_rooms[i].n1 == -1) {
			main_rooms[i].n1 = id1; // set room i to be linked with id1
			if (main_rooms[id1].n1 == -1) {
				main_rooms[id1].n1 = i;
			} else if (main_rooms[id1].n2 == -1) {
				main_rooms[id1].n2 = i;
			} else if (main_rooms[id1].n3 == -1) {
				main_rooms[id1].n3 = i;
			}
		} else if (main_rooms[i].n2 == -1) {
			main_rooms[i].n2 = id1; // set room i to be linked with id1
			if (main_rooms[id1].n1 == -1) {
				main_rooms[id1].n1 = i;
			} else if (main_rooms[id1].n2 == -1) {
				main_rooms[id1].n2 = i;
			} else if (main_rooms[id1].n3 == -1) {
				main_rooms[id1].n3 = i;
			}			
		} else if (main_rooms[i].n3 == -1) {
			main_rooms[i].n3 = id1; // set room i to be linked with id1
			if (main_rooms[id1].n1 == -1) {
				main_rooms[id1].n1 = i;
			} else if (main_rooms[id1].n2 == -1) {
				main_rooms[id1].n2 = i;
			} else if (main_rooms[id1].n3 == -1) {
				main_rooms[id1].n3 = i;
			}
		}
		
		//graph_edge_t g1 = {global_uuid_idx++, i, id1, d1};
		links[top_link_idx++] = {global_uuid_idx++, i, id1, d1};
		if (main_rooms[i].n3 > 0) continue;
		
		for (int j = 0; j < param_n_rooms; j++){
			if (j == i) continue; // this would be buggy
			if (j == id1) continue; // if this is prior room skip it too
			
			// if room j has no free slots skip it
			if (main_rooms[j].n1 != -1 &&
				main_rooms[j].n2 != -1 && 
				main_rooms[j].n3 != -1) continue;
				
			float dist = sqrt( pow(main_rooms[i].x - main_rooms[j].x, 2) * pow(main_rooms[i].y - main_rooms[j].y, 2) );
			
			if (dist < d2) {
				d2 = dist;
				id2 = j;
			}	
		}
		
		// assign link for id2	
		if (main_rooms[i].n1 == -1) {
			main_rooms[i].n1 = id2; // set room i to be linked with id1
			if (main_rooms[id2].n1 == -1) {
				main_rooms[id2].n1 = i;
			} else if (main_rooms[id2].n2 == -1) {
				main_rooms[id2].n2 = i;
			} else if (main_rooms[id2].n3 == -1) {
				main_rooms[id2].n3 = i;
			}
		} else if (main_rooms[i].n2 == -1) {
			main_rooms[i].n2 = id2; // set room i to be linked with id1
			if (main_rooms[id2].n1 == -1) {
				main_rooms[id2].n1 = i;
			} else if (main_rooms[id2].n2 == -1) {
				main_rooms[id2].n2 = i;
			} else if (main_rooms[id2].n3 == -1) {
				main_rooms[id2].n3 = i;
			}			
		} else if (main_rooms[i].n3 == -1) {
			main_rooms[i].n3 = id2; // set room i to be linked with id1
			if (main_rooms[id2].n1 == -1) {
				main_rooms[id2].n1 = i;
			} else if (main_rooms[id2].n2 == -1) {
				main_rooms[id2].n2 = i;
			} else if (main_rooms[id2].n3 == -1) {
				main_rooms[id2].n3 = i;
			}
		}
		
		links[top_link_idx++] = {global_uuid_idx++, i, id2, d2};
	}
	
	// clean up
	{ // array resize hack : the sequel
		graph_edge_t* n = new graph_edge_t[top_link_idx];
		memcpy(n, links, top_link_idx * sizeof(graph_edge_t));
		delete[] links;
		links = n;
	}
	
	// flatten rooms 
	int * grid = new int[param_width * param_n_rooms];
	for (int i = 0; i < param_width * param_height; i++)
		grid[i] = 0;
	
	for (int i = 0; i < num_main_rooms; i++){
		for (int x = main_rooms[i].x; x < main_rooms[i].x + main_rooms[i].w; x++){
			for (int y = main_rooms[i].y; y < main_rooms[i].y + main_rooms[i].h; y++){
				if (x >= param_width) continue;
				if (y >= param_height) continue;
				
				grid[y * param_width + x] = 1;
			}
		}
	}
	
	std::cout << "t" << std::endl;
	// convert links to horizontal and vertical lines of tiles
	for (int i = 0; i < top_link_idx; i++) {
		int x = main_rooms[links[i].id_target_a].x;
		int y = main_rooms[links[i].id_target_a].y;
		int dx = main_rooms[links[i].id_target_b].x - main_rooms[links[i].id_target_a].x;
		int dy = main_rooms[links[i].id_target_b].y - main_rooms[links[i].id_target_a].y;
		
		for (int j = x; j < x + dx; j++){
			
		}
		
		for (int j = y; y < y + dy; j++){
			
		}
	}
	
	// for all links of tiles, if they intersect any rooms, add the rooms to the current structure of valid tiles

	// print out dungeon
}