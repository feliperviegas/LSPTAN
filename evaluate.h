#ifndef EVALUATE_H__
#define EVALUATE_H__

#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <queue>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
//#include "ttmath/ttmath.h"

using namespace std;

struct Contigency {
	unsigned int TP;
	unsigned int FP;
	unsigned int TN;
	unsigned int FN;
};

double micro_f1(map<string, Contigency> contigency) {
	double sum_tp = 0.0; // numerators
	double sum_TPfp = 0.0, sum_TPfn = 0.0; // precision/recall denominators

	map<string, Contigency>::const_iterator it = contigency.begin();
	while (it != contigency.end()) {
		sum_tp += it->second.TP;
		sum_TPfp += it->second.TP + it->second.FP;
		sum_TPfn += it->second.TP + it->second.FN;
		++it;
	}
	double p_mic = (sum_TPfp == 0.0) ? 0.0 : sum_tp / sum_TPfp;
	double r_mic = (sum_TPfn == 0.0) ? 0.0 : sum_tp / sum_TPfn;

	// F1: harmonic mean of p and r
	return (2.0 * p_mic * r_mic) / (p_mic + r_mic);
}

double macro_f1(map<string, Contigency> contigency, set<string> classes) {
	double sum_f1 = 0.0;

	map<string, Contigency>::const_iterator it = contigency.begin();
	while (it != contigency.end()) {
		double sum_tp = it->second.TP;
		double sum_TPfp = it->second.TP + it->second.FP;
		double sum_TPfn = it->second.TP + it->second.FN;
		double p = (sum_TPfp == 0.0) ? 0.0 : sum_tp / sum_TPfp;
		double r = (sum_TPfn == 0.0) ? 0.0 : sum_tp / sum_TPfn;
		double f1 = (r == 0.0) ? 0.0 : (2.0 * p * r) / (p + r);
		//      cerr << "F1(" << it->first << ") = " << f1 << endl;
		sum_f1 += f1;
		++it;
	}
	return sum_f1 / classes.size();
}

void summary(set<string> classes, map<string, Contigency>& contigency,
		map<string, map<string, unsigned int> > preds) {
	set<string>::const_iterator posIt = classes.begin(); // positive class
	while (posIt != classes.end()) {
		contigency[*posIt].TP = 0;
		contigency[*posIt].FP = 0;
		contigency[*posIt].TN = 0;
		contigency[*posIt].FN = 0;

		set<string>::const_iterator gIt = classes.begin(); // gold standard
		while (gIt != classes.end()) {

			set<string>::const_iterator pIt = classes.begin(); // predicted class
			while (pIt != classes.end()) {

				if (gIt->compare(*posIt) != 0) { // evaluate negative classes
					if (pIt->compare(*posIt) == 0)
						contigency[*posIt].FP += preds[*gIt][*pIt];
					else
						contigency[*posIt].TN += preds[*gIt][*pIt];
				} else { // evaluate predictions of positive classes
					if (pIt->compare(*gIt) == 0)
						contigency[*posIt].TP += preds[*gIt][*pIt];
					else
						contigency[*posIt].FN += preds[*gIt][*pIt];
				}
				++pIt;
			}
			++gIt;
		}
		++posIt;
	}
}

void addPrediction(map<string, map<string, unsigned int> >& preds,
		set<string>& classes, const string &t, const string &p) {
	preds[t][p]++;
	classes.insert(t);
}

string int2string(int number) {
	ostringstream ss;
	ss << number;
	return ss.str();
}

double evaluate(int *realClass, int *predictClass, int numDocsTest, int opt) {
	map<string, map<string, unsigned int> > preds;
	map<string, Contigency> contigency;
	set < string > classes;
	int i;
	for (i = 0; i < numDocsTest; i++) {
		addPrediction(preds, classes, int2string(realClass[i]),
				int2string(predictClass[i]));
	}
	summary(classes, contigency, preds);
	double macF1 = macro_f1(contigency, classes);
	double micF1 = micro_f1(contigency);

	if (opt) {
		return macF1;
	} else {
		return micF1;
	}
}

#endif
