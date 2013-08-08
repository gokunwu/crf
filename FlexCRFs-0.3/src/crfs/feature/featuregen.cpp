/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * featuregen.cpp - this file is part of FlexCRFs.
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

#include <ctype.h>
#include "../../../include/featuregen.h"
#include "../../../include/data.h"
#include "../../../include/strtokenizer.h"
#include "../../../include/dictionary.h"

using namespace std;

// generating features
void featuregen::gen_features() {
    features.clear();
    sfeatures.clear();
    efeatures.clear();
    
    if (!pdata || !pdict) {
	return;
    }
    
    if (!(pdata->ptrndata)) {
	return;
    }
    
    dataset::iterator datait;
    sequence::iterator seqit;
    
    feature f;
    
    // scan over all data sequences
    for (datait = pdata->ptrndata->begin(); datait != pdata->ptrndata->end(); datait++) {
	
	// scan over all observations in each data sequence
	int pos = 0;
	for (seqit = datait->begin(); seqit != datait->end(); seqit++) {
	    
	    // generating edge feature (type 1)
	    if (pos > 0) {
		// for both first- and second-order Markov
		f.efeature1_init(seqit->label, (*datait)[pos - 1].label);
		f.strid2idx(fmap);
		if (f.idx == -1) {
		    // new feature, thus add to the feature list
		    add_feature(f);
		    // add the new edge feature to the vector of edge features
		    efeatures.push_back(f);
		}
	    }
	    
	    // create edge feature (type 2)
	    if (popt->order == SECOND_ORDER && pos > 0) {
		f.efeature2_init(seqit->label2order, (*datait)[pos - 1].label2order);
		f.strid2idx(fmap);
		if (f.idx == -1) {
		    // new feature, thus add to the feature list
		    add_feature(f);
		    // add the new edge feature to the vector of edge features
		    efeatures.push_back(f);
		}
	    }
	    
	    // generating state features
	    // scan over all context predicates
	    vector<int>::iterator cpit;
	    for (cpit = (seqit->cps).begin(); cpit != (seqit->cps).end(); cpit++) {
		
		// do not generate too rare features
		map<int, element>::iterator dictit;
		map<int, pair<int, int> >::iterator labelit, label2orderit;
		
		int create_sfeature2 = 0;
		
		dictit = pdict->dict.find(*cpit);
		if(dictit != pdict->dict.end()) {
		    if (dictit->second.count <= popt->cp_rare_threshold) {
			continue;
		    }
		    		    
		    int f_rare_threshold = popt->f_rare_threshold;
		    
		    if (popt->multiple_f_rare_thresholds) {
			// using multiple rare thresholds for features
			mapcpint2str::iterator cpmapit;
			cpmapit = pdata->pcpi2s->find(*cpit);
			if (cpmapit != pdata->pcpi2s->end()) {
			    if (isdigit(cpmapit->second[0])) {
				f_rare_threshold = cpmapit->second[0] - '0';
			    } else if (cpmapit->second.size() >= 2 && cpmapit->second[0] == '#' 
						    && isdigit(cpmapit->second[1])) {
				f_rare_threshold = cpmapit->second[1] - '0';
			    }
			}
		    }
		
		    labelit = dictit->second.lb_cnt_fidxes.find(seqit->label);
		    if (labelit != dictit->second.lb_cnt_fidxes.end()){
			if (labelit->second.first <= f_rare_threshold) {
			    continue;
			}
		    }		    
		    
		    label2orderit = dictit->second.lb2order_cnt_fidxes.find(seqit->label2order);
		    if (label2orderit != dictit->second.lb2order_cnt_fidxes.end()) {
			if (label2orderit->second.first > f_rare_threshold) {
			    create_sfeature2 = 1;
			}
		    }
		}		
		
		// create a state feature (type 1)
		f.sfeature1_init(seqit->label, *cpit);
		f.strid2idx(fmap);
		if (f.idx == -1) {
		    // new feature, add to the feature list
		    add_feature(f);
		    labelit->second.second = f.idx;
		    dictit->second.chosen = 1;
		}
		
		if (create_sfeature2) {
		    mapcpint2str::iterator cpmapit;
		    cpmapit = pdata->pcpi2s->find(*cpit);
		    if (cpmapit != pdata->pcpi2s->end()) {
			if (cpmapit->second[0] != '#') {
			    create_sfeature2 = 0;
			}
		    }
		}
		
		if (popt->order == SECOND_ORDER && create_sfeature2) {
		    // create a state feature (type 2)
		    f.sfeature2_init(seqit->label2order, *cpit);
		    f.strid2idx(fmap);
		    if (f.idx == -1) {
			// new feature, add to the feature list
			add_feature(f);
			label2orderit->second.second = f.idx;
		    }
		}
	    }
	    
	    pos++;
	}
    }
    
    // update the number of features
    if (popt) {
	popt->num_features = features.size();
    }
}

// write all features to file
void featuregen::write_features(FILE * fout) {
    list<feature>::iterator fit;
    
    // write number of features
    fprintf(fout, "%d\n", features.size());
    
    for (fit = features.begin(); fit != features.end(); fit++) {
	fprintf(fout, "%s\n", (fit->to_string(*(pdata->pcpi2s), *(pdata->plbi2s), *(pdata->plb2to1))).c_str());
    }
    
    fprintf(fout, "##################################################\n");
}

// read features from file
void featuregen::read_features(FILE * fin) {
    features.clear();
    efeatures.clear();
    
    string line;
    char buff[BUFF_SIZE_SHORT];
    
    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int num_features = atoi(buff);
    
    for (int i = 0; i < num_features; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;
	
	strtokenizer tok(line, " ");
	if (tok.count_tokens() != 3) {
	    continue;
	}    
	
	// create a new feature by parsing the line
	feature f(line, *(pdata->pcps2i), *(pdata->plbs2i), *(pdata->plb2to1s2i));
	
	map<string, int>::iterator it;
	it = fmap.find(f.strid);
	if (it == fmap.end()) {
	    // insert to the feature map
	    fmap.insert(pair<string, int>(f.strid, f.idx));
	    features.push_back(f);
	    
	    if (f.ftype == EDGE_FEATURE1 || f.ftype == EDGE_FEATURE2) {
		efeatures.push_back(f);
	    }
	}
    }

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);	// read the line ###...
    
    // update the number of features
    if (popt) {
	popt->num_features = features.size();
    }
}

// scan all features at position pos
void featuregen::start_scan_features_at(sequence & seq, int pos) {
    start_scan_sfeatures_at(seq, pos);
    start_scan_efeatures();
}

// have more features?
int featuregen::has_next_feature() {
    return (has_next_efeature() || has_next_sfeature());
}

// get the next feature
void featuregen::next_feature(feature & f) {
    if (has_next_efeature()) {
	next_efeature(f);
    } else if (has_next_sfeature()) {
	next_sfeature(f);
    } else {
	// do nothing
    }
}

// scan all state features
void featuregen::start_scan_sfeatures_at(sequence & seq, int pos) {
    sfeatures.clear();
    sf_idx = 0;
    
    // get the pointer to the observation at pos
    obsr * pobsr = &seq[pos];
    
    vector<int>::iterator cpit;
    map<int, element>::iterator dictit;
    map<int, pair<int, int> >::iterator labelit, label2orderit, label2orderit1;
    
    feature sf;
    
    // scan over all context predicates
    for (cpit = pobsr->cps.begin(); cpit != pobsr->cps.end(); cpit++) {
	dictit = pdict->dict.find(*cpit);
	
	if (dictit == pdict->dict.end()) {
	    continue;
	}
	
	if (!(dictit->second.is_scanned)) {
	    // scan all first-order labels for state feature (type 1)
	    for (labelit = dictit->second.lb_cnt_fidxes.begin(); 
		    labelit != dictit->second.lb_cnt_fidxes.end(); labelit++) {
		if (labelit->second.second >= 0) {
		    sf.sfeature1_init(labelit->first, *cpit);
		    sf.idx = labelit->second.second;
		    
		    if (popt->highlight_feature) {
			// highlight feature
			sf.val += (double)labelit->second.first / dictit->second.count;
		    }
		    
		    dictit->second.cpfeatures.push_back(sf);
		}
	    }
	    
	    // scan all second-order labels for state feature (type 2)
	    for (label2orderit = dictit->second.lb2order_cnt_fidxes.begin(); 
		    label2orderit != dictit->second.lb2order_cnt_fidxes.end(); label2orderit++) {

		if (label2orderit->second.second >= 0) {
		    sf.sfeature2_init(label2orderit->first, *cpit);
		    sf.idx = label2orderit->second.second;
		    
		    if (popt->highlight_feature) {
			// highlight feature
			int prev_label = label2orderit->first / popt->num_labels;

			int sum_count = 0;
			for (label2orderit1 = dictit->second.lb2order_cnt_fidxes.begin();
				    label2orderit1 != dictit->second.lb2order_cnt_fidxes.end(); 
			    	    label2orderit1++) {
				
			    if (label2orderit1->second.second >= 0 &&
					(label2orderit1->first / popt->num_labels) == prev_label) {
				sum_count += label2orderit1->second.first;				
			    }
			}
		    
			if (sum_count > 0) {
			    sf.val += (double)label2orderit->second.first / sum_count;			    
			}
		    }
		    
		    dictit->second.cpfeatures.push_back(sf);
		}
	    }
	    	
	    dictit->second.is_scanned = 1;
	}
	
	for (int i = 0; i < dictit->second.cpfeatures.size(); i++) {
	    sfeatures.push_back(&(dictit->second.cpfeatures[i]));
	}
    }
}

// has more state features?
int featuregen::has_next_sfeature() {
    return (sf_idx < sfeatures.size());
}

// get the next state feature
void featuregen::next_sfeature(feature & sf) {
    sf = *(sfeatures[sf_idx++]);
}

// scan all edge features
void featuregen::start_scan_efeatures() {
    ef_idx = 0;
}

// have more edge feature
int featuregen::has_next_efeature() {
    return (ef_idx < efeatures.size());
}

// get the next edge feature
void featuregen::next_efeature(feature & ef) {
    ef = efeatures[ef_idx++];
}

