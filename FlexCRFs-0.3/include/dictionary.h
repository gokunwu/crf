/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * dictionary.h - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Oct. 28, 2005
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

#ifndef	_DICTIONARY_H
#define	_DICTIONARY_H

#include <stdio.h>
#include <map>
#include <vector>
#include <string>

#include "data.h"
#include "option.h"
#include "feature.h"

using namespace std;

class element {
public:
    int count;	// the number of occurrences of this context predicate
    int chosen;	// indicating whether or not it is incorporated into the model
    
    // map of labels to pairs <count, feature index>
    map<int, pair<int, int> > lb_cnt_fidxes; 
    // map of second-order label to pairs <count, feature index>
    map<int, pair<int, int> > lb2order_cnt_fidxes;
    
    // storing all features associated with this context predicate
    vector<feature> cpfeatures;
    int is_scanned;
    
    element() {
	count = 0;
	chosen = 0;
	is_scanned = 0;
    }
};


class dictionary {
public:
    map<int, element> dict;
    data * pdata;	// pointer to data object
    option * popt;	// pointer to option object
    
    dictionary(data * pdata = NULL, option * popt = NULL) {
	this->pdata = pdata;
	this->popt = popt;
    }
    
    // add a context predicate into the dictionary
    void add_cp(int cp, int y, int y2order, int cnt);
    
    // generating dictionary from training data
    void dict_gen();
    
    // write dictionary to file
    // each line: <context predicate>:<count> <order>:<label_1>:<count_1>:<fidx_1> ... <order>:<label_i>:<count_i>:<fidx_i>
    // order = 1 if first-order Markov, = 2 if second-order Markov
    void write_dict(FILE * fout);
    
    // read dictionary from file    
    void read_dict(FILE * fin);
};

#endif

