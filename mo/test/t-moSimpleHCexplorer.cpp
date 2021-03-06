/*
  <t-moSimpleHCexplorer.cpp>
  Copyright (C) DOLPHIN Project-Team, INRIA Lille - Nord Europe, 2006-2010

  Sébastien Verel, Arnaud Liefooghe, Jérémie Humeau

  This software is governed by the CeCILL license under French law and
  abiding by the rules of distribution of free software.  You can  use,
  modify and/ or redistribute the software under the terms of the CeCILL
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info".

  As a counterpart to the access to the source code and  rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty  and the software's author,  the holder of the
  economic rights,  and the successive licensors  have only  limited liability.

  In this respect, the user's attention is drawn to the risks associated
  with loading,  using,  modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean  that it is complicated to manipulate,  and  that  also
  therefore means  that it is reserved for developers  and  experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or
  data to be ensured and,  more generally, to use and operate it in the
  same conditions as regards security.
  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL license and that you accept its terms.

  ParadisEO WebSite : http://paradiseo.gforge.inria.fr
  Contact: paradiseo-help@lists.gforge.inria.fr
*/

#include "moTestClass.h"
#include <eval/moFullEvalByCopy.h>
#include <explorer/moSimpleHCexplorer.h>
#include <comparator/moNeighborComparator.h>
#include <comparator/moSolNeighborComparator.h>

#include <cstdlib>
#include <cassert>

int main() {

    //Pas grand chose à faire: le gros du travail est fait par le voisin et l'eval

    std::cout << "[t-moSimpleHCexplorer] => START" << std::endl;

    Solution sol;
    moDummyNeighborTest neighbor;
    moDummyEvalTest eval;
    moDummyNeighborhoodTest nh;
    moFullEvalByCopy<moDummyNeighborTest> fulleval(eval);
    moNeighborComparator<moDummyNeighborTest> comp;
    moSolNeighborComparator<moDummyNeighborTest> solNeighborComp;


    //verif constructor
    moSimpleHCexplorer<moDummyNeighborTest> test(nh, fulleval, comp, solNeighborComp);

    //verif operator() et accept: le neigorhood est construit pour qu'on tombe dans les 3 cas suivants:
    //hasNeighbor() retourne faux a l'entrée de l'operator() donc on doit pas continuer
    sol.fitness(3);
    test(sol);
    test.accept(sol);
    assert(!test.isContinue(sol));

    //hasNeighbor() retourne faux a l'entrée de accept() donc on doit pas continuer
    test(sol);
    test.accept(sol);
    assert(!test.isContinue(sol));

    //hasNeighbor() retourne vrai et on ameliore la fitness donc on doit continuer
    test(sol);
    test.accept(sol);
    assert(test.isContinue(sol));

    //verif de move -> on affecte la fitness du best d'avant
    test.move(sol);

    //hasNeighbor() retourne vrai et on ameliore pas la fitness donc on doit pas continuer
    test(sol);
    test(sol);
    test.accept(sol);
    assert(!test.isContinue(sol));

    assert(test.className()=="moSimpleHCexplorer");

    std::cout << "[t-moSimpleHCexplorer] => OK" << std::endl;

    return EXIT_SUCCESS;
}
