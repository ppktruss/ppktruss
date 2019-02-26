#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>
#include "../Parser/Parser.h"
#include "../KTrussIndex/KTrussDecomposition.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../Graph/GraphGeneration.h"
#include "../PPGraph/PPGraphGeneration.h"
#include "../KTrussUpdate/KTrussUpdate.h"
#include "../Utilities/ProcessTimer.h"
#include "../PPKTruss/GeneralPPKTruss.h"
#include "../Graph/GraphUtility.h"
#include "../KTrussIndex/KTrussQuery.h"
#include "../PPGraph/PPGraph.h"
#include "GeneralPPTrussExperiment.h"

using namespace std;

PPGraph * GeneralPPTrussExperiment::getPPGraph(char *_publicAddress, char *_privateAddress) {
    string publicAddressStr(_publicAddress);
    string privateAddressStr(_privateAddress);
    Graph *publicG = GraphGeneration::generateGraph(publicAddressStr);
    Graph *privateG = GraphGeneration::generateGraph(privateAddressStr);
    PPGraph *ppG = PPGraphGeneration::generateFromGraph(publicG, privateG);
    delete privateG;
    return ppG;
}

void GeneralPPTrussExperiment::doSampling(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "Few arguments. run as ./k-truss.out sample public.txt private.txt max_sample" << endl;
        return;
    }
    PPGraph *ppG = getPPGraph(argv[2], argv[3]);
    int maxSample = atoi(argv[4]);
    // Sampling nodes
    clog << " Start of Sampling Nodes " << endl;
    vector<int> queryNodes;
    ProcessTimer samplingTimer;
    samplingTimer.start();
    int numBinPrivate = 100, numBinPublic = 100;
    vector<int> bin[numBinPublic][numBinPrivate];
    int maxPrivateDegree = 0, maxPublicDegree = 0;
    vector<int> privateNodes;
    for (int v: ppG->getPrivateNodes()) {
        privateNodes.push_back(v);
        maxPublicDegree = max(maxPublicDegree, (int) ppG->publicNeighbors(v).size());
        maxPrivateDegree = max(maxPrivateDegree, (int) ppG->privateNeighbors(v).size());
    }
    numBinPrivate = min(numBinPrivate, maxPrivateDegree + 1);
    numBinPublic = min(numBinPublic, maxPublicDegree + 1);

    for (int v: privateNodes) {
        int publicD = ppG->publicNeighbors(v).size();
        int privateD = ppG->privateNeighbors(v).size();
        bin[publicD * (numBinPublic - 1) / maxPublicDegree][privateD * (numBinPrivate - 1) / maxPrivateDegree]
                .push_back(v);
    }
    for (int i = 0; i < numBinPublic; i++)
        for (int j = 0; j < numBinPrivate; j++) {
            vector<int> &currentBin = bin[i][j];
            random_shuffle(currentBin.begin(), currentBin.end());
            for (int k = 0; k < (int) currentBin.size() && k < maxSample; k++) {
                queryNodes.push_back(currentBin[k]);
                cout << queryNodes.back() << endl;
            }
        }
    samplingTimer.end();
    clog << " End of sampling, selected " << queryNodes.size() << " nodes in " <<
         setprecision(9) << fixed << samplingTimer.getDuration().count() << endl;
}

vector<int> GeneralPPTrussExperiment::readSampleFile(char *path) {
    FILE *fin = fopen(path, "r");
    vector<int> ans;
    int x;
    while (fscanf(fin, "%d", &x) != EOF)
        ans.push_back(x);
    return ans;
}

map<int, int> GeneralPPTrussExperiment::findBounds(KTrussIndex *index, vector<int> nodes, bool forLowerBound) {
    map<int, int> bound;
    unordered_map<int, unordered_set<int> > relativeTriangles;
    typedef pair<int, int> pii;
    set <pii> degreeSet;
    vector <pair<int, Edge>> allEdges;
    Graph *graph = index->getGraph();
    auto gNV = GraphUtility::getInducedSubGraph(graph, nodes);

    const vector <Edge> &edges = gNV->getEdges();
    for (Edge edge: edges) {
        int v = edge.getX();
        int u = edge.getY();
        int truss = index->getTruss(edge.getX(), edge.getY());
        allEdges.push_back(pair<int, Edge>(
                truss, edge
        ));
        relativeTriangles[v].insert(u);
        relativeTriangles[u].insert(v);
    }
    sort(allEdges.begin(), allEdges.end());
    reverse(allEdges.begin(), allEdges.end());
    for (int node: nodes)
        degreeSet.insert(pii(relativeTriangles[node].size(), node));
    int k = 2;
    int kDiff = forLowerBound ? 2 : 1;
    while (allEdges.size() && degreeSet.size()) {
        while (allEdges.size() && allEdges.back().first < k) {
            int truss;
            Edge e;
            tie(truss, e) = allEdges.back();
            allEdges.pop_back();
            int u = e.getX(), v = e.getY();
            if (relativeTriangles.count(u) && relativeTriangles.count(v)) {
                relativeTriangles[u].erase(v);
                relativeTriangles[v].erase(u);
                degreeSet.erase(pii(relativeTriangles[u].size() + 1, u));
                degreeSet.insert(pii(relativeTriangles[u].size(), u));
                degreeSet.erase(pii(relativeTriangles[v].size() + 1, v));
                degreeSet.insert(pii(relativeTriangles[v].size(), v));
            }
        }
        while (degreeSet.size() && degreeSet.begin()->first < k - kDiff) {
            int v = degreeSet.begin()->second;
            degreeSet.erase(degreeSet.begin());
            for (int u: relativeTriangles[v]) {
                relativeTriangles[u].erase(v);
                degreeSet.erase(pii(relativeTriangles[u].size() + 1, u));
                degreeSet.insert(pii(relativeTriangles[u].size(), u));
            }
            relativeTriangles.erase(v);
            if (forLowerBound) {
                bound[v] = k - 1;
            } else {
                bound[v] = k;
            }
        }
        k++;
    }
    for (int v: nodes)
        bound[v] = max(bound[v], 2);
    return bound;
}

void GeneralPPTrussExperiment::doData(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "Few arguments. run as ./k-truss.out data public.txt private.txt sample.txt" << endl;
        return;
    }
    PPGraph *ppG = getPPGraph(argv[2], argv[3]);
    vector<int> queryNodes = readSampleFile(argv[4]);
    Graph *g = ppG->getPublicGraph();

    clog << "Calculating Index on Public Graph" << endl;
    KTrussIndex *publicIndex = nullptr;
    {
        ProcessTimer decompositionTimer;
        decompositionTimer.start();
        KTrussDecomposition ktd = KTrussDecomposition(g);
        decompositionTimer.end();
        clog << "KTD initialization " << setprecision(9) << fixed << decompositionTimer.getDuration().count() << endl;
        decompositionTimer.reset();
        decompositionTimer.start();
        ktd.createIndex();
        decompositionTimer.end();
        cout << "Decomposition " << setprecision(9) << fixed << decompositionTimer.getDuration().count() << endl;
        publicIndex = ktd.getFinalIndex();
    }
    clog << "Calculated Index of Public Graph" << endl;

    for (int v: queryNodes) {
        if (!ppG->hasPrivateNode(v))
            continue;
        auto publicNEdges = ppG->publicNeighbors(v);
        auto privateNEdges = ppG->privateNeighbors(v);
        vector<int> publicN;
        vector<int> privateN;
        for (Edge e: publicNEdges)
            publicN.push_back(e.other(v));
        for (Edge e: privateNEdges)
            privateN.push_back(e.other(v));
        int ppTri = 0, publicTri = 0, privateTri = 0;
        for (int u: publicN)
            for (int w: privateN)
                if (g->hasEdge(u, w))
                    ppTri++;
        for (int u: publicN)
            for (int w: publicN)
                if (g->hasEdge(u, w))
                    publicTri++;
        for (int u: privateN)
            for (int w: privateN)
                if (g->hasEdge(u, w))
                    privateTri++;
        int maxTruss = 0;
        int sumTruss = 0;
        for (int u: publicN) {
            int tmp = publicIndex->getTruss(u, v);
            maxTruss = max(maxTruss, tmp);
            sumTruss += tmp;
        }
        vector<int> allNeighbors;
        for (int u: publicN)
            allNeighbors.push_back(u);
        for (int u: privateN)
            allNeighbors.push_back(u);
        map<int, int> upperBound = findBounds(publicIndex, allNeighbors, false);
        map<int, int> lowerBound = findBounds(publicIndex, allNeighbors, true);
        int maxLowerTruss = 0;
        int maxUpperTruss = 0;
        int sumLowerTruss = 0;
        int sumUpperTruss = 0;
        for (int u: allNeighbors) {
            maxLowerTruss = max(maxLowerTruss, lowerBound[u]);
            maxUpperTruss = max(maxUpperTruss, upperBound[u]);
            sumLowerTruss += lowerBound[u];
            sumUpperTruss += upperBound[u];
        }
        int maxPrivateLowerTruss = 0;
        int maxPrivateUpperTruss = 0;
        int sumPrivateLowerTruss = 0;
        int sumPrivateUpperTruss = 0;
        for (int u: privateN) {
            sumPrivateLowerTruss += lowerBound[u];
            sumPrivateUpperTruss += upperBound[u];
            maxPrivateLowerTruss = max(maxPrivateLowerTruss, lowerBound[u]);
            maxPrivateUpperTruss = max(maxPrivateUpperTruss, upperBound[u]);
        }
        cout << v << ", " << publicN.size() << ", " << privateN.size() << ", " << ppTri << ", " << publicTri << ", "
             << privateTri << ", " << maxTruss << ", " << sumTruss << ", "
             << maxLowerTruss << ", " << maxUpperTruss << ", " << sumLowerTruss << ", " << sumUpperTruss << ", "
             << maxPrivateLowerTruss << ", " << maxPrivateUpperTruss << ", " << sumPrivateLowerTruss << ", "
             << sumPrivateUpperTruss << endl;
    }
}

void GeneralPPTrussExperiment::doRun(int argc, char *argv[]) {
    int experimentKs[8] = {5, 7, 9, 11, 13, 15, 20, 40};

    if (argc < 7) {
        cout << "Few arguments."
                "Run as ./k-truss.out run [node/edge] public.txt private.txt sample.txt [queryK e.g. 1000]"
                "[start end]" << endl;
        return;
    }
    // Read from arguments
    bool nodeBased = false;
    bool edgeBased = false;
    bool decompositionBased = false;
    string type(argv[2]);
    if (type == "node")
        nodeBased = true;
    else if (type == "edge")
        edgeBased = true;
    else if (type == "decompose")
        decompositionBased = true;
    else {
        cout << "Wrong mode of run. ./k-truss.out run [node/edge] ..." << endl;
        return;
    }
    PPGraph *ppG = getPPGraph(argv[3], argv[4]);
    Graph *g = ppG->getPublicGraph();
    vector<int> queryNodes = readSampleFile(argv[5]);
    int queryK = atoi(argv[6]);

    int qStart = 0, qEnd = queryNodes.size();
    if (argc > 7) {
        if (argc < 9) {
            cout << "Few arguments" << endl;
            return;
        }
        qStart = max(qStart, atoi(argv[7]));
        qEnd = min(qEnd, atoi(argv[8]));
    }

    // Truss Decomposition
    KTrussIndex *publicIndex = nullptr;
    if (!decompositionBased) {
        clog << "Calculating Index on Public Graph" << endl;
        {
            ProcessTimer decompositionTimer;
            decompositionTimer.start();
            KTrussDecomposition ktd = KTrussDecomposition(g);
            decompositionTimer.end();
            clog << "KTD initialization " << setprecision(9) << fixed << decompositionTimer.getDuration().count()
                 << endl;
            decompositionTimer.reset();
            decompositionTimer.start();
            ktd.createIndex();
            decompositionTimer.end();
            cout << "Decomposition " << setprecision(9) << fixed << decompositionTimer.getDuration().count() << endl;
            publicIndex = ktd.getFinalIndex();
        }
        clog << "Calculated Index of Public Graph" << endl;
    }

    // Copying Graph
    Graph *newG = nullptr;
    {
        ProcessTimer tmpP1;
        tmpP1.start();
        newG = new Graph();
        for (Edge e: g->getEdges())
            newG->addEdge(e);
        tmpP1.end();
        clog << " End of copying graph g " << setprecision(9) << fixed << tmpP1.getDuration().count() << endl;
    }
    // Copying Index
    KTrussIndex *newIndex = nullptr;
    if (!decompositionBased) {
        ProcessTimer tmpP1;
        tmpP1.start();
        newIndex = new KTrussIndex(newG);
        for (Edge e: newG->getEdges())
            newIndex->setTruss(e, publicIndex->getTruss(e));
        tmpP1.end();
        clog << " End of copying index " << setprecision(9) << fixed << tmpP1.getDuration().count() << endl;
    }

    // Experiment
    ProcessTimer::TimeIntervalType totalDuration;
    totalDuration = ProcessTimer::TimeIntervalType::zero();
    for (int i = qStart; i < qEnd; i++) {
        int v = queryNodes[i];
        clog << type << " querying v: " << v << " k: " << queryK
             << " publicD: " << ppG->publicNeighbors(v).size()
             << " privateD " << ppG->publicNeighbors(v).size() << endl;
        ProcessTimer::TimeIntervalType runDuration;
        // Run query
        {
            ProcessTimer runTimer;
            runTimer.start();
            if (nodeBased) {
                auto _ = GeneralPPKTruss::queryGeneralPPKTrussNodeBased(v, queryK, newIndex, ppG);
            } else if (edgeBased) {
                auto _ = GeneralPPKTruss::queryGeneralPPKTrussEdgeBased(v, queryK, newIndex, ppG);
            } else if (decompositionBased) {
                auto _ = GeneralPPKTruss::queryGeneralPPKTrussDecompositionBased(v, queryK, newG, ppG);
            }
            runTimer.end();
            runDuration = runTimer.getDuration();
        }
        clog << " Updated index. duration: " << setprecision(9) << fixed << runDuration.count() << endl;

        // Run more Ks
        if (!decompositionBased)
            for (int expK: experimentKs) {
                ProcessTimer runTimer;
                runTimer.start();
                auto _ = KTrussQuery::findKTruss(v, expK, newIndex);
                runTimer.end();
                cout << setprecision(9) << fixed
                     << "Query, " << v << ", " << expK << ", " << runTimer.getDuration().count() << endl;
                cout << setprecision(9) << fixed
                     << "Query, " << v << ", " << expK << ", " << runTimer.getDuration().count() << endl;
            }

        // Back to initial graph
        {
            ProcessTimer revertTimer;
            revertTimer.start();
            if (!decompositionBased) {
                KTrussUpdate::updateNodeDeletion(v, newIndex);
                newG->deleteNode(v);
                vector<int> prevNeighbors;
                for (Edge e: g->neighbors(v))
                    newG->addEdge(e), prevNeighbors.push_back(e.other(v));
                KTrussUpdate::updateNodeInsertion(v, prevNeighbors, newIndex);
            } else {
                for (Edge e: ppG->privateNeighbors(v))
                    newG->deleteEdge(e);
            }
            revertTimer.end();
            clog << "   Reverted in " << setprecision(9) << fixed << revertTimer.getDuration().count() << endl;
        }

        cout << setprecision(9) << fixed << v << ", " << ppG->publicNeighbors(v).size()
             << ", " << ppG->privateNeighbors(v).size()
             << ", " << runDuration.count() << endl;

        clog << setprecision(9) << fixed << v << ", PublicD: " << ppG->publicNeighbors(v).size()
             << ", PrivateD: " << ppG->privateNeighbors(v).size()
             << ", " << type << ": " << runDuration.count() << endl;
        totalDuration += runDuration;
    }
    clog << setprecision(9) << fixed << "Total " << type << " time: " << totalDuration.count() << endl;
    cout << setprecision(9) << fixed << "Total " << type << " time: " << totalDuration.count() << endl;
}

int GeneralPPTrussExperiment::generalPPKTrussMain(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Few arguments. run as ./k-truss.out [CommandType] [arg1] [arg2] ..." << endl;
        return 0;
    }
    string action(argv[1]);
    if (action == "sample")
        doSampling(argc, argv);
    else if (action == "run")
        doRun(argc, argv);
    else if (action == "data")
        doData(argc, argv);
    else
        cout << "Action not found" << endl;
    return 0;
}