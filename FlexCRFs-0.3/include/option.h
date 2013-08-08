/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * option.h - this file is part of FlexCRFs.
 *
 * Begin:	Dec. 15, 2004
 * Last change:	Oct. 18, 2005
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

#ifndef _OPTION_H
#define _OPTION_H

#include <stdio.h>
#include <map>
#include <string>
#include <stdlib.h>

#include "strtokenizer.h"

using namespace std;

#define	BUFF_SIZE_LONG	20000
#define	BUFF_SIZE_SHORT	512

#define	FIRST_ORDER	1
#define	SECOND_ORDER	2

// the label of the first observation of a sequence
#define	FIRST_OBSR_LABEL	"#FOL#"

// chunk types
#define	IOB1	1
#define	IOB2	2
#define	IOE1	3
#define	IOE2	4

class option {	
public:
    string model_dir;		// directory of data and model
    string trndata_file;	// training data
    string tstdata_file;	// testing data
    string ulbdata_file; 	// unlabeled data (need to be predicted)
    string model_file;		// model file (contain dictionary, features, etc.)

    string trainlog_file;	// training log file
    int is_logging;		// logging or not

    int order;			// 1: frist-order Markov; 2: second-order Markov
    string lfostr;		// label of the first observation (for second-order Markov)
    int lfo;
    
    int nbest;			// for searching n-best label paths

    int num_labels;		// number of labels
    int num_2orderlabels;	// number of second-order labels
    int num_trnseqs;		// number of training sequences
    int num_tstseqs;		// number of testing sequences
    int num_ulbseqs; 		// number of unlabeled sequences
    int num_cps;		// number of context predicates
    int num_features;		// number of features    

    int f_rare_threshold;	// rare threshold for features
    int cp_rare_threshold;	// rare threshold for context predicates
    int multiple_f_rare_thresholds;	// using different thresholds for features or not
    
    int highlight_feature;	// emphasizing features or not
    
    // training options
    int num_iterations;		// number of training iterations
    double init_lambda_val;	// initial values for weights (lambda)
    double sigma_square;	// penalty term for likelihood functions 
				// is ||lambda||^2 (2 * sigma_square)
				// set this to zero if no penalty needed
    double eps_for_convergence;	// convergence criterion for finding optimum
				// lambda using L-BFGS optimization technique
    int m_for_hessian;		// the number of corrections used in L-BFGS update
    int is_scaling;		// scaling to avoid numerical problems
    int debug_level;		// control amount of status information output
    int evaluate_during_training;	// evaluating during training or not    
    
    int chunk_evaluate_during_training;	// chunk-based evaluation
    vector<vector<string> > chunks;	// chunks
    int chunktype;			// IOB1, IOB2, IOE1, IOE2
    
    // constraints for Viterbi (a particular label follows particular labels)
    vector<vector<string> > prevfixedstrlabels;
    vector<vector<int> >  prevfixedintlabels;
    // constraints for Viterbi (a particular label is followed by particular labels)
    vector<vector<string> > nextfixedstrlabels;
    vector<vector<int> >  nextfixedintlabels;

    option() {
	init_default_values();
    }    
    
    option(FILE * fin) {
	init_default_values();
	read_and_parse(fin);
    }
    
    option(FILE * fin, string model_dir) {
	init_default_values();
	this->model_dir = model_dir;
	read_and_parse(fin);
    }
    
    // initialize default values
    void init_default_values() {
	model_dir = "./";
	trndata_file = "train.tagged";
	tstdata_file = "test.tagged";
	ulbdata_file = "data.untagged";
	model_file = "model.txt";
	
	trainlog_file = "trainlog.txt";
	is_logging = 1;
	
	order = FIRST_ORDER;
	lfostr = "";
	lfo = -1;
	
	nbest = 1;
	
	num_labels = 0;
	num_2orderlabels = 0;
	num_trnseqs = 0;
	num_tstseqs = 0;
	num_ulbseqs = 0;
	num_cps = 0;
	num_features = 0;

	f_rare_threshold = 1;
	cp_rare_threshold = 1;
	multiple_f_rare_thresholds = 0;
	
	highlight_feature = 0;
	
	num_iterations = 50;
	init_lambda_val = 0.0;
	sigma_square = 100;
	eps_for_convergence = 0.0001;
	m_for_hessian = 7;
	debug_level = 1;
	is_scaling = 1;
	evaluate_during_training = 0;
	
	chunk_evaluate_during_training = 0;
	chunktype = IOB2;
    }
    
    void read_and_parse(FILE * fin);
    
    void write_options(FILE * fout);
};

#endif

