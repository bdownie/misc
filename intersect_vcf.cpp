/*
 * =====================================================================================
 *
 *       Filename:  intersect_vcf.cpp
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  05/05/14 09:24:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <set>

#include <getopt.h>


using namespace std;

//void printUsage(void);

unsigned int num_columns = 80;

int main(int argc, char* argv[]) {
	// Variables for options.
	char c;
	extern char *optarg;

	char *MasterFile = NULL;
	char *SlaveFile = NULL;



	bool reverse = 0;
	// Get the different options necessary to run
	while ((c = getopt (argc, argv, "m:s:vh")) != -1) {
		switch(c) {
			case 'm':
				MasterFile = optarg;
				break;
			case 's':
				SlaveFile = optarg;
				break;
			case 'v':
				reverse = 1;
				break;
			case 'h':
				cerr << "Usage: -m <file to output sites of> -s <file to intersect> -v" << endl;
				return -1;
			default:
				//printUsage();
				cerr << "Usage: -m <file to output sites of> -s <file to intersect> -v" << endl;
				return -1;
		}
	}

	if (MasterFile == NULL) { 
		cerr << "No master file given!" << endl;
		cerr << "Usage: -m <file to output sites of> -s <file to intersect> -v" << endl;
		return 3;
	}
	if ((MasterFile[0] == '-') && (SlaveFile[0] == '-')) { 
		cerr << "Master and slave can't both read from stdin!" << endl;
		return 5;
	}

//	ifstream s;
//	s.open(SlaveFile);

//	if (!s.is_open()) {
//		cerr << "Couldn't open slave file!" << endl;
//		cerr << "Usage: -m <file to output sites of> -s <file to intersect> -v" << endl;
//		return 4;
//	}
	//for (vector <string>::iterator h_it = OutputBuffer.begin(); h_it != OutputBuffer.end(); h_it++) {
	//	cout << *h_it << endl;
	//}
			
//	vector <string> MasterBuffer;
//	vector <string> OutputBuffer;
	set <string> MasterBufferIndex;
	set <string>::iterator MBI_it;
//	unsigned int i = 0;
	istream *in;
	ifstream slave;
	if (SlaveFile[0] == '-') { 
		in = &cin;
	}
	else { 
		slave.open(SlaveFile);

		if (!slave.is_open()) {
			cerr << "No slave file given!" << endl;
			return 3;
		}
		in = &slave;
	}


	string line;
	while (getline(*in, line)) { 

	//while (s.good()) {
	//	string line;
	//	getline(s, line);
	//	if (!s.good()) {
	//		break;
	//	}
		if (line[0] != '#') {
			string chr, site;
			stringstream tmp;
			tmp << line;
			tmp >> chr;
			tmp >> site;

			chr.append(":");
			chr.append(site);

			MBI_it = MasterBufferIndex.find(chr);
			if (MBI_it != MasterBufferIndex.end()) { 
				cerr << "Duplicate key found for " << chr << " " << site << endl << line << endl;
				return 5;
			}
			MasterBufferIndex.insert(chr);
		}
	}
	slave.close();
	//s.close();
	//


	ifstream master;
	if (MasterFile[0] == '-') { 
		in = &cin;
	}
	else { 
		master.open(MasterFile);

		if (!master.is_open()) {
			cerr << "Couldn't open!" << endl;
			return 3;
		}
		else if (!master.good()) { 
			cerr << "Bad handle!" << MasterFile << endl;
			return 4;
		}

		in = &master;
	}


	while (getline(*in, line)) { 
		//getline(in, line);
		//if (!in.good()) {
		//	break;
		//}
		if (line[0] == '#') {
			cout << line << endl;
		}
		else {
			string chr, site;
			stringstream tmp;
			tmp << line;
			tmp >> chr;
			tmp >> site;

			chr.append(":");
			chr.append(site);

			MBI_it = MasterBufferIndex.find(chr);
			if (!reverse && MBI_it != MasterBufferIndex.end()) { 
				cout << line << endl;
				//OutputBuffer.push_back(line);
				//MasterBuffer[MBI_it->second] = "";
			}
			else if (reverse && MBI_it == MasterBufferIndex.end()) { 
				cout << line << endl;
			}
			//MasterBuffer.push_back(line);
		//	MBI_it = MasterBufferIndex.find(chr);
		//	if (MBI_it != MasterBufferIndex.end()) { 
		//		cerr << "Duplicate key found for " << line << endl;
		//		return 5;
		//	}
		//	MasterBufferIndex.insert(pair<string,unsigned int>(chr,i++));
		}
	}
	master.close();


	return 0;


//	for (vector <string>::iterator mb_it = OutputBuffer.begin(); mb_it != OutputBuffer.end(); mb_it++) {
//		cout << *mb_it << endl;
//	}
}
