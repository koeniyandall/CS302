//Keoni Yandall
//CS302
//This checks if the input strings are a palindrome by calculating the amount of odd letter occurances
// Challenge 03: Palindrome Permutation
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <cctype>

using namespace std;

int main(int argc, char *argv[]) {
	string phrase;

	while(getline(cin,phrase)){
		unordered_map<char, int> charCount;
		int odd_count = 0;

		//put letters into the map
		for(size_t i = 0; i < phrase.length(); i++){
			
			//ignore spaces and punctuation
			if(phrase[i] == ' ' || !(isalpha(phrase[i]))){
				continue;
			}else{
				//increase count for that char
				charCount[phrase[i]]++;
			}
		}

		//calculating how many letters occur odd
		unordered_map<char,int>::iterator it;
		for(it = charCount.begin(); it != charCount.end(); it++){
			//if theres an odd occurance of a letter
			if((it -> second % 2 == 0)){
				continue;
			}else{
				odd_count++;
		}		
	}

	//in order to be a palindrome a phrase must only have 1 odd letter or none
	if((odd_count == 1)	|| (odd_count == 0)){
		cout <<  "\"" << phrase << "\"" << " is a palindrome permutation" << endl;
	}else{
		cout <<"\"" << phrase << "\"" <<  " is not a palindrome permutation" <<  endl;
	}
}
}

