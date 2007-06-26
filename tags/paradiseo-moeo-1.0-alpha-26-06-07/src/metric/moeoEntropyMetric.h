// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// moeoEntropyMetric.h
// (c) OPAC Team (LIFL), Dolphin Project (INRIA), 2007
/*
    This library...

    Contact: paradiseo-help@lists.gforge.inria.fr, http://paradiseo.gforge.inria.fr
 */
//-----------------------------------------------------------------------------

#ifndef MOEOENTROPYMETRIC_H_
#define MOEOENTROPYMETRIC_H_

#include <metric/moeoMetric.h>

/**
 * The entropy gives an idea of the diversity of a Pareto set relatively to another
 * (Basseur, Seynhaeve, Talbi: 'Design of Multi-objective Evolutionary Algorithms: Application to the Flow-shop Scheduling Problem', in Proc. of the 2002 Congress on Evolutionary Computation, IEEE Press, pp. 1155-1156)
 */
template < class ObjectiveVector >
class moeoEntropyMetric : public moeoVectorVsVectorBinaryMetric < ObjectiveVector, double >
{
public:

    /**
     * Returns the entropy of the Pareto set '_set1' relatively to the Pareto set '_set2'
     * @param _set1 the first Pareto set
     * @param _set2 the second Pareto set
     */
    double operator()(const std::vector < ObjectiveVector > & _set1, const std::vector < ObjectiveVector > & _set2) {
        // normalization
        std::vector< ObjectiveVector > set1 = _set1;
        std::vector< ObjectiveVector > set2= _set2;
        removeDominated (set1);
        removeDominated (set2);
        prenormalize (set1);
        normalize (set1);
        normalize (set2);

        // making of PO*
        std::vector< ObjectiveVector > star; // rotf :-)
        computeUnion (set1, set2, star);
        removeDominated (star);

        // making of PO1 U PO*
        std::vector< ObjectiveVector > union_set1_star; // rotf again ...
        computeUnion (set1, star, union_set1_star);

        unsigned C = union_set1_star.size();
        float omega=0;
        float entropy=0;

        for (unsigned i=0 ; i<C ; i++) {
            unsigned N_i = howManyInNicheOf (union_set1_star, union_set1_star[i], star.size());
            unsigned n_i = howManyInNicheOf (set1, union_set1_star[i], star.size());
            if (n_i > 0) {
                omega += 1.0 / N_i;
                entropy += (float) n_i / (N_i * C) * log (((float) n_i / C) / log (2.0));
            }
        }
        entropy /= - log (omega);
        entropy *= log (2.0);
        return entropy;
    }


private:

    /** vector of min values */
    std::vector<double> vect_min_val;
    /** vector of max values */
    std::vector<double> vect_max_val;
    /** Functor to compare two objective vectors according to Pareto dominance relation */
    moeoParetoObjectiveVectorComparator < ObjectiveVector > paretoComparator;


    /**
     * Removes the dominated individuals contained in _f
     * @param _f a Pareto set
     */
    void removeDominated(std::vector < ObjectiveVector > & _f) {
        for (unsigned i=0 ; i<_f.size(); i++) {
            bool dom = false;
            for (unsigned j=0; j<_f.size(); j++)
                if (i != j && paretoComparator(_f[i],_f[j]))
                {
                    dom = true;
                    break;
                }
            if (dom) {
                _f[i] = _f.back();
                _f.pop_back();
                i--;
            }
        }
    }


    /**
     * Prenormalization
     * @param _f a Pareto set
     */
    void prenormalize (const std::vector< ObjectiveVector > & _f) {
        vect_min_val.clear();
        vect_max_val.clear();

        for (unsigned char i=0 ; i<ObjectiveVector::nObjectives(); i++) {
            float min_val = _f.front()[i], max_val = min_val;
            for (unsigned j=1 ; j<_f.size(); j++) {
                if (_f[j][i] < min_val)
                    min_val = _f[j][i];
                if (_f[j][i]>max_val)
                    max_val = _f[j][i];
            }
            vect_min_val.push_back(min_val);
            vect_max_val.push_back (max_val);
        }
    }


    /**
     * Normalization
     * @param _f a Pareto set
     */
    void normalize (std::vector< ObjectiveVector > & _f) {
        for (unsigned i=0 ; i<ObjectiveVector::nObjectives(); i++)
            for (unsigned j=0; j<_f.size(); j++)
                _f[j][i] = (_f[j][i] - vect_min_val[i]) / (vect_max_val[i] - vect_min_val[i]);
    }


    /**
     * Computation of the union of _f1 and _f2 in _f
     * @param _f1 the first Pareto set
     * @param _f2 the second Pareto set
     * @param _f the final Pareto set
     */
    void computeUnion(const std::vector< ObjectiveVector > & _f1, const std::vector< ObjectiveVector > & _f2, std::vector< ObjectiveVector > & _f) {
        _f = _f1 ;
        for (unsigned i=0; i<_f2.size(); i++) {
            bool b = false;
            for (unsigned j=0; j<_f1.size(); j ++)
                if (_f1[j] == _f2[i]) {
                    b = true;
                    break;
                }
            if (! b)
                _f.push_back(_f2[i]);
        }
    }


    /**
     * How many in niche
     */
    unsigned howManyInNicheOf (const std::vector< ObjectiveVector > & _f, const ObjectiveVector & _s, unsigned _size) {
        unsigned n=0;
        for (unsigned i=0 ; i<_f.size(); i++) {
            if (euclidianDistance(_f[i], _s) < (_s.size() / (double) _size))
                n++;
        }
        return n;
    }


    /**
     * Euclidian distance
     */
    double euclidianDistance (const ObjectiveVector & _set1, const ObjectiveVector & _to, unsigned _deg = 2) {
        double dist=0;
        for (unsigned i=0; i<_set1.size(); i++)
            dist += pow(fabs(_set1[i] - _to[i]), (int)_deg);
        return pow(dist, 1.0 / _deg);
    }

};

#endif /*MOEOENTROPYMETRIC_H_*/