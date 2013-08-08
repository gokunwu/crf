/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * data.h - this file is part of FlexCRFs.
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

#ifndef	_DATA_H
#define	_DATA_H

#include <stdio.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include "option.h"

using namespace std;

// for storing n-best label paths
typedef struct {
    vector<int> model_labels;
    double entropyval;
    vector<double> pathvals;
} nbestinfo;

// map of context predidates [string -> int]
typedef map<string, int> mapcpstr2int;
// map of context predicates [int -> string]
typedef map<int, string> mapcpint2str;
// map of labels [string -> int]
typedef map<string, int> maplbstr2int;
// map of labels [int -> string]
typedef map<int, string> maplbint2str;

// for second-order Markov CRFs
typedef map<int, pair<int, int> > maplb2to1order;

// data observation
class obsr {
public:
    int label;		// human label
    int label2order;	// human label (second-order label)
    int model_label;	// label predicted by the model
    vector<int> cps;	// context predicates
    
    // for searching n-best label paths
    nbestinfo * pnbestinfo;
    
    obsr() {
	label = label2order = model_label = -1;
	pnbestinfo = NULL;
    }
    
    obsr(vector<int> & cps) {
	label = label2order = model_label = -1;
	this->cps = cps;
	pnbestinfo = NULL;
    }
    
    obsr(int label, vector<int> & cps) {
	this->label = label;
	label2order = -1;
	model_label = -1;
	this->cps = cps;
	pnbestinfo = NULL;
    }
    
    ~obsr() {
	if (pnbestinfo) {
	    delete pnbestinfo;
	}
    }
    
    // write training obsr
    void display_trnobsr(mapcpint2str & cpi2s, maplbint2str & lbi2s, FILE * fout);
    // write testing obsr
    void display_tstobsr(mapcpint2str & cpi2s, maplbint2str & lbi2s, FILE * fout, double bestpathval = -1.0);
    // write unlabeled obsr
    void display_ulbobsr(mapcpint2str & cpi2s, maplbint2str & lbi2s, FILE * fout);
};

typedef vector<obsr> sequence;	// data sequence
typedef list<sequence> dataset;	// data set

class dictionary;

class data {
public:
    dataset * ptrndata;
    dataset * ptstdata;
    dataset * pulbdata;
    
    mapcpstr2int * pcps2i;
    mapcpint2str * pcpi2s;
    maplbstr2int * plbs2i;
    maplbint2str * plbi2s;
    
    // for second-order Markov CRFs
    maplbstr2int * plb2to1s2i;
    maplb2to1order * plb2to1;
    
    option * popt;
    
    data(option * popt = NULL) {
	ptrndata = NULL; 
	ptstdata = NULL; 
	pulbdata = NULL; 
	
	pcps2i = NULL; 
	pcpi2s = NULL; 
	plbs2i = NULL; 
	plbi2s = NULL;
	
	plb2to1s2i = NULL;
	plb2to1 = NULL;
	
	this->popt = NULL;
	if (popt) {
	    this->popt = popt;
	}
    }
    
    ~data() {
	if (ptrndata) {
	    delete ptrndata;
	}
	if (ptstdata) {
	    delete ptstdata;
	}
	if (pulbdata) {
	    delete pulbdata;
	}
	if (pcps2i) {
	    delete pcps2i;
	}
	if (pcpi2s) {
	    delete pcpi2s;
	}
	if (plbs2i) {
	    delete plbs2i;
	}
	if (plbi2s) {
	    delete plbi2s;
	}
	if (plb2to1s2i) {
	    delete plb2to1s2i;
	}
	if (plb2to1) {
	    delete plb2to1;
	}
    }
    
    // read context predicate map from file
    void read_cp_map(FILE * fin);
    // write context predicate map to file
    void write_cp_map(FILE * fout, dictionary * pdict);
    
    // read label map from file
    void read_lb_map(FILE * fin);
    // write label map to file
    void write_lb_map(FILE * fout);
    
    // read and write second-order label map
    void read_2order_lb_map(FILE * fin);
    void write_2order_lb_map(FILE * fout);
    // read and write mapping from second-order labels to first-order labels
    void read_2to1_lb_map(FILE * fin);
    void write_2to1_lb_map(FILE * fout);
    
    // read training data
    void read_trndata(FILE * fin);
    
    // read testing data
    void read_tstdata(FILE * fin);
    // write testing data
    void write_tstdata(FILE * fout);
    
    // read ulabeled data need to be predicted
    void read_ulbdata(FILE * fin);
    // write unlabel data (after predicted) to file
    void write_ulbdata(FILE * fout);
    
    // eliminating redundant context predicates in dictionary and training data
    void cp_prune(dictionary * pdict);
};

#endif

