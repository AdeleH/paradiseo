// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// eoPop.h 
// (c) GeNeura Team, 1998
/* 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
 */
//-----------------------------------------------------------------------------

#ifndef _EOPOP_H
#define _EOPOP_H

#include <vector>
#include <strstream>
#include <algorithm>

// EO includes
#include <eoOp.h> // for eoInit
#include <eoPersistent.h>
#include <eoInit.h>

/** Subpopulation: it is used to move parts of population
 from one algorithm to another and one population to another. It is safer
to declare it as a separate object. I have no idea if a population can be
some other thing that a vector, but if somebody thinks of it, this concrete
implementation will be moved to "generic" and an abstract Population 
interface will be provided.

It can be instantiated with anything, provided that it accepts a "size" and a 
random generator in the ctor. This happens to all the eo1d chromosomes declared 
so far. EOT must also have a copy ctor, since temporaries are created and copied
to the population.

It can also be instantiated with a "size" and an eoInit derived object. 
This object must supply a full chromosome (or something the ctor of the EO
will accept).

@author Geneura Team
@version 0.0
*/

template<class EOT>
class eoPop: public vector<EOT>, public eoObject, public eoPersistent 
{

 public:
     typedef typename EOT::Fitness Fitness;
	/** Default ctor. Creates empty pop
	*/
	eoPop()	  : vector<EOT>(), eoObject(), eoPersistent() {};
	
	/** Ctor for the initialization of chromosomes
    
	@param _popSize total population size
	@param _chromInit Initialization routine, produces EO's, needs to be an eoInit 
	*/
    eoPop( unsigned _popSize, eoInit<EOT>& _chromInit )
	  :vector<EOT>() 
    {
        resize(_popSize);
	  for ( unsigned i = 0; i < _popSize; i++ )
      {
	        _chromInit(operator[](i));
	  }
	};

	/** SAME Initialization task than init. ctor, but is NOT a constructor
    
	@param _popSize total population size
	@param _chromInit Initialization routine, produces EO's, needs to be an eoInit 
	*/
    void append( unsigned _popSize, eoInit<EOT>& _chromInit )
    {
      resize(size()+_popSize);	   // adjust the size
      for ( unsigned i = 0; i < _popSize; i++ )
	{
	  _chromInit(operator[](i));
	}
    };
  
  
	/** Ctor from an istream; reads the population from a stream,
	    each element should be in different lines
	    @param _is the stream
	*/
	eoPop( istream& _is ):vector<EOT>() {
	  readFrom( _is );
	}
	
	///
	~eoPop() {};


	/// helper struct for getting a pointer
      struct Ref { const EOT* operator()(const EOT& eot) { return &eot;}};
  /// helper struct for comparing on pointers
      struct Cmp {
          bool operator()(const EO<Fitness>* a, const EO<Fitness>* b) const
            { return b->operator<(*a); }
      };

    /**
    sort the population. Use this member to sort in order
    of descending Fitness, so the first individual is the best!
   */
  void sort(void)
  {
      std::sort(begin(), end(), greater<EO<Fitness> >());
  }

  void sort(vector<const EOT*>& result) const
  {
      result.resize(size());

      std::transform(begin(), end(), result.begin(), Ref());

      std::sort(result.begin(), result.end(), Cmp());
  }

  /**
  slightly faster algorithm than sort to find all individuals that are better
  than the nth individual
  */
  eoPop<EOT>::iterator nth_element(int nth)
  {
      iterator it = begin() + nth;
      std::nth_element(begin(), it, end(), greater<EO<Fitness> >());
      return it;
  }

  struct GetFitness { Fitness operator()(const EOT& _eo) const { return _eo.fitness(); } };
  
  Fitness nth_element_fitness(int which) const
  { // probably not the fastest way to do this, but what the heck
      
      vector<Fitness> fitness(size());
      std::transform(begin(), end(), fitness.begin(), GetFitness());

      vector<Fitness>::iterator it = fitness.begin() + which;
      std::nth_element(fitness.begin(), it, fitness.end(), greater<Fitness>());
      return *it;
  }

  /// const nth_element function, returns pointers to sorted individuals
  void nth_element(int which, vector<const EOT*>& result) const
  {

      result.resize(size());
      std::transform(begin(), end(), result.begin(), Ref());
  
      vector<const EOT*>::iterator it  = result.begin() + which;

      std::nth_element(result.begin(), it, result.end(), Cmp());
  }

  void swap(eoPop<EOT>& other)
  {
      std::swap(static_cast<vector<EOT>& >(*this), static_cast<vector<EOT>& >(other));
  }
  
  /**
   * Write object. It's called printOn since it prints the object _on_ a stream.
   * @param _os A ostream. 
   */
  virtual void printOn(ostream& _os) const 
  {
        _os << size() << '\n';
        copy( begin(), end(), ostream_iterator<EOT>( _os, "\n") );
  };

    	/** @name Methods from eoObject	*/
	//@{
	/**
	* Read object. The EOT class must have a ctor from a stream;
	in this case, a strstream is used.
	* @param _is A istream.

	*/
  virtual void readFrom(istream& _is) 
  {
      size_t sz;
    _is >> sz;

    resize(sz);

    for (size_t i = 0; i < sz; ++i)
    {  
        operator[](i).readFrom( _is );
    }
  }

  /** Inherited from eoObject. Returns the class name.
      @see eoObject
  */
  virtual string className() const {return "eoPop";};
    //@}

 protected:

};

#endif

