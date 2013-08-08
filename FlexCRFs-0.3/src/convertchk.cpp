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
#include "../include/convertchunk.h"
#include "../include/strtokenizer.h"

using namespace std;

void convert(int is_cap, int is_last, string & src, string & dst, sequence & seq) {
    if (src == "IOB1" && dst == "IOB2") {
	convert_IOB1_IOB2(is_cap, is_last, seq);
    }
    if (src == "IOB1" && dst == "IOE1") {
	convert_IOB1_IOE1(is_cap, is_last, seq);
    }
    if (src == "IOB1" && dst == "IOE2") {
	convert_IOB1_IOE2(is_cap, is_last, seq);
    }

    if (src == "IOB2" && dst == "IOB1") {
	convert_IOB2_IOB1(is_cap, is_last, seq);
    }
    if (src == "IOB2" && dst == "IOE1") {
	convert_IOB2_IOE1(is_cap, is_last, seq);
    }
    if (src == "IOB2" && dst == "IOE2") {
	convert_IOB2_IOE2(is_cap, is_last, seq);
    }
    
    if (src == "IOE1" && dst == "IOB1") {	
	convert_IOE1_IOB1(is_cap, is_last, seq);
    }
    if (src == "IOE1" && dst == "IOB2") {
	convert_IOE1_IOB2(is_cap, is_last, seq);
    }
    if (src == "IOE1" && dst == "IOE2") {
	convert_IOE1_IOE2(is_cap, is_last, seq);
    }
    
    if (src == "IOE2" && dst == "IOB1") {
	convert_IOE2_IOB1(is_cap, is_last, seq);
    }
    if (src == "IOE2" && dst == "IOB2") {
	convert_IOE2_IOB2(is_cap, is_last, seq);
    }
    if (src == "IOE2" && dst == "IOE1") {
	convert_IOE2_IOE1(is_cap, is_last, seq);
    }
}

int main_convert_chunk(int argc, char ** argv) {
    if (argc < 6) {
	cout << "usage: convertchunk -lst/-blst -cap/-lower <source chunk type> <destination chunk type> <input file> <output file>" << endl;
	cout << "<source chunk type> and <destination chunk type> can be one of IOB1, IOB2, IOE1, IOE2" << endl;
	return 0;
    }
    
    int i, j;
    
    ifstream idataf(argv[5]);
    ofstream odataf(argv[6]);
    
    int is_last = 1;
    if (!(strcmp(argv[1], "-blst"))) {
	is_last = 0;
    }
    
    int is_cap = 1;
    if (!(strcmp(argv[2], "-lower"))) {
	is_cap = 0;
    }
    
    string src = argv[3];
    string dst = argv[4];
    
    string line;
    sequence seq;
    while (getline(idataf, line)) {
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		convert(is_cap, is_last, src, dst, seq);
		for (i = 0; i < seq.size(); i++) {
		    for (j = 0; j < seq[i].size() - 1; j++) {
			odataf << seq[i][j] << " ";
		    }
		    odataf << seq[i][seq[i].size() - 1] << endl;
		}
		odataf << endl;
	    }
	    
	    seq.clear();
	    continue;
	}
	
	obsr ob;
	for (i = 0; i < len; i++) {
	    ob.push_back(tok.token(i));
	}
	
	seq.push_back(ob);
    }
    
    return 0;
}

