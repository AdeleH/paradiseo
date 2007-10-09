// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// moeoComparator.h
// (c) OPAC Team (LIFL), Dolphin Project (INRIA), 2007
/*
    This library...

    Contact: paradiseo-help@lists.gforge.inria.fr, http://paradiseo.gforge.inria.fr
 */
//-----------------------------------------------------------------------------

#ifndef MOEOCOMPARATOR_H_
#define MOEOCOMPARATOR_H_

#include <eoFunctor.h>

/**
 * Functor allowing to compare two solutions.
 */
template < class MOEOT >
class moeoComparator : public eoBF < const MOEOT &, const MOEOT &, const bool >
    {};


/**
 * Functor allowing to compare two solutions according to their first objective value, then their second, and so on.
 */
template < class MOEOT >
class moeoObjectiveComparator : public moeoComparator < MOEOT >
{
public:

    /**
     * Returns true if _moeo1 < _moeo2 on the first objective, then on the second, and so on
     * @param _moeo1 the first solution
     * @param _moeo2 the second solution
     */
    const bool operator()(const MOEOT & _moeo1, const MOEOT & _moeo2)
    {
        return cmp(_moeo1.objectiveVector(), _moeo2.objectiveVector());
    }

private:

    /** the objective vector type of the solutions */
    typedef typename MOEOT::ObjectiveVector ObjectiveVector;
    /** the corresponding comparator for objective vectors */
    moeoObjectiveObjectiveVectorComparator < ObjectiveVector > cmp;

};


/**
 * Functor allowing to compare two solutions according to one objective.
 */
template < class MOEOT >
class moeoOneObjectiveComparator : public moeoComparator < MOEOT >
{
public:

    /**
     * Ctor.
     * @param _obj the index of objective
     */
    moeoOneObjectiveComparator(unsigned _obj) : obj(_obj)
    {
        if (obj > MOEOT::ObjectiveVector::nObjectives())
        {
            throw std::runtime_error("Problem with the index of objective in moeoOneObjectiveComparator");
        }
    }

    /**
     * Returns true if _moeo1 < _moeo2 on the obj objective
     * @param _moeo1 the first solution
     * @param _moeo2 the second solution
     */
    const bool operator()(const MOEOT & _moeo1, const MOEOT & _moeo2)
    {
        return _moeo1.objectiveVector()[obj] < _moeo2.objectiveVector()[obj];
    }

private:
    /** the index of objective */
    unsigned obj;

};


/**
 * Functor allowing to compare two solutions according to their fitness values, then according to their diversity values.
 */
template < class MOEOT >
class moeoFitnessThenDiversityComparator : public moeoComparator < MOEOT >
{
public:

    /**
     * Returns true if _moeo1 < _moeo2 according to their fitness values, then according to their diversity values	
     * @param _moeo1 the first solution
     * @param _moeo2 the second solution
     */
    const bool operator()(const MOEOT & _moeo1, const MOEOT & _moeo2)
    {
        if (_moeo1.fitness() == _moeo2.fitness())
        {
            return _moeo1.diversity() < _moeo2.diversity();
        }
        else
        {
            return _moeo1.fitness() < _moeo2.fitness();
        }
    }

};


/**
 * Functor allowing to compare two solutions according to their diversity values, then according to their fitness values.
 */
template < class MOEOT >
class moeoDiversityThenFitnessComparator : public moeoComparator < MOEOT >
{
public:

    /**
     * Returns true if _moeo1 < _moeo2 according to their diversity values, then according to their fitness values
     * @param _moeo1 the first solution
     * @param _moeo2 the second solution
     */
    const bool operator()(const MOEOT & _moeo1, const MOEOT & _moeo2)
    {
        if (_moeo1.diversity() == _moeo2.diversity())
        {
            return _moeo1.fitness() < _moeo2.fitness();
        }
        else
        {
            return _moeo1.diversity() < _moeo2.diversity();
        }
    }

};


/**
 * Functor allowing to compare two solutions according to their fitness and diversity values, each according to its aggregative value.
 */
template < class MOEOT >
class moeoAggregativeComparator : public moeoComparator < MOEOT >
{
public:

    /**
        * Ctor.
        * @param _weightFitness the weight for fitness
        * @param _weightDiversity the weight for diversity
        */
    moeoAggregativeComparator(double _weightFitness = 1.0, double _weightDiversity = 1.0) : weightFitness(_weightFitness), weightDiversity(_weightDiversity)
    {}

    /**
     * Returns true if _moeo1 < _moeo2 according to the aggregation of their fitness and diversity values
     * @param _moeo1 the first solution
     * @param _moeo2 the second solution
     */
    const bool operator()(const MOEOT & _moeo1, const MOEOT & _moeo2)
    {
        return ( weightFitness * _moeo1.fitness() + weightDiversity * _moeo1.diversity() ) < ( weightFitness * _moeo2.fitness() + weightDiversity * _moeo2.diversity() );
    }

private:
    /** the weight for fitness */
    double weightFitness;
    /** the weight for diversity */
    double weightDiversity;

};


#endif /*MOEOCOMPARATOR_H_*/