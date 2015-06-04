/*
 * =====================================================================================
 *
 *       Filename:  reverseComplement.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/12/13 14:29:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

unsigned int quality_mode = 64;




int main(int argc, char* argv[]) {
//	if (argc > 1) { cerr << "reverseComplement reads from STDIN" << endl; return 0; }

	vector <char> complement;
	complement.resize(255,255);
	complement['A'] = 'T';
	complement['a'] = 'T';
	complement['C'] = 'G';
	complement['c'] = 'G';
	complement['G'] = 'C';
	complement['g'] = 'C';
	complement['T'] = 'A';
	complement['t'] = 'A';
	complement['N'] = 'N';
	complement['n'] = 'N';

	string first, second;
	string out_second;

	while (cin.good()) { 
		getline(cin,first);
		getline(cin,second);
		if (!cin.good()) { break; }

		if (first[0] == '+') {
			for (string::reverse_iterator i=second.rbegin(); i != second.rend(); i++) { 
				out_second.push_back(*i);
			}
		}
		else { 
			//cout << second << endl;
			for (string::reverse_iterator i=second.rbegin(); i != second.rend(); i++) { 
				//cout << *i;
				out_second.push_back(complement[*i]);
			}
		}
		cout << first << endl << out_second << endl;
		out_second.clear();
	}
}



