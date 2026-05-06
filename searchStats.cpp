#include "searchStats.hpp"

#include "chessAI.hpp"
#include <ostream>


void printSearchStats(const ChessAI &ai, double elapsedSeconds, std::ostream &output)
{
    double nodesPerSecond = elapsedSeconds > 0
                            ? ai.getNodesEvaluated() / elapsedSeconds
                            : 0;
    
    output << "Computer searched " << ai.getSearchDepth()
           << " plies, evaluated " << ai.getNodesEvaluated()
           << " nodes in " << elapsedSeconds
           << " seconds of a " << ai.getTimeBudgetSeconds()
           << " second budget (" << nodesPerSecond << " nodes/sec)" << std::endl;
    
    const ChessAI::TranspositionStats &ttStats = ai.getTranspositionStats();
    output << "Transposition table: " << ttStats.probes << " probes, "
           << ttStats.hits << " hits, "
           << ttStats.exactHits << " exact hits, "
           << ttStats.boundCutoffs << " bound cutoffs, "
           << ttStats.stores << " stores" << std::endl;
}
