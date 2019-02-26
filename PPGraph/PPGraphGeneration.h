#include "PPGraph.h"

#ifndef KTRUSS_V1_PPGRAPHGENERATOR_H
#define KTRUSS_V1_PPGRAPHGENERATOR_H

namespace PPGraphGeneration {
    PPGraph* generateCopyPPGraph(Graph *publicGraph);
    PPGraph* generateFromGraph(Graph *publicGraph, Graph *privateGraph);
};

#endif //KTRUSS_V1_PPGRAPHGENERATOR_H
