/*
 * =====================================================================================
 *
 *       Filename:  unjoinFasta.cpp
 *
 *    Description: joins fasta lines into one sequence per line or unjoins into X col sequences.
 *
 *        Version:  1.0
 *        Created:  09/04/13 14:56:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <string>
#include <iostream>
#include <stdlib.h>
#include <fstream>

#include <getopt.h>

using namespace std;

//void printUsage(void);

unsigned int num_columns = 80;

int main(int argc, char* argv[]) {
	// Variables for options.
	char c;
	extern char *optarg;

	char *File = NULL;

	bool join = 0;
	bool unjoin = 0;



	// Get the different options necessary to run
	while ((c = getopt (argc, argv, "f:juc:")) != -1) {
		switch(c) {
			case 'c':
				num_columns = atoi(optarg);
				break;
			case 'f':
				File = optarg;
				break;
			case 'j': 
				join = 1;
				break;
			case 'u': 
				unjoin = 1;
				break;
			default:
			//	printUsage();
				return 0;
		}
	}

	if (File == NULL) { 
		cerr << "Usage: -f <fasta file to modify> [-j/u]" << endl;
		return 3;
	}

	ifstream f;
	f.open(File);

	if (!f.is_open()) {
		cerr << "Usage: <fasta file to modify> : " << File << endl;
		return 3;
	}

	while (f.good()) {
		string line;
		getline(f, line);
		if (!f.good()) {
			break;
		}
		if (line[0] == '>') {
			cout << line << endl;
		}
		else {
			if (!unjoin && !join) { 
				if (line.size() > 80) { 
					unjoin = 1;
					cerr << "Unjoining sequences" << endl;
				}
				else { 
					join = 1; 
					cerr << "Joining sequences" << endl;
				}
			}
			if (unjoin) { 
				unsigned long i = 0;
				for (; i < line.size(); i++) { 
					cout << line[i];
					if ((i % 80) == 79) { cout << endl; }
				}
				if ((i % 80) != 0) { cout << endl; } 
			}
			else {
				while (f.good()) {
					if (line[0] == '>') {
						cout << endl << line << endl;
					}
					else { 
						cout << line;
					}
					getline(f, line);
				}
				cout << endl;
			}
		}
	}
}

