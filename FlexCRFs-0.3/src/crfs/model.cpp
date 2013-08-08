/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * model.cpp - this file is part of FlexCRFs.
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

#include <stdio.h>
#include <stdlib.h>
#include "../../include/model.h"
#include "../../include/trainer.h"
#include "../../include/viterbi.h"
#include "../../include/evaluation.h"

using namespace std;

model::model() {
    popt = NULL;
    pdata = NULL;
    pdict = NULL;
    pfgen = NULL;
    ptrn = NULL;
    pvtb = NULL;
    peval = NULL;
    lambda = NULL;
}

model::~model() {
    if (lambda) {
	delete lambda;
    }
}

model::model(option * popt, data * pdata, dictionary * pdict, featuregen * pfgen, 
	 trainer * ptrn, viterbi * pvtb, evaluation * peval) {
    this->popt = popt;
    this->pdata = pdata;
    this->pdict = pdict;
    this->pfgen = pfgen;
    
    if (ptrn) {
	this->ptrn = ptrn; ptrn->pmodel = this; ptrn->init();
    }
    
    if (pvtb) {
	this->pvtb = pvtb; pvtb->init(this);
    }
    
    if (peval) {
	this->peval = peval; peval->init(this);
    }
    
    lambda = NULL;
    
    // map chunk information
    maplbstr2int::iterator lbmapit;
    char buff[50];
    for (int i = 0; i < popt->chunks.size(); i++) {
	for (int j = 0; j < 2; j++) {
	    lbmapit = pdata->plbs2i->find(popt->chunks[i][j]);
	    if (lbmapit != pdata->plbs2i->end()) {
		sprintf(buff, "%d", lbmapit->second);
	    } else {
		sprintf(buff, "%d", -1);
	    }
	    popt->chunks[i][j] = buff;
	}
    }    
}

void model::train(FILE * fout) {
    if (!lambda) {
	lambda = new double[pfgen->num_features()];	
    }
    
    if (!ptrn) {
	return;
    }
    
    // call this to train the model
    ptrn->train(fout);
    
    // call this to update the feature weights
    update_features();
}

void model::update_features() {
    list<feature>::iterator fit;
    for (fit = pfgen->features.begin(); fit != pfgen->features.end(); fit++) {
	fit->wgt = lambda[fit->idx];
    }
}

void model::apply_tstdata() {
    apply(pdata->ptstdata);
}

void model::apply_tstdata(int n) {
    apply(pdata->ptstdata, n);
}

void model::apply_ulbdata() {
    apply(pdata->pulbdata);
}

void model::apply_ulbdata(int n) {
    apply(pdata->pulbdata, n);
}

void model::apply(dataset * pdataset) {
    if (!pdataset) {
	return;
    }
    
    if (!pvtb) {
	return;
    }
    
    if (!lambda) {
	lambda = new double[pfgen->num_features()];
	
	// reading lambda values from the feature list
	list<feature>::iterator fit;
	for (fit = pfgen->features.begin(); fit != pfgen->features.end(); fit++) {
	    lambda[fit->idx] = fit->wgt;
	}
    }
    
    pvtb->apply(pdataset);
}

void model::apply(dataset * pdataset, int n) {
    if (!pdataset) {
	return;
    }
    
    if (!pvtb) {
	return;
    }
    
    if (!lambda) {
	lambda = new double[pfgen->num_features()];
	
	// reading lambda values from the feature list
	list<feature>::iterator fit;
	for (fit = pfgen->features.begin(); fit != pfgen->features.end(); fit++) {
	    lambda[fit->idx] = fit->wgt;
	}
    }
    
    pvtb->apply(pdataset, n);
}


