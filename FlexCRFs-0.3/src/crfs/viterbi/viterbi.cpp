/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * viterbi.cpp - this file is part of FlexCRFs.
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

#include <math.h>
#include "../../../include/viterbi.h"
#include "../../../include/model.h"

using namespace std;

viterbi::viterbi() {
    pmodel = NULL;
    popt = NULL;
    pdata = NULL;
    pdict = NULL;
    pfgen = NULL;
    
    Mi = NULL;
    Vi = NULL;    
}

viterbi::~viterbi() {
    if (Mi) { 
	delete Mi; 
    }
    
    if (Vi) { 
	delete Vi; 
    }    
}

void viterbi::init(model * pmodel) {
    this->pmodel = pmodel;
    popt = pmodel->popt;
    pdata = pmodel->pdata;
    pdict = pmodel->pdict;
    pfgen = pmodel->pfgen;

    int dim = popt->num_labels;
    if (popt->order == SECOND_ORDER) {
	dim = popt->num_2orderlabels;
    }

    Mi = new doublematrix(dim, dim);
    Vi = new doublevector(dim);    
    
    for (int i = 0; i < dim; i++) {
	temp.push_back(pair<double, int>(0.0, -1));
    }
    
    // mapping label (string) => label (index) for constraints
    maplbstr2int::iterator it;
    
    popt->prevfixedintlabels.clear();
    popt->nextfixedintlabels.clear();
    
    vector<int> labels;
    
    int len = popt->prevfixedstrlabels.size();
    for (int i = 0; i < len; i++) {
	labels.clear();
	
	it = pdata->plbs2i->find(popt->prevfixedstrlabels[i][0]);
	if (it != pdata->plbs2i->end()) {
	    labels.push_back(it->second);
	} else {
	    continue;
	}
	
	for (int j = 1; j < popt->prevfixedstrlabels[i].size(); j++) {
	    it = pdata->plbs2i->find(popt->prevfixedstrlabels[i][j]);
	    if (it != pdata->plbs2i->end()) {
		labels.push_back(it->second);
	    }
	}
	
	if (labels.size() <= 1) {
	    continue;
	}
	
	popt->prevfixedintlabels.push_back(labels);
    }

    len = popt->nextfixedstrlabels.size();
    for (int i = 0; i < len; i++) {
	labels.clear();
	
	it = pdata->plbs2i->find(popt->nextfixedstrlabels[i][0]);
	if (it != pdata->plbs2i->end()) {
	    labels.push_back(it->second);
	} else {
	    continue;
	}
	
	for (int j = 1; j < popt->nextfixedstrlabels[i].size(); j++) {
	    it = pdata->plbs2i->find(popt->nextfixedstrlabels[i][j]);
	    if (it != pdata->plbs2i->end()) {
		labels.push_back(it->second);
	    }
	}
	
	if (labels.size() <= 1) {
	    continue;
	}
	
	popt->nextfixedintlabels.push_back(labels);
    }
}

void viterbi::computeMi() {
    *Mi = 0;
    
    pfgen->start_scan_efeatures();
    while (pfgen->has_next_efeature()) {
	feature f;
	pfgen->next_efeature(f);
	
	if (f.ftype == EDGE_FEATURE1) {
	    // edge feature type 1

	    if (popt->order == FIRST_ORDER) {
		Mi->get(f.yp, f.y) += pmodel->lambda[f.idx] * f.val;	
	    
	    } else if (popt->order == SECOND_ORDER) {
		int col = f.yp * popt->num_labels + f.y;
		for (int row = 0; row < Mi->rows; row++) {
		    Mi->get(row, col) += pmodel->lambda[f.idx] * f.val;
		}
	    }
	    
	} else if (f.ftype == EDGE_FEATURE2) {
	    // edge feature type 2

	    if (popt->order == FIRST_ORDER) {
		// do nothing
		    
	    } else if (popt->order == SECOND_ORDER) {
		Mi->get(f.yp, f.y) += pmodel->lambda[f.idx] * f.val;
	    }	    
	}
    }
    
    if (popt->order == FIRST_ORDER) {
	for (int i = 0; i < Mi->rows; i++) {
	    for (int j = 0; j < Mi->cols; j++) {
		Mi->get(i, j) = exp(Mi->get(i, j));
	    }
	}
    
    } else if (popt->order == SECOND_ORDER) {
	for (int i = 0; i < Mi->rows; i++) {
	    for (int j = 0; j < Mi->cols; j++) {
		if (i % popt->num_labels == j / popt->num_labels) {
		    Mi->get(i, j) = exp(Mi->get(i, j));
		} else {
		    Mi->get(i, j) = 0.0;
		}
	    }
	}
    }    
}

void viterbi::apply(dataset * pdataset) {
    computeMi();

    dataset::iterator datait;
    int count = 0;
    for (datait = pdataset->begin(); datait != pdataset->end(); datait++) {
	if (popt->order == FIRST_ORDER) {
	    apply_1order(*datait);
	} else if (popt->order == SECOND_ORDER) {
	    apply_2order(*datait);
	}
	count++;
	// cout << "sequence: " << count << endl;
    }
}

void viterbi::apply(dataset * pdataset, int n) {
    computeMi();

    dataset::iterator datait;
    int count = 0;
    for (datait = pdataset->begin(); datait != pdataset->end(); datait++) {
	if (popt->order == FIRST_ORDER) {
	    apply_1order(*datait, n);
	} else if (popt->order == SECOND_ORDER) {
	    apply_2order(*datait, n);
	}
	count++;
	// cout << "sequence: " << count << endl;
    }
}

void viterbi::apply_1order(sequence & seq) {
    int i, j, k;
    
    int seq_len = seq.size();
    if (seq_len == 0) {	
	return;
    }
    
    int memorysize = memory.size();
    // if the current sequence is the longest one (up to the current point), 
    // then allocate more memory
    if (memorysize < seq_len) {
	for (i = 0; i < seq_len - memorysize; i++) {
	    memory.push_back(temp);
	}
    }
    
    // we need to scale forward variable to [0, 1] to avoid numerical problems        
    int scalesize = scale.size();
    // if the current sequence is the longest one (up to the current point),
    // then allocate more room for scale variable
    if (scalesize < seq_len) {
    	for (i = 0; i < seq_len - scalesize; i++) {
    	    scale.push_back(1.0);
    	}
    }
    
    // compute Mi and Vi for the first position in the sequence
    compute_log_Mi_1order(seq, 0, Mi, Vi, 1);
    for (j = 0; j < popt->num_labels; j++) {    
	memory[0][j].first = (*Vi)[j];
	memory[0][j].second = j;
    }
    // calculate scale factor for the first position
    scale[0] = (popt->is_scaling) ? viterbi::sum(memory[0]) : 1;
    // scaling for the first position
    viterbi::divide(memory[0], scale[0]);
    
    // the main loop
    for (i = 1; i < seq_len; i++) {
	// compute Mi matrix and Vi vector at position "i"
	compute_log_Mi_1order(seq, i, Mi, Vi, 1);

	// applying constraints 
	int num_cnts = popt->prevfixedintlabels.size();
	for (int cc = 0; cc < num_cnts; cc++) {
	    int col = popt->prevfixedintlabels[cc][0];
	    for (int row = 0; row < popt->num_labels; row++) {
		int in = 0;
		for (int count = 1; count < popt->prevfixedintlabels[cc].size(); count++) {
		    if (row == popt->prevfixedintlabels[cc][count]) {
			in = 1;
		    }
		}
		if (!in) {
		    Mi->mtrx[row][col] = 0;
		}
	    }
	}

	num_cnts = popt->nextfixedintlabels.size();
	for (int cc = 0; cc < num_cnts; cc++) {
	    int row = popt->nextfixedintlabels[cc][0];
	    for (int col = 0; col < popt->num_labels; col++) {
		int in = 0;
		for (int count = 1; count < popt->nextfixedintlabels[cc].size(); count++) {
		    if (col == popt->nextfixedintlabels[cc][count]) {
			in = 1;
		    }
		}
		if (!in) {
		    Mi->mtrx[row][col] = 0;
		}
	    }
	}

	// for all possible labels at the position "i"
	for (j = 0; j < popt->num_labels; j++) {
	    memory[i][j].first = 0.0;
	    memory[i][j].second = 0;
	    
	    // find the maximal value and its index and store them in memory
	    // for later tracing back to find the best path
	    for (k = 0; k < popt->num_labels; k++) {		
		double tempval = memory[i-1][k].first * Mi->mtrx[k][j] * (*Vi)[j];
		
		if (tempval > memory[i][j].first) {
		    memory[i][j].first = tempval;
		    memory[i][j].second = k;
		}  
	    }	    
	}
	
	// scaling for memory at position "i"
	scale[i] = (popt->is_scaling) ? viterbi::sum(memory[i]) : 1;
	viterbi::divide(memory[i], scale[i]);
    }
    
    // viterbi backtrack to find the best path
    int max_idx = viterbi::find_max(memory[seq_len - 1]);
    seq[seq_len - 1].model_label = max_idx;
    for (i = seq_len - 2; i >= 0; i--) {
	seq[i].model_label = memory[i + 1][max_idx].second;
	max_idx = seq[i].model_label;		
    }
}

void viterbi::apply_1order(sequence & seq, int n) {
    int i, j, k, h;
    
    int seq_len = seq.size();
    if (seq_len == 0) {	
	return;
    }
    
    mem infor;
    infor.pathval = 0.0;
    infor.previouslabel = -1;
    infor.previousindex = -1;
    
    int premaxlen = statelens.size();
    if (premaxlen < seq_len) {
	for (i = 0; i < seq_len - premaxlen; i++) {
	    statelens.push_back(0);
	}
    }

    if (statelbls.size() != n * popt->num_labels) {
	for (i = 0; i < n * popt->num_labels; i++) {
	    statelbls.push_back(infor);
	    sortidxes.push_back(pair<int, double>(0, 0.0));
	}
    }
    
    premaxlen = seqlbls.size();
    if (premaxlen < seq_len) {
	for (i = 0; i < seq_len - premaxlen; i++) {
	    seqlbls.push_back(statelbls);
	}
    }

    // scaling
    premaxlen = scale.size();
    if (premaxlen < seq_len) {
	for (i = 0; i < seq_len - premaxlen; i++) {
	    scale.push_back(1.0);
	}	
    }
    
    // compute Mi and Vi for the first position in the sequence
    compute_log_Mi_1order(seq, 0, Mi, Vi, 1);
    
    statelens[0] = 1;
    // for the first position
    for (j = 0; j < popt->num_labels; j++) {
	seqlbls[0][j * n].pathval = (*Vi)[j];
	seqlbls[0][j * n].previouslabel = j;
	seqlbls[0][j * n].previousindex = 0;
    }
    
    // scaling
    scale[0] = (popt->is_scaling) ? 
		viterbi::sum(seqlbls[0], popt->num_labels, n, statelens[0]) : 1;
    viterbi::divide(seqlbls[0], popt->num_labels, n, statelens[0], scale[0]);

    // the main loop
    for (i = 1; i < seq_len; i++) {
	// compute Mi matrix and Vi vector at position "i"
	compute_log_Mi_1order(seq, i, Mi, Vi, 1);
	
	statelens[i] = n;
	if (statelens[i] > statelens[i - 1] * popt->num_labels) {
	    statelens[i] = statelens[i - 1] * popt->num_labels;
	}
	
	// for all possible labels at the position "i"
	for (j = 0; j < popt->num_labels; j++) {
	    int count = 0;
	    // for all possible labels at the position "i-1"
	    for (k = 0; k < popt->num_labels; k++) {
		for (h = 0; h < statelens[i - 1]; h++) {
		    sortidxes[count].first = k * n + h;
		    sortidxes[count].second = 
			seqlbls[i - 1][k * n + h].pathval * 
			Mi->mtrx[k][j] * (*Vi)[j];

		    count++;
		}		
	    }   

	    quicksort(sortidxes, 0, count - 1);
	    
	    for (k = 0; k < statelens[i]; k++) {
		seqlbls[i][j * n + k].pathval = sortidxes[k].second;
		seqlbls[i][j * n + k].previouslabel = sortidxes[k].first / n;
		seqlbls[i][j * n + k].previousindex = sortidxes[k].first % n;
	    }	    
	} // end of (for all possible labels at the position "i")
	
	// scaling for the current position
	scale[i] = (popt->is_scaling) ? 
		viterbi::sum(seqlbls[i], popt->num_labels, n, statelens[i]) : 1;
	viterbi::divide(seqlbls[i], popt->num_labels, n, statelens[i], scale[i]);
			
    } // end of the main loop     

    int count = 0;
    for (j = 0; j < popt->num_labels; j++) {
	for (k = 0; k < statelens[seq_len - 1]; k++) {
	    sortidxes[count].first = j * n + k;
	    sortidxes[count].second = seqlbls[seq_len - 1][j * n + k].pathval;
	    count++;
	}
    }   
    
    quicksort(sortidxes, 0, count - 1);
    
    int realsize = n;
    if (realsize > count) {
	realsize = count;
    }    

    // allocate memory for n-best path information
    for (i = 0; i < seq_len; i++) {
	seq[i].pnbestinfo = new nbestinfo;
	while (seq[i].pnbestinfo->model_labels.size() < realsize) {
	    seq[i].pnbestinfo->model_labels.push_back(-1);
	}
	
	if (i == 0) {
	    while (seq[0].pnbestinfo->pathvals.size() < realsize) {	
		seq[0].pnbestinfo->pathvals.push_back(0.0);
	    }
	}
    }
    
    double sumpathvals = 0.0;
    // n-best backtracking
    for (i = 0; i < realsize; i++) {
	seq[0].pnbestinfo->pathvals[i] = sortidxes[i].second;
	sumpathvals += seq[0].pnbestinfo->pathvals[i];
	
	int major = sortidxes[i].first / n;
	seq[seq_len - 1].pnbestinfo->model_labels[i] = major;
	int minor = sortidxes[i].first % n;
	
	for (j = seq_len - 2; j >= 0; j--) {
	    seq[j].pnbestinfo->model_labels[i] = 
		     seqlbls[j + 1][major * n + minor].previouslabel;
	    int mj = seqlbls[j + 1][major * n + minor].previouslabel;
	    int mn = seqlbls[j + 1][major * n + minor].previousindex;
	    major = mj;
	    minor = mn;
	}
    }
    
    // scaling path values
    if (sumpathvals > 0) {
	for (i = 0; i < realsize; i++) {
	    seq[0].pnbestinfo->pathvals[i] = seq[0].pnbestinfo->pathvals[i] / sumpathvals;
	}
    }
    
    // calculating entropy
    vector<double> ps;
    for (i = 0; i < seq_len; i++) {
	// the best path
	seq[i].model_label = seq[i].pnbestinfo->model_labels[0];
	
	ps.clear();
	for (j = 0; j < popt->num_labels; j++) {
	    ps.push_back(0.0);
	}
	
	for (j = 0; j < realsize; j++) {
	    ps[seq[i].pnbestinfo->model_labels[j]] += seq[0].pnbestinfo->pathvals[j];
	}
	
	int count = 0;
	for (j = 0; j < popt->num_labels; j++) {
	    if (ps[j] > 0.0) {
		count++;
	    }
	}
	
	seq[i].pnbestinfo->entropyval = 0.0;
	if (count > 1) {
	    for (j = 0; j < popt->num_labels; j++) {
		if (ps[j] > 0.0) {
		    seq[i].pnbestinfo->entropyval -= ps[j] * log(ps[j]);
		}
	    }
	}
	seq[i].pnbestinfo->entropyval /= ps.size();
    }
}

void viterbi::apply_2order(sequence & seq) {
    int i, j, k;
    
    map<int, pair<int, int> >::iterator lbmapit;

    int seq_len = seq.size();
    if (seq_len == 0) {	
	return;
    }
    
    int lfo = popt->num_labels - 1;
    if (popt->lfo >= 0) {
	lfo = popt->lfo;
    }
    
    int memorysize = memory.size();
    // if the current sequence is the longest one (up to the current point), 
    // then allocate more memory
    if (memorysize < seq_len) {
	for (i = 0; i < seq_len - memorysize; i++) {
	    memory.push_back(temp);
	}
    }
    
    // we need to scale forward variable to [0, 1] to avoid numerical problems        
    int scalesize = scale.size();
    // if the current sequence is the longest one (up to the current point),
    // then allocate more room for scale variable
    if (scalesize < seq_len) {
    	for (i = 0; i < seq_len - scalesize; i++) {
    	    scale.push_back(1.0);
    	}
    }
    
    // compute Mi and Vi for the first position in the sequence
    compute_log_Mi_2order(seq, 0, Mi, Vi, 1);

    for (j = 0; j < popt->num_2orderlabels; j++) {    
	memory[0][j].first = (*Vi)[j];
	memory[0][j].second = j;
    }
    // calculate scale factor for the first position
    scale[0] = (popt->is_scaling) ? viterbi::sum(memory[0]) : 1;
    // scaling for the first position
    viterbi::divide(memory[0], scale[0]);
    
    // the main loop
    for (i = 1; i < seq_len; i++) {
	// compute Mi matrix and Vi vector at position "i"
	compute_log_Mi_2order(seq, i, Mi, Vi, 1);

	// applying constraints 
	int num_cnts = popt->prevfixedintlabels.size();
	for (int cc = 0; cc < num_cnts; cc++) {
	    int col = popt->prevfixedintlabels[cc][0];
	    for (int row = 0; row < popt->num_labels; row++) {
		int in = 0;
		for (int count = 1; count < popt->prevfixedintlabels[cc].size(); count++) {
		    if (row == popt->prevfixedintlabels[cc][count]) {
			in = 1;
		    }
		}
		if (!in) {
		    int index = row * popt->num_labels + col;
		    (*Vi)[index] = 0;
		}
	    }
	}

	num_cnts = popt->nextfixedintlabels.size();
	for (int cc = 0; cc < num_cnts; cc++) {
	    int row = popt->nextfixedintlabels[cc][0];
	    for (int col = 0; col < popt->num_labels; col++) {
		int in = 0;
		for (int count = 1; count < popt->nextfixedintlabels[cc].size(); count++) {
		    if (col == popt->nextfixedintlabels[cc][count]) {
			in = 1;
		    }
		}
		if (!in) {
		    int index = row * popt->num_labels + col;
		    (*Vi)[index] = 0;
		}
	    }
	}

	// for all possible labels at the position "i"
	for (j = 0; j < popt->num_2orderlabels; j++) {
	    memory[i][j].first = 0.0;
	    memory[i][j].second = 0;
	    
	    // find the maximal value and its index and store them in memory
	    // for later tracing back to find the best path
	    for (k = 0; k < popt->num_2orderlabels; k++) {		
		double tempval = memory[i-1][k].first * Mi->mtrx[k][j] * (*Vi)[j];
		
		if (tempval > memory[i][j].first) {
		    memory[i][j].first = tempval;
		    memory[i][j].second = k;
		}  
	    }	    
	}
	
	// scaling for memory at position "i"
	scale[i] = (popt->is_scaling) ? viterbi::sum(memory[i]) : 1;
	viterbi::divide(memory[i], scale[i]);
    }
    
    // viterbi backtrack to find the best path
    int max_idx = viterbi::find_max(memory[seq_len - 1]);
    seq[seq_len - 1].model_label = max_idx;
    for (i = seq_len - 2; i >= 0; i--) {
	seq[i].model_label = memory[i + 1][max_idx].second;
	max_idx = seq[i].model_label;		
    }

    // converting from second-order labels to first-order ones
    for (i = 0; i < seq_len; i++) {
	lbmapit = pdata->plb2to1->find(seq[i].model_label);
	if (lbmapit != pdata->plb2to1->end()) {
	    seq[i].model_label = lbmapit->second.second;
	}
    }
}

void viterbi::apply_2order(sequence & seq, int n) {
    int i, j, k, h;

    map<int, pair<int, int> >::iterator lbmapit;
    
    int seq_len = seq.size();
    if (seq_len == 0) {	
	return;
    }
    
    mem infor;
    infor.pathval = 0.0;
    infor.previouslabel = -1;
    infor.previousindex = -1;
    
    int premaxlen = statelens.size();
    if (premaxlen < seq_len) {
	for (i = 0; i < seq_len - premaxlen; i++) {
	    statelens.push_back(0);
	}
    }

    if (statelbls.size() != n * popt->num_2orderlabels) {
	for (i = 0; i < n * popt->num_2orderlabels; i++) {
	    statelbls.push_back(infor);
	    sortidxes.push_back(pair<int, double>(0, 0.0));
	}
    }
    
    premaxlen = seqlbls.size();
    if (premaxlen < seq_len) {
	for (i = 0; i < seq_len - premaxlen; i++) {
	    seqlbls.push_back(statelbls);
	}
    }

    // scaling
    premaxlen = scale.size();
    if (premaxlen < seq_len) {
	for (i = 0; i < seq_len - premaxlen; i++) {
	    scale.push_back(1.0);
	}	
    }
    
    // compute Mi and Vi for the first position in the sequence
    compute_log_Mi_2order(seq, 0, Mi, Vi, 1);
    
    statelens[0] = 1;
    // for the first position
    for (j = 0; j < popt->num_2orderlabels; j++) {
	seqlbls[0][j * n].pathval = (*Vi)[j];
	seqlbls[0][j * n].previouslabel = j;
	seqlbls[0][j * n].previousindex = 0;
    }
    
    // scaling
    scale[0] = (popt->is_scaling) ? 
		viterbi::sum(seqlbls[0], popt->num_2orderlabels, n, statelens[0]) : 1;
    viterbi::divide(seqlbls[0], popt->num_2orderlabels, n, statelens[0], scale[0]);

    // the main loop
    for (i = 1; i < seq_len; i++) {
	// compute Mi matrix and Vi vector at position "i"
	compute_log_Mi_2order(seq, i, Mi, Vi, 1);
	
	statelens[i] = n;
	if (statelens[i] > statelens[i - 1] * popt->num_2orderlabels) {
	    statelens[i] = statelens[i - 1] * popt->num_2orderlabels;
	}
	
	// for all possible labels at the position "i"
	for (j = 0; j < popt->num_2orderlabels; j++) {
	    int count = 0;
	    // for all possible labels at the position "i-1"
	    for (k = 0; k < popt->num_2orderlabels; k++) {
		for (h = 0; h < statelens[i - 1]; h++) {
		    sortidxes[count].first = k * n + h;
		    sortidxes[count].second = 
					seqlbls[i - 1][k * n + h].pathval * Mi->mtrx[k][j] * (*Vi)[j];

		    count++;
		}		
	    }   

	    quicksort(sortidxes, 0, count - 1);
	    
	    for (k = 0; k < statelens[i]; k++) {
		seqlbls[i][j * n + k].pathval = sortidxes[k].second;
		seqlbls[i][j * n + k].previouslabel = sortidxes[k].first / n;
		seqlbls[i][j * n + k].previousindex = sortidxes[k].first % n;
	    }	    
	} // end of (for all possible labels at the position "i")
	
	// scaling for the current position
	scale[i] = (popt->is_scaling) ? 
		viterbi::sum(seqlbls[i], popt->num_2orderlabels, n, statelens[i]) : 1;
	viterbi::divide(seqlbls[i], popt->num_2orderlabels, n, statelens[i], scale[i]);
			
    } // end of the main loop     

    int count = 0;
    for (j = 0; j < popt->num_2orderlabels; j++) {
	for (k = 0; k < statelens[seq_len - 1]; k++) {
	    sortidxes[count].first = j * n + k;
	    sortidxes[count].second = seqlbls[seq_len - 1][j * n + k].pathval;
	    count++;
	}
    }   
    
    quicksort(sortidxes, 0, count - 1);
    
    int realsize = n;
    if (realsize > count) {
	realsize = count;
    }    

    // allocate memory for n-best path information
    for (i = 0; i < seq_len; i++) {
	seq[i].pnbestinfo = new nbestinfo;
	while (seq[i].pnbestinfo->model_labels.size() < realsize) {
	    seq[i].pnbestinfo->model_labels.push_back(-1);
	}
	
	if (i == 0) {
	    while (seq[0].pnbestinfo->pathvals.size() < realsize) {	
		seq[0].pnbestinfo->pathvals.push_back(0.0);
	    }
	}
    }
    
    double sumpathvals = 0.0;
    // n-best backtracking
    for (i = 0; i < realsize; i++) {
	seq[0].pnbestinfo->pathvals[i] = sortidxes[i].second;
	sumpathvals += seq[0].pnbestinfo->pathvals[i];
	
	int major = sortidxes[i].first / n;
	seq[seq_len - 1].pnbestinfo->model_labels[i] = major;
	int minor = sortidxes[i].first % n;
	
	for (j = seq_len - 2; j >= 0; j--) {
	    seq[j].pnbestinfo->model_labels[i] = 
		     seqlbls[j + 1][major * n + minor].previouslabel;
	    int mj = seqlbls[j + 1][major * n + minor].previouslabel;
	    int mn = seqlbls[j + 1][major * n + minor].previousindex;
	    major = mj;
	    minor = mn;
	}
    }
    
    // scaling path values
    if (sumpathvals > 0) {
	for (i = 0; i < realsize; i++) {
	    seq[0].pnbestinfo->pathvals[i] = seq[0].pnbestinfo->pathvals[i] / sumpathvals;
	}
    }
    
    // calculating entropy
    vector<double> ps;
    for (i = 0; i < seq_len; i++) {
	// the best path
	seq[i].model_label = seq[i].pnbestinfo->model_labels[0];
	
	ps.clear();
	for (j = 0; j < popt->num_2orderlabels; j++) {
	    ps.push_back(0.0);
	}
	
	for (j = 0; j < realsize; j++) {
	    ps[seq[i].pnbestinfo->model_labels[j]] += seq[0].pnbestinfo->pathvals[j];
	}
	
	int count = 0;
	for (j = 0; j < popt->num_2orderlabels; j++) {
	    if (ps[j] > 0.0) {
		count++;
	    }
	}
	
	seq[i].pnbestinfo->entropyval = 0.0;
	if (count > 1) {
	    for (j = 0; j < popt->num_2orderlabels; j++) {
		if (ps[j] > 0.0) {
		    seq[i].pnbestinfo->entropyval -= ps[j] * log(ps[j]);
		}
	    }
	}
	seq[i].pnbestinfo->entropyval /= ps.size();
    }

    // converting from second-order labels to first-order ones
    for (i = 0; i < seq_len; i++) {
	lbmapit = pdata->plb2to1->find(seq[i].model_label);
	if (lbmapit != pdata->plb2to1->end()) {
	    seq[i].model_label = lbmapit->second.second;
	}
	
	for (j = 0; j < realsize; j++) {
	    lbmapit = pdata->plb2to1->find(seq[i].pnbestinfo->model_labels[j]);
	    if (lbmapit != pdata->plb2to1->end()) {
		seq[i].pnbestinfo->model_labels[j] = lbmapit->second.second;
	    }
	}
    }
}

// compute log Mi (for first-order Markov)
void viterbi::compute_log_Mi_1order(sequence & seq, int pos, doublematrix * Mi,
		  doublevector * Vi, int is_exp) {
    *Vi = 0.0;

    // start scan features for sequence "seq" at position "i"
    pfgen->start_scan_sfeatures_at(seq, pos);
    // examine all features at position "pos"
    while (pfgen->has_next_sfeature()) {
	feature f;
	pfgen->next_sfeature(f);
	
	if (f.ftype == STAT_FEATURE1) {
	    // state feature (type 1)
	    (*Vi)[f.y] += pmodel->lambda[f.idx] * f.val;
	    
	} 
    }
    
    // take exponential operator
    if (is_exp) {
	for (int i = 0; i < Mi->rows; i++) {
	    // update for Vi
	    (*Vi)[i] = exp((*Vi)[i]);
	}
    }
}

// compute log Mi (second-order Markov)
void viterbi::compute_log_Mi_2order(sequence & seq, int pos, doublematrix * Mi, 
		  doublevector * Vi, int is_exp) {
    *Vi = 0.0;

    // start scan features for sequence "seq" at position "i"
    pfgen->start_scan_sfeatures_at(seq, pos);
    // examine all features at position "pos"
    while (pfgen->has_next_sfeature()) {
	feature f;
	pfgen->next_sfeature(f);
	
	if (f.ftype == STAT_FEATURE1) {
	    // state feature (type 1)
	    for (int i = 0; i < popt->num_labels; i++) {
		(*Vi)[i * popt->num_labels + f.y] += pmodel->lambda[f.idx] * f.val;
	    }
	
	} else if (f.ftype == STAT_FEATURE2) {
	    // state feature (type 2)
	    (*Vi)[f.y] += pmodel->lambda[f.idx] * f.val;
	}
    }
    
    int lfo = popt->num_labels - 1;
    if (popt->lfo >= 0) {
	lfo = popt->lfo;
    }
    
    // take exponential operator
    if (is_exp) {
	if (pos == 0) {
	    for (int j = 0; j < Mi->rows; j++) {
		if (j / popt->num_labels == lfo) {
		    (*Vi)[j] = exp((*Vi)[j]);
		} else {
		    (*Vi)[j] = 0;
		}
	    }
	
	} else {
	    for (int j = 0; j < Mi->rows; j++) {
		    (*Vi)[j] = exp((*Vi)[j]);
	    }	    	
	}
    }
}

// this is used by viterbi search
double viterbi::sum(vector<pair<double, int> > & vect) {
    double res = 0.0;
    
    for (int i = 0; i < vect.size(); i++) {
	res += vect[i].first;
    }
    
    // if the sum in (-1, 1), then set it to 1
    if (res < 1 && res > -1) {
	res = 1;
    }
    
    return res;
}

// this is necessary for scaling
double viterbi::divide(vector<pair<double, int> > & vect, double val) {
    for (int i = 0; i < vect.size(); i++) {
	vect[i].first /= val;
    }
	return 0;
}

// this is called once in the viterbi search to trace back the best path
int viterbi::find_max(vector<pair<double, int> > & vect) {
    int max_idx = 0;
    double max_val = -1.0;
    
    for (int i = 0; i < vect.size(); i++) {
	if (vect[i].first > max_val) {
	    max_val = vect[i].first;
	    max_idx = i;
	}
    }
    
    return max_idx;    
}

double viterbi::sum(vector<pair<vector<mem>, int> > & vect) {
    double res = 0.0;
    
    for (int i = 0; i < vect.size(); i++) {
	for (int j = 0; j < vect[i].second; j++) {
	    res += vect[i].first[j].pathval;
	}	
    }
    
    if (res < 1 && res > -1) {
	res = 1;
    }
    
    return res;
}

double viterbi::sum(vector<mem> & vect, int num_labels, int n, int len) {
    double res = 0.0;
    
    for (int i = 0; i < num_labels; i++) {
	for (int j = 0; j < len; j++) {
	    res += vect[i * n + j].pathval;
	}
    }
    
    if (res < 1 && res > -1) {
	res = 1;
    }
    
    return res;
}

double viterbi::divide(vector<pair<vector<mem>, int> > & vect, double val) {
    for (int i = 0; i < vect.size(); i++) {
	for (int j = 0; j < vect[i].second; j++) {
	    vect[i].first[j].pathval /= val;
	}
    }
}

double viterbi::divide(vector<mem> & vect, int num_labels, int n, 
	int len, double val) {

    for (int i = 0; i < num_labels; i++) {
	for (int j = 0; j < len; j++) {
	    vect[i * n + j].pathval /= val;
	}
    }
	return 0;
}

void viterbi::quicksort(vector<pair<int, double> > & vect, int left, int right) {
    int l_hold, r_hold;
    pair<int, double> pivot;
    
    l_hold = left;
    r_hold = right;    
    int pivotidx = left;
    pivot = vect[pivotidx];

    while (left < right) {
	while (vect[right].second <= pivot.second && left < right) {
	    right--;
	}
	if (left != right) {
	    vect[left] = vect[right];
	    left++;
	}
	while (vect[left].second >= pivot.second && left < right) {
	    left++;
	}
	if (left != right) {
	    vect[right] = vect[left];
	    right--;
	}
    }

    vect[left] = pivot;
    pivotidx = left;
    left = l_hold;
    right = r_hold;
    
    if (left < pivotidx) {
	quicksort(vect, left, pivotidx - 1);
    }
    if (right > pivotidx) {
	quicksort(vect, pivotidx + 1, right);
    }    
}

