/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * feature.h - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Nov. 07, 2005
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

#ifndef	_FEATURE_H
#define	_FEATURE_H

#include <stdio.h>
#include <map>
#include <string>
#include "data.h"

using namespace std;

#define	UNKNOWN_FEATURE	0
#define	EDGE_FEATURE1	1
#define	EDGE_FEATURE2	2
#define	STAT_FEATURE1	3
#define	STAT_FEATURE2	4

/*
    o = {o_1, o_2, ..., o_T}
    s = {s_1, s_2, ..., s_T}

    s_1 --- s_2 --- s_3 --- ... --- s_T
     |       |       |       .       |
     |       |       |       .       |
    o_1     o_2     o_3     ...     o_T  

    A. Edge features:
    - type 1: [s_2] 	 -> [s_3]
    - type 2: [s_1, s_2] -> [s_2, s_3]
    
    B. State features:
    - type 1: [o]	 -> [s_3]
    - type 2: [o]	 -> [s_2, s_3]
*/

// first-order Markov: EGDE_FEATURE1, STAT_FEATURE1
// all kinds of features are for second-order Markov

class feature {
public:
    int ftype;		// feature type
    int idx;		// feature index
    string strid;	// string identifier
    int y, yp;		// the current label, previous label, 
			// and previous label of the previous label
    int cp;		// context predicate
    float val;		// feature value
    double wgt;		// feature weight
    
    feature() {
	ftype = UNKNOWN_FEATURE;
	idx = -1;
	strid = "";
	y = yp = -1;
	cp = -1;
	val = 1;
	wgt = 0.0;
    }
    
    // edge feature (type 1) initialization
    void efeature1_init(int y, int yp) {
	ftype = EDGE_FEATURE1;
	idx = -1;
	this->y = y;
	this->yp = yp;
	cp = -1;
	val = 1;
	wgt = 0.0;
	
	// string identifier = "e1_<current label>_<previous label>"
	char buff[100];
	sprintf(buff, "e1_%d_%d", y, yp);
	strid = buff;
    }
    
    void efeature1_init(int y, int yp, map<string, int> & fmap) {
	efeature1_init(y, yp);
	strid2idx_add(fmap);
    }
    
    // edge feature (type 2) initialization
    void efeature2_init(int y, int yp) {
	ftype = EDGE_FEATURE2;
	idx = -1;
	this->y = y;
	this->yp = yp;
	cp = -1;
	val = 1;
	wgt = 0.0;
	
	// string identifier = "e2_<current label>_<previous label>"
	char buff[100];
	sprintf(buff, "e2_%d_%d", y, yp);
	strid = buff;
    }
    
    void efeature2_init(int y, int yp, map<string, int> & fmap) {
	efeature2_init(y, yp);
	strid2idx_add(fmap);
    }    
    
    // state feature (type 1) initialization
    void sfeature1_init(int y, int cp) {
	ftype = STAT_FEATURE1;
	idx = -1;
	this->y = y;
	this->yp = -1;
	this->cp = cp;
	val = 1;
	wgt = 0.0;
	
	// string identifier = "s1_<current label>_<context predicate>"
	char buff[100];
	sprintf(buff, "s1_%d_%d", y, cp);
	strid = buff;
    }
    
    void sfeature1_init(int y, int cp, map<string, int> & fmap) {
	sfeature1_init(y, cp);
	strid2idx_add(fmap);
    }

    // state feature (type 2) initialization
    void sfeature2_init(int y, int cp) {
	ftype = STAT_FEATURE2;
	idx = -1;
	this->y = y;
	this->yp = -1;
	this->cp = cp;
	val = 1;
	wgt = 0.0;
	
	// string identifier = "s2_<current label>_<context predicate>"
	char buff[100];
	sprintf(buff, "s2_%d_%d", y, cp);
	strid = buff;
    }
    
    void sfeature2_init(int y, int cp, map<string, int> & fmap) {
	sfeature2_init(y, cp);
	strid2idx_add(fmap);
    }
    
    // feature constructor that parses an input line
    feature(string str, mapcpstr2int & cps2i, maplbstr2int & lbs2i,
	    maplbstr2int & lb2to1s2i);
    
    // feature constructor that parses an input line (adding to the feature map)
    feature(string str, mapcpstr2int & cps2i, maplbstr2int & lbs2i, 
	    maplbstr2int & lb2to1s2i, map<string, int> & fmap);    
    
    // mapping from string identifier to index
    int strid2idx(map<string, int> & fmap);
    
    // mapping from string identifier to index (adding feature to the map if
    // the mapping does not exist
    int strid2idx_add(map<string, int> & fmap);
    
    // return the index of the feature
    int index() {
	return idx;
    }
    
    // return the index of the feature (lookup the map)
    int index(map<string, int> & fmap) {
	return strid2idx(fmap);
    }
    
    // convert feature to string "<identifier> <index> <weight>"
    string to_string(mapcpint2str & cpi2s, maplbint2str & lbi2s, 
		     maplb2to1order & lb2to1);
};

#endif









