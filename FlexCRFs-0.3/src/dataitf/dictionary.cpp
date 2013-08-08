/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * dictionary.cpp - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Oct. 29, 2005
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

#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include "../../include/dictionary.h"
#include "../../include/strtokenizer.h"

using namespace std;

// add a context predicate into the dictionary
void dictionary::add_cp(int cp, int y, int y2order, int cnt) {
    map<int, element>::iterator mapit;
    
    // lookup the context predicate in the map
    mapit = dict.find(cp);
    
    if (mapit == dict.end()) {
	// if the context predicate not found
	element elem;
	elem.count = cnt;
	
	pair<int, int> cnt_fidx(cnt, -1);
	elem.lb_cnt_fidxes.insert(pair<int, pair<int, int> >(y, cnt_fidx));
	
	if (y2order >= 0) {
	    cnt_fidx.first = cnt;
	    cnt_fidx.second = -1;
	    elem.lb2order_cnt_fidxes.insert(pair<int, pair<int, int> >(y2order, cnt_fidx));
	}
	
	// insert the new element to the dict
	dict.insert(pair<int, element>(cp, elem));
    
    } else {
	// the context predicate found, update the count only
	mapit->second.count += cnt;
	
	map<int, pair<int, int> >::iterator lbcntit;
	lbcntit = mapit->second.lb_cnt_fidxes.find(y);
	if (lbcntit == mapit->second.lb_cnt_fidxes.end()) {
	    // the label not found, insert to the map
	    pair<int, int> cnt_fidx(cnt, -1);
	    mapit->second.lb_cnt_fidxes.insert(pair<int, pair<int, int> >(y, cnt_fidx));
	} else {
	    // label found, update the count only
	    lbcntit->second.first += cnt;
	}
	
	// for second-order label
	if (y2order >= 0) {
	    map<int, pair<int, int> >::iterator lbcntit;
	    lbcntit = mapit->second.lb2order_cnt_fidxes.find(y2order);
	    if (lbcntit == mapit->second.lb2order_cnt_fidxes.end()) {
		// the label not found, insert to the map
		pair<int, int> cnt_fidx(cnt, -1);
		mapit->second.lb2order_cnt_fidxes.insert(pair<int, pair<int, int> >(y2order, cnt_fidx));
	    } else {
		// label found, update the count only
		lbcntit->second.first += cnt;
	    }
	}
    }
}

// generating dictionary from the training data
void dictionary::dict_gen() {
    dataset::iterator datait;
    sequence::iterator seqit;
    vector<int>::iterator cpit;
    
    if (!pdata) {
	// no data available
	return;
    }
    
    if (!(pdata->ptrndata)) {
	// no training available
	return;
    }
    
    // scan all data sequences of the training data
    for (datait = pdata->ptrndata->begin(); datait != pdata->ptrndata->end(); datait++) {
	// scan all the observations of the current sequence
	seqit = datait->begin();
	while (seqit != datait->end()) {
	    // scan all context predicates of the current observation
	    for (cpit = (seqit->cps).begin(); cpit != (seqit->cps).end(); cpit++) {
		if (popt->order == FIRST_ORDER) {
		    add_cp(*cpit, seqit->label, -1, 1);
		    
		} else if (popt->order == SECOND_ORDER) {
		    add_cp(*cpit, seqit->label, seqit->label2order, 1);
		}
	    }
	    seqit++;
	}	
    }
    
    // update the number of context predicates
    if (popt) {
	popt->num_cps = dict.size();    
    }
}

// write the dictionary to file
void dictionary::write_dict(FILE * fout) {
    if (dict.size() <= 0) {
	return;
    }
    
    // write dictionary size
    fprintf(fout, "%d\n", dict.size());
    
    map<int, element>::iterator dictit;
    map<int, pair<int, int> >::iterator lbcntit;
    
    // go through all <cp, element> pairs
    for (dictit = dict.begin(); dictit != dict.end(); dictit++) {
	// write the context predicate and its count
	fprintf(fout, "%d:%d", dictit->first, dictit->second.count);
	
	for (lbcntit = dictit->second.lb_cnt_fidxes.begin();
		    lbcntit != dictit->second.lb_cnt_fidxes.end(); lbcntit++) {
	    fprintf(fout, " 1:%d:%d:%d", lbcntit->first, 
		    lbcntit->second.first, lbcntit->second.second);
	}
	
	for (lbcntit = dictit->second.lb2order_cnt_fidxes.begin();
		    lbcntit != dictit->second.lb2order_cnt_fidxes.end(); lbcntit++) {
	    fprintf(fout, " 2:%d:%d:%d", lbcntit->first, 
		    lbcntit->second.first, lbcntit->second.second);
	}

	fprintf(fout, "\n");
    }

    fprintf(fout, "##################################################\n");
}

// read dictionary from file
void dictionary::read_dict(FILE * fin) {
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get dictionary size
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
    int dict_size = atoi(buff);
    
    if (dict_size <= 0) {
	return;
    }
    
    for (int i = 0; i < dict_size; i++) {
	fgets(buff, BUFF_SIZE_LONG - 1, fin);
	line = buff;

	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len < 2) {	// invalid line
	    continue;
	}
	
	strtokenizer cptok(tok.token(0), ":");
	int cp = atoi(cptok.token(0).c_str());
	int cp_count = atoi(cptok.token(1).c_str());
	
	// create the element
	element elem;
	elem.count = cp_count;
	
	for (int j = 1; j < len; j++) {
	    strtokenizer lbtok(tok.token(j), ":");

	    int order = atoi(lbtok.token(0).c_str());	    
	    int label = atoi(lbtok.token(1).c_str());
	    pair<int, int> cnt_fidx;
	    cnt_fidx.first = atoi(lbtok.token(2).c_str());
	    cnt_fidx.second = atoi(lbtok.token(3).c_str());
	    
	    if (order == 1) {
		elem.lb_cnt_fidxes.insert(pair<int, pair<int, int> >(label, cnt_fidx));		
	    } else if (order == 2) {
		elem.lb2order_cnt_fidxes.insert(pair<int, pair<int, int> >(label, cnt_fidx));
	    }	    
	}
	
	// insert the element to the dict
	dict.insert(pair<int, element>(cp, elem));
    }
    
    if (popt) {
	popt->num_cps = dict.size();
    }

    fgets(buff, BUFF_SIZE_LONG - 1, fin);	// read the line ###...
}

