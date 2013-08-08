/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * featuregen.h - this file is part of FlexCRFs.
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

#ifndef	_FEATUREGEN_H
#define	_FEATUREGEN_H

#include <map>
#include <vector>
#include <string>
#include "option.h"
#include "data.h"
#include "dictionary.h"
#include "feature.h"

using namespace std;

class featuregen {
public:
    // int num_labels;
    list<feature> features;	// list of all features
    map<string, int> fmap;	// feature map: string identifier -> feature index
    option * popt;		// pointer to option object
    data * pdata;		// pointer to data
    dictionary * pdict;		// pointer to dictionary
    
    featuregen(option * popt, dictionary * pdict = NULL, data * pdata = NULL) {
	this->popt = popt;
	this->pdata = pdata;
	this->pdict = pdict;
    }    
    
    // adding a feature
    void add_feature(feature & f) {
	f.strid2idx_add(fmap);
	features.push_back(f);
    }
    
    // generating feature from the training data and dictionary
    void gen_features();
    
    // return the total number of features
    int num_features() {
	return features.size();
    }
    
    // write feature to file
    void write_features(FILE * fout);
    
    // read features from file
    void read_features(FILE * fin);
    
    // start to scan features at a particular position in a dat sequence
    void start_scan_features_at(sequence & seq, int pos);
    int has_next_feature();
    void next_feature(feature & f);
    
    // start to scan state features
    vector<feature*> sfeatures;
    int sf_idx;
    void start_scan_sfeatures_at(sequence & seq, int pos);
    int has_next_sfeature();
    void next_sfeature(feature & sf);
    
    // start to scan edge features
    vector<feature> efeatures;
    int ef_idx;
    void start_scan_efeatures();
    int has_next_efeature();
    void next_efeature(feature & ef);
};

#endif

