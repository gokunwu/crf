/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * viterbi.h - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Nov. 01, 2005
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

#ifndef	_VITERBI_H
#define	_VITERBI_H

#include "data.h"
#include "feature.h"
#include "featuregen.h"
#include "dictionary.h"
#include "option.h"
#include "doublevector.h"
#include "doublematrix.h"

using namespace std;

// for searching n-best label paths
typedef struct {
    double pathval;
    int previouslabel;
    int previousindex;
} mem;

class model;

class viterbi {
public:
    model * pmodel;	// pointer to conditional ramdom field object
    option * popt;	// pointer to option object
    data * pdata;	// pointer to data object
    dictionary * pdict;	// pointer to dictionary object
    featuregen * pfgen;	// pointer to featuregen object
    
    doublematrix * Mi;
    doublevector * Vi;
    vector<double> scale;
    vector<pair<double, int> > temp;
    vector<vector<pair<double, int> > > memory;	// storing viterbi information
    
    // for searching n-best label paths
    vector<int> statelens;
    vector<mem> statelbls;
    vector<vector<mem> > seqlbls;
    vector<pair<int, double> > sortidxes; // for sorting
    
    viterbi();

    ~viterbi();

    void init(model * pmodel);

    void computeMi();
    
    void apply(dataset * pdataset);
    void apply(dataset * pdataset, int n); // for searching n-best label paths
    void apply_1order(sequence & seq);
    void apply_1order(sequence & seq, int n); // for searching n-best label paths
    void apply_2order(sequence & seq);    
    void apply_2order(sequence & seq, int n); // for searching n-best label paths
    
    void compute_log_Mi_1order(sequence & seq, int pos, doublematrix * Mi, 
	    doublevector * Vi, int is_exp);

    void compute_log_Mi_2order(sequence & seq, int pos, doublematrix * Mi, 
	    doublevector * Vi, int is_exp);
    
    static double sum(vector<pair<double, int> > & vect);
    static double sum(vector<pair<vector<mem>, int> > & vect);
    static double sum(vector<mem> & vect, int num_labels, int n, int len);
    static double divide(vector<pair<double, int> > & vect, double val);
    static double divide(vector<pair<vector<mem>, int> > & vect, double val);
    static double divide(vector<mem> & vect, int num_labels, int n, int len, double val);
    static int find_max(vector<pair<double, int> > & vect);
    
    static void quicksort(vector<pair<int, double> > & vect, int left, int right);
};

#endif





