//Keoni Yandall
//sb-analyze
//This takes in a static board and implements a program that will read the table, calculate the goal cells, aswell as the size of these scoring sets.
//CS302 (Emrich)
//02-26-2025

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

#include "disjoint.h"

using namespace std;

class Superball {
	public:
		Superball(int argc, char **argv);
		int r;
		int c;
		int mss;
		int empty;
		vector <int> board;
		vector <int> goals;
		vector <int> colors;
};

void usage(const char *s) 
{
	fprintf(stderr, "usage: sb-analyze rows cols min-score-size colors\n");
	if (s != NULL) fprintf(stderr, "%s\n", s);
	exit(1);
}

Superball::Superball(int argc, char **argv)
{
	int i, j;
	string s;

	if (argc != 5) usage(NULL);

	if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
	if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
	if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

	colors.resize(256, 0);

	for (i = 0; i < (int) strlen(argv[4]); i++) {
		if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
		if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
		if (colors[argv[4][i]] != 0) usage("Duplicate color");
		colors[argv[4][i]] = 2+i;
		colors[toupper(argv[4][i])] = 2+i;
	}

	board.resize(r*c);
	goals.resize(r*c, 0);

	empty = 0;

	for (i = 0; i < r; i++) {
		if (!(cin >> s)) {
			fprintf(stderr, "Bad board: not enough rows on standard input\n");
			exit(1);
		}
		if ((int) s.size() != c) {
			fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
			exit(1);
		}
		for (j = 0; j < c; j++) {
			if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
				fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
				exit(1);
			}
			board[i*c+j] = s[j];
			if (board[i*c+j] == '.') empty++;
			if (board[i*c+j] == '*') empty++;
			if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
				goals[i*c+j] = 1;
				board[i*c+j] = tolower(board[i*c+j]);
			}
		}
	}
}

int main(int argc, char **argv)
{
	Superball *s;

	s = new Superball(argc, argv);

	DisjointSetByRankWPC ds(s->r*s->c);
	
	//basic idea is if we always check the right and bottom neighbor, starting at the top left, we will meet all the neighbors
	//making checking left and top redundant
	for (int i = 0; i < s->r; i++) {
		for (int j = 0; j < s->c; j++) {
			int index = i * s->c + j;
			int right_index = i * s->c + (j + 1);
			int bottom_index = (i + 1) * s->c + j;

			//we want to check the element to da right, find their leaders and union magic
			if (j + 1 < s->c && s->board[index] == s->board[right_index]) {
				int s1 = ds.Find(index);
				int s2 = ds.Find(right_index);
				ds.Union(s1, s2);
			}

			//we want to check the element under it, if theyre of same character, we need to make sure to union THEIR LEADERS
			if (i + 1 < s->r && s->board[index] == s->board[bottom_index]) {
				int s1 = ds.Find(index);
				int s2 = ds.Find(bottom_index);
				ds.Union(s1, s2);
			}
		}
	}

	unordered_map<int, int> score_sheet;

	//im using a map to store my sets and their size
	for (int k = 0; k < s->r; k++) {
		for (int l = 0; l < s->c; l++) {
			int index = k * s->c + l;
			int id = ds.Find(index);
			//for each char we find, increment its count
			score_sheet[id]++;
		}
	}
	cout << "Scoring sets:" << endl;
	unordered_map<int, int>::iterator it;

	//use a set for scored for ease
	set<int> scored;
	for (it = score_sheet.begin(); it != score_sheet.end(); it++) {

		//if a sets size even meets the mss
		if (it->second >= s->mss) {

			//for every row except first two and last two
			for (int row = 2; row < s->r - 2; row++) {
				
				//we only want to check 0,1,c-2, and c-1
				for (int col : {0, 1, s->c - 2, s->c - 1}) {  
					int goal_index = row * s->c + col;

					
					//if not scored yet, not a placeholder character (* and .)
					if (ds.Find(goal_index) == it->first && scored.find(it->first) == scored.end() && char(s->board[goal_index]) != '*' && char(s->board[goal_index]) != '.') {
						
						//mark as scored
						scored.insert(it->first);
						
						//output
						cout << "     Size: " << it->second << "  Char: " << char(s->board[goal_index]) 
							<< "  Scoring Cell: " << row << "," << col << endl;
					}
				}
			}
		}
	}
}





