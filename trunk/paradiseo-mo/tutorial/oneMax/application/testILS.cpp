//-----------------------------------------------------------------------------
/** testILS.cpp
 *
 * SV - 12/01/10
 *
 */
//-----------------------------------------------------------------------------

// standard includes
#define HAVE_SSTREAM

#include <stdexcept>  // runtime_error
#include <iostream>   // cout
#include <sstream>  // ostrstream, istrstream
#include <fstream>
#include <string.h>

// the general include for eo
#include <eo>
#include <ga.h>
#include <ga/eoBitOp.h>

using namespace std;

//-----------------------------------------------------------------------------
// fitness function
#include <funcOneMax.h>
#include <funcNK.h>
#include <eoInt.h>
#include <neighborhood/moOrderNeighborhood.h>
#include <neighborhood/moRndWithoutReplNeighborhood.h>
#include <oneMaxBitNeighbor.h>

#include <eval/moFullEvalByModif.h>
#include <eval/moFullEvalByCopy.h>
#include <comparator/moNeighborComparator.h>
#include <comparator/moSolNeighborComparator.h>
#include <continuator/moTrueContinuator.h>
#include <algo/moLocalSearch.h>
#include <explorer/moSimpleHCexplorer.h>
#include <explorer/moILSexplorer.h>

#include <perturb/moMonOpPerturb.h>
#include <perturb/moRestartPerturb.h>
#include <perturb/moNeighborhoodPerturb.h>
#include <acceptCrit/moAlwaysAcceptCrit.h>
#include <acceptCrit/moBetterAcceptCrit.h>
#include <continuator/moIterContinuator.h>

// REPRESENTATION
//-----------------------------------------------------------------------------
typedef eoBit<unsigned int> Indi;
typedef moBitNeighbor<unsigned int> Neighbor ; // incremental evaluation
typedef moOrderNeighborhood<Neighbor> Neighborhood ;
typedef moRndWithoutReplNeighborhood<Neighbor> Neighborhood2 ;
typedef moSimpleHCexplorer<Neighborhood> NHE;

void main_function(int argc, char **argv)
{
    /* =========================================================
    *
    * Parameters
    *
    * ========================================================= */

    // First define a parser from the command-line arguments
    eoParser parser(argc, argv);

    // For each parameter, define Parameter, read it through the parser,
    // and assign the value to the variable

    eoValueParam<uint32_t> seedParam(time(0), "seed", "Random number seed", 'S');
    parser.processParam( seedParam );
    unsigned seed = seedParam.value();

    // description of genotype
    eoValueParam<unsigned int> vecSizeParam(8, "vecSize", "Genotype size", 'V');
    parser.processParam( vecSizeParam, "Representation" );
    unsigned vecSize = vecSizeParam.value();

    // the name of the "status" file where all actual parameter values will be saved
    string str_status = parser.ProgramName() + ".status"; // default value
    eoValueParam<string> statusParam(str_status.c_str(), "status", "Status file");
    parser.processParam( statusParam, "Persistence" );

    // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
    // i.e. in case you need parameters somewhere else, postpone these
    if (parser.userNeedsHelp()) {
        parser.printHelp(cout);
        exit(1);
    }
    if (statusParam.value() != "") {
        ofstream os(statusParam.value().c_str());
        os << parser;// and you can use that file as parameter file
    }

    /* =========================================================
     *
     * Random seed
     *
     * ========================================================= */

    //reproducible random seed: if you don't change SEED above,
    // you'll aways get the same result, NOT a random run
    rng.reseed(seed);


    /* =========================================================
     *
     * Eval fitness function
     *
     * ========================================================= */

    FuncOneMax<Indi> eval(vecSize);

    //FuncNK<Indi> eval(vecSize, 2);

    /* =========================================================
     *
     * Initilisation of the solution
     *
     * ========================================================= */

    // a Indi random initializer
    eoUniformGenerator<bool> uGen;
    eoInitFixedLength<Indi> random(vecSize, uGen);


    /* =========================================================
     *
     * evaluation of a neighbor solution
     *
     * ========================================================= */

    moFullEvalByModif<Neighbor> fulleval(eval);

    //An eval by copy can be used instead of the eval by modif
    //moFullEvalByCopy<Neighbor> fulleval(eval);


    /* =========================================================
     *
     * Comparator of neighbors
     *
     * ========================================================= */

    moNeighborComparator<Neighbor> comparator;
    moSolNeighborComparator<Neighbor> solComparator;


    /* =========================================================
     *
     * the neighborhood of a solution
     *
     * ========================================================= */

    Neighborhood neighborhood(vecSize);
    Neighborhood2 neighborhood2(vecSize);


    /* =========================================================
     *
     * a neighborhood explorer solution
     *
     * ========================================================= */

    moSimpleHCexplorer<Neighbor> explorer(neighborhood, fulleval, comparator, solComparator);


    /* =========================================================
     *
     * the local search algorithm
     *
     * ========================================================= */

    moTrueContinuator<Neighbor> continuator;//always continue

    moLocalSearch< Neighbor > hc(explorer, continuator, eval);

    eoBitMutation<Indi> monOp(1.0/vecSize);

    moMonOpPerturb<Neighbor> perturb(monOp, eval);

    //moRestartPerturb<Neighbor> perturb(random, eval, 5);

	//moNeighborhoodPerturb<Neighbor, Neighborhood2> perturb(neighborhood2, fulleval);

    moSolComparator<Indi> comp;

    //moAlwaysAcceptCrit<Neighbor> accept;
    moBetterAcceptCrit<Neighbor> accept(comp);

    moILSexplorer< Neighbor > explorerILS(hc, perturb, accept);

    moIterContinuator<Neighbor> continuatorILS(100);

    moLocalSearch<Neighbor>localSearch(explorerILS, continuatorILS, eval);


    /* =========================================================
     *
     * execute the local search from random sollution
     *
     * ========================================================= */

    Indi solution;

    random(solution);

    //Can be eval here, else it will be done at the beginning of the localSearch
    //eval(solution);

    std::cout << "initial: " << solution << std::endl ;

    localSearch(solution);

    std::cout << "final:   " << solution << std::endl ;

}

// A main that catches the exceptions

int main(int argc, char **argv)
{
    try {
        main_function(argc, argv);
    }
    catch (exception& e) {
        cout << "Exception: " << e.what() << '\n';
    }
    return 1;
}