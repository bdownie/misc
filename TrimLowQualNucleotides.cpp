//============================================================================
// Name        : TrimLowQualNucleotides
// Author      : Bryan Downie
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <omp.h>

#include <getopt.h>

#define PROGRAM_NAME "TrimLowQualNucleotides"
#define MAX_BUFFER_SIZE 100000

using namespace std;

// Function declarations here
void printUsage();

class fastq_sequence {
		string accession;
		string sequence;
		string quality;
		int minimum_quality;
		unsigned int minimum_length;
		vector<char> complement;
		bool filtered;
	public:
	/*
		fastq_sequence(string acc, string s, string q, unsigned int m, unsigned int l) { 
			accession = acc; 
			sequence = s; 
			quality = q; 
			minimum_quality = m;
			minimum_length = l;
		};
		*/
		fastq_sequence(void);
		void reverse_complement(void);
		void set_fastq_sequence(string , string , string , int , unsigned int );
		void set_sequence (string seq, string qual) { sequence = seq; quality = qual; };
		void set_minimum_quality(int i) {minimum_quality = i; };
		void set_accession(string a) { accession = a; }
		void process_quality(int, float, int);
		bool get_filtered(void) { return filtered; }
		string get_fastq_sequence(void);
};

void fastq_sequence::reverse_complement (void) {
	string seq;
	string qual;

	for (string::reverse_iterator i = sequence.rbegin(); i != sequence.rend(); i++) {
		char c = complement[(*i)];
		seq.push_back(c);
	}
	for (string::reverse_iterator i = quality.rbegin(); i != quality.rend(); i++) {
		qual.push_back(*i);
	}
	sequence = seq;
	quality = qual;
}


fastq_sequence::fastq_sequence (void) {
	complement.resize(255, 255);

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
	filtered = 0;
}

class fastq_buffer {
		vector <fastq_sequence> buffer;
	public:
		void push_back(fastq_sequence s) { buffer.push_back(s); };
		void init(void) { buffer.clear(); };
		void process_buffer(int, float, int, bool);
		string output_buffer(void);
};


void fastq_sequence::set_fastq_sequence(string acc, string s, string q, int m, unsigned int l) { 
	accession = acc; 
	sequence = s; 
	quality = q; 
	minimum_quality = m;
	minimum_length = l;
}

void fastq_sequence::process_quality (int cm, float ratio, int rq) { 
	string::iterator s;
	string::iterator q;
	string::reverse_iterator revq;
	unsigned int size = sequence.size();
	unsigned int good_count = 0;

	for (revq = quality.rbegin(); revq != quality.rend(); revq++, size--) {
		if (*revq > minimum_quality) { 
			break;
		}
	}

	sequence.resize(size);
	quality.resize(size);

	for (s = sequence.begin(), q = quality.begin(); s != sequence.end(); s++, q++) {
		if (*q >= rq) {
			good_count++;
		}
		if (cm) { 
			*q = *q + cm; 
		}
	}
	if ((((float)good_count / size) < ratio) || (size < minimum_length)) { 
		sequence.clear();
		quality.clear();
		sequence.resize(10, 'N');
		minimum_quality += cm;
		quality.resize(10, minimum_quality - 1);
		filtered = 1;
	}
/*
	for (s = sequence.begin(), q = quality.begin(); s != sequence.end(); s++, q++, size++) {
		if ((*q < minimum_quality) && (*(q+1) < minimum_quality)) {
		//if (*q < minimum_quality) { 
			if (minimum_length <= size) { 
				sequence.resize(size);
				quality.resize(size);
			}
			else {
				sequence.clear();
				quality.clear();
		//		if (minimum_length == 0) { minimum_length = 10; }
				sequence.resize(10, 'N');
				minimum_quality += cm;
				quality.resize(10, minimum_quality - 1);
			}
			break;
		}
		else if (*q >= rq) {
			good_count++;
		}
		if (cm) { 
			*q = *q + cm; 
		}
	}
	//float tmp = (float) good_count /size;
	if (((float)good_count / size) < ratio) { 
		//if (minimum_length == 0) { minimum_length = 10; }
		sequence.clear();
		quality.clear();
		sequence.resize(10, 'N');
		minimum_quality += cm;
		quality.resize(10, minimum_quality - 1);
	}
	*/
}

string fastq_sequence::get_fastq_sequence (void) {
	string return_string;
	return_string = accession;
	return_string.append("\n");
	return_string.append(sequence);
	return_string.append("\n");
	return_string.append("+\n");
	return_string.append(quality);
	return_string.append("\n");

	return return_string;
}

void fastq_buffer::process_buffer(int cm, float ratio, int rq, bool rev_comp) { 
	//vector <fastq_sequence>::iterator i;

	#pragma omp parallel for schedule (dynamic,1000)
	//for (i = buffer.begin(); i != buffer.end(); i++) { 
	for (unsigned int i = 0; i < buffer.size(); i++) { 
		buffer[i].process_quality(cm, ratio, rq);
		if (rev_comp) { buffer[i].reverse_complement(); }
	}
}

string fastq_buffer::output_buffer(void) {
	vector<fastq_sequence>::iterator i;
	vector<fastq_sequence>::iterator i2;
	string output;
//	int j = 1;
	for (i = buffer.begin(); i != buffer.end(); i++) { 
		//if (rn) { 
		//	stringstream tmp;
		//	string renumber_string;
		//	tmp << "@" << rn;
		//	tmp >> renumber_string;
		//	(*i).set_accession(renumber_string);

			//j++;
		//}
		output.append((*i).get_fastq_sequence());
	}
	return output;
}

int main(int argc, char* argv[]) {
	char c;
	char *FastQFile = NULL;
	int quality = 67;
	unsigned int quality_mode = 64;
	unsigned int min_length = 0;
	int change_mode = 0;
	//char *SeqFile = NULL;
	bool paired_end = 0;

	unsigned int ratio_int = 100;
	unsigned int ratio_quality = 0;
	float ratio = 1;
	unsigned long renumber = 0;
	bool reverse_complement = 0;

	// All purpose index variables
	//unsigned int i, j; 
	//unsigned long numLines = 0;
	//int lastSize = 0;

	// Get the different options necessary to run
	while ((c = getopt (argc, argv, "hvncm:pl:q:f:r:t:")) != -1) {
		switch(c) {
			case 'f':
				FastQFile = optarg;
				break;
			case 'p':
				paired_end = 1;
				break;
			case 'v':
				reverse_complement = 1;
				break;
			case 'c':
				change_mode = 1;
				break;
			case 'q':
				quality = atoi(optarg);
				break;
			case 'r':
				ratio_int = atoi(optarg);
				break;
			case 't':
				ratio_quality = atoi(optarg);
				break;
			case 'm':
				quality_mode = atoi(optarg);
				if ((quality_mode != 33) && (quality_mode != 64)) {
					cerr << "Bad quality mode (must be 33 or 64)" << endl;
					return 0;
				}
				break;
			case 'l':
				min_length = atoi(optarg);
				break;
			case 'n':
				renumber = 1;
				break;
			case 'h':
				printUsage();
				return 0;
			default:
				printUsage();
				return 0;
		}
	}
	quality += quality_mode;
	if (change_mode) {
		if (quality_mode == 33) { change_mode = 31; }
		else { change_mode = -33; }
	}
	if (ratio_quality) { 
		ratio_quality += quality_mode; 
		ratio = (float) ratio_int / 100;
	}
	if (FastQFile == NULL ) { printUsage(); return 0; }


	ifstream fhFastq;
	fhFastq.open(FastQFile);
	if (!fhFastq.good()) { cout << "Couldn't open file " << FastQFile << endl; return 1; }
	unsigned int buffer_size = 0;
	fastq_buffer buffer;
	buffer.init();

	while (fhFastq.good()) {
		string a;
		string s;
		string q;
		string lastacc;
		string qualacc;
		fastq_sequence fastq, last_fastq;
		
		getline(fhFastq, a);
		if (!fhFastq.good()) { break; }
		getline(fhFastq, s);
		getline(fhFastq, qualacc);
		getline(fhFastq, q);
		if (renumber) {
			stringstream tmp;
			string renumber_string;

			tmp << "@" << renumber;
			tmp >> a;

			renumber++;
		}


		fastq.set_fastq_sequence(a,s,q,quality, min_length);

		if (a[0] != '@') { 
			cerr << "Badly formed fastq file after entry: " << endl<< 
				 	 last_fastq.get_fastq_sequence() << endl; 
			cerr << "Current fastq sequence: " << endl << 
					 fastq.get_fastq_sequence() << endl;
			return 2; 
		}
		else { last_fastq = fastq; }
	//	fastq.process_quality(change_mode, ratio, ratio_quality);
	//	string output = fastq.get_fastq_sequence();
		buffer.push_back(fastq);
		buffer_size++;

		if (buffer_size > MAX_BUFFER_SIZE) { 
			
			buffer.process_buffer(change_mode, ratio, ratio_quality, reverse_complement);
//			string output = buffer.output_buffer(&renumber, paired_end);
			string output = buffer.output_buffer();
/*
			if (paired_end) { 
				getline(fhFastq, a);
				if (!fhFastq.good()) { break; }
				getline(fhFastq, s);
				getline(fhFastq, qualacc);
				getline(fhFastq, q);
	
				fastq_sequence fastq(a,s,q,quality, min_length);
				fastq.process_quality(change_mode, ratio, ratio_quality);
				output.append(fastq.get_fastq_sequence());
			}
			*/
			cout << output;
			buffer.init();
			buffer_size = 0;
		}
	}

	buffer.process_buffer(change_mode, ratio, ratio_quality, reverse_complement);
	string output = buffer.output_buffer();
//	string output = buffer.output_buffer(&renumber, paired_end);
	cout << output;
			

	return 0;
}


void printUsage() {
	cout << "Usage:\t" <<  PROGRAM_NAME << endl << endl;
	cout << "-f <fastq file> : input file" << endl;
	cout << "-q <quality score> : trim all nucleotides with lower quality from read end" << endl;
	cout << "-t <ratio threshold> : minimum threshold for ratio (only usable with -r)" << endl;
	cout << "-r <ratio of threshold> : at least <ratio> percent of bases must have quality of at least <threshold>" << endl;
	cout << "-m <mode> : 33 or 64 base quality" << endl;
	cout << "-l <minimum sequence length> : sequences which have less than length are replaced with Ns" << endl;

	cout <<endl << "Flags:" << endl;

	cout << "-v : reverse complement reads" << endl;
	cout << "-c : change quality mode (33 vs 64 base)" << endl;
	cout << "-n : renumber sequences starting from index of 1" << endl;
	cout << "-h : print this help screen" << endl;
	cout << endl;

}

