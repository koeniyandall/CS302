// Challenge 02: Closest Numbers
// Name:  Keoni Yandall
// netid: kyandall

// Brief description:

//this code take in user input and a vector and finds the minimum absolute difference between two numbers, if another pair as the same difference, the program
//outputs that pair as well

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

// Main Execution

int main(int argc, char *argv[]) {
	int length;
	vector<int> myVec;
	//while we get input
	while(cin >> length){
		myVec.resize(length);
		//take in the vector
		for(int i = 0; i < length; i++){
			cin >> myVec[i];
		}

		//sort it 
		sort(myVec.begin(), myVec.end());
		
		vector<pair<int,int>> pairVec;
		//start the difference as the max
		int min_diff = INT_MAX;

		//find the pair of numbers that as the minimum absolute difference
		for(size_t j = 0; j < myVec.size() - 1; j++){
			for(size_t k = j+1; k < myVec.size(); k++){
				int diff = abs(myVec[j] - myVec[k]);

				//if statements to check if diff for this pair
				if(diff < min_diff){
					min_diff = diff;
					pairVec.clear();
					pairVec.push_back({myVec[j],myVec[k]});
				}else if(min_diff == diff){
					pairVec.push_back({myVec[j], myVec[k]});
				}else{
					continue;
				}
			}

		}

		//outputting the pairs
		for(size_t m = 0; m < pairVec.size(); m++){
			if(m == pairVec.size()-1){
				cout << pairVec[m].first << " " << pairVec[m].second;
			}else{
				cout << pairVec[m].first << " " << pairVec[m].second << " ";
			}
		}
		cout << "\n";
	}
}
