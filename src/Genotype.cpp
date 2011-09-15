#include "Genotype.h"
#include "multichoose.h"
#include "multipermute.h"


vector<Allele> Genotype::uniqueAlleles(void) {
    vector<Allele> uniques;
    for (Genotype::iterator g = this->begin(); g != this->end(); ++g)
        uniques.push_back(g->allele);
    return uniques;
}

int Genotype::getPloidy(void) {
    int result = 0;
    for (Genotype::const_iterator i = this->begin(); i != this->end(); ++i) {
        result += i->count;
    }
    return result;
}

vector<int> Genotype::counts(void) {
    vector<int> counts;
    for (Genotype::iterator i = this->begin(); i != this->end(); ++i) {
        counts.push_back(i->count);
    }
    return counts;
}

vector<Allele> Genotype::alternateAlleles(string& base) {
    vector<Allele> alleles;
    for (Genotype::iterator i = this->begin(); i != this->end(); ++i) {
        Allele& b = i->allele;
        if (base != b.currentBase)
            alleles.push_back(b);
    }
    return alleles;
}

int Genotype::alleleCount(const string& base) {
    map<string, int>::iterator ge = alleleCounts.find(base);
    if (ge == alleleCounts.end()) {
        return 0;
    } else {
        return ge->second;
    }
}

int Genotype::alleleCount(Allele& allele) {
    map<string, int>::iterator ge = alleleCounts.find(allele.currentBase);
    if (ge == alleleCounts.end()) {
        return 0;
    } else {
        return ge->second;
    }
}

string Genotype::relativeGenotype(string& refbase, vector<Allele>& alts) {
    vector<string> rg;
    for (Genotype::iterator i = this->begin(); i != this->end(); ++i) {
        Allele& b = i->allele;
        string& base = b.currentBase;
        if (base == refbase) {
            for (int j = 0; j < i->count; ++j)
                rg.push_back("0");
        } else {
            int n = 1;
            bool matchingalt = false;
            for (vector<Allele>::iterator a = alts.begin(); a != alts.end(); ++a, ++n) {
                if (base == a->base()) {
                    matchingalt = true;
                    for (int j = 0; j < i->count; ++j)
                        rg.push_back(convert(n));
                    break;
                }
            }
            if (!matchingalt) {
                for (int j = 0; j < i->count; ++j)
                    rg.push_back(".");
            }
        }
    }
    sort(rg.begin(), rg.end()); // enforces the same ordering for all genotypes
    //reverse(rg.begin(), rg.end()); // 1/0 ordering, or 1/1/0 etc.
    string result = join(rg, "/");
    return result; // chop trailing '/'
}

void Genotype::relativeGenotype(vector<int>& rg, string& refbase, vector<Allele>& alts) {
    for (Genotype::iterator i = this->begin(); i != this->end(); ++i) {
        Allele& b = i->allele;
        string& base = b.currentBase;
        if (base == refbase) {
            for (int j = 0; j < i->count; ++j)
                rg.push_back(0);
        } else {
            int n = 1;
            bool matchingalt = false;
            for (vector<Allele>::iterator a = alts.begin(); a != alts.end(); ++a, ++n) {
                if (base == a->base()) {
                    matchingalt = true;
                    for (int j = 0; j < i->count; ++j)
                        rg.push_back(n);
                    break;
                }
            }
            if (!matchingalt) {
                for (int j = 0; j < i->count; ++j)
                    rg.push_back(-1);
            }
        }
    }
    sort(rg.begin(), rg.end()); // enforces the same ordering for all genotypes
    //reverse(rg.begin(), rg.end()); // 1/0 ordering, or 1/1/0 etc.
}

void Genotype::relativeGenotype(vector<int>& rg, vector<Allele>& alleles) {
    for (Genotype::iterator i = this->begin(); i != this->end(); ++i) {
        Allele& b = i->allele;
        string& base = b.currentBase;
        int n = 0;
        bool matchingalt = false;
        for (vector<Allele>::iterator a = alleles.begin(); a != alleles.end(); ++a, ++n) {
            if (base == a->base()) {
                matchingalt = true;
                for (int j = 0; j < i->count; ++j)
                    rg.push_back(n);
                break;
            }
        }
        if (!matchingalt) {
            for (int j = 0; j < i->count; ++j)
                rg.push_back(-1);
        }
    }
    sort(rg.begin(), rg.end()); // enforces the same ordering for all genotypes
    //reverse(rg.begin(), rg.end()); // 1/0 ordering, or 1/1/0 etc.
}

string Genotype::relativeGenotype(string& refbase, string& altbase) {
    vector<string> rg;
    for (Genotype::iterator i = this->begin(); i != this->end(); ++i) {
        Allele& b = i->allele;
        if (b.currentBase == altbase && refbase != b.currentBase) {
            for (int j = 0; j < i->count; ++j)
                rg.push_back("1/");
        } else if (b.currentBase != altbase && refbase != b.currentBase) {
            for (int j = 0; j < i->count; ++j)
                rg.push_back("./");
        } else {
            for (int j = 0; j < i->count; ++j)
                rg.push_back("0/");
        }
    }
    sort(rg.begin(), rg.end()); // enforces the same ordering for all genotypes
    //reverse(rg.begin(), rg.end()); // 1/0 ordering, or 1/1/0 etc.
    string result = accumulate(rg.begin(), rg.end(), string(""));
    return result.substr(0, result.size() - 1); // chop trailing '/'
}

bool Genotype::containsAllele(const string& base) {
    map<string, int>::iterator ge = alleleCounts.find(base);
    if (ge == alleleCounts.end()) {
        return false;
    } else {
        return true;
    }
}

bool Genotype::containsAllele(Allele& allele) {
    map<string, int>::iterator ge = alleleCounts.find(allele.currentBase);
    if (ge == alleleCounts.end()) {
        return false;
    } else {
        return true;
    }
}

bool Genotype::isHomozygous(void) {
    return this->size() == 1;
}

// the probability of drawing each allele out of the genotype, ordered by allele
vector<long double> Genotype::alleleProbabilities(void) {
    vector<long double> probs;
    for (vector<GenotypeElement>::const_iterator a = this->begin(); a != this->end(); ++a) {
        probs.push_back((long double) a->count / (long double) ploidy);
    }
    return probs;
}

string Genotype::slashstr(void) {
    string s;
    for (Genotype::const_iterator ge = this->begin(); ge != this->end(); ++ge) {
        for (int i = 0; i < ge->count; ++i)
            s += ((ge == this->begin() && i == 0) ? "" : "/") + ge->allele.alternateSequence;
    }
    return s;
}

string Genotype::str(void) {
    string s;
    for (Genotype::const_iterator ge = this->begin(); ge != this->end(); ++ge) {
        for (int i = 0; i < ge->count; ++i)
            s += ge->allele.alternateSequence;
    }
    return s;
}

string IUPAC(Genotype& genotype) {
    const string g = genotype.str();
    if (g == "AA") return "A";
    if (g == "AC") return "M";
    if (g == "AG") return "R";
    if (g == "AT") return "W";
    if (g == "CA") return "M";
    if (g == "CC") return "C";
    if (g == "CG") return "S";
    if (g == "CT") return "Y";
    if (g == "GA") return "R";
    if (g == "GC") return "S";
    if (g == "GG") return "G";
    if (g == "GT") return "K";
    if (g == "TA") return "W";
    if (g == "TC") return "Y";
    if (g == "TG") return "K";
    if (g == "TT") return "T";
    return g;
}

string IUPAC2GenotypeStr(string iupac, int ploidy) {
    if (iupac == "A") return "AA";
    if (iupac == "M") return "AC";
    if (iupac == "R") return "AG";
    if (iupac == "W") return "AT";
    if (iupac == "C") return "CC";
    if (iupac == "S") return "CG";
    if (iupac == "Y") return "CT";
    if (iupac == "G") return "GG";
    if (iupac == "K") return "GT";
    if (iupac == "T") return "TT";
    return iupac;
}

ostream& operator<<(ostream& out, const GenotypeElement& rhs) {
    for (int i = 0; i < rhs.count; ++i)
        out << rhs.allele.base();
    //for (int i = 0; i < rhs.second; ++i)
    //    out << rhs.first.alternateSequence;
    return out;
}

ostream& operator<<(ostream& out, const Genotype& g) {
    Genotype::const_iterator i = g.begin(); ++i;
    out << g.front();
    for (;i != g.end(); ++i) {
        out << *i;
    }
    return out;
}

ostream& operator<<(ostream& out, list<GenotypeCombo>& g) {
    for (list<GenotypeCombo>::iterator i = g.begin(); i != g.end(); ++i) {
        out << *i << endl;
    }
    return out;
}

ostream& operator<<(ostream& out, GenotypeCombo& g) {
    GenotypeCombo::iterator i = g.begin(); ++i;
    out << "combo posterior prob: " << g.posteriorProb << endl;
    out << "{\"" << g.front()->name << "\":[\"" << *(g.front()->genotype) << "\"," << exp(g.front()->prob) << "]";
    for (;i != g.end(); ++i) {
        out << ", \"" << (*i)->name << "\":[\"" << *((*i)->genotype) << "\"," << exp((*i)->prob) << "]";
    }
    out << "}";
    return out;
}


bool operator<(Genotype& a, Genotype& b) {
    // genotypes of different ploidy are evaluated according to their relative ploidy
    if (a.ploidy != b.ploidy)
        return a.ploidy < b.ploidy;
    // because our constructor sorts each Genotype.alleles, we assume that we
    // have two equivalently sorted vectors to work with
    Genotype::iterator ai = a.begin();
    Genotype::iterator bi = b.begin();
    // step through each genotype, and if we find a difference between either
    // their allele or count return a<b
    for (; ai != a.end() && bi != b.end(); ++ai, ++bi) {
        if (ai->allele != bi->allele)
            return ai->allele < bi->allele;
        else if (ai->count != bi->count)
            return ai->count < bi->count;
    }
    return false; // if the two are equal, then we return false per C++ convention
}

vector<Genotype> allPossibleGenotypes(int ploidy, vector<Allele> potentialAlleles) {
    vector<Genotype> genotypes;
    vector<vector<Allele> > alleleCombinations = multichoose(ploidy, potentialAlleles);
    for (vector<vector<Allele> >::iterator combo = alleleCombinations.begin(); combo != alleleCombinations.end(); ++combo) {
        genotypes.push_back(Genotype(*combo));
    }
    return genotypes;
}


int GenotypeCombo::numberOfAlleles(void) {
    int count = 0;
    for (map<string, AlleleCounter>::iterator f = alleleCounters.begin(); f != alleleCounters.end(); ++f) {
        const AlleleCounter& allele = f->second;
        count += allele.frequency;
    }
    return count;
}

// initializes cached counts associated with each GenotypeCombo
void GenotypeCombo::init(bool useObsExpectations) {
    for (GenotypeCombo::iterator s = begin(); s != end(); ++s) {
        const SampleDataLikelihood& sdl = **s;
        const Sample& sample = *sdl.sample;

        ++genotypeCounts[sdl.genotype];

        permutationsln += sdl.genotype->permutationsln;

        //cerr << *sdl.genotype << endl;
        for (Genotype::iterator a = sdl.genotype->begin(); a != sdl.genotype->end(); ++a) {
            const string& alleleBase = a->allele.currentBase;

            // allele frequencies in selected genotypes in combo
            AlleleCounter& alleleCounter = alleleCounters[alleleBase];
            //cerr <<"init "<< alleleCounter.frequency;
            alleleCounter.frequency += a->count;
            //cerr <<" after "<< alleleCounter.frequency << endl;

            if (useObsExpectations) {
                // observational frequencies for binomial priors
                Sample::const_iterator as = sample.find(alleleBase);
                if (as != sample.end()) {
                    vector<Allele*> alleles = as->second;
                    alleleCounter.observations += alleles.size();
                    for (vector<Allele*>::iterator o = alleles.begin(); o != alleles.end(); ++o) {
                        const Allele& allele = **o;
                        if (allele.basesLeft >= allele.basesRight) {
                            ++alleleCounter.placedLeft;
                            if (allele.strand == STRAND_FORWARD) {
                                ++alleleCounter.placedStart;
                            } else {
                                ++alleleCounter.placedEnd;
                            }
                        } else {
                            ++alleleCounter.placedRight;
                            if (allele.strand == STRAND_FORWARD) {
                                ++alleleCounter.placedEnd;
                            } else {
                                ++alleleCounter.placedStart;
                            }
                        }
                        if (allele.strand == STRAND_FORWARD) {
                            ++alleleCounter.forwardStrand;
                        } else {
                            ++alleleCounter.reverseStrand;
                        }
                    }
                }
            }
        }
    }
}

// frequency... should this just be "allele count"?
int GenotypeCombo::alleleCount(Allele& allele) {
    map<string, AlleleCounter>::iterator f = alleleCounters.find(allele.currentBase);
    if (f == alleleCounters.end()) {
        return 0;
    } else {
        return f->second.frequency;
    }
}

int GenotypeCombo::alleleCount(const string& allele) {
    map<string, AlleleCounter>::iterator f = alleleCounters.find(allele);
    if (f == alleleCounters.end()) {
        return 0;
    } else {
        return f->second.frequency;
    }
}

long double GenotypeCombo::alleleFrequency(Allele& allele) {
    return alleleCount(allele) / (long double) numberOfAlleles();
}

long double GenotypeCombo::alleleFrequency(const string& allele) {
    return alleleCount(allele) / (long double) numberOfAlleles();
}

long double GenotypeCombo::genotypeFrequency(Genotype* genotype) {
    map<Genotype*, int>::iterator g = genotypeCounts.find(genotype);
    if (g == genotypeCounts.end()) {
        return 0;
    } else {
        return g->second / size();
    }
}

void GenotypeCombo::updateCachedCounts(
        Sample* sample,
        Genotype* oldGenotype,
        Genotype* newGenotype,
        bool useObsExpectations) {

    // update genotype counts
    --genotypeCounts[oldGenotype];
    ++genotypeCounts[newGenotype];

    // update permutations
    permutationsln -= oldGenotype->permutationsln;
    permutationsln += newGenotype->permutationsln;

    // remove allele frequencies which are now 0 or below
    map<Genotype*, int>::iterator gc = genotypeCounts.begin();
    while (gc != genotypeCounts.end()) {
        assert(gc->second >= 0);
        if (gc->second == 0) {
            genotypeCounts.erase(gc++);
        } else {
            ++gc;
        }
    }

    // TODO can we improve efficiency by only adjusting for bases which are actually changed

    // remove allele frequency information for old genotype
    for (Genotype::iterator g = oldGenotype->begin(); g != oldGenotype->end(); ++g) {
        GenotypeElement& ge = *g;
        const string& base = ge.allele.currentBase;
        AlleleCounter& alleleCounter = alleleCounters[base];
        alleleCounter.frequency -= ge.count;
        if (useObsExpectations) {
            Sample::iterator s = sample->find(base);
            if (s != sample->end()) {
                const vector<Allele*>& alleles = s->second;
                alleleCounter.observations -= alleles.size();
                int forward_strand = 0;
                int reverse_strand = 0;
                int placed_left = 0;
                int placed_right = 0;
                int placed_start = 0;
                int placed_end = 0;
                for (vector<Allele*>::const_iterator a = alleles.begin(); a != alleles.end(); ++a) {
                    const Allele& allele = **a;
                    if (allele.strand == STRAND_FORWARD) {
                        ++forward_strand;
                    } else {
                        ++reverse_strand;
                    }
                    if (allele.basesLeft >= allele.basesRight) {
                        ++placed_left;
                        if (allele.strand == STRAND_FORWARD) {
                            ++placed_start;
                        } else {
                            ++placed_end;
                        }
                    } else {
                        ++placed_right;
                        if (allele.strand == STRAND_FORWARD) {
                            ++placed_end;
                        } else {
                            ++placed_start;
                        }
                    }
                }
                alleleCounter.forwardStrand -= forward_strand;
                alleleCounter.reverseStrand -= reverse_strand;
                alleleCounter.placedLeft -= placed_left;
                alleleCounter.placedRight -= placed_right;
                alleleCounter.placedStart -= placed_start;
                alleleCounter.placedEnd -= placed_end;
            }
        }
    }

    // add allele frequency information for new genotype
    for (Genotype::iterator g = newGenotype->begin(); g != newGenotype->end(); ++g) {
        GenotypeElement& ge = *g;
        const string& base = ge.allele.currentBase;
        AlleleCounter& alleleCounter = alleleCounters[base];
        alleleCounter.frequency += ge.count;
        if (useObsExpectations) {
            Sample::iterator s = sample->find(base);
            if (s != sample->end()) {
                const vector<Allele*>& alleles = s->second;
                alleleCounter.observations += alleles.size();
                int forward_strand = 0;
                int reverse_strand = 0;
                int placed_left = 0;
                int placed_right = 0;
                int placed_start = 0;
                int placed_end = 0;
                for (vector<Allele*>::const_iterator a = alleles.begin(); a != alleles.end(); ++a) {
                    const Allele& allele = **a;
                    if (allele.strand == STRAND_FORWARD) {
                        ++forward_strand;
                    } else {
                        ++reverse_strand;
                    }
                    if (allele.basesLeft >= allele.basesRight) {
                        ++placed_left;
                        if (allele.strand == STRAND_FORWARD) {
                            ++placed_start;
                        } else {
                            ++placed_end;
                        }
                    } else {
                        ++placed_right;
                        if (allele.strand == STRAND_FORWARD) {
                            ++placed_end;
                        } else {
                            ++placed_start;
                        }
                    }
                }
                alleleCounter.forwardStrand += forward_strand;
                alleleCounter.reverseStrand += reverse_strand;
                alleleCounter.placedLeft += placed_left;
                alleleCounter.placedRight += placed_right;
                alleleCounter.placedStart += placed_start;
                alleleCounter.placedEnd += placed_end;
            }
        }
    }

    // remove allele frequencies which are now 0 or below
    map<string, AlleleCounter>::iterator af = alleleCounters.begin();
    while (af != alleleCounters.end()) {
        assert(af->second.frequency >= 0);
        if (af->second.frequency == 0) {
            assert(af->second.observations == 0);
            alleleCounters.erase(af++);
        } else {
            ++af;
        }
    }

}

map<int, int> GenotypeCombo::countFrequencies(void) {
    map<int, int> frequencyCounts;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        const AlleleCounter& allele = a->second;
        map<int, int>::iterator c = frequencyCounts.find(allele.frequency);
        if (c != frequencyCounts.end()) {
            c->second += 1;
        } else {
            frequencyCounts[allele.frequency] = 1;
        }
    }
    return frequencyCounts;
}

vector<int> GenotypeCombo::counts(void) {
    //map<string, int> alleleCounters = countAlleles();
    vector<int> counts;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        const AlleleCounter& allele = a->second;
        counts.push_back(allele.frequency);
    }
    return counts;
}

int GenotypeCombo::hetCount(void) {
    int hc = 0;
    for (GenotypeCombo::iterator s = begin(); s != end(); ++s) {
        if (!(*s)->genotype->homozygous) {
            ++hc;
        }
    }
    return hc;
}

vector<int> GenotypeCombo::observationCounts(void) {
    vector<int> counts;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        const AlleleCounter& allele = a->second;
        counts.push_back(allele.observations);
    }
    return counts;
}

// how many copies of the locus are in the whole genotype combination?
int GenotypeCombo::ploidy(void) {
    int copies = 0;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        const AlleleCounter& allele = a->second;
        copies += allele.frequency;
    }
    return copies;
}

vector<long double> GenotypeCombo::alleleProbs(void) {
    vector<long double> probs;
    long double copies = ploidy();
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        const AlleleCounter& allele = a->second;
        probs.push_back(allele.frequency / copies);
    }
    return probs;
}

vector<string> GenotypeCombo::alleles(void) {
    vector<string> bases;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        bases.push_back(a->first);
    }
    return bases;
}

// returns true if the combination is 100% homozygous
bool GenotypeCombo::isHomozygous(void) {
    return alleleCounters.size() == 1;
}

void sortSampleDataLikelihoods(vector<SampleDataLikelihood>& likelihoods) {
    SampleDataLikelihoodCompare datalikelihoodCompare;
    sort(likelihoods.begin(), likelihoods.end(), datalikelihoodCompare);
    int i = 0;
    for (vector<SampleDataLikelihood>::iterator sdl = likelihoods.begin(); sdl != likelihoods.end(); ++sdl) {
        sdl->rank = i++;
    }
}

bool sortSampleDataLikelihoodsByMarginals(vector<SampleDataLikelihood>& likelihoods) {
    SampleMarginalCompare marginalLikelihoodCompare;
    sort(likelihoods.begin(), likelihoods.end(), marginalLikelihoodCompare);
    bool reordered = false;
    int i = 0;
    for (vector<SampleDataLikelihood>::iterator sdl = likelihoods.begin(); sdl != likelihoods.end(); ++sdl) {
        int newrank = i++;
        if (sdl->rank != newrank) {
            reordered = true;
            sdl->rank = newrank;
        }
    }
    return reordered;
}

bool sortSampleDataLikelihoodsByMarginals(SampleDataLikelihoods& samplesLikelihoods) {
    bool reordered = false;
    for (SampleDataLikelihoods::iterator s = samplesLikelihoods.begin(); s != samplesLikelihoods.end(); ++s) {
        reordered |= sortSampleDataLikelihoodsByMarginals(*s);
    }
    return reordered;
}

bool sortSampleDataLikelihoodsByMarginalsAndObs(vector<SampleDataLikelihood>& likelihoods) {
    SampleMarginalAndObsCompare marginalLikelihoodAndObsCompare;
    sort(likelihoods.begin(), likelihoods.end(), marginalLikelihoodAndObsCompare);
    bool reordered = false;
    int i = 0;
    for (vector<SampleDataLikelihood>::iterator sdl = likelihoods.begin(); sdl != likelihoods.end(); ++sdl) {
        int newrank = i++;
        if (sdl->rank != newrank) {
            reordered = true;
            sdl->rank = newrank;
        }
    }
    return reordered;
}

bool sortSampleDataLikelihoodsByMarginalsAndObs(SampleDataLikelihoods& samplesLikelihoods) {
    bool reordered = false;
    for (SampleDataLikelihoods::iterator s = samplesLikelihoods.begin(); s != samplesLikelihoods.end(); ++s) {
        reordered |= sortSampleDataLikelihoodsByMarginalsAndObs(*s);
    }
    return reordered;
}

bool sortSampleDataLikelihoodsScaledByMarginals(vector<SampleDataLikelihood>& likelihoods) {
    SampleLikelihoodCompare likelihoodCompare;
    sort(likelihoods.begin(), likelihoods.end(), likelihoodCompare);
    bool reordered = false;
    int i = 0;
    for (vector<SampleDataLikelihood>::iterator sdl = likelihoods.begin(); sdl != likelihoods.end(); ++sdl) {
        int newrank = i++;
        if (sdl->rank != newrank) {
            reordered = true;
            sdl->rank = newrank;
        }
    }
    return reordered;
}

bool sortSampleDataLikelihoodsScaledByMarginals(SampleDataLikelihoods& samplesLikelihoods) {
    bool reordered = false;
    for (SampleDataLikelihoods::iterator s = samplesLikelihoods.begin(); s != samplesLikelihoods.end(); ++s) {
        reordered |= sortSampleDataLikelihoodsScaledByMarginals(*s);
    }
    return reordered;
}

// assumes that the data likelihoods are sorted
void
dataLikelihoodMaxGenotypeCombo(
    GenotypeCombo& combo,
    SampleDataLikelihoods& sampleDataLikelihoods,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    for (SampleDataLikelihoods::iterator s = sampleDataLikelihoods.begin();
            s != sampleDataLikelihoods.end(); ++s) {
        SampleDataLikelihood* sdl = &s->at(0);
        combo.push_back(sdl);
        combo.probObsGivenGenotypes += sdl->prob;
    }

    combo.init(binomialObsPriors);
    combo.calculatePosteriorProbability(theta,
                                        pooled,
                                        ewensPriors,
                                        permute,
                                        hwePriors,
                                        binomialObsPriors,
                                        alleleBalancePriors,
                                        diffusionPriorScalar);

}

void
makeComboByDatalLikelihoodRank(
    GenotypeCombo& combo,
    vector<int>& initialPosition,  // starting combo in terms of offsets from data likelihood maximum
    SampleDataLikelihoods& variantSampleDataLikelihoods,
    SampleDataLikelihoods& invariantSampleDataLikelihoods,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    // generate the best genotype combination according to data
    // likelihoods
    vector<int>::iterator offset = initialPosition.begin();
    for (SampleDataLikelihoods::iterator s = variantSampleDataLikelihoods.begin();
            s != variantSampleDataLikelihoods.end(); ++s) {
        // use the offsets to generate the starting combination
        SampleDataLikelihood* sdl = &s->at(*offset++);
        combo.push_back(sdl);
        combo.probObsGivenGenotypes += sdl->prob;
    }

    // these samples have well-differentiated data likelihoods, and
    // aren't changed during posterior integration
    for (SampleDataLikelihoods::iterator s = invariantSampleDataLikelihoods.begin();
            s != invariantSampleDataLikelihoods.end(); ++s) {
        SampleDataLikelihood* sdl = &s->at(*offset++);
        combo.push_back(sdl);
        combo.probObsGivenGenotypes += sdl->prob;
    }

    combo.init(binomialObsPriors);
    combo.calculatePosteriorProbability(theta,
                                        pooled,
                                        ewensPriors,
                                        permute,
                                        hwePriors,
                                        binomialObsPriors,
                                        alleleBalancePriors,
                                        diffusionPriorScalar);

}

// 'local' genotype combinations which step only in one sample away from the
// data likelihood maxiumum.  deal with all genotypes.
void
allLocalGenotypeCombinations(
    list<GenotypeCombo>& combos,
    GenotypeCombo& comboKing,
    SampleDataLikelihoods& sampleDataLikelihoods,
    Samples& samples,
    float logStepMax,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    // make the data likelihood maximum if needed
    if (comboKing.empty()) {
        vector<int> initialPosition;
        initialPosition.assign(sampleDataLikelihoods.size(), 0);
        SampleDataLikelihoods nullDataLikelihoods; // dummy variable
        makeComboByDatalLikelihoodRank(comboKing,
                initialPosition,
                sampleDataLikelihoods,
                nullDataLikelihoods,
                theta,
                pooled,
                ewensPriors,
                permute,
                hwePriors,
                binomialObsPriors,
                alleleBalancePriors,
                diffusionPriorScalar);
    }

    // ensure the comboKing is added
    if (combos.empty()) {
        combos.push_back(comboKing);
    }

    // for each sampledatalikelihood
    // add a combo for each genotype where the combo is one step from the comboKing
    size_t sampleOffset = 0;
    //GenotypeCombo::iterator sampleGenotypeItr = comboKing.begin();
    for (SampleDataLikelihoods::iterator s = sampleDataLikelihoods.begin();
            s != sampleDataLikelihoods.end(); ++s, ++sampleOffset) {
        SampleDataLikelihood& oldsdl = *comboKing.at(sampleOffset);
        vector<SampleDataLikelihood>& sdls = *s;
        for (vector<SampleDataLikelihood>::iterator dl = sdls.begin(); dl != sdls.end(); ++dl) {

            SampleDataLikelihood& newsdl = *dl;
            if (newsdl.genotype == oldsdl.genotype) {  // don't duplicate the comboKing
                continue;
            }
            combos.push_back(comboKing);
            GenotypeCombo& combo = combos.back();
            // get the old and new genotypes, which we compare
            // to change the cached counts and probability of
            // the combo
            combo.updateCachedCounts(oldsdl.sample,
                    oldsdl.genotype, newsdl.genotype,
                    binomialObsPriors);
            // replace genotype with new genotype
            combo.at(sampleOffset) = &*dl;
            // find data likelihood difference from ComboKing
            long double diff = oldsdl.prob - newsdl.prob;
            // adjust combination total data likelihood
            combo.probObsGivenGenotypes -= diff;
            combo.calculatePosteriorProbability(theta,
                                            pooled,
                                            ewensPriors,
                                            permute,
                                            hwePriors,
                                            binomialObsPriors,
                                            alleleBalancePriors,
                                            diffusionPriorScalar);
        }
    }

}

bool
bandedGenotypeCombinations(
    list<GenotypeCombo>& combos,
    GenotypeCombo& comboKing,
    SampleDataLikelihoods& variantSampleDataLikelihoods,
    SampleDataLikelihoods& invariantSampleDataLikelihoods,
    Samples& samples,
    int bandwidth, int banddepth,
    float logStepMax,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    // get the number of samples that vary
    int nsamples = variantSampleDataLikelihoods.size();

    // cap bandwidth at the number of variant samples
    bandwidth = (bandwidth > nsamples) ? nsamples : bandwidth;

    // no variant samples
    if (nsamples == 0) {
        combos.push_back(comboKing);
        return true;
    }

    // overview:
    //
    // For each order of indexes in the bandwidth and banddepth, Obtain
    // all multiset permutations of a set of indexes.  Then use these
    // indexes to get the nth-best genotype from each individual's set
    // of genotypes for which we have data likelihoods
    // (sampleDataLikelihoods), and turn this set into a genotype
    // combination.  Update the combination probability inline here so
    // we don't incur O(N^2) penalty calculating the probability within
    // our genotypeCombinationPriors calculation loop, where we
    // estimate the posterior probability of each genotype combination
    // given its data likelihood and the prior probability of the
    // distribution of alleles it represents.
    // 
    // example (bandwidth = 2, banddepth = 2)
    //  indexes:      0 0 0 0 1, 0 0 0 1 1
    //
    //  permutations: 0 0 0 0 1 
    //                0 0 0 1 0
    //                0 0 1 0 0
    //                0 1 0 0 0
    //                1 0 0 0 0
    //                1 1 0 0 0 
    //                0 1 1 0 0 
    //                1 0 1 0 0 
    //                0 1 0 1 0 
    //                0 0 1 1 0 
    //                1 0 0 1 0 
    //                0 1 0 0 1 
    //                0 0 1 0 1 
    //                0 0 0 1 1 
    //                1 0 0 0 1 
    //
    // We then convert these permutation to genotype combinations by
    // using the index to pick the nth-best genotype according to
    // sorted individual genotype data likelihoods.
    //
    // In addition to this simple case, We can flexibly extend this to
    // larger search spaces by changing the depth and width of the
    // deviations from the data likelihood maximizer (aka 'king').
    //
    vector<int> depths;
    depths.reserve(banddepth);
    for (int i = 0; i < banddepth; ++i) {
        depths.push_back(i);
    }
    vector<vector<int> > deviations = multichoose(bandwidth, depths);

    // skip the first vector, which will always be the same as the
    // combo king, and has been pushed into our combinations already
    for (vector<vector<int> >::iterator d = deviations.begin(); d != deviations.end(); ++d) {
        vector<int>& indexes = *d;
        indexes.reserve(nsamples);
        for (int h = 0; h < (nsamples - bandwidth); ++h) {
            indexes.push_back(0);
        }
        vector<vector<int> > indexPermutations = multipermute(indexes);
        bool reuseLastCombo = false;
        for (vector<vector<int> >::const_iterator p = indexPermutations.begin(); p != indexPermutations.end(); ++p) {
            if (reuseLastCombo) {  // reuse the last combo if we've skipped it, saving a few % runtime copying combos
                reuseLastCombo = false;
            } else {
                combos.push_back(comboKing); // copy the king, and then we'll modify it according to the indicies
            }
            GenotypeCombo& combo = combos.back();
            GenotypeCombo::iterator sampleGenotypeItr = combo.begin();
            vector<int>::const_iterator n = p->begin();
            for (SampleDataLikelihoods::iterator s = variantSampleDataLikelihoods.begin();
                    s != variantSampleDataLikelihoods.end(); ++s, ++n, ++sampleGenotypeItr) {
                SampleDataLikelihood& oldsdl = **sampleGenotypeItr;
                SampleDataLikelihood*& oldsdl_ptr = *sampleGenotypeItr;
                vector<SampleDataLikelihood>& sdls = *s;
                int offset = *n + oldsdl.rank;
                if (offset > 0) {
                    // shift-back if this combo is beyond the bounds of the individual's set of genotypes
                    offset %= s->size();
                    // ignore this combo if the swapped genotype has a data likelihood more than logStepMax
                    // from the best data likelihood.  logStepMax == -1 indicates no filtering
                    if (offset > 0 && logStepMax >= 0 && sdls.front().prob - sdls.at(offset).prob > logStepMax) {
                        reuseLastCombo = true;
                        break;
                    }
                    // TODO factor out this update code
                    SampleDataLikelihood* newsdl = &sdls.at(offset);
                    // get the old and new genotypes, which we compare
                    // to change the cached counts and probability of
                    // the combo
                    combo.updateCachedCounts(oldsdl.sample,
                            oldsdl.genotype, newsdl->genotype,
                            binomialObsPriors);
                    // replace genotype with new genotype
                    oldsdl_ptr = newsdl;
                    // find data likelihood difference from ComboKing
                    long double diff = oldsdl.prob - newsdl->prob;
                    // adjust combination total data likelihood
                    combo.probObsGivenGenotypes -= diff;
                }
            }
            if (!reuseLastCombo) {
                combo.calculatePosteriorProbability(theta,
                                                pooled,
                                                ewensPriors,
                                                permute,
                                                hwePriors,
                                                binomialObsPriors,
                                                alleleBalancePriors,
                                                diffusionPriorScalar);
            }
        }
        if (reuseLastCombo) {
            combos.pop_back();
        }
    }

    return true;
}

void
convergentGenotypeComboSearch(
    list<GenotypeCombo>& combos,
    GenotypeCombo& comboKing,
    SampleDataLikelihoods& sampleDataLikelihoods,
    SampleDataLikelihoods& variantSampleDataLikelihoods,
    SampleDataLikelihoods& invariantSampleDataLikelihoods,
    Samples& samples,
    vector<Allele>& genotypeAlleles,
    int bandwidth, int banddepth,
    float logStepMax,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar,
    int maxiterations,
    bool addHomozygousCombos) {

    // sorting function object
    GenotypeComboResultSorter gcrSorter;

    if (comboKing.empty()) {
        // seed EM with the data likelihood maximum
        vector<int> initialPosition;
        initialPosition.assign(sampleDataLikelihoods.size(), 0);
        makeComboByDatalLikelihoodRank(comboKing,
                initialPosition,
                variantSampleDataLikelihoods,
                invariantSampleDataLikelihoods,
                theta,
                pooled,
                ewensPriors,
                permute,
                hwePriors,
                binomialObsPriors,
                alleleBalancePriors,
                diffusionPriorScalar);
    }

    // set best position, which is updated during the EM step
    GenotypeCombo bestCombo = comboKing;

    int i = 0;
    for (; i < maxiterations; ++i) {

        combos.clear();

        if (bandwidth == 0 && banddepth == 0) {
            allLocalGenotypeCombinations(
                    combos,
                    bestCombo,
                    sampleDataLikelihoods,
                    samples,
                    logStepMax,
                    theta,
                    pooled,
                    ewensPriors,
                    permute,
                    hwePriors,
                    binomialObsPriors,
                    alleleBalancePriors,
                    diffusionPriorScalar);
        } else {
            bandedGenotypeCombinations(
                    combos,
                    bestCombo,
                    variantSampleDataLikelihoods,
                    invariantSampleDataLikelihoods,
                    samples,
                    bandwidth,
                    banddepth,
                    logStepMax,
                    theta,
                    pooled,
                    ewensPriors,
                    permute,
                    hwePriors,
                    binomialObsPriors,
                    alleleBalancePriors,
                    diffusionPriorScalar);
        }

        // sort the results
        combos.sort(gcrSorter);
        //int previous_size = combos.size();
        // remove duplicates
        combos.unique();
        //cerr << "removed " << previous_size - combos.size() << " duplicates" << endl;

        // we've converged on the best homozygous combo, which suggests weak support for variation
        if (combos.front().isHomozygous()) {
            //cerr << "homozygous convergence in " << i << " iterations" << endl;
            break;
        }

        // check for convergence
        if (bestCombo == combos.front()) {
            // we've converged
            //cerr << "standard convergence in " << i << " iterations" << endl;
            //cerr << bestCombo << endl;
            break;
        } else {
            bestCombo = combos.front();
        }

    }

    //cout << i << " iterations" << "\t" << variantSampleDataLikelihoods.size() << " varying samples" << endl;

    // add the homozygous cases

    if (addHomozygousCombos) {
        addAllHomozygousCombos(combos,
                sampleDataLikelihoods,
                variantSampleDataLikelihoods,
                invariantSampleDataLikelihoods,
                samples,
                genotypeAlleles,
                bandwidth,
                banddepth,
                logStepMax,
                theta,
                pooled,
                ewensPriors,
                permute,
                hwePriors,
                binomialObsPriors,
                alleleBalancePriors,
                diffusionPriorScalar);
    }

    // sort the homozygous combos
    combos.sort(gcrSorter);

    // get best homozygous combo
    //GenotypeCombo* bestHomozygousCombo = &combos.front();

    combos.unique(); // remove duplicate combos!

}


void
bandedGenotypeCombinationsIncludingAllHomozygousCombos(
    list<GenotypeCombo>& combos,
    GenotypeCombo& comboKing,
    SampleDataLikelihoods& sampleDataLikelihoods,
    SampleDataLikelihoods& variantSampleDataLikelihoods,
    SampleDataLikelihoods& invariantSampleDataLikelihoods,
    Samples& samples,
    vector<Allele>& genotypeAlleles,
    int bandwidth, int banddepth,
    float logStepMax,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    if (comboKing.empty()) {
        // generate the initial maximum likelihood relative position for
        // integration. in this case we use the data likelihood maximum.
        vector<int> initialPosition;
        initialPosition.assign(sampleDataLikelihoods.size(), 0);
        makeComboByDatalLikelihoodRank(comboKing,
                initialPosition,
                variantSampleDataLikelihoods,
                invariantSampleDataLikelihoods,
                theta,
                pooled,
                ewensPriors,
                permute,
                hwePriors,
                binomialObsPriors,
                alleleBalancePriors,
                diffusionPriorScalar);
    }

    // obtain the combos

    bandedGenotypeCombinations(
            combos,
            comboKing,
            variantSampleDataLikelihoods,
            invariantSampleDataLikelihoods,
            samples,
            bandwidth,
            banddepth,
            logStepMax,
            theta,
            pooled,
            ewensPriors,
            permute,
            hwePriors,
            binomialObsPriors,
            alleleBalancePriors,
            diffusionPriorScalar);

    // add back the homozygous cases

    addAllHomozygousCombos(combos,
            sampleDataLikelihoods,
            variantSampleDataLikelihoods,
            invariantSampleDataLikelihoods,
            samples,
            genotypeAlleles,
            bandwidth,
            banddepth,
            logStepMax,
            theta,
            pooled,
            ewensPriors,
            permute,
            hwePriors,
            binomialObsPriors,
            alleleBalancePriors,
            diffusionPriorScalar);

}


void
bandedGenotypeCombinationsNoHomozygousCombos(
    list<GenotypeCombo>& combos,
    SampleDataLikelihoods& sampleDataLikelihoods,
    SampleDataLikelihoods& variantSampleDataLikelihoods,
    SampleDataLikelihoods& invariantSampleDataLikelihoods,
    Samples& samples,
    vector<Allele>& genotypeAlleles,
    int bandwidth, int banddepth,
    float logStepMax,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    // generate the initial maximum likelihood relative position for
    // integration. in this case we use the data likelihood maximum.
    vector<int> initialPosition;
    initialPosition.assign(sampleDataLikelihoods.size(), 0);
    GenotypeCombo comboKing;
    makeComboByDatalLikelihoodRank(comboKing,
            initialPosition,
            variantSampleDataLikelihoods,
            invariantSampleDataLikelihoods,
            theta,
            pooled,
            ewensPriors,
            permute,
            hwePriors,
            binomialObsPriors,
            alleleBalancePriors,
            diffusionPriorScalar);

    // obtain the combos

    bandedGenotypeCombinations(
            combos,
            comboKing,
            variantSampleDataLikelihoods,
            invariantSampleDataLikelihoods,
            samples,
            bandwidth,
            banddepth,
            logStepMax,
            theta,
            pooled,
            ewensPriors,
            permute,
            hwePriors,
            binomialObsPriors,
            alleleBalancePriors,
            diffusionPriorScalar);

}


void addAllHomozygousCombos(
    list<GenotypeCombo>& combos,
    SampleDataLikelihoods& sampleDataLikelihoods,
    SampleDataLikelihoods& variantSampleDataLikelihoods,
    SampleDataLikelihoods& invariantSampleDataLikelihoods,
    Samples& samples,
    vector<Allele>& genotypeAlleles,
    int bandwidth, int banddepth,
    float logStepMax,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    // determine which homozygous combos we already have

    map<Allele, bool> allelesWithHomozygousCombos;

    for (list<GenotypeCombo>::iterator c = combos.begin(); c != combos.end(); ++c) {
        bool allSameAndHomozygous = true;
        GenotypeCombo::iterator gc = c->begin();
        Genotype* genotype;
        if ((*gc)->genotype->homozygous) {
            genotype = (*gc)->genotype;
        } else {
            continue;
        }
        for (; gc != c->end(); ++gc) {
            if (! ((*gc)->genotype == genotype) ) {
                allSameAndHomozygous = false;
                break;
            }
        }
        if (allSameAndHomozygous) {
            allelesWithHomozygousCombos[genotype->front().allele] == true;
        }
    }

    // accumulate the needed homozygous combos

    map<Allele, GenotypeCombo> homozygousCombos;

    for (vector<Allele>::iterator a = genotypeAlleles.begin(); a != genotypeAlleles.end(); ++a) {
        Allele& allele = *a;
        map<Allele, bool>::iterator g = allelesWithHomozygousCombos.find(allele);
        if (g == allelesWithHomozygousCombos.end()) {
            // we need to make a new combo
            // iterate through the sample genotype vector
            GenotypeCombo& combo = homozygousCombos[allele];
            // match the way we make combos in bandedCombos*()
            SampleDataLikelihoods::iterator s = variantSampleDataLikelihoods.begin();
            while (s != invariantSampleDataLikelihoods.end()) {
                // for each sample genotype, if the genotype is the same as our currently needed genotype, push it back onto a new combo
                for (vector<SampleDataLikelihood>::iterator d = s->begin(); d != s->end(); ++d) {
                    SampleDataLikelihood& sdl = *d;
                    // this check is ploidy-independent
                    if (sdl.genotype->homozygous && sdl.genotype->front().allele == allele) {
                        combo.push_back(&sdl);
                        break;
                    }
                }
                ++s;
                if (s == variantSampleDataLikelihoods.end()) {
                    s = invariantSampleDataLikelihoods.begin();
                }
            }
        }
    }

    // accumulate homozygous combos and set their combo data probabilities
    for (map<Allele, GenotypeCombo>::iterator c = homozygousCombos.begin(); c != homozygousCombos.end(); ++c) {
        GenotypeCombo& gc = c->second;
        gc.probObsGivenGenotypes = 0;
        for (GenotypeCombo::iterator sdl = gc.begin(); sdl != gc.end(); ++sdl) {
            gc.probObsGivenGenotypes += (*sdl)->prob; // set up data likelihood for combo
        }
        gc.init(binomialObsPriors);  // cache allele frequency information
        gc.calculatePosteriorProbability(theta,
                                     pooled,
                                     ewensPriors,
                                     permute,
                                     hwePriors,
                                     binomialObsPriors,
                                     alleleBalancePriors,
                                     diffusionPriorScalar);
        combos.push_back(gc);
    }

}

// conditional probability of the genotype combination given the represented allele frequencies
long double GenotypeCombo::probabilityGivenAlleleFrequencyln(bool permute) {

    //return -multinomialCoefficientLn(numberOfAlleles(), counts());

    int n = numberOfAlleles();
    long double lnhetscalar = 0;

    if (permute) {
        // scale by the product of permutations of heterozygotes
        lnhetscalar = permutationsln; // cached permutations of this combo
    }

    return lnhetscalar - multinomialCoefficientLn(n, counts());

}

// probability of the combo under HWE
long double GenotypeCombo::hweExpectedFrequencyln(Genotype* genotype) {

    int ploidy = genotype->ploidy;

    vector<int> genotypeAlleleCounts;
    vector<long double> alleleFrequencies;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        genotypeAlleleCounts.push_back(genotype->alleleCount(a->first));
        alleleFrequencies.push_back((long double) a->second.frequency / (long double) numberOfAlleles());
    }

    long double HWECoefficientln = multinomialCoefficientLn(ploidy, genotypeAlleleCounts);

    vector<int>::iterator c = genotypeAlleleCounts.begin();
    vector<long double>::iterator f = alleleFrequencies.begin();
    for (; c != genotypeAlleleCounts.end(); ++c, ++f) {
         HWECoefficientln += powln(log(*f), *c);
    }

    return HWECoefficientln;

}

// probability that the genotype count in the combo is what it is given the
// counts of the other alleles
long double GenotypeCombo::hweProbGenotypeFrequencyln(Genotype* genotype) {

    //cout << endl << *genotype << endl;

    int popTotalAlleles = numberOfAlleles();
    //cout << "popTotalAlleles = " << popTotalAlleles << endl;
    vector<int> popAlleleCounts;
    vector<int> thisGenotypeAlleleCounts;
    for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
        //cout << a->first << "\t" << a->second.frequency << "\t" << genotype->alleleCount(a->first) << endl;
        popAlleleCounts.push_back(a->second.frequency);
        thisGenotypeAlleleCounts.push_back(genotype->alleleCount(a->first));
    }

    int popTotalGenotypes = 0;
    vector<int> popGenotypeCounts;
    // for haploid, estimate as if we have all ploidy 1
    if (genotype->ploidy == 1) {
        for (map<string, AlleleCounter>::iterator a = alleleCounters.begin(); a != alleleCounters.end(); ++a) {
            popGenotypeCounts.push_back(a->second.frequency);
            popTotalGenotypes += a->second.frequency;
        }
    } else {
        for (map<Genotype*, int>::iterator g = genotypeCounts.begin(); g != genotypeCounts.end(); ++g) {
            if (g->first->ploidy == genotype->ploidy) {
                //cout << *g->first << "\t" << g->second << endl;
                popGenotypeCounts.push_back(g->second);
                popTotalGenotypes += g->second;
            }
        }
    }

    long double arrangementsOfAllelesInSample = multinomialCoefficientLn(popTotalAlleles, popAlleleCounts);
    //cout << "arrangementsOfAllelesInSample = " << exp(arrangementsOfAllelesInSample) << endl;

    long double arrangementsWithExactlyCountGenotypesGivenAF =
        multinomialCoefficientLn(genotype->ploidy, thisGenotypeAlleleCounts)
        + multinomialCoefficientLn(popTotalGenotypes, popGenotypeCounts);
    /*
    cout << "multinomialCoefficientLn(genotype->ploidy, thisGenotypeAlleleCounts) = "
         << exp(multinomialCoefficientLn(genotype->ploidy, thisGenotypeAlleleCounts)) << endl;
    cout << "multinomialCoefficientLn(popTotalGenotypes, popGenotypeCounts) = "
         << exp(multinomialCoefficientLn(popTotalGenotypes, popGenotypeCounts)) << endl;
    cout << "arrangementsWithExactlyCountGenotypesGivenAF = " << exp(arrangementsWithExactlyCountGenotypesGivenAF) << endl;

    cout << "hwe prob = " << exp(arrangementsWithExactlyCountGenotypesGivenAF - arrangementsOfAllelesInSample) << endl;
    */

    return arrangementsWithExactlyCountGenotypesGivenAF - arrangementsOfAllelesInSample;


}


// core calculation of genotype combination likelihoods
//
void
GenotypeCombo::calculatePosteriorProbability(
        long double theta,
        bool pooled,
        bool ewensPriors,
        bool permute,
        bool hwePriors,
        bool binomialObsPriors,
        bool alleleBalancePriors,
        long double diffusionPriorScalar) {

    posteriorProb = 0;
    priorProb = 0;
    priorProbG_Af = 0;
    priorProbAf = 0;
    priorProbObservations = 0;
    priorProbGenotypesGivenHWE = 0;

    // when we are operating on pooled samples, we will not be able to
    // ascertain the number of heterozygotes in the pool,
    // rendering P(Genotype combo | Allele frequency) meaningless
    if (!pooled) {
        priorProbG_Af = probabilityGivenAlleleFrequencyln(permute);
    }

    // XXX XXX hwe
    if (hwePriors) {
        for (map<Genotype*, int>::iterator gc = genotypeCounts.begin(); gc != genotypeCounts.end(); ++gc) {
            Genotype* genotype = gc->first;
            priorProbGenotypesGivenHWE += hweProbGenotypeFrequencyln(genotype);
        }
    }

    if (binomialObsPriors) {
        // for each alternate and the reference allele
        // calculate the binomial probability that we see the given strand balance and read placement prob
        // cerr << *combo << endl;
        for (map<string, AlleleCounter>::iterator ac = alleleCounters.begin(); ac != alleleCounters.end(); ++ac) {
            //const string& allele = ac->first;
            const AlleleCounter& alleleCounter = ac->second;
            int obs = alleleCounter.observations;
            /*
            cerr << allele <<  " counts: " << alleleCounter.frequency
                << " observations " << alleleCounter.observations
                << " " << alleleCounter.forwardStrand
                << "," << alleleCounter.reverseStrand
                << " " << alleleCounter.placedLeft
                << "," << alleleCounter.placedRight
                << " " << alleleCounter.placedStart
                << "," << alleleCounter.placedEnd
                << endl;
                */

            priorProbObservations
                += binomialProbln(alleleCounter.forwardStrand, obs, 0.5)
                +  binomialProbln(alleleCounter.placedLeft, obs, 0.5)
                +  binomialProbln(alleleCounter.placedStart, obs, 0.5);
        }
    }

    // ok... now do the same move for the observation counts
    // --- this should capture "Allele Balance"
    if (alleleBalancePriors) {
        priorProbObservations += multinomialSamplingProbLn(alleleProbs(), observationCounts());
    }

    // with larger population samples, the effect of
    // P(Genotype combo | Allele frequency) may bias us against reporting
    // true variants which are under selection despite overwhelming evidence
    // for variation.  this allows us to scale the effect of this prior
    if (diffusionPriorScalar != 1) {
        priorProbG_Af /= diffusionPriorScalar;
    }

    // Ewens' Sampling Formula
    if (ewensPriors) {
        priorProbAf = alleleFrequencyProbabilityln(countFrequencies(), theta);
    }

    // posterior probability
    priorProb = priorProbG_Af + priorProbAf + priorProbObservations + priorProbGenotypesGivenHWE;
    posteriorProb = priorProb + probObsGivenGenotypes;

}


pair<int, int> alternateAndReferenceCount(vector<Allele*>& observations, string& refbase, string altbase) {
    int altcount = 0;
    int refcount = 0;
    for (vector<Allele*>::iterator allele = observations.begin(); allele != observations.end(); ++allele) {
        if ((*allele)->currentBase == refbase)
            ++refcount;
        else if ((*allele)->currentBase == altbase)
            ++altcount;
    }
    return make_pair(altcount, refcount);
}


void genotypeCombo2Map(GenotypeCombo& gc, GenotypeComboMap& gcm) {
    for (GenotypeCombo::iterator g = gc.begin(); g != gc.end(); ++g) {
        gcm[(*g)->name] = *g;;
    }
}


void orderedGenotypeCombo(
    GenotypeCombo& combo,
    GenotypeCombo& orderedCombo,
    SampleDataLikelihoods& sampleDataLikelihoods,
    long double theta,
    bool pooled,
    bool ewensPriors,
    bool permute,
    bool hwePriors,
    bool binomialObsPriors,
    bool alleleBalancePriors,
    long double diffusionPriorScalar) {

    GenotypeComboMap bestComboMap;

    genotypeCombo2Map(combo, bestComboMap);
    for (SampleDataLikelihoods::iterator sdl = sampleDataLikelihoods.begin(); sdl != sampleDataLikelihoods.end(); ++sdl) {
        orderedCombo.push_back(bestComboMap[sdl->front().name]);
    }

    orderedCombo.init(binomialObsPriors);
    orderedCombo.calculatePosteriorProbability(theta, pooled, ewensPriors, permute,
            hwePriors, binomialObsPriors, alleleBalancePriors,
            diffusionPriorScalar);

}


// returns a list of the alternate alleles represented by the given genotype
// combo sorted by frequency
vector<pair<Allele, int> > alternateAlleles(GenotypeCombo& combo, string referenceBase) {

    map<Allele, int> alternates;

    for (GenotypeCombo::iterator g = combo.begin(); g != combo.end(); ++g) {
        vector<Allele> alts = (*g)->genotype->alternateAlleles(referenceBase);
        for (vector<Allele>::iterator a = alts.begin(); a != alts.end(); ++a) {
            if (alternates.find(*a) == alternates.end()) {
                alternates[*a] = 1;
            } else {
                alternates[*a] += 1;
            }
        }
    }

    vector<pair<Allele, int> > sortedAlternates;

    for (map<Allele, int>::iterator a = alternates.begin(); a != alternates.end(); ++a) {
        sortedAlternates.push_back(make_pair(a->first, a->second));
    }

    AllelePairIntCompare alleleCountCompare;
    sort(sortedAlternates.begin(), sortedAlternates.end(), alleleCountCompare);

    return sortedAlternates;

}

int Genotype::containedAlleleTypes(void) {
    int t = 0;
    for (Genotype::iterator g = begin(); g != end(); ++g) {
        t |= g->allele.type;
    }
    return t;
}


vector<int> Genotype::alleleObservationCounts(Sample& sample) {
    vector<int> counts;
    for (Genotype::iterator i = begin(); i != end(); ++i) {
        Allele& b = i->allele;
        counts.push_back(sample.observationCount(b));
    }
    return counts;
}

int Genotype::alleleObservationCount(Sample& sample) {
    int count = 0;
    for (Genotype::iterator i = begin(); i != end(); ++i) {
        Allele& b = i->allele;
        count += sample.observationCount(b);
    }
    return count;
}

bool Genotype::sampleHasSupportingObservations(Sample& sample) {
    for (Genotype::iterator i = begin(); i != end(); ++i) {
        Allele& b = i->allele;
        if (sample.observationCount(b) != 0) {
            return true;
        }
    }
    return false;
}

bool Genotype::sampleHasSupportingObservationsForAllAlleles(Sample& sample) {
    vector<int> counts = alleleObservationCounts(sample);
    for (vector<int>::iterator c = counts.begin(); c != counts.end(); ++c) {
        if (*c == 0) {
            return false;
        }
    }
    return true;
}
