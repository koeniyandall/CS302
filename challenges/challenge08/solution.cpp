// Challenge 08: sequence alignment
// keoni yandall
// cs302
// 4/12/25
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[]) {
    string seq1,seq2;
	int mismatch_pen = -1;
	int gap_pen = -1;
	int match_pen = 1;
	int match;
	cin >> seq1 >> seq2;
	vector<vector<int>> board(seq1.length()+1,vector<int>(seq2.length()+1));
	for(size_t i = 0; i < seq1.length(); i++){
		board[0][i] = i * mismatch_pen;
	}
	for(size_t j = 0; j < seq2.length(); j ++){
		board[j][0] = j * mismatch_pen;
	}

	for(size_t row = 1; row <= seq1.length();row++){
		for(size_t col = 1; col <= seq2.length(); col++){
			if(seq1[row - 1] == seq2[col - 1]){
				match = board[row - 1][col - 1] + match_pen;
			}else{
				match = board[row - 1][col - 1] + mismatch_pen;
			}
			int top = board[row -1][col] + gap_pen;
			int left = board[row][col - 1] + gap_pen;
			 board[row][col] = max(match, max(top, left));
		}
	}
	cout << board[seq1.length()][seq2.length()] << endl;

}


