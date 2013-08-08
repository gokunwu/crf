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
#include <stdlib.h>
#include <stdio.h>
#include "../include/evaluatechunk.h"
#include "../include/strtokenizer.h"

using namespace std;

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
	for (int i = 0; i < len; i++) {
	    ob.push_back(tok.token(i));
	}
	
	seq.push_back(ob);
    }
}

void read_options(ifstream & in, labelset & labels, chunkset & chunks, string & chunktype) {
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
	
	if (optstr == "chunktype") {
	    chunktype = optval;
	    
	} else if (optstr == "chunk") {
	    strtokenizer tok(optval, ":");
	    int len = tok.count_tokens();
	    if (len == 3) {
		vector<string> tags;
		for (int i = 0; i < len; i++) {
		    tags.push_back(tok.token(i));
		}
		chunks.push_back(tags);
	    }

	} else if (optstr == "label") {
	    labels.push_back(optval);	
	}
    }
}


int main_evaluate(int argc, char ** argv) {
    if (argc < 4) {
	cout << "usage: evaluatechunk -o <option file> <data file>" << endl;
	return 0;
    }

    ifstream ioptionf(argv[2]);
    ifstream idataf(argv[3]);

    string chunktype = "IOB2";
    labelset labels;
    chunkset chunks;
    read_options(ioptionf, labels, chunks, chunktype);    

    dataset data;
    read_data(idataf, data);
    
    evaluate(data, chunktype, labels, chunks);
    
    return 0;
}

