/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * model.cpp - this file is part of FlexCRFs.
 *
 * Begin:	Oct. 31, 2005
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
#include "../../../include/evaluation.h"

using namespace std;

// call this to compute precision, recall, and F1-measure
double evaluation::evaluate(FILE * fout) {
    if (!(pdata->ptstdata)) {
	// return if no testing data available
	return 0.0;
    }    
    
    int num_labels = popt->num_labels;
    if (popt->order == SECOND_ORDER && popt->lfo < 0) {
	num_labels--;
    }

    int i; 
    int len = human_lb_count.size();
    if (len < num_labels) {
	for (i = 0; i < num_labels - len; i++) {	
	    human_lb_count.push_back(0);
	}
    } else {
	for (i = 0; i < num_labels; i++) {
	    human_lb_count[i] = 0;
	}
    }
    
    len = model_lb_count.size();
    if (len < num_labels) {
	for (i = 0; i < num_labels - len; i++) {
	    model_lb_count.push_back(0);
	}
    } else {
	for (i = 0; i < num_labels; i++) {
	    model_lb_count[i] = 0;
	}
    }

    len = human_model_lb_count.size();
    if (len < num_labels) {
	for (i = 0; i < num_labels - len; i++) {
	    human_model_lb_count.push_back(0);
	}
    } else {
	for (i = 0; i < num_labels; i++) {
	    human_model_lb_count[i] = 0;
	}
    }
    
    // start to count
    dataset::iterator datait;
    sequence::iterator seqit;    
    for (datait = pdata->ptstdata->begin(); datait != pdata->ptstdata->end(); datait++) {
	for (seqit = datait->begin(); seqit != datait->end(); seqit++) {
	    human_lb_count[seqit->label]++;
	    model_lb_count[seqit->model_label]++;
	    
	    if (seqit->label == seqit->model_label) {
		human_model_lb_count[seqit->label]++;
	    }
	}
    }
    
    // print out    
    printf("\tLabel-based performance evaluation:\n\n");
    printf("\t\tLabel\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\tLabel-based performance evaluation:\n\n");
	fprintf(fout, "\t\tLabel\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }
    int count = 0;
    double precision = 0.0, recall = 0.0, f1, total1_pre = 0.0, 
	   total1_rec = 0.0, total1_f1 = 0.0, total2_pre = 0.0, 
	   total2_rec = 0.0, total2_f1 = 0.0;
    int total_human = 0, total_model = 0, total_match = 0;
    for (i = 0; i < num_labels; i++) {
	if (model_lb_count[i] > 0) {
	    precision = (double)human_model_lb_count[i] / model_lb_count[i];
	    total_model += model_lb_count[i];
	    total1_pre += precision;
	} else {
	    precision = 0.0;
	}
	if (human_lb_count[i] > 0) {
	    recall = (double)human_model_lb_count[i] / human_lb_count[i];
	    total_human += human_lb_count[i];
	    total1_rec += recall;
	    count++;
	} else {
	    recall = 0.0;
	}
	
	total_match += human_model_lb_count[i];
	
	if (recall + precision > 0) {
	    f1 = (double) 2 * precision * recall / (precision + recall);
	} else {
	    f1 = 0;
	}
	
	char buff[50];
	sprintf(buff, "%d", i);
	
	maplbint2str::iterator lbmapit;
	lbmapit = pdata->plbi2s->find(i);
	if (lbmapit != pdata->plbi2s->end()) {
	    sprintf(buff, "%s", lbmapit->second.c_str());
	}
	
	printf("\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
	      buff, human_lb_count[i], model_lb_count[i], human_model_lb_count[i],
	      precision * 100, recall * 100, f1 * 100);	      
	if (popt->is_logging) {
	    fprintf(fout, "\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
		buff, human_lb_count[i], model_lb_count[i], human_model_lb_count[i],
		precision * 100, recall * 100, f1 * 100);	      
	}
    }
    
    total1_pre /= count;
    total1_rec /= count;
    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);

    // print the average performance    
    total2_pre = (double)total_match / total_model;
    total2_rec = (double)total_match / total_human;
    total2_f1 = 2 * total2_pre * total2_rec / (total2_pre + total2_rec);
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n", total1_pre * 100, total1_rec * 100,
	    total1_f1 * 100);
    printf("\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n", total_human, total_model, total_match,
	    total2_pre * 100, total2_rec * 100, total2_f1 * 100);
    if (popt->is_logging) {
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
	fprintf(fout, "\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n", total1_pre * 100, total1_rec * 100,
	    total1_f1 * 100);
	fprintf(fout, "\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n", total_human, total_model, total_match,
	    total2_pre * 100, total2_rec * 100, total2_f1 * 100);
    }
    
    if (popt->chunk_evaluate_during_training) {
	double chunk_total2_f1;

	if (popt->chunktype == IOB1) {
	    chunk_total2_f1 = chunk_evaluate_iob1(fout);
	}
	if (popt->chunktype == IOB2) {
	    chunk_total2_f1 = chunk_evaluate_iob2(fout);
	}
	if (popt->chunktype == IOE1) {
	    chunk_total2_f1 = chunk_evaluate_ioe1(fout);
	}
	if (popt->chunktype == IOE2) {
	    chunk_total2_f1 = chunk_evaluate_ioe2(fout);
	}

	return chunk_total2_f1;
    }

    return total2_f1 * 100;
}

//================================================================

double evaluation::chunk_evaluate_iob2(FILE * fout) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;
    int i;

    int num_chunks = popt->chunks.size();
    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    for (i = 0; i < num_chunks; i++) {
	human_chk_count[i] = count_chunks_iob2(1, popt->chunks[i][0], popt->chunks[i][1]);
	model_chk_count[i] = count_chunks_iob2(2, popt->chunks[i][0], popt->chunks[i][1]);
	match_chk_count[i] = count_matching_chunks_iob2(popt->chunks[i][0], popt->chunks[i][1]);
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\tChunk-based performance evaluation:\n\n");
	fprintf(fout, "\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    int count = 0;
    double pre = 0.0, rec = 0.0, f1 = 0.0;
    double total1_pre = 0.0, total1_rec = 0.0, total1_f1 = 0.0;
    double total2_pre = 0.0, total2_rec = 0.0, total2_f1 = 0.0;
    int total_human = 0, total_model = 0, total_match = 0;

    for (i = 0; i < num_chunks; i++) {
	if (model_chk_count[i] > 0) {
	    pre = (double)match_chk_count[i] / model_chk_count[i];
	    total_model += model_chk_count[i];
	    total1_pre += pre;
	} else {
	    pre = 0.0;
	}

	if (human_chk_count[i] > 0) {
	    rec = (double)match_chk_count[i] / human_chk_count[i];
	    total_human += human_chk_count[i];
	    total1_rec += rec;
	    count++;
	} else {
	    rec = 0.0;
	}

	total_match += match_chk_count[i];

	if (pre + rec > 0) {
	    f1 = (double) 2 * pre * rec / (pre + rec);
	} else {
	    f1 = 0.0;
	}

	printf("\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
	       popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
		popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
		match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	}
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
		total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	}       
    }
    
    if (total_model > 0) {
	total2_pre = (double)total_match / total_model;
    }
    if (total_human > 0) {
	total2_rec = (double)total_match / total_human;
    }
    if (total2_pre + total2_rec > 0) {
	total2_f1 = 2 * total2_rec * total2_pre / (total2_rec + total2_pre);
    }
    
    printf("\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	   total_human, total_model, total_match,
	   total2_pre * 100, total2_rec * 100, total2_f1 * 100);

    if (popt->is_logging) {
	fprintf(fout, "\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	    total_human, total_model, total_match,
	    total2_pre * 100, total2_rec * 100, total2_f1 * 100);
    }
    
    return total2_f1 * 100;
}

// is start of a chunk (IOB2)?
int evaluation::is_start_of_chunk_iob2(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	return (seq[i].label == atoi(b_tag.c_str()));
	
    } else if (human_model == 2) {
	return (seq[i].model_label == atoi(b_tag.c_str()));
	
    } else {
	return 0;
    }
}

// is end of a chunk (IOB2)?
int evaluation::is_end_of_chunk_iob2(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	if (seq[i].label == atoi(b_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i].label == atoi(i_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}	
    
    } else if (human_model == 2) {
	if (seq[i].model_label == atoi(b_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i].model_label == atoi(i_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}	
    
    } else {
	return 0;
    }
}

// counting number of chunks (IOB2)
int evaluation::count_chunks_iob2(int human_model, string b_tag, string i_tag) {
    int count = 0;
    dataset::iterator dtit;
    sequence::iterator seqit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	seqit = dtit->begin();
	int i = 0;
	while (seqit != dtit->end()) {
	    if (human_model == 1 && is_start_of_chunk_iob2(1, i, *dtit, b_tag, i_tag)) {
		count++;
	    }
	    if (human_model == 2 && is_start_of_chunk_iob2(2, i, *dtit, b_tag, i_tag)) {
		count++;
	    }

	    seqit++;
	    i++;
	}
    }        
    
    return count;
}

// is matching chunk (IOB2)? 
int evaluation::is_matching_chunk_iob2(int i, sequence & seq, string b_tag, string i_tag) {
    if (!is_start_of_chunk_iob2(1, i, seq, b_tag, i_tag) || 
	    !is_start_of_chunk_iob2(2, i, seq, b_tag, i_tag)) {
	return 0;
    }
    
    int len = seq.size();
    int j = i, k = i;
    while (j < len) {
	if (is_end_of_chunk_iob2(1, j, seq, b_tag, i_tag)) {
	    break;
	} else {
	    j++;
	}
    }

    while (k < len) {
	if (is_end_of_chunk_iob2(2, k, seq, b_tag, i_tag)) {
	    break;
	} else {
	    k++;
	}
    }

    return (j == k);
}

// counting matching chunks (IOB2)
int evaluation::count_matching_chunks_iob2(string b_tag, string i_tag) {
    int count = 0;
    dataset::iterator dtit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	int i = 0, len = dtit->size();
	while (i < len) {
	    if (is_start_of_chunk_iob2(1, i, *dtit, b_tag, i_tag)) {
		if (is_matching_chunk_iob2(i, *dtit, b_tag, i_tag)) {
		    count++;
		}
	    }

	    i++;
	}
    }
    
    return count;
}

//==============================================================

double evaluation::chunk_evaluate_iob1(FILE * fout) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;
    int i;

    int num_chunks = popt->chunks.size();
    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    for (i = 0; i < num_chunks; i++) {
	human_chk_count[i] = count_chunks_iob1(1, popt->chunks[i][0], popt->chunks[i][1]);
	model_chk_count[i] = count_chunks_iob1(2, popt->chunks[i][0], popt->chunks[i][1]);
	match_chk_count[i] = count_matching_chunks_iob1(popt->chunks[i][0], popt->chunks[i][1]);
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\tChunk-based performance evaluation:\n\n");
	fprintf(fout, "\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    int count = 0;
    double pre = 0.0, rec = 0.0, f1 = 0.0;
    double total1_pre = 0.0, total1_rec = 0.0, total1_f1 = 0.0;
    double total2_pre = 0.0, total2_rec = 0.0, total2_f1 = 0.0;
    int total_human = 0, total_model = 0, total_match = 0;

    for (i = 0; i < num_chunks; i++) {
	if (model_chk_count[i] > 0) {
	    pre = (double)match_chk_count[i] / model_chk_count[i];
	    total_model += model_chk_count[i];
	    total1_pre += pre;
	} else {
	    pre = 0.0;
	}

	if (human_chk_count[i] > 0) {
	    rec = (double)match_chk_count[i] / human_chk_count[i];
	    total_human += human_chk_count[i];
	    total1_rec += rec;
	    count++;
	} else {
	    rec = 0.0;
	}

	total_match += match_chk_count[i];

	if (pre + rec > 0) {
	    f1 = (double) 2 * pre * rec / (pre + rec);
	} else {
	    f1 = 0.0;
	}

	printf("\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
	       popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
		popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
		match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	}
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
		total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	}       
    }
    
    if (total_model > 0) {
	total2_pre = (double)total_match / total_model;
    }
    if (total_human > 0) {
	total2_rec = (double)total_match / total_human;
    }
    if (total2_pre + total2_rec > 0) {
	total2_f1 = 2 * total2_rec * total2_pre / (total2_rec + total2_pre);
    }
    
    printf("\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	   total_human, total_model, total_match,
	   total2_pre * 100, total2_rec * 100, total2_f1 * 100);

    if (popt->is_logging) {
	fprintf(fout, "\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	    total_human, total_model, total_match,
	    total2_pre * 100, total2_rec * 100, total2_f1 * 100);
    }
    
    return total2_f1 * 100;
}

// is start of a chunk (IOB1)?
int evaluation::is_start_of_chunk_iob1(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	if (seq[i].label == atoi(b_tag.c_str())) {
	    return 1;
	    
	} else if (seq[i].label == atoi(i_tag.c_str())) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1].label != atoi(i_tag.c_str()) && 
			seq[i - 1].label != atoi(b_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else if (human_model == 2) {
	if (seq[i].model_label == atoi(b_tag.c_str())) {
	    return 1;
	    
	} else if (seq[i].model_label == atoi(i_tag.c_str())) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1].model_label != atoi(i_tag.c_str()) && 
			seq[i - 1].model_label != atoi(b_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
    
    } else {
	return 0;
    }
}

// is end of a chunk (IOB1)?
int evaluation::is_end_of_chunk_iob1(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	if (seq[i].label == atoi(b_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i].label == atoi(i_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}	
    
    } else if (human_model == 2) {
	if (seq[i].model_label == atoi(b_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i].model_label == atoi(i_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}	
    
    } else {
	return 0;
    }
}

// counting number of chunks (IOB1)
int evaluation::count_chunks_iob1(int human_model, string b_tag, string i_tag) {
    int count = 0;
    dataset::iterator dtit;
    sequence::iterator seqit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	seqit = dtit->begin();
	int i = 0;
	while (seqit != dtit->end()) {
	    if (human_model == 1 && is_start_of_chunk_iob1(1, i, *dtit, b_tag, i_tag)) {
		count++;
	    }
	    if (human_model == 2 && is_start_of_chunk_iob1(2, i, *dtit, b_tag, i_tag)) {
		count++;
	    }
	    
	    seqit++;
	    i++;
	}
    }        
    
    return count;
}

// is matching chunk (IOB1)? 
int evaluation::is_matching_chunk_iob1(int i, sequence & seq, string b_tag, string i_tag) {
    if (!is_start_of_chunk_iob1(1, i, seq, b_tag, i_tag) || 
	    !is_start_of_chunk_iob1(2, i, seq, b_tag, i_tag)) {
	return 0;
    }
    
    int len = seq.size();
    int j = i, k = i;
    while (j < len) {
	if (is_end_of_chunk_iob1(1, j, seq, b_tag, i_tag)) {
	    break;
	} else {
	    j++;
	}
    }

    while (k < len) {
	if (is_end_of_chunk_iob1(2, k, seq, b_tag, i_tag)) {
	    break;
	} else {
	    k++;
	}
    }

    return (j == k);
}

// counting matching chunks (IOB1)
int evaluation::count_matching_chunks_iob1(string b_tag, string i_tag) {
    int count = 0;
    dataset::iterator dtit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	int i = 0, len = dtit->size();
	while (i < len) {
	    if (is_start_of_chunk_iob1(1, i, *dtit, b_tag, i_tag)) {
		if (is_matching_chunk_iob1(i, *dtit, b_tag, i_tag)) {
		    count++;
		}
	    }

	    i++;
	}
    }
    
    return count;
}

//==================================================================

double evaluation::chunk_evaluate_ioe2(FILE * fout) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;
    int i;

    int num_chunks = popt->chunks.size();
    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    for (i = 0; i < num_chunks; i++) {
	human_chk_count[i] = count_chunks_ioe2(1, popt->chunks[i][0], popt->chunks[i][1]);
	model_chk_count[i] = count_chunks_ioe2(2, popt->chunks[i][0], popt->chunks[i][1]);
	match_chk_count[i] = count_matching_chunks_ioe2(popt->chunks[i][0], popt->chunks[i][1]);
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\tChunk-based performance evaluation:\n\n");
	fprintf(fout, "\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    int count = 0;
    double pre = 0.0, rec = 0.0, f1 = 0.0;
    double total1_pre = 0.0, total1_rec = 0.0, total1_f1 = 0.0;
    double total2_pre = 0.0, total2_rec = 0.0, total2_f1 = 0.0;
    int total_human = 0, total_model = 0, total_match = 0;

    for (i = 0; i < num_chunks; i++) {
	if (model_chk_count[i] > 0) {
	    pre = (double)match_chk_count[i] / model_chk_count[i];
	    total_model += model_chk_count[i];
	    total1_pre += pre;
	} else {
	    pre = 0.0;
	}

	if (human_chk_count[i] > 0) {
	    rec = (double)match_chk_count[i] / human_chk_count[i];
	    total_human += human_chk_count[i];
	    total1_rec += rec;
	    count++;
	} else {
	    rec = 0.0;
	}

	total_match += match_chk_count[i];

	if (pre + rec > 0) {
	    f1 = (double) 2 * pre * rec / (pre + rec);
	} else {
	    f1 = 0.0;
	}

	printf("\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
	       popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
		popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
		match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	}
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
		total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	}       
    }
    
    if (total_model > 0) {
	total2_pre = (double)total_match / total_model;
    }
    if (total_human > 0) {
	total2_rec = (double)total_match / total_human;
    }
    if (total2_pre + total2_rec > 0) {
	total2_f1 = 2 * total2_rec * total2_pre / (total2_rec + total2_pre);
    }
    
    printf("\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	   total_human, total_model, total_match,
	   total2_pre * 100, total2_rec * 100, total2_f1 * 100);

    if (popt->is_logging) {
	fprintf(fout, "\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	    total_human, total_model, total_match,
	    total2_pre * 100, total2_rec * 100, total2_f1 * 100);
    }
    
    return total2_f1 * 100;
}

// is start of a chunk (IOE2)?
int evaluation::is_start_of_chunk_ioe2(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	if (seq[i].label == atoi(e_tag.c_str())) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i].label == atoi(i_tag.c_str())) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else if (human_model == 2) {
	if (seq[i].model_label == atoi(e_tag.c_str())) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i].model_label == atoi(i_tag.c_str())) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else {
	return 0;
    }
}

// is end of a chunk (IOE2)?
int evaluation::is_end_of_chunk_ioe2(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	return (seq[i].label == atoi(e_tag.c_str()));
    
    } else if (human_model == 2) {
	return (seq[i].model_label == atoi(e_tag.c_str()));
    
    } else {
	return 0;
    }
}

// counting number of chunks (IOE2)
int evaluation::count_chunks_ioe2(int human_model, string i_tag, string e_tag) {
    int count = 0;
    dataset::iterator dtit;
    sequence::iterator seqit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	seqit = dtit->begin();
	int i = 0;
	while (seqit != dtit->end()) {
	    if (human_model == 1 && is_start_of_chunk_ioe2(1, i, *dtit, i_tag, e_tag)) {
		count++;
	    }
	    if (human_model == 2 && is_start_of_chunk_ioe2(2, i, *dtit, i_tag, e_tag)) {
		count++;
	    }

	    seqit++;
	    i++;
	}
    }        
    
    return count;
}

// is matching chunk (IOE2)? 
int evaluation::is_matching_chunk_ioe2(int i, sequence & seq, string i_tag, string e_tag) {
    if (!is_start_of_chunk_ioe2(1, i, seq, i_tag, e_tag) || 
	    !is_start_of_chunk_ioe2(2, i, seq, i_tag, e_tag)) {
	return 0;
    }
    
    int len = seq.size();
    int j = i, k = i;
    while (j < len) {
	if (is_end_of_chunk_ioe2(1, j, seq, i_tag, e_tag)) {
	    break;
	} else {
	    j++;
	}
    }

    while (k < len) {
	if (is_end_of_chunk_ioe2(2, k, seq, i_tag, e_tag)) {
	    break;
	} else {
	    k++;
	}
    }

    return (j == k);
}

// counting matching chunks (IOE2)
int evaluation::count_matching_chunks_ioe2(string i_tag, string e_tag) {
    int count = 0;
    dataset::iterator dtit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	int i = 0, len = dtit->size();
	while (i < len) {
	    if (is_start_of_chunk_ioe2(1, i, *dtit, i_tag, e_tag)) {
		if (is_matching_chunk_ioe2(i, *dtit, i_tag, e_tag)) {
		    count++;
		}
	    }

	    i++;
	}
    }
    
    return count;
}

//======================================================================

double evaluation::chunk_evaluate_ioe1(FILE * fout) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;
    int i;

    int num_chunks = popt->chunks.size();
    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    for (i = 0; i < num_chunks; i++) {
	human_chk_count[i] = count_chunks_ioe1(1, popt->chunks[i][0], popt->chunks[i][1]);
	model_chk_count[i] = count_chunks_ioe1(2, popt->chunks[i][0], popt->chunks[i][1]);
	match_chk_count[i] = count_matching_chunks_ioe1(popt->chunks[i][0], popt->chunks[i][1]);
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\tChunk-based performance evaluation:\n\n");
	fprintf(fout, "\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    int count = 0;
    double pre = 0.0, rec = 0.0, f1 = 0.0;
    double total1_pre = 0.0, total1_rec = 0.0, total1_f1 = 0.0;
    double total2_pre = 0.0, total2_rec = 0.0, total2_f1 = 0.0;
    int total_human = 0, total_model = 0, total_match = 0;

    for (i = 0; i < num_chunks; i++) {
	if (model_chk_count[i] > 0) {
	    pre = (double)match_chk_count[i] / model_chk_count[i];
	    total_model += model_chk_count[i];
	    total1_pre += pre;
	} else {
	    pre = 0.0;
	}

	if (human_chk_count[i] > 0) {
	    rec = (double)match_chk_count[i] / human_chk_count[i];
	    total_human += human_chk_count[i];
	    total1_rec += rec;
	    count++;
	} else {
	    rec = 0.0;
	}

	total_match += match_chk_count[i];

	if (pre + rec > 0) {
	    f1 = (double) 2 * pre * rec / (pre + rec);
	} else {
	    f1 = 0.0;
	}

	printf("\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
	       popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
		popt->chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
		match_chk_count[i], pre * 100, rec * 100, f1 * 100);
	}
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    if (popt->is_logging) {
	fprintf(fout, "\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");
    }

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	if (popt->is_logging) {
	    fprintf(fout, "\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
		total1_pre * 100, total1_rec * 100, total1_f1 * 100);
	}       
    }
    
    if (total_model > 0) {
	total2_pre = (double)total_match / total_model;
    }
    if (total_human > 0) {
	total2_rec = (double)total_match / total_human;
    }
    if (total2_pre + total2_rec > 0) {
	total2_f1 = 2 * total2_rec * total2_pre / (total2_rec + total2_pre);
    }
    
    printf("\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	   total_human, total_model, total_match,
	   total2_pre * 100, total2_rec * 100, total2_f1 * 100);

    if (popt->is_logging) {
	fprintf(fout, "\t\tAvg2.\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n\n",
	    total_human, total_model, total_match,
	    total2_pre * 100, total2_rec * 100, total2_f1 * 100);
    }
    
    return total2_f1 * 100;
}

// is start of a chunk (IOE1)?
int evaluation::is_start_of_chunk_ioe1(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	if (seq[i].label == atoi(e_tag.c_str())) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i].label == atoi(i_tag.c_str())) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1].label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else if (human_model == 2) {
	if (seq[i].model_label == atoi(e_tag.c_str())) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i].model_label == atoi(i_tag.c_str())) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1].model_label != atoi(i_tag.c_str())) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else {
	return 0;
    }
}

// is end of a chunk (IOE1)?
int evaluation::is_end_of_chunk_ioe1(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	if (seq[i].label == atoi(e_tag.c_str())) {
	    return 1;
	    
	} else if (seq[i].label == atoi(i_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].label == atoi(e_tag.c_str()) ||
			seq[i + 1].label == atoi(i_tag.c_str())) {
		    return 0;
		} else {
		    return 1;
		}
	    }
		
	} else {
	    return 0;
	}
    
    } else if (human_model == 2) {
	if (seq[i].model_label == atoi(e_tag.c_str())) {
	    return 1;
	    
	} else if (seq[i].model_label == atoi(i_tag.c_str())) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1].model_label == atoi(e_tag.c_str()) ||
			seq[i + 1].model_label == atoi(i_tag.c_str())) {
		    return 0;
		} else {
		    return 1;
		}
	    }
		
	} else {
	    return 0;
	}
    
    } else {
	return 0;
    }
}

// counting number of chunks (IOE1)
int evaluation::count_chunks_ioe1(int human_model, string i_tag, string e_tag) {
    int count = 0;
    dataset::iterator dtit;
    sequence::iterator seqit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	seqit = dtit->begin();
	int i = 0;
	while (seqit != dtit->end()) {
	    if (human_model == 1 && is_start_of_chunk_ioe1(1, i, *dtit, i_tag, e_tag)) {
		count++;
	    }
	    if (human_model == 2 && is_start_of_chunk_ioe1(2, i, *dtit, i_tag, e_tag)) {
		count++;
	    }

	    seqit++;
	    i++;
	}
    }        
    
    return count;
}

// is matching chunk (IOE1)? 
int evaluation::is_matching_chunk_ioe1(int i, sequence & seq, string i_tag, string e_tag) {
    if (!is_start_of_chunk_ioe1(1, i, seq, i_tag, e_tag) || 
	    !is_start_of_chunk_ioe1(2, i, seq, i_tag, e_tag)) {
	return 0;
    }
    
    int len = seq.size();
    int j = i, k = i;
    while (j < len) {
	if (is_end_of_chunk_ioe1(1, j, seq, i_tag, e_tag)) {
	    break;
	} else {
	    j++;
	}
    }

    while (k < len) {
	if (is_end_of_chunk_ioe1(2, k, seq, i_tag, e_tag)) {
	    break;
	} else {
	    k++;
	}
    }

    return (j == k);
}

// counting matching chunks (IOE1)
int evaluation::count_matching_chunks_ioe1(string i_tag, string e_tag) {
    int count = 0;
    dataset::iterator dtit;
    
    for (dtit = pdata->ptstdata->begin(); dtit != pdata->ptstdata->end(); dtit++) {
	int i = 0, len = dtit->size();
	while (i < len) {
	    if (is_start_of_chunk_ioe1(1, i, *dtit, i_tag, e_tag)) {
		if (is_matching_chunk_ioe1(i, *dtit, i_tag, e_tag)) {
		    count++;
		}
	    }

	    i++;
	}
    }
    
    return count;
}





