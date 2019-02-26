#include "Experiments/GeneralPPTrussExperiment.h"
#include "Experiments/SubgraphTrussExperiment.h"

using namespace std;

const bool PublicPrivateExperiment = true;

int main(int argc, char *argv[]) {
    if (PublicPrivateExperiment)
        GeneralPPTrussExperiment::generalPPKTrussMain(argc, argv);
    else
        SubGraphTrussExperiment::subGraphTrussMain(argc, argv);
    return 0;

}
