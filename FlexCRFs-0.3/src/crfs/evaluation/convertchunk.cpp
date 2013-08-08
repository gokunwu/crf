/*
 * Copyright (C) 2004 - 2005 by
 *     Hieu Xuan Phan & Minh Le Nguyen {hieuxuan, nguyenml}@jaist.ac.jp
 *     Graduate School of Information Science,
 *     Japan Advanced Institute of Science and Technology (JAIST)
 *
 * convertchunk.cpp - this file is part of FlexCRFs.
 *
 * Begin:	Nov. 26, 2005
 * Last change:	Nov. 26, 2005
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

#include <map>
#include "../../../include/convertchunk.h"

using namespace std;

string strtail(string & str) {
    return str.substr(1, str.size() - 1);
}

void convert_IOB2_IOB1(int is_cap, int is_last, sequence & seq) {
    map<int, int> fixedlabels;
    map<int, int>::iterator obsrit;

    char begin = is_cap ? 'B' : 'b';
    char inside = is_cap ? 'I' : 'i';
    string INSIDE = is_cap ? "I" : "i";

    string str1, str2, istr, newlabel;
    int i, len = seq.size();        

    for (i = 1; i < len; i++) {
	int col1 = is_last ? seq[i - 1].size() - 1 : seq[i - 1].size() - 2;
	int col2 = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	
	str1 = seq[i - 1][col1];
	str2 = seq[i][col2];
	
	if (str2[0] != begin) {
	    continue;
	}
	
	istr = INSIDE;
	istr += strtail(str2);
	
	if (str1 == str2 || str1 == istr) {
	    fixedlabels.insert(pair<int, int>(i, i));
	}
    }
    
    for (i = 0; i < len; i++) {
	obsrit = fixedlabels.find(i);
	if (obsrit != fixedlabels.end()) {
	    continue;
	}
	
	int col = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	if (seq[i][col][0] == begin) {
	    // B- or b- => I- or i-
	    newlabel = INSIDE;
	    newlabel += strtail(seq[i][col]);
	    seq[i][col] = newlabel;
	}		
    }
}

void convert_IOB1_IOB2(int is_cap, int is_last, sequence & seq) {
    vector<int> firstlabels;
    char begin = is_cap ? 'B' : 'b';
    string BEGIN = is_cap ? "B" : "b";
    char inside = is_cap ? 'I' : 'i';

    string str1, str2, bstr, newlabel;
    int i, len = seq.size();        
    
    for (i = 0; i < len; i++) {
	int col2 = is_last ? seq[i].size() - 1 : seq[i].size() - 2;	
	str2 = seq[i][col2];
	
	if (str2[0] != inside) {
	    continue;
	}
	
	if (i == 0) {
	    firstlabels.push_back(i);
	
	} else {
	    int col1 = is_last ? seq[i - 1].size() - 1 : seq[i - 1].size() - 2;
	    str1 = seq[i - 1][col1];
	
	    bstr = BEGIN;
	    bstr += strtail(str2);
	    
	    if (str2 != str1 && str1 != bstr) {
		firstlabels.push_back(i);
	    }
	}
    }

    for (i = 0; i < firstlabels.size(); i++) {
	int col = is_last ? seq[firstlabels[i]].size() - 1 : seq[firstlabels[i]].size() - 2;
	
	newlabel = BEGIN;
	newlabel += strtail(seq[firstlabels[i]][col]);
	seq[firstlabels[i]][col] = newlabel;
    }
}

void convert_IOE2_IOE1(int is_cap, int is_last, sequence & seq) {
    map<int, int> fixedlabels;
    map<int, int>::iterator obsrit;

    char inside = is_cap ? 'I' : 'i';
    string INSIDE = is_cap ? "I" : "i";
    char end = is_cap ? 'E' : 'e';

    string str1, str2, istr, newlabel;
    int i, len = seq.size();        
    
    for (i = 0; i < len - 1; i++) {
	int col1 = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	int col2 = is_last ? seq[i + 1].size() - 1 : seq[i + 1].size() - 2;

	str1 = seq[i][col1];
	str2 = seq[i + 1][col2];
	
	if (str1[0] != end) {
	    continue;
	}
	
	istr = INSIDE;
	istr += strtail(str1);
	
	if (str2 == str1 || str2 == istr) {
	    fixedlabels.insert(pair<int, int>(i, i));
	}
    }
    
    for (i = 0; i < len; i++) {
	obsrit = fixedlabels.find(i);
	if (obsrit != fixedlabels.end()) {
	    continue;
	}
	
	int col = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	if (seq[i][col][0] == end) {
	    // E- or e- => I- or i-
	    newlabel = INSIDE;
	    newlabel += strtail(seq[i][col]);
	    seq[i][col] = newlabel;
	}		
    }
}

void convert_IOE1_IOE2(int is_cap, int is_last, sequence & seq) {
    vector<int> endlabels;
    
    char end = is_cap ? 'E' : 'e';
    string END = is_cap ? "E" : "e";
    char inside = is_cap ? 'I' : 'i';

    string str1, str2, estr, newlabel;
    int i, len = seq.size();        
    
    for (i = 0; i < len; i++) {
	int col1 = is_last ? seq[i].size() - 1 : seq[i].size() - 2;	
	str1 = seq[i][col1];
	
	if (str1[0] != inside) {
	    continue;
	}
	
	if (i == len - 1) {
	    endlabels.push_back(i);
	
	} else {
	    int col2 = is_last ? seq[i + 1].size() - 1 : seq[i + 1].size() - 2;
	    str2 = seq[i + 1][col2];
	
	    estr = END;
	    estr += strtail(str1);
	    
	    if (str2 != str1 && str2 != estr) {
		endlabels.push_back(i);
	    }
	}
    }

    for (i = 0; i < endlabels.size(); i++) {
	int col = is_last ? seq[endlabels[i]].size() - 1 : seq[endlabels[i]].size() - 2;
	
	newlabel = END;
	newlabel += strtail(seq[endlabels[i]][col]);
	seq[endlabels[i]][col] = newlabel;
    }
}

void convert_IOB2_IOE2(int is_cap, int is_last, sequence & seq) {
    map<int, int> endlabels;
    map<int, int>::iterator elbit;
    
    char begin = is_cap ? 'B' : 'b';
    char inside = is_cap ? 'I' : 'i';
    string INSIDE = is_cap ? "I" : "i";
    char end = is_cap ? 'E' : 'e';
    string END = is_cap ? "E" : "e";
    
    string str1, str2, istr, str, newlabel;
    int i, len = seq.size();
    
    for (i = 0; i < len; i++) {
	int col1 = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	str1 = seq[i][col1];
	
	if (str1[0] != begin && str1[0] != inside) {
	    continue;
	}
	
	if (i == len - 1) {
	    endlabels.insert(pair<int, int>(i, i));

	} else {
	    int col2 = is_last ? seq[i + 1].size() - 1 : seq[i + 1].size() - 2;
	    str2 = seq[i + 1][col2];
	    
	    istr = INSIDE;
	    istr += strtail(str1);
	    
	    if (str1[0] == begin && str2 != istr) {
		endlabels.insert(pair<int, int>(i, i));
	    
	    } else if (str1[0] == inside && str2 != str1) {
		endlabels.insert(pair<int, int>(i, i));	    
	    }
	}
    }
    
    for (i = 0; i < len; i++) {
	int col = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	str = seq[i][col];
	
	if (str[0] != begin && str[0] != inside) {
	    continue;
	}
	
	elbit = endlabels.find(i);
	if (elbit != endlabels.end()) {
	    newlabel = END;
	    newlabel += strtail(str);
	    seq[i][col] = newlabel;
	
	} else {
	    newlabel = INSIDE;
	    newlabel += strtail(str);
	    seq[i][col] = newlabel;
	}	
    }
} 

void convert_IOE2_IOB2(int is_cap, int is_last, sequence & seq) {
    map<int, int> beginlabels;
    map<int, int>::iterator blbit;
    
    char begin = is_cap ? 'B' : 'b';
    string BEGIN = is_cap ? "B" : "b";
    char inside = is_cap ? 'I' : 'i';
    string INSIDE = is_cap ? "I" : "i";
    char end = is_cap ? 'E' : 'e';
    
    string str1, str2, istr, str, newlabel;
    int i, len = seq.size();
    
    for (i = 0; i < len; i++) {
	int col2 = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	str2 = seq[i][col2];
	
	if (str2[0] != end && str2[0] != inside) {
	    continue;
	}
	
	if (i == 0) {
	    beginlabels.insert(pair<int, int>(i, i));

	} else {
	    int col1 = is_last ? seq[i - 1].size() - 1 : seq[i - 1].size() - 2;
	    str1 = seq[i - 1][col1];
	    
	    istr = INSIDE;
	    istr += strtail(str2);
	    
	    if (str2[0] == end && str1 != istr) {
		beginlabels.insert(pair<int, int>(i, i));
	    
	    } else if (str2[0] == inside && str1 != str2) {
		beginlabels.insert(pair<int, int>(i, i));	    
	    }
	}
    }
    
    for (i = 0; i < len; i++) {
	int col = is_last ? seq[i].size() - 1 : seq[i].size() - 2;
	str = seq[i][col];
	
	if (str[0] != end && str[0] != inside) {
	    continue;
	}
	
	blbit = beginlabels.find(i);
	
	if (blbit != beginlabels.end()) {
	    newlabel = BEGIN;
	    newlabel += strtail(str);
	    seq[i][col] = newlabel;
	
	} else {
	    newlabel = INSIDE;
	    newlabel += strtail(str);
	    seq[i][col] = newlabel;
	}	
    }
} 

void convert_IOB2_IOE1(int is_cap, int is_last, sequence & seq) {
    convert_IOB2_IOE2(is_cap, is_last, seq);
    convert_IOE2_IOE1(is_cap, is_last, seq);
}
void convert_IOE1_IOB2(int is_cap, int is_last, sequence & seq) {
    convert_IOE1_IOE2(is_cap, is_last, seq);
    convert_IOE2_IOB2(is_cap, is_last, seq);
}

void convert_IOB1_IOE1(int is_cap, int is_last, sequence & seq) {
    convert_IOB1_IOB2(is_cap, is_last, seq);
    convert_IOB2_IOE1(is_cap, is_last, seq);
}
void convert_IOE1_IOB1(int is_cap, int is_last, sequence & seq) {
    convert_IOE1_IOB2(is_cap, is_last, seq);
    convert_IOB2_IOB1(is_cap, is_last, seq);
}

void convert_IOB1_IOE2(int is_cap, int is_last, sequence & seq) {
    convert_IOB1_IOB2(is_cap, is_last, seq);
    convert_IOB2_IOE2(is_cap, is_last, seq);
}
void convert_IOE2_IOB1(int is_cap, int is_last, sequence & seq) {
    convert_IOE2_IOB2(is_cap, is_last, seq);
    convert_IOB2_IOB1(is_cap, is_last, seq);
}


