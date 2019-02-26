#include "../PPGraph/PPGraph.h"
#include "AbstractPPKTruss.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../KTrussIndex/KTrussDecomposition.h"

#ifndef KTRUSS_V1_NAIVEPPKTRUSS_H
#define KTRUSS_V1_NAIVEPPKTRUSS_H

class NaivePPKTruss : public AbstractPPKTruss {
public:
    NaivePPKTruss(PPGraph* ppGraph);
    ~NaivePPKTruss();

    void preProcess() override ;
    AbstractPPKTruss::KTrussAnswer query(int v, int k) override ;
private:
    Graph createNeighborhoodGraph(int v, int k);
};

#endif //KTRUSS_V1_NAIVEPPKTRUSS_H
