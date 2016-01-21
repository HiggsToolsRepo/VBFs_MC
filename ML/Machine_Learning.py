#! /usr/bin/env python3

from optparse import OptionParser

## General import stuff! 
from multiprocessing import cpu_count
from concurrent.futures import ProcessPoolExecutor

import numpy as np
import pandas as pd
import itertools

import statsmodels.api as sm

import matplotlib.pyplot as plt
from sklearn.metrics import roc_auc_score, roc_curve
from sklearn.tree import export_graphviz

## Import classifier 
from sklearn.ensemble import GradientBoostingClassifier as Classifier
from sklearn.cross_validation import train_test_split

plt.style.use("../small.mplstyle")

##############################
#Option parser:
parser = OptionParser()
parser.add_option("-p", "--p", default=0, 
                  help="print distribution")

(options, args) = parser.parse_args()

print_distrib   = options.p
##############################


###### Set Random Seed  ######
RANDOM_SEED = 1234 # 0xdeadbeef
##############################

###
def mask_cuts(data):
    return (data['m j1, j2'] > 500) & (data['| Delta eta j1,j2 |'] > 2.4)

def cuts(data):
    return data[mask_cuts(data)]

def grid_fit(clf, params, X, Y, score, sample_weight=None):
    for spec in dict_product(params):
        for key in spec:
            setattr(clf, key, spec[key])
        clf.fit(X,Y,sample_weight=sample_weight)
        yield (spec, score(clf))

def score(clf):
    return roc_auc_score(y_test, clf.predict_proba(X_test)[:,1], sample_weight=w_test)
###


#### Import events
Signal_File     = '../ML_Events/NoCuts/signal_nocuts.txt'
Background_File = '../ML_Events/NoCuts/background_nocuts.txt'

signal = pd.DataFrame.from_csv(Signal_File, index_col= 0, sep='\t')
background = pd.DataFrame.from_csv(Background_File, index_col= 0, sep='\t')
signal.columns = background.columns = [s.strip() for s in signal.columns]

#Drop empy columns
emptycols = [c for c in signal.columns if c.startswith("Empty")]
signal.drop(emptycols, axis=1, inplace=True)
background.drop(emptycols, axis=1, inplace=True)

#Balance signal and background weights. Otherwise saying that everything is a signal is a cheap local minimum.
background.Weight *= signal.Weight.sum()/ background.Weight.sum()

data = pd.concat([signal, background])
features, weights = data.iloc[:, :-1], data.iloc[:,-1]
weights = np.abs(weights).as_matrix()
labels = np.r_[np.ones(len(signal)), np.zeros(len(background))]

#We split between testing and training by further starifying on weights, by ignoring the smaller ones for each
#category (for the purposes of splitting, we assign them to a third category '2' that tunrns out to be split evenly as well).
weighted_cats = labels.copy()
label_mask = labels.astype(bool)
small_signal = (weights < weights[label_mask].mean()) & label_mask
weighted_cats[small_signal] = 2

small_background = (weights < weights[~label_mask].mean()) & ~label_mask
weighted_cats[small_background] = 2


X_train, X_test, y_train, y_test, w_train, w_test = train_test_split(features, labels, weights,
                                                                     random_state = RANDOM_SEED, 
                                                                     test_size=0.5, stratify=weighted_cats)


##############################

### Classifier

clf = Classifier(n_estimators=3000, learning_rate=2, 
                 min_weight_fraction_leaf=0.20,
                 random_state=RANDOM_SEED)
clf.min_weight_fraction_leaf = 100


w_fractions = np.r_[np.logspace(-5,-2, 8) , np.linspace(0.1, 0.5, 20)]
params = {'min_weight_fraction_leaf': w_fractions}

it = iter(grid_fit(clf, params, X_train, y_train, score=score, sample_weight=w_train))

###########
#pool = ProcessPoolExecutor()
#result = pool.map(it)
###########
# I don't really understand what happens here!
# let's assume we have a maximum (M) and an argument (Mi)
###########

Mi = 0.1
clf.min_weight_fraction_leaf = Mi
clf.fit(X_train,y_train, sample_weight=w_train)

signal_cut = cuts(signal)
background_cut = cuts(background)

predicted_test = clf.predict_proba(X_test)[:,1]
predicted_train = clf.predict_proba(X_train)[:,1]

predicted_test_cuts  = mask_cuts(X_test)

fpr, tpr, thresholds = roc_curve(y_test, predicted_test, sample_weight=w_test)
fpr_cuts, tpr_cuts, thresholds_cuts = roc_curve(y_test, predicted_test_cuts, sample_weight=w_test)

clf_score = roc_auc_score(y_test, predicted_test, sample_weight=w_test)
cut_score = roc_auc_score(y_test, predicted_test_cuts, sample_weight=w_test)

plt.plot(fpr, fpr, 'k--')

plt.plot(fpr, tpr, label="Classifier (%.2f)" % clf_score)

plt.plot(fpr_cuts, tpr_cuts, label="Cuts (%.2f)" % cut_score)

plt.title("ROC AUC")
plt.xlabel("False positive Rate")
plt.ylabel("True Positive Rate")
plt.xlim(0,1)
plt.ylim(0,1)
plt.legend()
plt.savefig("ROC_Curve.pdf")
plt.clf()

##### Best Features ##########

importances = pd.Series(importances, index=features.columns)
sv = importances.sort_values()
c,d = sv.index[-2:]
best_features = sv.index[-4:]
pairs = (itertools.combinations(best_features,2))
fig, axes = plt.subplots(2,3, figsize=(20, 10))
axiter = (ax  for l in axes for ax in l)
for (c,d), ax in zip(pairs, axiter):
    ax.scatter(c,d, data=signal, label="Signal")
    ax.scatter(c,d, data=background, color='orange', label="Background")
    ax.set_xlabel(c)
    ax.set_ylabel(d)
    ax.legend(loc='best')
plt.tight_layout()
plt.savefig("features.pdf")
plt.clf()

##############################

#### Plot distributions of single variable
# disabled by default, use -p 1 to enable
if(print_distrib == 1):
    for feature in cols:
    
        dens_sig = sm.nonparametric.KDEUnivariate(signal[feature].as_matrix())
        dens_sig.fit(weights=signal.Weight.as_matrix(), fft=False)

        dens_bg= sm.nonparametric.KDEUnivariate(background[feature].as_matrix())
        dens_bg.fit(weights=background.Weight.as_matrix(), fft=False)
        plt.figure()
        plt.title(feature)
    
        x = np.linspace(data[feature].min(), data[feature].max(), 1000)
        plt.plot(x, dens_sig.evaluate(x), label="Signal")
        plt.plot(x, dens_bg.evaluate(x), label="Background")
        plt.legend()
        title = str(cols) + '.pdf'
        plt.savefig(title)
        plt.clf()

##############################
probs = np.linspace(0,1,30)
sp = clf.predict_proba(signal[cols])[:,1]
bp = clf.predict_proba(background[cols])[:,1]

plt.hist(sp, weights=signal.Weight.abs(), normed=True, bins=20, label="Signal")
plt.hist(bp, weights=background.Weight.abs(), normed=True, bins=20, alpha=0.5, label="Background")
plt.legend()
plt.savefig('SvsB.pdf')
plt.clf
