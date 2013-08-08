/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * option.cpp - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Nov. 06, 2005
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
#include <string>
#include "../../../include/option.h"
#include "../../../include/strtokenizer.h"

using namespace std;

void option::read_and_parse(FILE * fin) {
    char buff[BUFF_SIZE_SHORT];
    string line;
    
    while (fgets(buff, BUFF_SIZE_SHORT - 1, fin)) {
	line = buff;
    
	// find '#' character: if found, the current line is a comment
	int i = 0;
	while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
	    i++;
	}	
	if (i < line.size() && line[i] == '#') {
	    continue;
	}	
	
	strtokenizer tok(line, "= \t\r\n");
	int len = tok.count_tokens();
	if (len != 2) {
	    // invalid, ignore this line
	    continue;
	}
	
	string optstr = tok.next_token();
	string optval = tok.next_token();
	
	if (optstr == "traindata_file") {
	    trndata_file = optval;
	
	} else if (optstr == "testdata_file") {
	    tstdata_file = optval;
	
	} else if (optstr == "unlbldata_file") {
	    ulbdata_file = optval;
	    
	} else if (optstr == "model_file") {
	    model_file = optval;
	    
	} else if (optstr == "order") {
	    order = atoi(optval.c_str());
	    
	} else if (optstr == "label_of_first_observation") {
	    lfostr = optval;
	
	} else if (optstr == "trainlog_file") {
	    trainlog_file = optval;
	
	} else if (optstr == "num_labels") {
	    num_labels = atoi(optval.c_str());
	
	} else if (optstr == "num_trnseqs") {
	    num_trnseqs = atoi(optval.c_str());
	    
	} else if (optstr == "num_tstseqs") {
	    num_tstseqs = atoi(optval.c_str());
	    
	} else if (optstr == "num_ulbseqs") {
	    num_ulbseqs = atoi(optval.c_str());
	    
	} else if (optstr == "num_cps") {
	    num_cps = atoi(optval.c_str());
	    
	} else if (optstr == "num_features") {
	    num_features = atoi(optval.c_str());
	
	} else if (optstr == "f_rare_threshold") {
	    f_rare_threshold = atoi(optval.c_str());
	    
	} else if (optstr == "cp_rare_threshold") {
	    cp_rare_threshold = atoi(optval.c_str());
	    
	} else if (optstr == "multiple_f_rare_thresholds") {
	    multiple_f_rare_thresholds = atoi(optval.c_str());

	} else if (optstr == "highlight_feature") {
	    highlight_feature = atoi(optval.c_str());
	    
	} else if (optstr == "nbest") {
	    nbest = atoi(optval.c_str());
	    
	} else if (optstr == "num_iterations") {
	    num_iterations = atoi(optval.c_str());
	
	} else if (optstr == "init_lambda_val") {
	    init_lambda_val = atof(optval.c_str());	
	
	} else if (optstr == "sigma_square") {
	    sigma_square = atof(optval.c_str());
	
	} else if (optstr == "eps_for_convergence") {
	    eps_for_convergence = atof(optval.c_str());
	
	} else if (optstr == "m_for_hessian") {
	    m_for_hessian = atoi(optval.c_str());	    
	
	} else if (optstr == "debug_level") {
	    debug_level = atoi(optval.c_str());
	
	} else if (optstr == "is_scaling") {
	    is_scaling = atoi(optval.c_str());
	
	} else if (optstr == "is_logging") {
	    is_logging = atoi(optval.c_str());
	
	} else if (optstr == "evaluate_during_training") {
	    evaluate_during_training = atoi(optval.c_str());
		
	} else if (optstr == "chunk_evaluate_during_training") {
	    chunk_evaluate_during_training = atoi(optval.c_str());
	    
	} else if (optstr == "chunktype") {
	    if (optval == "IOB1" || optval == "iob1") {
		chunktype = IOB1;
	    }
	    if (optval == "IOB2" || optval == "iob2") {
		chunktype = IOB2;
	    }
	    if (optval == "IOE1" || optval == "ioe1") {
		chunktype = IOE1;
	    }
	    if (optval == "IOE2" || optval == "ioe2") {
		chunktype = IOE2;
	    }
	    
	} else if (optstr == "chunk") {
	    strtokenizer tok(optval, ":");
	    int len = tok.count_tokens();
	    if (len == 3) {
		vector<string> tags;
		for (int i = 0; i < len; i++) {
		    tags.push_back(tok.token(i));
		}
		chunks.push_back(tags);
	    }

	} else if (optstr == "prevfixedlabels") {
	    // examples:
	    // for IOB2: prevfixedlabels=b-np:i-np|i-np
	    // for IOB1: prevfixedlabels=b-np:i-np|b-np

	    strtokenizer tok(optval, "|");
	    if (tok.count_tokens() == 2) {
		vector<string> cnt;
		
		cnt.push_back(tok.token(1));
		
		strtokenizer tok1(tok.token(0), ":");
		for (int count = 0; count < tok1.count_tokens(); count++) {
		    cnt.push_back(tok1.token(count));
		}
		
		prevfixedstrlabels.push_back(cnt);
	    }
	    
	} else if (optstr == "nextfixedlabels") {
	    // examples:
	    // for IOE2: nextfixedlabels=i-np|i-np:e-np
	    // for IOE1: nextfixedlabels=e-np|i-np:e-np

	    strtokenizer tok(optval, "|");
	    if (tok.count_tokens() == 2) {
		vector<string> cnt;
		
		cnt.push_back(tok.token(0));
		
		strtokenizer tok1(tok.token(1), ":");
		for (int count = 0; count < tok1.count_tokens(); count++) {
		    cnt.push_back(tok1.token(count));
		}
		
		nextfixedstrlabels.push_back(cnt);
	    }

	} else {
	}
    }
}

void option::write_options(FILE * fout) {
    fprintf(fout, "OPTION VALUES:\n\n");
    fprintf(fout, "Model directory: %s\n", model_dir.c_str());
    fprintf(fout, "Training data file: %s\n", trndata_file.c_str());
    fprintf(fout, "Testing data file: %s\n", tstdata_file.c_str());
    fprintf(fout, "Unlabeled data file: %s\n", ulbdata_file.c_str());
    if (chunk_evaluate_during_training) {
	if (chunktype == IOB1) {
	    fprintf(fout, "Label representation: IOB1\n");
	} else if (chunktype == IOB2) {
	    fprintf(fout, "Label representation: IOB2\n");
	} else if (chunktype == IOE1) {
	    fprintf(fout, "Label representation: IOE1\n");
	} else if (chunktype == IOE2) {
	    fprintf(fout, "Label representation: IOE2\n");
	}
    }
    fprintf(fout, "Model file: %s\n", model_file.c_str());
    fprintf(fout, "Training log file (this one): %s\n\n", trainlog_file.c_str());
            
    if (order == FIRST_ORDER) {
	fprintf(fout, "First-order Markov CRFs\n\n");
    } else if (order == SECOND_ORDER) {
	fprintf(fout, "Second-order Markov CRFs\n\n");
    }

    fprintf(fout, "Number of labels: %d\n", num_labels);
    fprintf(fout, "Number of training sequences: %d\n", num_trnseqs);
    fprintf(fout, "Number of testing sequences: %d\n", num_tstseqs);
    fprintf(fout, "Number of unlabeled sequences: %d\n", num_ulbseqs);
    fprintf(fout, "Number of context predicates: %d\n", num_cps);
    fprintf(fout, "Number of features: %d\n", num_features);
    fprintf(fout, "Feature rare threshold: %d\n", f_rare_threshold);
    fprintf(fout, "Context predicate rare threshold: %d\n", cp_rare_threshold);
    fprintf(fout, "Using multiple rare thresholds for features: %d\n", multiple_f_rare_thresholds);
    fprintf(fout, "Highlight feature: %d\n\n", highlight_feature);

    fprintf(fout, "Number of training iterations: %d\n", num_iterations);
    fprintf(fout, "Initial lambda value: %10.4f\n", init_lambda_val);
    fprintf(fout, "Sigma square (for smoothing): %10.4f\n", sigma_square);
    fprintf(fout, "Epsilon for L-BFGS convergence: %10.6f\n", eps_for_convergence);
    fprintf(fout, "Number of approximated hessian matrixes: %d\n", m_for_hessian);
}

