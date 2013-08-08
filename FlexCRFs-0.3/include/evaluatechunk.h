/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * evaluatechunk.h - this file is part of FlexCRFs.
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

#ifndef	_EVALUATECHUNK_H
#define	_EVALUATECHUNK_H

#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "strtokenizer.h"

using namespace std;

typedef vector<string> obsr;
typedef vector<obsr> sequence;
typedef list<sequence> dataset;

typedef vector<string> labelset;
typedef vector<vector<string> > chunkset;

int str_2_int(map<string, int> & str2int, string & label);
string int_2_str(map<int, string> & int2str, int label);

void evaluate(dataset & data, string & chunktype, labelset & labels, chunkset & chunks);

// chunk-based evaluation (segmentation applications such as
// text chunking and named entity recognition

// chunk evaluation (IOB2) 
double chunk_evaluate_iob2(dataset & data, chunkset & chunks);
int is_start_of_chunk_iob2(int human_model, int i, sequence & seq, string b_tag, string i_tag);
int is_end_of_chunk_iob2(int human_model, int i, sequence & seq, string b_tag, string i_tag);    
int count_chunks_iob2(int human_model, sequence & seq, string b_tag, string i_tag);
int is_matching_chunk_iob2(int i, sequence & seq, string b_tag, string i_tag);
int count_matching_chunks_iob2(sequence & seq, string b_tag, string i_tag);

// chunk evaluation (IOB1) 
double chunk_evaluate_iob1(dataset & data, chunkset & chunks);
int is_start_of_chunk_iob1(int human_model, int i, sequence & seq, string b_tag, string i_tag);
int is_end_of_chunk_iob1(int human_model, int i, sequence & seq, string b_tag, string i_tag);    
int count_chunks_iob1(int human_model, sequence & seq, string b_tag, string i_tag);
int is_matching_chunk_iob1(int i, sequence & seq, string b_tag, string i_tag);
int count_matching_chunks_iob1(sequence & seq, string b_tag, string i_tag);

// chunk evaluation (IOE2) 
double chunk_evaluate_ioe2(dataset & data, chunkset & chunks);
int is_start_of_chunk_ioe2(int human_model, int i, sequence & seq, string i_tag, string e_tag);
int is_end_of_chunk_ioe2(int human_model, int i, sequence & seq, string i_tag, string e_tag);    
int count_chunks_ioe2(int human_model, sequence & seq, string i_tag, string e_tag);
int is_matching_chunk_ioe2(int i, sequence & seq, string i_tag, string e_tag);
int count_matching_chunks_ioe2(sequence & seq, string i_tag, string e_tag);

// chunk evaluation (IOE1) 
double chunk_evaluate_ioe1(dataset & data, chunkset & chunks);
int is_start_of_chunk_ioe1(int human_model, int i, sequence & seq, string i_tag, string e_tag);
int is_end_of_chunk_ioe1(int human_model, int i, sequence & seq, string i_tag, string e_tag);    
int count_chunks_ioe1(int human_model, sequence & seq, string i_tag, string e_tag);
int is_matching_chunk_ioe1(int i, sequence & seq, string i_tag, string e_tag);
int count_matching_chunks_ioe1(sequence & seq, string i_tag, string e_tag);

#endif

