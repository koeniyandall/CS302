// dijsktras.cpp
#include <iostream>
#include <vector>
#include <pair>
// Main Execution
using namespace std;

int main(int argc, char *argv[]) {
    int nodes, value, rows, columns;
	char letter;
	pair<int,int> start_index;
	pair<int,int> end_index;
	vector<char> chars;
	vector<int> values;
	vector<vector<char,char>> table;
	cin >> nodes;
	for(int i = 0; i < nodes; i++){
		cin >> letter >> value;
		chars[i] = letter;
		values[i] = value;
	}
	cin >> rows >> columns;
	for(int row = 0; j < rows; j++){
		for(int k = column; k < columns; k++){
			cin >> table[row][column];
		}		
	}
	cin >> start_index.first >> start_index.second;
	cin >> end_index.first >> end_index.second;
}

