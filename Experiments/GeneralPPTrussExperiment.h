#include <map>
#include <vector>
#include "../KTrussIndex/KTrussIndex.h"
#include "../PPGraph/PPGraph.h"

#ifndef KTRUSS_V1_GENERALPPTRUSSEXPERIMENT_H
#define KTRUSS_V1_GENERALPPTRUSSEXPERIMENT_H

namespace GeneralPPTrussExperiment {
    int generalPPKTrussMain(int argc, char *argv[]);
    void doRun(int argc, char *argv[]);
    void doData(int argc, char *argv[]);
    void doSampling(int argc, char *argv[]);
    std::map<int, int> findBounds(KTrussIndex *index, std::vector<int> nodes, bool forLowerBound);
    std::vector<int> readSampleFile(char *path);
    PPGraph * getPPGraph(char *_publicAddress, char *_privateAddress);
};

#endif //KTRUSS_V1_GENERALPPTRUSSEXPERIMENT_H
