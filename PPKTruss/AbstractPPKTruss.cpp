#include "AbstractPPKTruss.h"


AbstractPPKTruss::AbstractPPKTruss() {
}

AbstractPPKTruss::~AbstractPPKTruss() {
}

AbstractPPKTruss::AbstractPPKTruss(PPGraph *ppGraph) {
    this->ppGraph = ppGraph;
}
