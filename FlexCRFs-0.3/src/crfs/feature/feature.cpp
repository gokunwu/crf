/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * feature.cpp - this file is part of FlexCRFs.
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

#include <stdio.h>
#include <stdlib.h>
#include "../../../include/feature.h"
#include "../../../include/strtokenizer.h"

using namespace std;

// feature initialization from an input string
feature::feature(string str, mapcpstr2int & cps2i, maplbstr2int & lbs2i, 
		 maplbstr2int & lb2to1s2i) {
		 
    strtokenizer tok(str, " \t\r\n");
    int len = tok.count_tokens();
    
    string stridstr = tok.token(0);
    int idx = atoi(tok.token(1).c_str());
    float val = 1;
    double wgt = atof(tok.token(2).c_str());
    
    // parsing string identifier
    strtokenizer stridtok(stridstr, "_");
    
    mapcpstr2int::iterator cpmapit;
    maplbstr2int::iterator lbmapit;
    
    int y = -1, yp = -1, ypp = -1;
    int cp = -1;
    
    if (stridtok.token(0) == "e1") {
	// edge feature (type 1)
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}

	lbmapit = lbs2i.find(stridtok.token(2));
	if (lbmapit != lbs2i.end()){
	    yp = lbmapit->second;
	}
	
	efeature1_init(y, yp);
    
    } else if (stridtok.token(0) == "e2") {
	// edge feature (type 2)
	// edge feature (type 1)
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}

	lbmapit = lbs2i.find(stridtok.token(2));
	if (lbmapit != lbs2i.end()){
	    yp = lbmapit->second;
	}
	
	lbmapit = lbs2i.find(stridtok.token(3));
	if (lbmapit != lbs2i.end()){
	    ypp = lbmapit->second;
	}
	
	int y2, yp2;
	
	char buff[50];
	sprintf(buff, "%d-%d", yp, y);
	string y2str = buff;
		
	lbmapit = lb2to1s2i.find(y2str);
	if (lbmapit != lb2to1s2i.end()){
	    y2 = lbmapit->second;
	}
	
	sprintf(buff, "%d-%d", ypp, yp);
	string yp2str = buff;

	lbmapit = lb2to1s2i.find(yp2str);
	if (lbmapit != lb2to1s2i.end()){
	    yp2 = lbmapit->second;
	}
	
	efeature2_init(y2, yp2);
    
    } else if (stridtok.token(0) == "s1") {
	// state feature (type 1)	
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}
	
	cpmapit = cps2i.find(stridtok.token(2));
	if (cpmapit != cps2i.end()) {
	    cp = cpmapit->second;
	}
	
	sfeature1_init(y, cp);	
    
    } else if (stridtok.token(0) == "s2") {
	// state feature (type 2)
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}

	lbmapit = lbs2i.find(stridtok.token(2));
	if (lbmapit != lbs2i.end()){
	    yp = lbmapit->second;
	}
	
	cpmapit = cps2i.find(stridtok.token(3));
	if (cpmapit != cps2i.end()) {
	    cp = cpmapit->second;
	}
	
	int y2;
	
	char buff[50];
	sprintf(buff, "%d-%d", yp, y);
	string y2str = buff;
		
	lbmapit = lb2to1s2i.find(y2str);
	if (lbmapit != lb2to1s2i.end()){
	    y2 = lbmapit->second;
	}
	
	sfeature2_init(y2, cp);
    }
    
    this->idx = idx;
    this->val = val;
    this->wgt = wgt;
}

// feature initialization from an input string
feature::feature(string str, mapcpstr2int & cps2i, maplbstr2int & lbs2i,
	maplbstr2int & lb2to1s2i, map<string, int> & fmap) {

    strtokenizer tok(str, " \t\r\n");
    int len = tok.count_tokens();
    
    string stridstr = tok.token(0);
    int idx = atoi(tok.token(1).c_str());
    float val = 1;
    double wgt = atof(tok.token(2).c_str());
    
    // parsing string identifier
    strtokenizer stridtok(stridstr, "_");
    
    mapcpstr2int::iterator cpmapit;
    maplbstr2int::iterator lbmapit;
    
    int y = -1, yp = -1, ypp = -1;
    int cp = -1;
    
    if (stridtok.token(0) == "e1") {
	// edge feature (type 1)
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}

	lbmapit = lbs2i.find(stridtok.token(2));
	if (lbmapit != lbs2i.end()){
	    yp = lbmapit->second;
	}
	
	efeature1_init(y, yp);
    
    } else if (stridtok.token(0) == "e2") {
	// edge feature (type 2)
	// edge feature (type 1)
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}

	lbmapit = lbs2i.find(stridtok.token(2));
	if (lbmapit != lbs2i.end()){
	    yp = lbmapit->second;
	}
	
	lbmapit = lbs2i.find(stridtok.token(3));
	if (lbmapit != lbs2i.end()){
	    ypp = lbmapit->second;
	}
	
	int y2, yp2;
	
	char buff[50];
	sprintf(buff, "%d-%d", yp, y);
	string y2str = buff;
		
	lbmapit = lb2to1s2i.find(y2str);
	if (lbmapit != lb2to1s2i.end()){
	    y2 = lbmapit->second;
	}
	
	sprintf(buff, "%d-%d", ypp, yp);
	string yp2str = buff;

	lbmapit = lb2to1s2i.find(yp2str);
	if (lbmapit != lb2to1s2i.end()){
	    yp2 = lbmapit->second;
	}
	
	efeature2_init(y2, yp2);
    
    } else if (stridtok.token(0) == "s1") {
	// state feature (type 1)	
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}
	
	cpmapit = cps2i.find(stridtok.token(2));
	if (cpmapit != cps2i.end()) {
	    cp = cpmapit->second;
	}
	
	sfeature1_init(y, cp);	
    
    } else if (stridtok.token(0) == "s2") {
	// state feature (type 2)
	lbmapit = lbs2i.find(stridtok.token(1));
	if (lbmapit != lbs2i.end()){
	    y = lbmapit->second;
	}

	lbmapit = lbs2i.find(stridtok.token(2));
	if (lbmapit != lbs2i.end()){
	    yp = lbmapit->second;
	}
	
	cpmapit = cps2i.find(stridtok.token(3));
	if (cpmapit != cps2i.end()) {
	    cp = cpmapit->second;
	}
	
	int y2;
	
	char buff[50];
	sprintf(buff, "%d-%d", yp, y);
	string y2str = buff;
		
	lbmapit = lb2to1s2i.find(y2str);
	if (lbmapit != lb2to1s2i.end()){
	    y2 = lbmapit->second;
	}
	
	sfeature2_init(y2, cp);
    }
    
    this->idx = idx;
    this->val = val;
    this->wgt = wgt;
    
    strid2idx_add(fmap);
}

// mapping from string identifier to index
int feature::strid2idx(map<string, int> & fmap) {
    map<string, int>::iterator it;
    
    it = fmap.find(strid);
    if (it != fmap.end()) {
	// the feature exists in the feature map
	idx = it->second;    
    }
    
    return idx;
}

// mapping from string identifier to index (also add to the 
// map if the mapping does not exist
int feature::strid2idx_add(map<string, int> & fmap) {
    strid2idx(fmap);

    if (idx < 0) {	
	idx = fmap.size();
	fmap.insert(pair<string, int>(strid, idx));
    }
    
    return idx;
}

// string format is "<string idenfifier> <feature index> <feature weight>"
string feature::to_string(mapcpint2str & cpi2s, maplbint2str & lbi2s,
			  maplb2to1order & lb2to1) {
    mapcpint2str::iterator cpmapit;
    maplbint2str::iterator lbmapit;
    maplb2to1order::iterator lb2to1mapit;
    
    string str;

    if (ftype == EDGE_FEATURE1) {
	// edge feature (type 1)
	str = "e1_";
	
	lbmapit = lbi2s.find(y);
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second + "_";
	}
	
	lbmapit = lbi2s.find(yp); 
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second;
	}
    
    } else if (ftype == EDGE_FEATURE2) {
	// edge feature (type 2)
	str = "e2_";
	
	int y1, y1p, y1pp;
	
	lb2to1mapit = lb2to1.find(y);
	if (lb2to1mapit != lb2to1.end()) {
	    y1 = lb2to1mapit->second.second;
	    y1p = lb2to1mapit->second.first;
	}
	
	lb2to1mapit = lb2to1.find(yp);
	if (lb2to1mapit != lb2to1.end()) {
	    y1p = lb2to1mapit->second.second;
	    y1pp = lb2to1mapit->second.first;
	}
	
	lbmapit = lbi2s.find(y1);
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second + "_";
	}

	lbmapit = lbi2s.find(y1p);
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second + "_";
	}
	
	lbmapit = lbi2s.find(y1pp); 
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second;
	}	
    
    } else if (ftype == STAT_FEATURE1) {
	// state feature (type 1)
	str = "s1_";

	lbmapit = lbi2s.find(y);
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second + "_";
	}

	cpmapit = cpi2s.find(cp);
	if (cpmapit != cpi2s.end()) {
	    str += cpmapit->second;
	}
    
    } else if (ftype == STAT_FEATURE2) {
	// state feature (type 2)
	str = "s2_";
	
	int y1, y1p;
	
	lb2to1mapit = lb2to1.find(y);
	if (lb2to1mapit != lb2to1.end()) {
	    y1 = lb2to1mapit->second.second;
	    y1p = lb2to1mapit->second.first;
	}
	
	lbmapit = lbi2s.find(y1);
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second + "_";
	}

	lbmapit = lbi2s.find(y1p);
	if (lbmapit != lbi2s.end()) {
	    str += lbmapit->second + "_";
	}
	
	cpmapit = cpi2s.find(cp);
	if (cpmapit != cpi2s.end()) {
	    str += cpmapit->second;
	}
    }
    
    char buff[100];
    sprintf(buff, " %d %f", idx, wgt);
    str += buff;
    
    return str;
}

