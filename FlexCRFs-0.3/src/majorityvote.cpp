/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * convertchk.cpp - this file is part of FlexCRFs.
 *
 * Begin:	Nov. 27, 2005
 * Last change:	Nov. 27, 2005
 *
 * FlexCRFs is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * FlexCRFs is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FlexCRFs; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include "../include/strtokenizer.h"

using namespace std;

typedef vector<string> obsr;
typedef vector<obsr> sequence;
typedef vector<sequence> dataset;
typedef vector<dataset> votingdatasets;

typedef vector<vector<double> > votingseq;

class option {
public:
    vector<string> labels;    
    map<string, int> lbstr2int;
    map<int, string> lbint2str;

    vector<vector<string> > prevfixedstrlabels;
    vector<vector<string> > nextfixedstrlabels;
    
    vector<string> files;
    vector<double> weights;

    vector<vector<double> > transitions;
    
    option(char * optionfile) {
	ifstream in(optionfile);
	read_and_parse(in);
	preprocess();
    }    
    
    void read_and_parse(ifstream & in);
    void preprocess();
};

void option::read_and_parse(ifstream & in) {
    string line;
    
    while (getline(in, line)) {
	// find '#' character: if found, the current line is a comment
	int i = 0;
	while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
	    i++;
	}	
	if (i < line.size() && line[i] == '#') {
	    continue;
	}	
	
	strtokenizer tok(line, "= \t\r\n");
	int len = tok.count_tokens();
	if (len != 2) {
	    // invalid, ignore this line
	    continue;
	}
	
	string optstr = tok.next_token();
	string optval = tok.next_token();
	
	if (optstr == "label") {
	    labels.push_back(optval);
	
	} else if (optstr == "prevfixedlabels") {
	    // examples:
	    // for IOB2: prevfixedlabels=b-np:i-np|i-np
	    // for IOB1: prevfixedlabels=b-np:i-np|b-np

	    strtokenizer tok(optval, "|");
	    if (tok.count_tokens() == 2) {
		vector<string> cnt;
		
		cnt.push_back(tok.token(1));
		
		strtokenizer tok1(tok.token(0), ":");
		for (int count = 0; count < tok1.count_tokens(); count++) {
		    cnt.push_back(tok1.token(count));
		}
		
		prevfixedstrlabels.push_back(cnt);
	    }
	    
	} else if (optstr == "nextfixedlabels") {
	    // examples:
	    // for IOE2: nextfixedlabels=i-np|i-np:e-np
	    // for IOE1: nextfixedlabels=e-np|i-np:e-np

	    strtokenizer tok(optval, "|");
	    if (tok.count_tokens() == 2) {
		vector<string> cnt;
		
		cnt.push_back(tok.token(0));
		
		strtokenizer tok1(tok.token(1), ":");
		for (int count = 0; count < tok1.count_tokens(); count++) {
		    cnt.push_back(tok1.token(count));
		}
		
		nextfixedstrlabels.push_back(cnt);
	    }

	} else if (optstr == "file") {
	    strtokenizer tok(optval, ":");
	    if (tok.count_tokens() == 2) {
		weights.push_back(atof(tok.token(0).c_str()));
		files.push_back(tok.token(1));
	    }
	}
    }
}

void option::preprocess() {
    for (int i = 0; i < labels.size(); i++) {
	lbstr2int.insert(pair<string, int>(labels[i], i));
	lbint2str.insert(pair<int, string>(i, labels[i]));
    }
    
    for (int i = 0; i < labels.size(); i++) {
	vector<double> row;
	for (int j = 0; j < labels.size(); j++) {
	    row.push_back(1);
	}
	transitions.push_back(row);
    }
    
    map<string, int>::iterator lbit;
    
    for (int i = 0; i < prevfixedstrlabels.size(); i++) {
	string colstr = prevfixedstrlabels[i][0];
	lbit = lbstr2int.find(colstr);
	if (lbit == lbstr2int.end()) {
	    continue;
	}
	int col = lbit->second;
	
	for (int row = 0; row < labels.size(); row++) {
	    int in = 0;
	    for (int j = 1; j < prevfixedstrlabels[i].size(); j++) {
		lbit = lbstr2int.find(prevfixedstrlabels[i][j]);
		if (lbit != lbstr2int.end()) {
		    if (lbit->second == row) {
			in = 1;
		    }
		}
    	    }
	    if (!in) {
		transitions[row][col] = 0;
	    }
	}
    }

    for (int i = 0; i < nextfixedstrlabels.size(); i++) {
	string rowstr = nextfixedstrlabels[i][0];
	lbit = lbstr2int.find(rowstr);
	if (lbit == lbstr2int.end()) {
	    continue;
	}
	int row = lbit->second;
	
	for (int col = 0; col < labels.size(); col++) {
	    int in = 0;
	    for (int j = 1; j < nextfixedstrlabels[i].size(); j++) {
		lbit = lbstr2int.find(nextfixedstrlabels[i][j]);
		if (lbit != lbstr2int.end()) {
		    if (lbit->second == col) {
			in = 1;
		    }
		}
    	    }
	    if (!in) {
		transitions[row][col] = 0;
	    }
	}
    }
}

class voting {
public:
    vector<pair<double, int> > temp;
    vector<vector<pair<double, int> > > memory;
    
    option * popt;
    
    voting(option * popt) {
	this->popt = popt;
	
	for (int i = 0; i < popt->labels.size(); i++) {
	    temp.push_back(pair<double, int>(0.0, -1));
	}
    }
    
    void vote(votingseq & vseq, vector<string> & vlabels);
};

void init_vseq(votingseq & vseq, int seq_len, int num_labels) {
    vseq.clear();
    for (int i = 0; i < seq_len; i++) {
	vector<double> obsr;
	for (int j = 0; j < num_labels; j++) {
	    obsr.push_back(0);
	}
	vseq.push_back(obsr);
    }
}

void voting::vote(votingseq & vseq, vector<string> & vlabels) {
    int i, j, k;

    int seq_len = vseq.size();
    if (seq_len == 0) {
	return;
    }
    
    vlabels.clear();
    for (int i = 0; i < seq_len; i++) {
	string label = "";
	vlabels.push_back(label);
    }
    
    int num_labels = popt->labels.size();
    
    int memorysize = memory.size();
    for (i = 0; i < seq_len - memorysize; i++) {
	memory.push_back(temp);
    }
    
    for (j = 0; j < num_labels; j++) {
	memory[0][j].first = vseq[0][j];
	memory[0][j].second = j;
    }
    
    for (i = 1; i < seq_len; i++) {
	for (j = 0; j < num_labels; j++) {
	    memory[i][j].first = 0.0;
	    memory[i][j].second = 0;
	    
	    for (k = 0; k < num_labels; k++) {
		double tempval = memory[i - 1][k].first * popt->transitions[k][j] * vseq[i][j];
		
		if (tempval > memory[i][j].first) {
		    memory[i][j].first = tempval;
		    memory[i][j].second = k;
		}
	    }
	}    
    }
    
    int max_idx = 0;
    double max_val = -1;
    for (j = 0; j < num_labels; j++) {
	if (memory[seq_len - 1][j].first > max_val) {
	    max_val = memory[seq_len - 1][j].second;
	    max_idx = j;
	}
    }
    
    map<int, string>::iterator lbit;
    lbit = popt->lbint2str.find(max_idx);
    if (lbit != popt->lbint2str.end()) {
	vlabels[seq_len - 1] = lbit->second;
    }
    for (i = seq_len - 2; i >= 0; i--) {
	lbit = popt->lbint2str.find(memory[i + 1][max_idx].second);
	if (lbit != popt->lbint2str.end()) {
	    vlabels[i] = lbit->second;
	}
	max_idx = memory[i + 1][max_idx].second;
    }
}

void read_data(ifstream & in, dataset & data) {
    string line;
    sequence seq;
    while (getline(in, line)) {
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		data.push_back(seq);
	    }
	    
	    seq.clear();
	    continue;
	}
	
	obsr ob;
	ob.push_back(tok.token(len - 2));
	ob.push_back(tok.token(len - 1));
	ob.push_back(tok.token(len - 4));
	
	seq.push_back(ob);
    }
}

void votingdata(votingdatasets & vdata, voting & vt) {
    if (vdata.size() <= 0) {
	return;
    }

    for (int i = 0; i < vdata[0].size(); i++) {
	// cout << "sequence: " << i << endl;
    
	int seq_len = vdata[0][i].size();
	votingseq vseq;
	init_vseq(vseq, seq_len, vt.popt->labels.size());    
	
	for (int j = 0; j < seq_len; j++) {
	    map<string, int>::iterator lbit;
	    
	    for (int k = 0; k < vdata.size(); k++) {
		lbit = vt.popt->lbstr2int.find(vdata[k][i][j][1]);
		if (lbit != vt.popt->lbstr2int.end()) {
		    vseq[j][lbit->second] += /*atof(vdata[k][i][j][2].c_str()) * */
					     vt.popt->weights[k];
		}
	    }	
	}
	
	vector<string> vlabels;
	vt.vote(vseq, vlabels);

	for (int j = 0; j < seq_len; j++) {
	    cout << vdata[0][i][j][0] << " " << vlabels[j] << endl;
	}	    
	
	cout << endl;
    }
}

//main marjovting
int main_marjovte(int argc, char ** argv) {
    if (argc < 2) {
	cout << "usage: votingweight <option file>" << endl;
	return 0;
    }

    option opt(argv[1]);
    
    votingdatasets vdata;
    
    for (int i = 0; i < opt.files.size(); i++) {
	dataset data;
	vdata.push_back(data);
    }
    
    for (int i = 0; i < opt.files.size(); i++) {
	ifstream in(opt.files[i].c_str());
	read_data(in, vdata[i]);
    }
    
    voting vt(&opt);
    
    votingdata(vdata, vt);
    
    return 0;
}

