// Challenge 04: Graph Paths
// given a graph on stl, this program determines if theres a path from A to B using adjacency lists and
// Keoni Yandall
// CS302
// 2/17/24
#include <iostream>
#include <map>
#include <string>
#include <set>
#include <vector>
using namespace std;

//global visited
set<string> visited;

bool DFS(string source, string dest, map<string, vector<string>> adjMap){
	//if we found our original dest
	if(source == dest){
		return true;
	//if weve already visited it
	}else if(visited.find(source) != visited.end()){
		return false;	
	}
	
	visited.insert(source);
	
	//for every neigher of source doo DFS
	for(string k: adjMap[source]){
		if(DFS(k , dest, adjMap)){
			return true;
		}
	}
	return false;
}
// Main Execution

int main(int argc, char *argv[]) {
	int NEDGES = 0;
	int NPATHS = 0;
	int graph_count = 1;
	string source, dest;

	//input parsing
	while(cin >> NEDGES){
		if(graph_count != 1){
			cout << "\n";
		}
		map<string, vector<string>> adjMap; 
		for(int i = 0; i < NEDGES; i++){
			cin >> source >> dest;
			adjMap[source].push_back(dest);
		}
		cin >> NPATHS;
		for(int j = 0; j < NPATHS; j++){
			cin >> source >> dest;
			visited.clear();
			
			//returns true if there is a path!
			if(DFS(source, dest, adjMap)){
				cout << "In Graph " << graph_count << " there is a path from " << source << " to " << dest << endl;
			}else{
				cout << "In Graph " << graph_count << " there is no path from " << source << " to " << dest << endl;
			}
		}
		graph_count++;
}
}

