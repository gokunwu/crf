#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <ctype.h>
#include "../../../include/strtokenizer.h"

using namespace std;

typedef vector<string> observation;
typedef vector<observation> sequence;

void get_features(sequence & seq, int i, vector<string> & cps) {
    cps.clear();
    
    int j;
    int len = seq.size();
    
    string cp;
    char buff[100];
    
    if (i < 0 || i > len - 1) {
	return;
    }
    
    // single word
    for (j = -2; j <= 2; j++) {
	if (i + j >= 0 && i + j < len) {
	    if (j == -1 || j == 0) {
		sprintf(buff, "#w:%d:%s", j, seq[i + j][0].c_str());
	    } else {
		sprintf(buff, "w:%d:%s", j, seq[i + j][0].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }

    // two consecutive words
    for (j = -1; j <= 0; j++) {
        if (i + j >= 0 && i + j + 1 < len) {
	    if (j == -1) {
    		sprintf(buff, "#ww:%d:%d:%s:%s", j, j + 1, 
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str());
	    } else {
    		sprintf(buff, "ww:%d:%d:%s:%s", j, j + 1, 
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }

    // single POS tag
    for (j = -2; j <= 2; j++) {
	if (i + j >= 0 && i + j < len) {
	    if (j == -1 || j == 0) {
		sprintf(buff, "#p:%d:%s", j, seq[i + j][1].c_str());
	    } else {
		sprintf(buff, "p:%d:%s", j, seq[i + j][1].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }

    // two consecutive POS tags
    for (j = -2; j <= 1; j++) {
        if (i + j >= 0 && i + j + 1 < len) {
	    if (j == -1) {
    		sprintf(buff, "#pp:%d:%d:%s:%s", j, j + 1, 
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str());
	    } else {
    		sprintf(buff, "pp:%d:%d:%s:%s", j, j + 1, 
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }

    // three consecutive POS tags
    for (j = -2; j <= 0; j++) {
        if (i + j >= 0 && i + j + 2 < len) {
    	    sprintf(buff, "ppp:%d:%d:%d:%s:%s:%s", j, j + 1, j + 2, 
	        seq[i + j][1].c_str(), seq[i + j + 1][1].c_str(), seq[i + j + 2][1].c_str());
	    
	    cp = buff;
	    cps.push_back(cp);
	    
	    if (j == -1) {
		sprintf(buff, "pppw:%d:%d:%d:%d:%s:%s:%s:%s", j, j + 1, j + 2, j + 1,
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str(), seq[i + j + 2][1].c_str(),
		    seq[i + j + 1][0].c_str());
		
		cp = buff;
		cps.push_back(cp);
	    }	    
	}
    }

    // single POS tag and single word
    for (j = -1; j <= 0; j++) {
	if (i + j >= 0 && i + j < len) {
	    if (j == -1 || j == 0) {
		sprintf(buff, "#pw:%d:%d:%s:%s", j, j, seq[i + j][1].c_str(), seq[i + j][0].c_str());
	    } else {
		sprintf(buff, "pw:%d:%d:%s:%s", j, j, seq[i + j][1].c_str(), seq[i + j][0].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }

    // two consecutive POS tags and single word
    for (j = -1; j < 0; j++) {
        if (i + j >= 0 && i + j + 1 < len) {
	    if (j == -1) {
    		sprintf(buff, "#ppw:%d:%d:%d:%s:%s:%s", j, j + 1, j,
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str(), seq[i + j][0].c_str());
	    } else {
    		sprintf(buff, "ppw:%d:%d:%d:%s:%s:%s", j, j + 1, j,
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str(), seq[i + j][0].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);

	    if (j == -1) {
    		sprintf(buff, "#ppw:%d:%d:%d:%s:%s:%s", j, j + 1, j + 1,
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str(), seq[i + j + 1][0].c_str());
	    } else {
    		sprintf(buff, "ppw:%d:%d:%d:%s:%s:%s", j, j + 1, j + 1,
		    seq[i + j][1].c_str(), seq[i + j + 1][1].c_str(), seq[i + j + 1][0].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }

    // two consecutive words and single POS tag
    for (j = -1; j < 0; j++) {
        if (i + j >= 0 && i + j + 1 < len) {
	    if (j == -1) {
    		sprintf(buff, "#pww:%d:%d:%d:%s:%s:%s", j, j + 1, j,
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str(), seq[i + j][1].c_str());
	    } else {
    		sprintf(buff, "pww:%d:%d:%d:%s:%s:%s", j, j + 1, j,
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str(), seq[i + j][1].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);

	    if (j == -1) {
    		sprintf(buff, "#pww:%d:%d:%d:%s:%s:%s", j, j + 1, j + 1,
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str(), seq[i + j + 1][1].c_str());
	    } else {
    		sprintf(buff, "pww:%d:%d:%d:%s:%s:%s", j, j + 1, j + 1,
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str(), seq[i + j + 1][1].c_str());
	    }
	    cp = buff;
	    cps.push_back(cp);
	}
    }
}

int main_chunk_feature(int argc, char ** argv) {
    // usage: ./chunkingfeasel -lbl/-ulb (labeled or unlabeled data) <input file> <output file> [tolower]
    if (argc < 5) {
	cout << "usage: ./chunkingfeasel -lbl/-ulb (labeled or unlabeled data) <input file> <output file> [tolower]" << endl;
	return 0;
    }
    
    ifstream idataf(argv[2]);
    ofstream odataf(argv[3]);
    int to_lower = !strcmp(argv[4], "tolower");
    int has_label = !strcmp(argv[1], "-lbl");
    
    vector<string> cps;
    sequence seq;
    char buff[512];
    string line;
    while (getline(idataf, line)) {
	if (to_lower) {
	    strcpy(buff, line.c_str());
	    for (int i = 0; i < strlen(buff); i++) {
		buff[i] = tolower(buff[i]);
	    }
	    line = buff;
	}
	
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		for (int i = 0; i < seq.size(); i++) {
		    get_features(seq, i, cps);
		    for (int j = 0; j < cps.size(); j++) {
			odataf << cps[j] << " ";
		    }	
		    if (has_label && seq[i].size() > 1) {
			odataf << seq[i][seq[i].size() - 1];
		    }
		    odataf << endl;
		}
		odataf << endl;
	    }
	    
	    seq.clear();
	    continue;
	}
	
	observation obsr;
	for (int i = 0; i < len; i++) {
	    obsr.push_back(tok.token(i));
	}
	
	seq.push_back(obsr);
    }

    return 0;
}

