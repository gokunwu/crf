/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * evaluatechunk.cpp - this file is part of FlexCRFs.
 *
 * Begin:	Nov. 29, 2005
 * Last change:	Nov. 29, 2005
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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../../../include/evaluatechunk.h"

using namespace std;

int str_2_int(map<string, int> & str2int, string & label) {
    map<string, int>::iterator it;
    
    it = str2int.find(label);
    if (it != str2int.end()) {
	return it->second;
    }
    
    return -1;
}

string int_2_str(map<int, string> & int2str, int label) {
    string result = "";
    map<int, string>::iterator it;
    
    it = int2str.find(label);
    if (it != int2str.end()) {
	result = it->second;
    }
    
    return result;    
}

// call this to compute precision, recall, and F1-measure
void evaluate(dataset & data, string & chunktype, labelset & labels, chunkset & chunks) {
    map<string, int> lbstr2int;
    map<int, string> lbint2str;
    vector<int> human_lb_count, model_lb_count, human_model_lb_count;

    int i;
    int num_labels = labels.size();
    
    for (i = 0; i < num_labels; i++) {
	lbstr2int.insert(pair<string, int>(labels[i], i));
	lbint2str.insert(pair<int, string>(i, labels[i]));
	
	human_lb_count.push_back(0);
	model_lb_count.push_back(0);
	human_model_lb_count.push_back(0);
    }

    // start to count
    dataset::iterator datait;
    sequence::iterator seqit;    
    for (datait = data.begin(); datait != data.end(); datait++) {
	for (seqit = datait->begin(); seqit != datait->end(); seqit++) {
	    int label = str_2_int(lbstr2int, (*seqit)[seqit->size() - 2]);
	    int model_label = str_2_int(lbstr2int, (*seqit)[seqit->size() - 1]);
	    
	    if (label >= 0 && label < num_labels) {
		human_lb_count[label]++;
	    }
	    
	    if (model_label >= 0 && model_label < num_labels) {
		model_lb_count[model_label]++;
	    }
	    
	    if (label == model_label && label >= 0 && label < num_labels) {
		human_model_lb_count[label]++;
	    }
	}
    }
    
    // print out    
    printf("\tLabel-based performance evaluation:\n\n");
    printf("\t\tLabel\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

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
	string strlabel = int_2_str(lbint2str, i);
	if (strlabel != "") {	
	    sprintf(buff, "%s", strlabel.c_str());
	}
	
	printf("\t\t%s\t%d\t%d\t%d\t%6.2f\t%6.2f\t%6.2f\n",
	      buff, human_lb_count[i], model_lb_count[i], human_model_lb_count[i],
	      precision * 100, recall * 100, f1 * 100);	      
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

    if (chunks.size() <= 0) {
	return;
    }

    // chunk based evaluation
    if (chunktype == "IOB1") {
	chunk_evaluate_iob1(data, chunks);
    }
    if (chunktype == "IOB2") {
	chunk_evaluate_iob2(data, chunks);
    }
    if (chunktype == "IOE1") {
	chunk_evaluate_ioe1(data, chunks);
    }
    if (chunktype == "IOE2") {
	chunk_evaluate_ioe2(data, chunks);
    }
}

//================================================================

double chunk_evaluate_iob2(dataset & data, chunkset & chunks) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;
    
    int i;
    int num_chunks = chunks.size();
    
    for (i = 0; i < num_chunks; i++) {	
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    dataset::iterator datait;
    for (datait = data.begin(); datait != data.end(); datait++) {
	for (i = 0; i < num_chunks; i++) {
	    human_chk_count[i] += count_chunks_iob2(1, *datait, chunks[i][0], chunks[i][1]);
	    model_chk_count[i] += count_chunks_iob2(2, *datait, chunks[i][0], chunks[i][1]);
	    match_chk_count[i] += count_matching_chunks_iob2(*datait, chunks[i][0], chunks[i][1]);
	}
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

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
	       chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
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
    
    return total2_f1 * 100;
}

// is start of a chunk (IOB2)?
int is_start_of_chunk_iob2(int human_model, int i, sequence & seq, string b_tag, string i_tag) {
    if (human_model == 1) {
	return (seq[i][seq[i].size() - 2] == b_tag);
	
    } else if (human_model == 2) {
	return (seq[i][seq[i].size() - 1] == b_tag);
	
    } else {
	return 0;
    }
}

// is end of a chunk (IOB2)?
int is_end_of_chunk_iob2(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	if (seq[i][seq[i].size() - 2] == b_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i][seq[i].size() - 2] == i_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}	
    
    } else if (human_model == 2) {
	if (seq[i][seq[i].size() - 1] == b_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 1] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i][seq[i].size() - 1] == i_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 1] != i_tag) {
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
int count_chunks_iob2(int human_model, sequence & seq, string b_tag, string i_tag) {
    int count = 0;
    
    for (int i = 0; i < seq.size(); i++) {
	if (human_model == 1 && is_start_of_chunk_iob2(1, i, seq, b_tag, i_tag)) {
	    count++;
	}
	
	if (human_model == 2 && is_start_of_chunk_iob2(2, i, seq, b_tag, i_tag)) {
	    count++;
	}
    }
    
    return count;
}

// is matching chunk (IOB2)? 
int is_matching_chunk_iob2(int i, sequence & seq, string b_tag, string i_tag) {
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
int count_matching_chunks_iob2(sequence & seq, string b_tag, string i_tag) {
    int count = 0;
    
    for (int i = 0; i < seq.size(); i++) {
	if (is_start_of_chunk_iob2(1, i, seq, b_tag, i_tag)) {
	    if (is_matching_chunk_iob2(i, seq, b_tag, i_tag)) {
		count++;
	    }
	}
    }
    
    return count;
}

//==============================================================

double chunk_evaluate_iob1(dataset & data, chunkset & chunks) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;

    int i;
    int num_chunks = chunks.size();

    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    dataset::iterator datait;
    for (datait = data.begin(); datait != data.end(); datait++) {
	for (i = 0; i < num_chunks; i++) {
	    human_chk_count[i] += count_chunks_iob1(1, *datait, chunks[i][0], chunks[i][1]);
	    model_chk_count[i] += count_chunks_iob1(2, *datait, chunks[i][0], chunks[i][1]);
	    match_chk_count[i] += count_matching_chunks_iob1(*datait, chunks[i][0], chunks[i][1]);
	}
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

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
	       chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
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

    return total2_f1 * 100;
}

// is start of a chunk (IOB1)?
int is_start_of_chunk_iob1(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	if (seq[i][seq[i].size() - 2] == b_tag) {
	    return 1;
	    
	} else if (seq[i][seq[i].size() - 2] == i_tag) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 2] != i_tag && 
			seq[i - 1][seq[i - 1].size() - 2] != b_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else if (human_model == 2) {
	if (seq[i][seq[i].size() - 1] == b_tag) {
	    return 1;
	    
	} else if (seq[i][seq[i].size() - 1] == i_tag) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 1] != i_tag && 
			seq[i - 1][seq[i - 1].size() - 1] != b_tag) {
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
int is_end_of_chunk_iob1(int human_model, int i, sequence & seq, 
					string b_tag, string i_tag) {
    if (human_model == 1) {
	if (seq[i][seq[i].size() - 2] == b_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i][seq[i].size() - 2] == i_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}	
    
    } else if (human_model == 2) {
	if (seq[i][seq[i].size() - 1] == b_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 1] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	    
	} else if (seq[i][seq[i].size() - 1] == i_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 1] != i_tag) {
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
int count_chunks_iob1(int human_model, sequence & seq, string b_tag, string i_tag) {
    int count = 0;
    
    for (int i = 0; i < seq.size(); i++) {
	if (human_model == 1 && is_start_of_chunk_iob1(1, i, seq, b_tag, i_tag)) {
	    count++;
	}
	
	if (human_model == 2 && is_start_of_chunk_iob1(2, i, seq, b_tag, i_tag)) {
	    count++;
	}
    }
    
    return count;
}

// is matching chunk (IOB1)? 
int is_matching_chunk_iob1(int i, sequence & seq, string b_tag, string i_tag) {
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
int count_matching_chunks_iob1(sequence & seq, string b_tag, string i_tag) {
    int count = 0;

    for (int i = 0; i < seq.size(); i++) {
	if (is_start_of_chunk_iob1(1, i, seq, b_tag, i_tag)) {
	    if (is_matching_chunk_iob1(i, seq, b_tag, i_tag)) {
		count++;
	    }
	}
    }
    
    return count;
}

//==================================================================

double chunk_evaluate_ioe2(dataset & data, chunkset & chunks) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;

    int i;
    int num_chunks = chunks.size();

    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    dataset::iterator datait;
    for (datait = data.begin(); datait != data.end(); datait++) {
	for (i = 0; i < num_chunks; i++) {
	    human_chk_count[i] += count_chunks_ioe2(1, *datait, chunks[i][0], chunks[i][1]);
	    model_chk_count[i] += count_chunks_ioe2(2, *datait, chunks[i][0], chunks[i][1]);
	    match_chk_count[i] += count_matching_chunks_ioe2(*datait, chunks[i][0], chunks[i][1]);
	}
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

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
	       chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
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

    return total2_f1 * 100;
}

// is start of a chunk (IOE2)?
int is_start_of_chunk_ioe2(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	if (seq[i][seq[i].size() - 2] == e_tag) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i][seq[i].size() - 2] == i_tag) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else if (human_model == 2) {
	if (seq[i][seq[i].size() - 1] == e_tag) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 1] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i][seq[i].size() - 1] == i_tag) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 1] != i_tag) {
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
int is_end_of_chunk_ioe2(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	return (seq[i][seq[i].size() - 2] == e_tag);
    
    } else if (human_model == 2) {
	return (seq[i][seq[i].size() - 1] == e_tag);
    
    } else {
	return 0;
    }
}

// counting number of chunks (IOE2)
int count_chunks_ioe2(int human_model, sequence & seq, string i_tag, string e_tag) {
    int count = 0;

    for (int i = 0; i < seq.size(); i++) {
	if (human_model == 1 && is_start_of_chunk_ioe2(1, i, seq, i_tag, e_tag)) {
	    count++;
	}
	
	if (human_model == 2 && is_start_of_chunk_ioe2(2, i, seq, i_tag, e_tag)) {
	    count++;
	}
    }

    return count;
}

// is matching chunk (IOE2)? 
int is_matching_chunk_ioe2(int i, sequence & seq, string i_tag, string e_tag) {
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
int count_matching_chunks_ioe2(sequence & seq, string i_tag, string e_tag) {
    int count = 0;

    for (int i = 0; i < seq.size(); i++) {
	if (is_start_of_chunk_ioe2(1, i, seq, i_tag, e_tag)) {
	    if (is_matching_chunk_ioe2(i, seq, i_tag, e_tag)) {
		count++;
	    }
	}
    }

    return count;
}

//======================================================================

double chunk_evaluate_ioe1(dataset & data, chunkset & chunks) {
    vector<int> human_chk_count;
    vector<int> model_chk_count;
    vector<int> match_chk_count;
    int i;

    int num_chunks = chunks.size();
    for (i = 0; i < num_chunks; i++) {
	human_chk_count.push_back(0);
	model_chk_count.push_back(0);
	match_chk_count.push_back(0);
    }

    dataset::iterator datait;
    for (datait = data.begin(); datait != data.end(); datait++) {
	for (i = 0; i < num_chunks; i++) {
	    human_chk_count[i] += count_chunks_ioe1(1, *datait, chunks[i][0], chunks[i][1]);
	    model_chk_count[i] += count_chunks_ioe1(2, *datait, chunks[i][0], chunks[i][1]);
	    match_chk_count[i] += count_matching_chunks_ioe1(*datait, chunks[i][0], chunks[i][1]);
	}
    }

    printf("\tChunk-based performance evaluation:\n\n");
    printf("\t\tChunk\tManual\tModel\tMatch\tPre.(%)\tRec.(%)\tF1-Measure(%)\n");
    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

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
	       chunks[i][2].c_str(), human_chk_count[i], model_chk_count[i], 
	       match_chk_count[i], pre * 100, rec * 100, f1 * 100);
    }

    printf("\t\t-----\t------\t-----\t-----\t-------\t-------\t-------------\n");

    if (count > 0) {
	total1_pre /= count;
	total1_rec /= count;
	if (total1_pre + total1_rec > 0) {
	    total1_f1 = 2 * total1_pre * total1_rec / (total1_pre + total1_rec);
	}
	printf("\t\tAvg1.\t\t\t\t%6.2f\t%6.2f\t%6.2f\n",
	       total1_pre * 100, total1_rec * 100, total1_f1 * 100);
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

    return total2_f1 * 100;
}

// is start of a chunk (IOE1)?
int is_start_of_chunk_ioe1(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	if (seq[i][seq[i].size() - 2] == e_tag) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i][seq[i].size() - 2] == i_tag) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 2] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else {
	    return 0;
	}
	
    } else if (human_model == 2) {
	if (seq[i][seq[i].size() - 1] == e_tag) {
	    if (i <= 0) {	
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 1] != i_tag) {
		    return 1;
		} else {
		    return 0;
		}
	    }
	
	} else if (seq[i][seq[i].size() - 1] == i_tag) {
	    if (i <= 0) {
		return 1;
	    } else {
		if (seq[i - 1][seq[i - 1].size() - 1] != i_tag) {
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
int is_end_of_chunk_ioe1(int human_model, int i, sequence & seq, 
					string i_tag, string e_tag) {
    if (human_model == 1) {
	if (seq[i][seq[i].size() - 2] == e_tag) {
	    return 1;
	    
	} else if (seq[i][seq[i].size() - 2] == i_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 2] == e_tag ||
			seq[i + 1][seq[i + 1].size() - 2] == i_tag) {
		    return 0;
		} else {
		    return 1;
		}
	    }
		
	} else {
	    return 0;
	}
    
    } else if (human_model == 2) {
	if (seq[i][seq[i].size() - 1] == e_tag) {
	    return 1;
	    
	} else if (seq[i][seq[i].size() - 1] == i_tag) {
	    if (i >= seq.size() - 1) {
		return 1;
	    } else {
		if (seq[i + 1][seq[i + 1].size() - 1] == e_tag ||
			seq[i + 1][seq[i + 1].size() - 1] == i_tag) {
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
int count_chunks_ioe1(int human_model, sequence & seq, string i_tag, string e_tag) {
    int count = 0;

    for (int i = 0; i < seq.size(); i++) {
	if (human_model == 1 && is_start_of_chunk_ioe1(1, i, seq, i_tag, e_tag)) {
	    count++;
	}
	
	if (human_model == 2 && is_start_of_chunk_ioe1(2, i, seq, i_tag, e_tag)) {
	    count++;
	}
    }

    return count;
}

// is matching chunk (IOE1)? 
int is_matching_chunk_ioe1(int i, sequence & seq, string i_tag, string e_tag) {
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
int count_matching_chunks_ioe1(sequence & seq, string i_tag, string e_tag) {
    int count = 0;

    for (int i = 0; i < seq.size(); i++) {
	if (is_start_of_chunk_ioe1(1, i, seq, i_tag, e_tag)) {
	    if (is_matching_chunk_ioe1(i, seq, i_tag, e_tag)) {
		count++;
	    }
	}
    }

    return count;
}

