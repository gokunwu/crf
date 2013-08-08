/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * data.cpp - this file is part of FlexCRFs.
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
#include "../../include/data.h"
#include "../../include/dictionary.h"
#include "../../include/option.h"
#include "../../include/strtokenizer.h"

using namespace std;

#define	CONST	40000000

void obsr::display_trnobsr(mapcpint2str & cpi2s, maplbint2str & lbi2s, FILE * fout) {
    int i;
    mapcpint2str::iterator cpmapit;
    maplbint2str::iterator lbmapit;
    
    for (i = 0; i < cps.size(); i++) {
	cpmapit = cpi2s.find(cps[i]);
	if (cpmapit != cpi2s.end()) {
	    fprintf(fout, "%s ", (cpmapit->second).c_str());
	    // out << cpmapit->second << " ";
	}    
    }

    lbmapit = lbi2s.find(label);
    if (lbmapit != lbi2s.end()) {
	fprintf(fout, "%s", (lbmapit->second).c_str());
	// out << lbmapit->second;
    }
}

void obsr::display_tstobsr(mapcpint2str & cpi2s, maplbint2str & lbi2s, FILE * fout, double bestpathval) {
    int i;
    mapcpint2str::iterator cpmapit;
    maplbint2str::iterator lbmapit;
    
    for (i = 0; i < cps.size(); i++) {    
	cpmapit = cpi2s.find(cps[i]);
	if (cpmapit != cpi2s.end()) {
	    fprintf(fout, "%s ", (cpmapit->second).c_str());
	    // out << cpmapit->second << " ";
	}    
    }
    
    if (pnbestinfo) {
	fprintf(fout, "%f %f ", bestpathval, pnbestinfo->entropyval);
    }
	
    if (label >= 0) {
        lbmapit = lbi2s.find(label);
        if (lbmapit != lbi2s.end()) {
	    fprintf(fout, "%s ", (lbmapit->second).c_str());
    	    // out << lbmapit->second << " ";
    	}
    }
	
    if (model_label >= 0) {
        lbmapit = lbi2s.find(model_label);
        if (lbmapit != lbi2s.end()) {
	    fprintf(fout, "%s", (lbmapit->second).c_str());
	    // out << lbmapit->second;
    	}
    }
}

void obsr::display_ulbobsr(mapcpint2str & cpi2s, maplbint2str & lbi2s, FILE * fout) {
    int i;
    mapcpint2str::iterator cpmapit;
    maplbint2str::iterator lbmapit;
    
    for (i = 0; i < cps.size(); i++) {
        cpmapit = cpi2s.find(cps[i]);
        if (cpmapit != cpi2s.end()) {
	    fprintf(fout, "%s ", (cpmapit->second).c_str());
	    // out << cpmapit->second << " ";
	}    
    }
	
    if (model_label >= 0) {
        lbmapit = lbi2s.find(model_label);
        if (lbmapit != lbi2s.end()) {
	    fprintf(fout, "%s", (lbmapit->second).c_str());
	    // out << lbmapit->second;
    	}
    }
}

void data::read_cp_map(FILE * fin) {
    if (pcps2i) {
	pcps2i->clear();
    } else {
	pcps2i = new mapcpstr2int;
    }
    
    if (pcpi2s) {
	pcpi2s->clear();
    } else {
	pcpi2s = new mapcpint2str;
    }

    char buff[BUFF_SIZE_SHORT];
    string line;
    
    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int num_cps = atoi(buff);
    
    for (int i = 0; i < num_cps; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;

	strtokenizer tok(line, " \t\r\n");

	if (tok.count_tokens() != 2) {
	    continue;
	}
	
	pcps2i->insert(pair<string, int>(tok.token(0), atoi(tok.token(1).c_str())));
	pcpi2s->insert(pair<int, string>(atoi(tok.token(1).c_str()), tok.token(0)));
    }        
    
    // update the number of context predicates in option variable
    if (popt) {
    	popt->num_cps = pcps2i->size();
    }

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);	// read the line ###...    
}

void data::write_cp_map(FILE * fout, dictionary * pdict) {
    mapcpstr2int::iterator mapcpit;
    map<int, element>::iterator dictit;
    
    int count = 0;
    for (mapcpit = pcps2i->begin(); mapcpit != pcps2i->end(); mapcpit++) {
	dictit = pdict->dict.find(mapcpit->second);
	if (dictit != pdict->dict.end()) {
	    count ++;
	}
    }
    
    fprintf(fout, "%d\n", count);
    // out << count << endl;

    for (mapcpit = pcps2i->begin(); mapcpit != pcps2i->end(); mapcpit++) {
	dictit = pdict->dict.find(mapcpit->second);
	if (dictit != pdict->dict.end()) {
	    fprintf(fout, "%s %d\n", (mapcpit->first).c_str(), mapcpit->second);
    	    // out << mapcpit->first << " " << mapcpit->second << endl;
	}
    }
    
    fprintf(fout, "##################################################\n");
}

void data::read_lb_map(FILE * fin) {
    if (plbs2i) {
	plbs2i->clear();
    } else {
	plbs2i = new maplbstr2int;
    }
    
    if (plbi2s) {
	plbi2s->clear();
    } else {
	plbi2s = new maplbint2str;
    }

    char buff[BUFF_SIZE_SHORT];
    string line;

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int num_labels = atoi(buff);
    
    for (int i = 0; i < num_labels; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;

	strtokenizer tok(line, " \t\r\n");

	if (tok.count_tokens() != 2) {
	    continue;
	}
	
	plbs2i->insert(pair<string, int>(tok.token(0), atoi(tok.token(1).c_str())));
	plbi2s->insert(pair<int, string>(atoi(tok.token(1).c_str()), tok.token(0)));
    }        
    
    // update the number of labels in option variable
    if (popt) {
	popt->num_labels = plbs2i->size();
    }

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);	// read the line ###...    

    if (popt->order == SECOND_ORDER) {
	maplbstr2int::iterator lbmapit;    
	lbmapit = plbs2i->find(popt->lfostr);
	if (lbmapit != plbs2i->end()) {
	    popt->lfo = lbmapit->second;
	}    
    }
}

void data::write_lb_map(FILE * fout) {
    maplbstr2int::iterator maplbit;
    
    fprintf(fout, "%d\n", plbs2i->size());
    
    for (maplbit = plbs2i->begin(); maplbit != plbs2i->end(); maplbit++) {
	fprintf(fout, "%s %d\n", (maplbit->first).c_str(), maplbit->second);
	// out << maplbit->first << " " << maplbit->second << endl;
    }
    
    fprintf(fout, "##################################################\n");
}

void data::read_2order_lb_map(FILE * fin) {
    if (plb2to1s2i) {
	plb2to1s2i->clear();
    } else {
	plb2to1s2i = new maplbstr2int;
    }
    
    char buff[BUFF_SIZE_SHORT];
    string line;

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int num_2order_labels = atoi(buff);
    
    for (int i = 0; i < num_2order_labels; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;

	strtokenizer tok(line, " \t\r\n");

	if (tok.count_tokens() != 2) {
	    continue;
	}
	
	plb2to1s2i->insert(pair<string, int>(tok.token(0), atoi(tok.token(1).c_str())));
    }        
    
    if (popt->order == SECOND_ORDER) {
	popt->num_2orderlabels = plb2to1s2i->size();
    }

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);	// read the line ###...
}

void data::write_2order_lb_map(FILE * fout) {
    maplbstr2int::iterator maplbit;
    
    fprintf(fout, "%d\n", plb2to1s2i->size());
    
    for (maplbit = plb2to1s2i->begin(); maplbit != plb2to1s2i->end(); maplbit++) {
	fprintf(fout, "%s %d\n", (maplbit->first).c_str(), maplbit->second);
	// out << maplbit->first << " " << maplbit->second << endl;
    }
    
    fprintf(fout, "##################################################\n");
}

void data::read_2to1_lb_map(FILE * fin) {
    if (plb2to1) {
	plb2to1->clear();
    } else {
	plb2to1 = new maplb2to1order;
    }
    
    char buff[BUFF_SIZE_SHORT];
    string line;

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int num_2order_labels = atoi(buff);
    
    for (int i = 0; i < num_2order_labels; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;

	strtokenizer tok(line, " \t\r\n");

	if (tok.count_tokens() != 3) {
	    continue;
	}

	pair<int, int> lbs1order(atoi(tok.token(1).c_str()), atoi(tok.token(2).c_str()));	
	plb2to1->insert(pair<int, pair<int, int> >(atoi(tok.token(0).c_str()), lbs1order));
    }        

    fgets(buff, BUFF_SIZE_SHORT - 1, fin);	// read the line ###...
}

void data::write_2to1_lb_map(FILE * fout) {
    maplb2to1order::iterator maplbit;
    
    fprintf(fout, "%d\n", plb2to1->size());
    
    for (maplbit = plb2to1->begin(); maplbit != plb2to1->end(); maplbit++) {
	fprintf(fout, "%d %d %d\n", maplbit->first, maplbit->second.first, maplbit->second.second);
	// out << maplbit->first << " " << maplbit->second.first << " " << maplbit->second.second << endl;
    }
    
    fprintf(fout, "##################################################\n");
}

void data::read_trndata(FILE * fin) {
    if (pcps2i) {
	pcps2i->clear();
    } else {
	pcps2i = new mapcpstr2int;
    }
    
    if (pcpi2s) {
	pcpi2s->clear();
    } else {
	pcpi2s = new mapcpint2str;
    }

    if (plbs2i) {
	plbs2i->clear();
    } else {
	plbs2i = new maplbstr2int;
    }
    
    if (plbi2s) {
	plbi2s->clear();
    } else {
	plbi2s = new maplbint2str;
    }
    
    if (plb2to1s2i) {
	plb2to1s2i->clear();
    } else {
	plb2to1s2i = new maplbstr2int;
    }
    
    if (plb2to1) {
	plb2to1->clear();
    } else {
	plb2to1 = new maplb2to1order;
    }

    if (ptrndata) {
	ptrndata->clear();
    } else {
	ptrndata = new dataset;
    }   
    
    mapcpstr2int::iterator cpmapit;
    maplbstr2int::iterator lbmapit;
    
    sequence seq;
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // read line
    while (fgets(buff, BUFF_SIZE_LONG - 1, fin)) {
	line = buff;
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		// finishing reading the current sequence
		ptrndata->push_back(seq);
	    }
	    // clear to store the next sequence
	    seq.clear();
	    continue;
	}	
	
	// new data observation
	obsr ob;
	
	lbmapit = plbs2i->find(tok.token(len - 1));	
	if (lbmapit == plbs2i->end()) {
	    // label not found, insert it to the map
	    ob.label = plbs2i->size();
	    plbs2i->insert(pair<string, int>(tok.token(len - 1), ob.label));
	    plbi2s->insert(pair<int, string>(ob.label, tok.token(len - 1)));
	} else {
	    ob.label = lbmapit->second;
	}
	
	for (int i = 0; i < len - 1; i++) {
	    cpmapit = pcps2i->find(tok.token(i));
	    if (cpmapit == pcps2i->end()) {
		// context predicate not found, insert it to the map
		ob.cps.push_back(pcps2i->size());
		pcps2i->insert(pair<string, int>(tok.token(i), pcps2i->size()));
		pcpi2s->insert(pair<int, string>(pcpi2s->size(), tok.token(i)));
	    } else {
		ob.cps.push_back(cpmapit->second);
	    }
	}	
	
	// push the observation into the training data
	seq.push_back(ob);
    }    
    
    // for the case of second-order Markov CRFs
    if (popt->order == SECOND_ORDER) {
	lbmapit = plbs2i->find(popt->lfostr);
	if (lbmapit != plbs2i->end()) {
	    popt->lfo = lbmapit->second;
	}
	
	if (popt->lfo < 0) {
    	    int lfo = plbs2i->size();
	    string lfostr = FIRST_OBSR_LABEL;
	
	    plbs2i->insert(pair<string, int>(lfostr, lfo));
	    plbi2s->insert(pair<int, string>(lfo, lfostr));
	}
	
	char buff[50];
	string lb2orderstr;
	int count = 0;
	for (int i = 0; i < plbs2i->size(); i++) {
	    for (int j = 0; j < plbs2i->size(); j++) {
		sprintf(buff, "%d-%d", i, j);
		lb2orderstr = buff;
		plb2to1s2i->insert(pair<string, int>(lb2orderstr, count));
		
		pair<int, int> lbs1order(i, j);
		plb2to1->insert(pair<int, pair<int, int> >(count, lbs1order));
				
		count++;
	    }
	}
    }
    
    // update the number of labels and the number of training 
    // data sequences in option variable
    if (popt) {
	popt->num_labels = plbs2i->size();
	popt->num_cps = pcps2i->size();
	popt->num_trnseqs = ptrndata->size();
	
	if (popt->order == SECOND_ORDER) {
	    popt->num_2orderlabels = plb2to1s2i->size();
	}
    }    
    
    // assign second-order label for data observations
    if (popt->order == SECOND_ORDER) {
	dataset::iterator datait;
	sequence::iterator seqit, pre_seqit;	
	maplbstr2int::iterator mapit;

	char buff[50];
	string lb2orderstr;

        for (datait = ptrndata->begin(); datait != ptrndata->end(); datait++) {
	    int count = 0;
	    seqit = datait->begin();
	    while (seqit != datait->end()) {
		if (!count) {
		    if (popt->lfo < 0) {
			sprintf(buff, "%d-%d", plbs2i->size() - 1, seqit->label);
		    } else {
			sprintf(buff, "%d-%d", popt->lfo, seqit->label);
		    }
		} else {
		    sprintf(buff, "%d-%d", pre_seqit->label, seqit->label);
		}
		lb2orderstr = buff;
		
		mapit = plb2to1s2i->find(lb2orderstr);
		if (mapit != plb2to1s2i->end()) {
		    seqit->label2order = mapit->second;
		} else {
		    printf("Error while lookup second-order labels!!!\n");
		}
	    
		count++;
		pre_seqit = seqit;
		seqit++;
	    }
	}
    }
}

void data::read_tstdata(FILE * fin) {
    if (ptstdata) {
	ptstdata->clear();
    } else {
	ptstdata = new dataset;
    }   
    
    mapcpstr2int::iterator cpmapit;
    maplbstr2int::iterator lbmapit;
    
    sequence seq;
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // read line
    while (fgets(buff, BUFF_SIZE_LONG - 1, fin)) {
	line = buff;
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		// finishing reading the current sequence
		ptstdata->push_back(seq);
	    }
	    // clear to store the next sequence
	    seq.clear();
	    continue;
	}	
	
	// new data observation
	obsr ob;
	
	lbmapit = plbs2i->find(tok.token(len - 1));	
	if (lbmapit == plbs2i->end()) {
	    // label not found, do nothing
	} else {
	    ob.label = lbmapit->second;
	}
	
	for (int i = 0; i < len - 1; i++) {
	    cpmapit = pcps2i->find(tok.token(i));
	    if (cpmapit == pcps2i->end()) {
		// context predicate not found, insert to the map
		ob.cps.push_back(pcps2i->size() + CONST);
		pcps2i->insert(pair<string, int>(tok.token(i), pcps2i->size() + CONST));
		pcpi2s->insert(pair<int, string>(pcpi2s->size() + CONST, tok.token(i)));
	    } else {
		ob.cps.push_back(cpmapit->second);
	    }
	}	
	
	// push the observation into the testing data
	if (ob.label >= 0) {
	    seq.push_back(ob);
	}
    }    

    // update the number of testing sequences in option variable
    if (popt) {
	popt->num_tstseqs = ptstdata->size();
    }
}

void data::read_ulbdata(FILE * fin) {
    if (pulbdata) {
	pulbdata->clear();
    } else {
	pulbdata = new dataset;
    }   
    
    mapcpstr2int::iterator cpmapit;
    maplbstr2int::iterator lbmapit;
    
    sequence seq;
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // read line
    while (fgets(buff, BUFF_SIZE_LONG - 1, fin)) {
	line = buff;
	strtokenizer tok(line, " \t\r\n");
	int len = tok.count_tokens();
	
	if (len <= 0) {
	    if (seq.size() > 0) {
		// finishing reading the current sequence
		pulbdata->push_back(seq);
	    }
	    // clear to store the next sequence
	    seq.clear();
	    continue;
	}	
	
	// new data observation
	obsr ob;
	
	for (int i = 0; i < len; i++) {
	    cpmapit = pcps2i->find(tok.token(i));
	    if (cpmapit == pcps2i->end()) {
		// if context predicate not found, insert to the map
		ob.cps.push_back(pcps2i->size() + CONST);
		pcps2i->insert(pair<string, int>(tok.token(i), pcps2i->size() + CONST));
		pcpi2s->insert(pair<int, string>(pcpi2s->size() + CONST, tok.token(i)));
	    } else {
		ob.cps.push_back(cpmapit->second);
	    }
	}	
	
	// push the observation into the unlabeled data sequence
	seq.push_back(ob);
    }    

    // update the number of unlabeled data sequences in option variable
    if (popt) {
	popt->num_ulbseqs = pulbdata->size();
    }
}

void data::write_tstdata(FILE * fout) {
    dataset::iterator datait;
    sequence::iterator seqit;
    
    for (datait = ptstdata->begin(); datait != ptstdata->end(); datait++) {
	double bestpathval = -1.0;
	for (seqit = datait->begin(); seqit != datait->end(); seqit++) {
	    if (seqit == datait->begin() && seqit->pnbestinfo) {
		bestpathval = seqit->pnbestinfo->pathvals[0];
	    }
	
	    seqit->display_tstobsr(*pcpi2s, *plbi2s, fout, bestpathval);
	    fprintf(fout, "\n");
	}
	fprintf(fout, "\n");
    }
}

void data::write_ulbdata(FILE * fout) {
    dataset::iterator datait;
    sequence::iterator seqit;
    
    for (datait = pulbdata->begin(); datait != pulbdata->end(); datait++) {
	for (seqit = datait->begin(); seqit != datait->end(); seqit++) {
	    seqit->display_ulbobsr(*pcpi2s, *plbi2s, fout);
	    fprintf(fout, "\n");
	}
	fprintf(fout, "\n");
    }
}

void data::cp_prune(dictionary * pdict) {
    dataset::iterator datait;
    sequence::iterator seqit;
    vector<int>::iterator cpit;
    map<int, element>::iterator dictit;
    
    for (datait = ptrndata->begin(); datait != ptrndata->end(); datait++) {
	for (seqit = datait->begin(); seqit != datait->end(); seqit++) {
	    cpit = seqit->cps.begin();
	    while (cpit != seqit->cps.end()) {
		dictit = pdict->dict.find(*cpit);
		if (dictit != pdict->dict.end()) {
		    if (dictit->second.chosen) {
			cpit++;
			continue;
		    } else {
			// delete context predicates
			pdict->dict.erase(dictit);
		    }
		} 
		// delete context predicate in the training data
		cpit = seqit->cps.erase(cpit);
	    }
	}
    }
    
    // update the number of context predicates after pruning
    if (popt) {
	popt->num_cps = pdict->dict.size();
    }
}

