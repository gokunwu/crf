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

void get_features_pos(sequence & seq, int i, vector<string> & cps) {

    cps.clear();
    
    map<string, string>::iterator mapit, fwmapit;
    
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
	    for (int k = 0; k < cp.size(); k++) {
		cp[k] = tolower(cp[k]);
	    }	    
	    cps.push_back(cp);
	}
    }

    // prefixes
    for (j = 0; j <= 0; j++) {
	if (i + j >= 0 && i + j < len) {
	
	    int tokenlen = seq[i + j][0].size();

	    int prefixlen = tokenlen - 2;
	    if (prefixlen > 4) {
		prefixlen = 4;
	    }
	    
	    for (int count = 1; count <= prefixlen; count++) {
		string prefix = seq[i + j][0].substr(0, count);

		if (j == -1 || j == 0) {
		    sprintf(buff, "p:%d:%s", j, prefix.c_str());
		} else {
		    sprintf(buff, "p:%d:%s", j, prefix.c_str());
		}				

		cp = buff;
		cps.push_back(cp);		
	    }
	}
    }

    // suffixes
    for (j = 0; j <= 0; j++) {
	if (i + j >= 0 && i + j < len) {

	    int tokenlen = seq[i + j][0].size();

	    int suffixlen = tokenlen - 2;
	    if (suffixlen > 4) {
		suffixlen = 4;
	    }
	    
	    for (int count = 1; count <= suffixlen; count++) {
		string suffix = seq[i + j][0].substr(tokenlen - count, count);

		if (j == -1 || j == 0) {
		    sprintf(buff, "s:%d:%s", j, suffix.c_str());
		} else {
		    sprintf(buff, "s:%d:%s", j, suffix.c_str());
		}				

		cp = buff;
		cps.push_back(cp);
	    }
	}
    }

    // two consecutive words
    for (j = -1; j <= 0; j++) {
        if (i + j >= 0 && i + j + 1 < len) {
	    if (j == -1) {
    		sprintf(buff, "#w:%d:%d:%s:%s", j, j + 1, 
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str());
	    } else {
    		sprintf(buff, "w:%d:%d:%s:%s", j, j + 1, 
		    seq[i + j][0].c_str(), seq[i + j + 1][0].c_str());
	    }
	    cp = buff;
	    for (int k = 0; k < cp.size(); k++) {
		cp[k] = tolower(cp[k]);
	    }	    
	    cps.push_back(cp);
	}
    }
    
    for (j = 0; j <= 0; j++) {
	if (i + j >= 0 && i + j < len) {

	    int k;
	
	    int toklen = seq[i + j][0].size();
	
	    int is_allcap = 1;
	    k = 0; 
	    while (k < toklen) {
		if (seq[i + j][0][k] < 'A' || seq[i + j][0][k] > 'Z') {
		    is_allcap = 0;
		    break;
		}
		k++;
	    }
	    
	    if (is_allcap) {
		if (j == -1 || j == 0) {
		    sprintf(buff, "#i:%d:allc", j);
		} else {
		    sprintf(buff, "i:%d:allc", j);
		}
		
		cp = buff;
		cps.push_back(cp);
		
		if (seq[i + j][0].substr(seq[i + j][0].size() - 1, 1) == "S") {
		    if (j == -1 || j == 0) {
			sprintf(buff, "#i:%d:allcs", j);
		    } else {
			sprintf(buff, "i:%d:allcs", j);
		    }
		
		    cp = buff;
		    cps.push_back(cp);
		}
	    }

	    if (!is_allcap && seq[i + j][0][0] >= 'A' && seq[i + j][0][0] <= 'Z') {
		if (j == -1 || j == 0) {
		    sprintf(buff, "#i:%d:fstc", j);
		} else {
		    sprintf(buff, "i:%d:fstc", j);
		}		
		cp = buff;
		cps.push_back(cp);
		
		if (i + j == 0 || (i + j > 0 && seq[i + j - 1][0] == "``")) {
		    if (j == -1 || j == 0) {
		        sprintf(buff, "#i:%d:fstsfstc", j);
		    } else {
			sprintf(buff, "i:%d:fstsfstc", j);
		    }		
		    cp = buff;
		    cps.push_back(cp);
		    
		} else {
		    if (j == -1 || j == 0) {
		        sprintf(buff, "#i:%d:nfstsfstc", j);
		    } else {
			sprintf(buff, "i:%d:nfstsfstc", j);
		    }		
		    cp = buff;
		    cps.push_back(cp);		
		}

		if (seq[i + j][0].substr(seq[i + j][0].size() - 1, 1) == "s") {
		    if (j == -1 || j == 0) {
			sprintf(buff, "#i:%d:fstcs", j);
		    } else {
			sprintf(buff, "i:%d:fstcs", j);
		    }		
		    cp = buff;
		    cps.push_back(cp);
		    
		    if (i + j == 0 || (i + j > 0 && seq[i + j - 1][0] == "``")) {
			if (j == -1 || j == 0) {
		    	    sprintf(buff, "#i:%d:fstsfstcs", j);
			} else {
			    sprintf(buff, "i:%d:fstsfstcs", j);
			}		
			cp = buff;
			cps.push_back(cp);
		    
		    } else {
			if (j == -1 || j == 0) {
		    	    sprintf(buff, "#i:%d:nfstsfstcs", j);
			} else {
			    sprintf(buff, "i:%d:nfstsfstcs", j);
			}		
			cp = buff;
			cps.push_back(cp);		
		    }		    
		}
	    }

	    int is_allnumber = 1;
	    k = 0; 
	    while (k < toklen) {
		if (seq[i + j][0][k] < '0' || seq[i + j][0][k] > '9' || 
		    seq[i + j][0][k] == '.' || seq[i + j][0][k] == ',') {
		    is_allnumber = 0;
		    break;
		}
		k++;
	    }

	    if (is_allnumber) {
		if (j == -1 || j == 0) {
		    sprintf(buff, "n:%d:alln", j);
		} else {
		    sprintf(buff, "n:%d:alln", j);
		}		
		cp = buff;
		cps.push_back(cp);
	    }
	    
	    
	    int has_number = 0;
	    k = 0;
	    while (k < toklen) {
		if (seq[i + j][0][k] >= '0' &&  seq[i + j][0][k] <= '9') {
		    has_number = 1;
		    break;
		}
		k++;
	    }
	    if (!is_allnumber && has_number) {
		if (j == -1 || j == 0) {
		    sprintf(buff, "n:%d:hasn", j);
		} else {
		    sprintf(buff, "n:%d:hasn", j);
		}		
		cp = buff;
		cps.push_back(cp);
	    }
    
	    int has_hyphen = 0;
	    k = 0;
	    while (k < toklen) {
		if (seq[i + j][0][k] == '-') {
		    has_hyphen = 1;
		    break;
		}
		k++;
	    }
	    if (has_hyphen) {
		if (j == -1 || j == 0) {
		    sprintf(buff, "h:%d:hyph", j);
		} else {
		    sprintf(buff, "h:%d:hyph", j);
		}		
		cp = buff;
		cps.push_back(cp);
	    }
	    	    
	}
    }
        
}

int main_pos_feature(int argc, char ** argv) {
    // usage: ./postaggingfeatsel -lbl/-ulb (labeled or unlabeled data) <input file> <output file>
    if (argc < 4) {
	cout << "usage: ./postaggingfeatsel -lbl/-ulb (labeled or unlabeled data) <input file> <output file>" << endl;
	return 0;
    }
    
    ifstream idataf(argv[2]);
    ofstream odataf(argv[3]);
    
    int has_label = !strcmp(argv[1], "-lbl");
    
    vector<string> cps;
    sequence seq;
    string line;
    while (getline(idataf, line)) {
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		for (int i = 0; i < seq.size(); i++) {
		    get_features_pos(seq, i, cps);
		    for (int j = 0; j < cps.size(); j++) {
			odataf << cps[j] << " ";
		    }	
		    
		    if (has_label && seq[i].size() > 1) {
			odataf << seq[i][1];
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

