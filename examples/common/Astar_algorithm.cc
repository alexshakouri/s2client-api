#include "Astar_algorithm.h"
#include <math.h>
#include <queue>
#include <vector>

Astar_cost::Astar_cost(Point point){
	//TODO: Create exception throwing for x,y
	this->position = point;
	this->cost_g = 0;
	this->cost_h = 0;
	this->cost_f = 0;
	this->parent = NULL;
}
Astar_cost::Astar_cost(){
	this->position.x = 0;
	this->position.y = 0;
	this->cost_g = 0;
	this->cost_h = 0;
	this->cost_f = 0;
	this->parent = NULL;
}

void Astar_cost::insert_parent(std::shared_ptr<Astar_cost> parent){
	this->parent = parent;
}

void Astar_cost::calculate_cost_f(){
	this->cost_f = this->cost_g + this->cost_h;
}

void Astar_cost::calculate_cost_g(double parent_cost_g, double distance_cost){
	//Add penalty for diagonal similar to distance
	if( (abs(this->position.x - this->parent->position.x)) + (abs(this->position.y - this->parent->position.y)) == 2){
		distance_cost = distance_cost * sqrt(2.0);
	}
	this->cost_g = parent_cost_g + distance_cost;
}

void Astar_cost::calculate_cost_h(Point goal){
	//Diagonal Distance for 8 possible moves
	double dx = abs(this->position.x - goal.x); 
	double dy = abs(this->position.y - goal.y);	
	this->cost_h = sqrt((dx*dx) + (dy*dy));
}

//Overload the equal operator
bool operator==(const Point &position1, const Point &position2){
	return ((position1.x == position2.x) && (position1.y == position2.y));
}

bool operator==(const Astar_cost &grid1, const Astar_cost &grid2){
	return ((grid1.get_position() == grid2.get_position())
		       	&& (grid1.get_cost_g() == grid2.get_cost_g()) && (grid1.get_cost_h() == grid2.get_cost_h())	
			&& (grid1.get_cost_f() == grid2.get_cost_f()) && (grid1.get_parent() == grid2.get_parent()));
}

//Overload > operator for the priority queue (get min cost_f on top)
bool compare_cost_f::operator()(const std::shared_ptr<Astar_cost> &grid1, const std::shared_ptr<Astar_cost> &grid2){
	return (grid1->get_cost_f() > grid2->get_cost_f());
}

std::shared_ptr<Astar_cost> minimum_cost_f(std::stack<std::shared_ptr<Astar_cost>> list){
	std::shared_ptr<Astar_cost> min_f = list.top();
	
	list.pop();
	while(!list.empty()){	
		if((list.top())->get_cost_f() < min_f->get_cost_f()){
			min_f = (list.top());
		}
		list.pop();
	}	
	return min_f;
}

void delete_element_stack(std::stack<std::shared_ptr<Astar_cost>> &list, Astar_cost element){
	std::stack<std::shared_ptr<Astar_cost>> save_list;

	while(!list.empty()){ 
		//TODO: Create error checks here
		if(*list.top() == element){
			list.pop();
			break;
		}	
		save_list.push(list.top());
		list.pop();
	}
	//Put the elements back into the list
	while(!save_list.empty()){
		list.push(save_list.top());
		save_list.pop();
	}
}

bool find_lower_cost_f(std::stack<std::shared_ptr<Astar_cost>> list, std::shared_ptr<Astar_cost> element){
	while(!list.empty()){
		if((list.top())->get_position() == element->get_position()){
			//Assume there can bever be multiple elements in the list that have the same (x,y)
			return ((list.top())->get_cost_f() <= element->get_cost_f());
		}
		list.pop();
	}
	return false;
}

bool find_lower_cost_f(std::priority_queue<std::shared_ptr<Astar_cost>, std::vector<std::shared_ptr<Astar_cost>>, compare_cost_f> list, std::shared_ptr<Astar_cost> element){
	while(!list.empty()){
		if((list.top())->get_position() == element->get_position()){
			//Assume there can bever be multiple elements in the list that have the same (x,y)
			return ((list.top())->get_cost_f() <= element->get_cost_f());
		}
		list.pop();
	}
	return false;
}

/*
 * A* Search Algorithm
1.  Initialize the open list
2.  Initialize the closed list
    put the starting node on the open list

3.  while the open list is not empty
	a) find the node with the least f on the open list, call it "q"
	b) pop q off the open list
		         
	c) generate q's 8 successors (skip parent) and set their parents to q (skip the invalid successors)
	     
	d) for each valid successor
		i) if successor is the goal, stop search
		successor.g = q.g + distance between successor and q
		successor.h = distance from goal to successor (euclidean distance)
		successor.f = successor.g + successor.h

		ii) if a node with the same position as successor is in the OPEN list which has a lower f than successor, skip this successor
 		iii) if a node with the same position as successor is in the CLOSED list which has a lower f than successor, skip this successor
		otherwise, add  the node to the open list
	end (for loop)
	e) push q on the closed list
     end (while loop) 
 * 
*/
std::shared_ptr<Astar_cost> Astar_algorithm(Point start_point, Point goal_point){
	std::shared_ptr<Astar_cost> start_position(new Astar_cost(start_point));

	//std::priority_queue<std::shared_ptr<Astar_cost>, std::vector<std::shared_ptr<Astar_cost>>, compare_cost_f> open_list; 
	std::stack<std::shared_ptr<Astar_cost>> open_list;
	std::stack<std::shared_ptr<Astar_cost>> closed_list;

	open_list.push(start_position);

	std::shared_ptr<Astar_cost> current_position;
	while(!open_list.empty()){
		//Find the minimum F in the open stack
		current_position = minimum_cost_f(open_list);
		delete_element_stack(open_list, *current_position);
		//current_position = open_list.top();
		//open_list.pop();

		//Generate successor paths
		Point successor_point;
		for(int x = -1; x < 2; x++){
			for(int y = -1; y < 2; y++){
				if(x == 0 && y == 0){
					continue;
				}
				successor_point.x = x + current_position->get_position_x();
				successor_point.y = y + current_position->get_position_y();	

				//Skip parent node position
				if(current_position->get_parent() != NULL){
						if((current_position->get_parent())->get_position() == successor_point){
							continue;
						}
				}
				//Need values that are within the GRID_SIZE
				//if(grid1->not_within_grid(successor_point)){
				//	continue;
				//}

				//Skip blockages in the path
				//if(grid1->within_blockage(successor_point)){
				//	continue;
				//}

				//Create successor point for valid point and set parent to current position
				std::shared_ptr<Astar_cost> successor_position(new Astar_cost(successor_point));
				successor_position->insert_parent(current_position);

				//Stop search if we are at the end
				if(successor_point == goal_point){
					return successor_position;
				}

				successor_position->calculate_cost_g(current_position->get_cost_g(), COST_PER_MOVE);
				successor_position->calculate_cost_h(goal_point);
				successor_position->calculate_cost_f();

				//Skip all the paths where there is already a lower cost path to that point
				if(find_lower_cost_f(open_list, successor_position) || find_lower_cost_f(closed_list, successor_position)){
					continue;
				}

				open_list.push(successor_position);
			}
		}
		//After going through all the successors push current node to closed_list
		closed_list.push(current_position);
	}
	std::cout << "Unable to find path to goal" << std::endl;
	return NULL;
}


