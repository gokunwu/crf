/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * evaluation.h - this file is part of FlexCRFs.
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

#ifndef	_EVALUATION_H
#define	_EVALUATION_H

#include <stdio.h>
#include "option.h"
#include "data.h"
#include "model.h"

using namespace std;

class model;

class evaluation {
public:
    model * pmodel;
    option * popt;
    data * pdata;
    
    evaluation() { }
    
    void init(model * pmodel) {
	this->pmodel = pmodel;
	this->popt = pmodel->popt;
	this->pdata = pmodel->pdata;
    }
    
    int num_correct_seqs;
    vector<int> human_lb_count, model_lb_count, human_model_lb_count;
    double evaluate(FILE * fout);
    
    // chunk-based evaluation (segmentation applications such as
    // text chunking and named entity recognition
    
    // chunk evaluation (IOB2) 
    double chunk_evaluate_iob2(FILE * fout);
    int is_start_of_chunk_iob2(int human_model, int i, sequence & seq, string b_tag, string i_tag);
    int is_end_of_chunk_iob2(int human_model, int i, sequence & seq, string b_tag, string i_tag);    
    int count_chunks_iob2(int human_model, string b_tag, string i_tag);
    int is_matching_chunk_iob2(int i, sequence & seq, string b_tag, string i_tag);
    int count_matching_chunks_iob2(string b_tag, string i_tag);

    // chunk evaluation (IOB1) 
    double chunk_evaluate_iob1(FILE * fout);
    int is_start_of_chunk_iob1(int human_model, int i, sequence & seq, string b_tag, string i_tag);
    int is_end_of_chunk_iob1(int human_model, int i, sequence & seq, string b_tag, string i_tag);    
    int count_chunks_iob1(int human_model, string b_tag, string i_tag);
    int is_matching_chunk_iob1(int i, sequence & seq, string b_tag, string i_tag);
    int count_matching_chunks_iob1(string b_tag, string i_tag);

    // chunk evaluation (IOE2) 
    double chunk_evaluate_ioe2(FILE * fout);
    int is_start_of_chunk_ioe2(int human_model, int i, sequence & seq, string i_tag, string e_tag);
    int is_end_of_chunk_ioe2(int human_model, int i, sequence & seq, string i_tag, string e_tag);    
    int count_chunks_ioe2(int human_model, string i_tag, string e_tag);
    int is_matching_chunk_ioe2(int i, sequence & seq, string i_tag, string e_tag);
    int count_matching_chunks_ioe2(string i_tag, string e_tag);

    // chunk evaluation (IOE1) 
    double chunk_evaluate_ioe1(FILE * fout);
    int is_start_of_chunk_ioe1(int human_model, int i, sequence & seq, string i_tag, string e_tag);
    int is_end_of_chunk_ioe1(int human_model, int i, sequence & seq, string i_tag, string e_tag);    
    int count_chunks_ioe1(int human_model, string i_tag, string e_tag);
    int is_matching_chunk_ioe1(int i, sequence & seq, string i_tag, string e_tag);
    int count_matching_chunks_ioe1(string i_tag, string e_tag);
};

#endif

