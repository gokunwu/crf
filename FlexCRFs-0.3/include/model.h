/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * model.h - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Oct. 31, 2005
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

#ifndef	_MODEL_H
#define	_MODEL_H

#include "data.h"
#include "dictionary.h"
#include "feature.h"
#include "featuregen.h"
#include "option.h"

using namespace std;

class trainer;
class viterbi;
class evaluation;

class model {
public:
    option * popt;	// pointer to option object
    data * pdata;	// pointer to data object
    dictionary * pdict;	// pointer to dictionary object
    featuregen * pfgen;	// pointer to featuregen object
    trainer * ptrn;	// pointer to trainer object
    viterbi * pvtb;	// pointer to viterbi object
    evaluation * peval;	// pointer to evaluation object
    
    double * lambda;	// feature weights
    
    model();
    
    ~model();
    
    model(option * popt, data * pdata, dictionary * pdict, featuregen * pfgen,
	trainer * ptrn, viterbi * pvtb, evaluation * peval);
	
    // training
    void train(FILE * fout);
    
    // update weight values
    void update_features();
    
    void apply_tstdata();
    void apply_ulbdata();
    void apply_tstdata(int n);
    void apply_ulbdata(int n);
    
    // finding the best label path
    void apply(dataset * pdataset);
    // finding the n-best label paths
    void apply(dataset * pdataset, int n);
};

#endif

