//Challenge 01 - rotating the array
//Keoni Yandall
//COSC302
//1-22-25
//this function takes input and outputs the input array but rotated by a specified amount left or right.
#include <string>
#include <vector>
#include <iostream>
using namespace std;

// function to rotate the array
vector<int> array_rotation(vector<int> newVec, int n, int r, string decision){
    vector<int> solutionVec(n);
    r = r % n;  
    if (decision == "L") {
        for (int i = 0; i < n; i++) {
            // calculate the offset for left shift
            solutionVec[i] = newVec[(i + r) % n];
        }
    }
    else if (decision == "R") {
        for (int i = 0; i < n; i++) {
            // calculate the offset for right shift
            solutionVec[i] = newVec[(i - r + n) % n];
        }
    }
    return solutionVec;
}

int main() {
    int r, n;
    string decision;
    
    // while we get input
    while (cin >> n >> r >> decision) {
        vector<int> newVec(n);
        vector<int> solutionVec(n);  
        
        // take in the second line (the vector elements)
        for (int i = 0; i < n; i++) {
            cin >> newVec[i];
        }
        
        // call the function to return the rotated vector
        solutionVec = array_rotation(newVec, n, r, decision);
        
        // output the rotated vector
        for (int j = 0; j < n; j++) {
            if (j == n - 1) {
                cout << solutionVec[j];
            }
            else {
                cout << solutionVec[j] << " ";
            }
        }
        cout << "\n";
    }
    return 0;
}

