/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * crf.cpp - this file is part of FlexCRFs.
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

#include <stdio.h>
#include <stdlib.h>
#include "../include/strtokenizer.h"
#include "../include/option.h"
#include "../include/data.h"
#include "../include/dictionary.h"
#include "../include/feature.h"
#include "../include/featuregen.h"
#include "../include/trainer.h"
#include "../include/viterbi.h"
#include "../include/evaluation.h"
#include "../include/model.h"

using namespace std;

// for displaying command line guide
void display_help();

// for displaying copyright notice
void display_copyright();

//main chunk feature
int main_chunk_feature(int argc, char ** argv);
//main pos features
int main_pos_feature(int argc, char ** argv) ;

//main covert chunk
int main_convert_chunk(int argc, char ** argv) ;

//main evaluate chunk
int main_evaluate(int argc, char ** argv) ;

//main majiovte
int main_marjovte(int argc, char ** argv);

//main function here
int main(int argc, char ** argv) {
    display_copyright();

    // the command line must be:
    // segment -all/-trn/-tst/-prd -d <model directory> -o <option filename>
    if (argc < 6) {
	   display_help();
	   return 0;
    }
    
    int is_all = !strcmp(argv[1], "-all");
    int is_trn = !strcmp(argv[1], "-trn");
    int is_tst = !strcmp(argv[1], "-tst");
    int is_prd = !strcmp(argv[1], "-prd");
    
    // the first parameter must be "-all", "-train", or "-test"
    if (!is_all && !is_trn && !is_tst && !is_prd) {
	display_help();
	return 0;
    }
    
    // the second parameter must be "-d"
    if (strcmp(argv[2], "-d")) {
	display_help();
	return 0;
    }
    
    // the third parameter must be the model directory
    string model_dir = argv[3];
    if (model_dir[model_dir.size() - 1] != '/') {
	model_dir += "/";
    }
    
    // the fourth parameter must be "-o"
    if (strcmp(argv[4], "-o")) {
	display_help();
	return 0;
    }	 
    
    // the fifth parameter must be the option filename
    string optfile = model_dir + argv[5];    

    // open the option file to read parameters
    FILE * optf;
    optf = fopen(optfile.c_str(), "r");
    if (!optf) {
	// if can not open the option file
	printf("cannot open the option file for reading\n");
	return 0;
    } 
    
    // create an option object
    option opt(optf, model_dir);
    
    FILE * itrndataf;
    FILE * itstdataf;
    FILE * iulbdataf;

    FILE * imodelf;
    
    if (is_all || is_trn) {
	// try to open training data file
	itrndataf = fopen((opt.model_dir + opt.trndata_file).c_str(), "r");
	if (!itrndataf) {
	    printf("cannot open the training data file for reading\n");
	    return 0;
	}
    }
    
    if (is_all || is_tst) {
	// try to open testing data file
	itstdataf = fopen((opt.model_dir + opt.tstdata_file).c_str(), "r");
	if (!itstdataf) {
	    printf("cannot open the testing data file for reading\n");
	    return 0;
	}
    }
    
    if (is_prd) {
	// try to open unlabeled data file	
	iulbdataf = fopen((opt.model_dir + opt.ulbdata_file).c_str(), "r");
	if (!iulbdataf) {
	    printf("cannot open the unlabeled data file for reading\n");
	    return 0;
	}
    }
    
    if (is_tst || is_prd) {
	// try to open model file (contain cpmap, labelmap, dictionary, and features)
	imodelf = fopen((opt.model_dir + opt.model_file).c_str(), "r");
	if (!imodelf) {
	    printf("cannot open the model file for reading\n");
	    return 0;
	}
    }
    
    // data object
    data dt(&opt);
    // dictionary object
    dictionary dict(&dt, &opt);
    // featuregen object
    featuregen fgen(&opt, &dict, &dt);

    // both training and testing
    if (is_all) {
	printf("reading training data ...\n");
	dt.read_trndata(itrndataf);
	printf("reading %d training sequence completed.\n\n", opt.num_trnseqs);
	
	printf("reading testing data ...\n");
	dt.read_tstdata(itstdataf);
	printf("reading %d testing sequences completed.\n\n", opt.num_tstseqs);
	
	printf("generating dictionary ...\n");
	dict.dict_gen();
	printf("generating %d context predicates completed.\n\n", opt.num_cps);	
	
	printf("generating CRF features from training data ...\n");
	fgen.gen_features();
	printf("generating %d CRF features completed.\n\n", opt.num_features);
	
	printf("pruning unused context predicates ...\n");
	dt.cp_prune(&dict);
	printf("the number of context predicates after pruning: %d\n\n", opt.num_cps);

	// create model file
	FILE * omodelf;
	omodelf = fopen((opt.model_dir + opt.model_file).c_str(), "w");
	
	printf("saving context predicate map ...\n");
	dt.write_cp_map(omodelf, &dict);
	printf("saving context predicate map completed.\n\n");
	
	printf("saving label map file ...\n");
	dt.write_lb_map(omodelf);
	printf("saving label map completed.\n\n");	
	
	if (opt.order == SECOND_ORDER) {
	    printf("saving second-order label map ...\n");
	    dt.write_2order_lb_map(omodelf);
	    printf("saving second-order label map completed.\n\n");
	    
	    printf("saving second-order to first-order label map ...\n");
	    dt.write_2to1_lb_map(omodelf);
	    printf("saving second-order to first-order label map completed.\n\n");
	}

	// create the trainer object
	trainer trn;
	// create the viterbi object
	viterbi vtb;
	// create the evaluation object
	evaluation eval;

	// create the CRF model
	model mdl(&opt, &dt, &dict, &fgen, &trn, &vtb, &eval);
	
	// create training log file
	FILE * otrnlogf;
	if (opt.is_logging) {
	    otrnlogf = fopen((opt.model_dir + opt.trainlog_file).c_str(), "w");
	}

	// saving dictionary	
	printf("saving the context predicate dictionary to file ...\n");
	dict.write_dict(omodelf);
	printf("saving the dictionary completed.\n\n");
	
	// start to train the CRF model
	printf("start to train the CRF model ...\n\n");
	mdl.train(otrnlogf);
	
	// saving the set of features
	printf("saving the CRF features to file ...\n");
	fgen.write_features(omodelf);
	printf("saving the CRF features completed.\n\n");
	
	// start to label for testing data
	printf("labeling for testing data ...\n");
	mdl.apply_tstdata();
	printf("labeling for testing data completed.\n\n");
	
	// saving testing output to file
	FILE * otstdataf;
	otstdataf = fopen((opt.model_dir + opt.tstdata_file + ".model").c_str(), "w");
	printf("writing testing output to file ...\n");
	dt.write_tstdata(otstdataf);
	printf("writing testing output completed.\n\n");
	
	// performance evaluation on testing dataset
	mdl.peval->evaluate(otrnlogf);		
	printf("\n");
	
	fclose(optf);
	fclose(itrndataf);
	fclose(itstdataf);
	fclose(otstdataf);
	fclose(otrnlogf);
	fclose(omodelf);
    }
    
    // training only
    if (is_trn) {
	printf("reading training data ...\n");
	dt.read_trndata(itrndataf);
	printf("reading %d training sequence completed.\n\n", opt.num_trnseqs);
	
	printf("generating dictionary ...\n");
	dict.dict_gen();
	printf("generating %d context predicates completed.\n\n", opt.num_cps);	
	
	printf("generating CRF features from training data ...\n");
	fgen.gen_features();
	printf("generating %d CRF features completed.\n\n", opt.num_features);
	
	printf("pruning unused context predicates ...\n");
	dt.cp_prune(&dict);
	printf("the number of context predicates after pruning: %d\n\n", opt.num_cps);

	// create model file
	FILE * omodelf;
	omodelf = fopen((opt.model_dir + opt.model_file).c_str(), "w");
	
	printf("saving context predicate map ...\n");
	dt.write_cp_map(omodelf, &dict);
	printf("saving context predicate map completed.\n\n");
	
	printf("saving label map file ...\n");
	dt.write_lb_map(omodelf);
	printf("saving label map completed.\n\n");	
	
	if (opt.order == SECOND_ORDER) {
	    printf("saving second-order label map ...\n");
	    dt.write_2order_lb_map(omodelf);
	    printf("saving second-order label map completed.\n\n");
	    
	    printf("saving second-order to first-order label map ...\n");
	    dt.write_2to1_lb_map(omodelf);
	    printf("saving second-order to first-order label map completed.\n\n");
	}

	// create the trainer object
	trainer trn;

	// create the CRF model
	model mdl(&opt, &dt, &dict, &fgen, &trn, NULL, NULL);
	
	// create training log file
	FILE * otrnlogf;
	if (opt.is_logging) {
	    otrnlogf = fopen((opt.model_dir + opt.trainlog_file).c_str(), "w");
	}
	
	// save the dictionary
	printf("saving the context predicate dictionary to file ...\n");
	dict.write_dict(omodelf);
	printf("saving the dictionary completed.\n\n");
	
	// start to train the CRF model
	printf("start to train the CRF model ...\n\n");
	mdl.train(otrnlogf);
	
	// saving the set of features
	printf("saving the CRF features to file ...\n");
	fgen.write_features(omodelf);
	printf("saving the CRF features completed.\n\n");

	fclose(optf);
	fclose(itrndataf);
	fclose(otrnlogf);
	fclose(omodelf);
    }
    
    // testing only
    if (is_tst) {
	printf("reading context predicate map from file ...\n");
	dt.read_cp_map(imodelf);
	printf("reading %d context predicate mappings completed.\n\n", opt.num_cps);
	
	printf("reading label map from file ...\n");
	dt.read_lb_map(imodelf);
	printf("reading %d label mappings completed.\n\n", opt.num_labels);
	
	if (opt.order == SECOND_ORDER) {
	    printf("reading second-order label map ...\n");
	    dt.read_2order_lb_map(imodelf);
	    printf("reading second-order label map completed.\n\n");
	    
	    printf("reading second-order to first-order label map ...\n");
	    dt.read_2to1_lb_map(imodelf);
	    printf("reading second-order to first-order label map completed.\n\n");
	}

	printf("reading dictionary from file...\n");
	dict.read_dict(imodelf);
	printf("reading %d context predicates completed.\n\n", opt.num_cps);	
	
	printf("reading CRF features from file ...\n");
	fgen.read_features(imodelf);
	printf("reading %d CRF features completed.\n\n", opt.num_features);

	printf("reading testing data ...\n");	
	dt.read_tstdata(itstdataf);
	printf("reading %d testing sequences completed.\n\n", opt.num_tstseqs);
	
	// create the viterbi object
	viterbi vtb;
	// create the evaluation object
	evaluation eval;

	// create the CRF model
	model mdl(&opt, &dt, &dict, &fgen, NULL, &vtb, &eval);
	
	// start to label for testing data
	printf("labeling for testing data ...\n");
	if (opt.nbest <= 1) {
	    mdl.apply_tstdata();
	} else {
	    mdl.apply_tstdata(opt.nbest);
	}
	printf("labeling for testing data completed.\n\n");
	
	// saving testing output to file
	FILE * otstdataf;
	otstdataf = fopen((opt.model_dir + opt.tstdata_file + ".model").c_str(), "w");
	printf("writing testing output to file ...\n");
	dt.write_tstdata(otstdataf);
	printf("writing testing output completed.\n\n");
	
	// performance evaluation on testing dataset
	opt.is_logging = 0;
	mdl.peval->evaluate(NULL);
	printf("\n");
	
	fclose(optf);
	fclose(itstdataf);
	fclose(otstdataf);
	fclose(imodelf);
    }
    
    // prediction
    if (is_prd) {
	printf("reading context predicate map from file ...\n");
	dt.read_cp_map(imodelf);
	printf("reading %d context predicate mappings completed.\n\n", opt.num_cps);
	
	printf("reading label map from file ...\n");
	dt.read_lb_map(imodelf);
	printf("reading %d label mappings completed.\n\n", opt.num_labels);
	
	if (opt.order == SECOND_ORDER) {
	    printf("reading second-order label map ...\n");
	    dt.read_2order_lb_map(imodelf);
	    printf("reading second-order label map completed.\n\n");
	    
	    printf("reading second-order to first-order label map ...\n");
	    dt.read_2to1_lb_map(imodelf);
	    printf("reading second-order to first-order label map completed.\n\n");
	}

	printf("reading dictionary from file...\n");
	dict.read_dict(imodelf);
	printf("reading %d context predicates completed.\n\n", opt.num_cps);	
	
	printf("reading CRF features from file ...\n");
	fgen.read_features(imodelf);
	printf("reading %d CRF features completed.\n\n", opt.num_features);

	printf("reading unlabeled data ...\n");
	dt.read_ulbdata(iulbdataf);
	printf("reading %d unlabeled sequences completed.\n\n", opt.num_ulbseqs);
	
	// create the viterbi object
	viterbi vtb;

	// create the CRF model
	model mdl(&opt, &dt, &dict, &fgen, NULL, &vtb, NULL);
	
	// start to label for testing data
	printf("predicting for unlabeled data ...\n");
	mdl.apply_ulbdata();
	printf("predicting for unlabeled data completed.\n\n");
	
	// saving data output to file
	FILE * oulbdataf;
	oulbdataf = fopen((opt.model_dir + opt.ulbdata_file + ".model").c_str(), "w");
	printf("writing data with predicted labels to file ...\n");
	dt.write_ulbdata(oulbdataf);
	printf("writing data with predicted labels completed.\n\n");
	
	fclose(optf);
	fclose(iulbdataf);
	fclose(oulbdataf);
	fclose(imodelf);
    }
    
    return 0;
} // end of main function


void display_help() {    
    printf("Command line can be one of the following cases:\n");
    printf("crf -all -d <model dir> -o <option file>\n");
    printf("crf -trn -d <model dir> -o <option file>\n");
    printf("crf -tst -d <model dir> -o <option file>\n");
    printf("crf -prd -d <model dir> -o <option file>\n");
    printf("\n");
    
    printf("-all:		for both training and testing. User must specify both\n");
    printf("		training and testing data files in the option file.\n");
    printf("-trn:		for training only (only training data file is required)\n");
    printf("-tst:		for testing only (only testing data file is required.\n");
    printf("		The CRF model must be trained in advance\n");
    printf("-prd:		for predicting new (unlabeled) data sequences. The unlabeled\n");
    printf("		data must be available.\n");
    printf("<model dir>:	is the relative path to the directory that contains option\n");
    printf("		file, training, testing, and unlabeled data, as well as the\n");
    printf("		trained model (together with several automatically generated files.\n");
    printf("<option file>:	is the file containing optional parameters that control the training,\n");
    printf("		testing, and predicting processes of the CRF model.\n");
    printf("\n");
    
    printf("For example (supposing one is in the \"src\" or \"bin\" directories):\n");
    printf("./crf -all -d ../models/model1 -o option.txt (for training and testing)\n");   
    printf("./crf -trn -d ../models/model1 -o option.txt (for training only)\n");
    printf("./crf -tst -d ../models/model1 -o option.txt (for testing only)\n");
    printf("./crf -prd -d ../models/model1 -o option.txt (for predicting only)\n");
    printf("\n");
}

void display_copyright() {
    printf("\n");
    printf("FlexCRFs - A Flexible Conditional Random Field Toolkit\n");
    printf("Copyright (C) 2004 by\n");
    printf("Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp\n");
    printf("Graduate School of Information Science,\n");
    printf("Japan Advanced Institute of Science and Technology (JAIST)\n");
    printf("\n");
}

