#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>
#include "../Parser/Parser.h"
#include "../KTrussIndex/KTrussDecomposition.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../Graph/GraphGeneration.h"
#include "../KTrussUpdate/KTrussUpdate.h"
#include "../Utilities/ProcessTimer.h"
#include "SubgraphTrussExperiment.h"

using namespace std;

vector<int> SubGraphTrussExperiment::readSampleFile(char *path) {
    FILE *fin = fopen(path, "r");
    vector<int> ans;
    int x;
    while (fscanf(fin, "%d", &x) != EOF)
        ans.push_back(x);
    return ans;
}

void SubGraphTrussExperiment::doSampling(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Few arguments. run as ./k-truss.out sample [bin/perc] graph.txt" << endl;
        return;
    }

    string samplingType(argv[2]);

    string graphFileName(argv[3]);
    Graph *g = GraphGeneration::generateGraph(graphFileName);

    typedef pair<int, int> pii;
    vector<pii> nodesWithDegree;
    for (int u: g->getNodes())
        nodesWithDegree.push_back(pii(g->deg(u), u));
    sort(nodesWithDegree.begin(), nodesWithDegree.end());

    if (samplingType == "bin") {
        int binCount = atoi(argv[4]), maxBinSize = atoi(argv[5]);
        if (argc < 6) {
            cout << "Few arguments. run as ./k-truss.out sample bin graph.txt bin_count max_bin" << endl;
            return;
        }

        // bin ranges
        int maxDegree = nodesWithDegree.back().first;
        vector<int> ranges;
        for (int i = 0; i < binCount; i++)
            ranges.push_back(i * maxDegree / binCount);
        ranges.push_back(maxDegree + 1);

        // select nodes
        vector<int> nodes;
        int index = 0;
        for (int i = 0; i < (int) ranges.size() - 1; i++) {
            vector<int> bin;
            while (index < nodesWithDegree.size() && nodesWithDegree[index].first < ranges[i + 1])
                bin.push_back(nodesWithDegree[index].second), index++;
            random_shuffle(bin.begin(), bin.end());
            for (int j = 0; j < min(maxBinSize, (int) bin.size()); j++)
                nodes.push_back(bin[j]);
        }
        clog << " Subgraph is selected with " << nodes.size() << " nodes from " << graphFileName << endl;
        for (int v: nodes)
            cout << v << endl;
    } else if (samplingType == "perc") {
        int perc = atoi(argv[4]);
        if (argc < 5) {
            cout << "Few arguments. run as ./k-truss.out sample perc graph.txt percentage" << endl;
            return;
        }
        int ind = perc * nodesWithDegree.size() / 100;
        clog << " min " << nodesWithDegree[0].first << " " << perc << "% " << nodesWithDegree[ind].first
             << " max " << nodesWithDegree.back().first << endl;
        random_shuffle(nodesWithDegree.begin(), nodesWithDegree.begin() + ind);
        random_shuffle(nodesWithDegree.begin() + ind, nodesWithDegree.end());
        int maxSampling = 100;
        for (int i = 0; i < maxSampling && i < ind; i++)
            cout << nodesWithDegree[i].second << endl;
        for (int i = ind; i < ind + maxSampling && i < (int) nodesWithDegree.size(); i++)
            cout << nodesWithDegree[i].second << endl;
    } else {
        cout << "Sampling type not defined" << endl;
        return;
    }
}

void SubGraphTrussExperiment::doRun(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "Few arguments."
                "Run as ./k-truss.out run [node/edge] graph.txt sample.txt [start, end]" << endl;
        return;
    }
    // Read from arguments
    bool nodeBased = true;
    string type(argv[2]);
    if (type == "node")
        nodeBased = true;
    else if (type == "edge")
        nodeBased = false;
    else {
        cout << "Wrong mode of run. ./k-truss.out run [node/edge] ..." << endl;
        return;
    }

    vector<int> queryNodes = readSampleFile(argv[4]);
    string graphFileName(argv[3]);
    Graph *g = GraphGeneration::generateGraph(graphFileName);

    int q_start = 0, q_end = queryNodes.size();
	cout << " checking " << nodeBased << " " << graphFileName << endl;
    if (argc > 5) {
        q_start = atoi(argv[5]);
        q_end = atoi(argv[6]);
    }
    q_start = max(q_start, 0);
    q_end = min(q_end, (int) queryNodes.size());

    // Loading sub-graph
    map<int, bool> subGraphMark;
    for (int v: queryNodes)
        subGraphMark[v] = true;

    Graph *newG = new Graph();
    for (Edge e: g->getEdges())
        if (!subGraphMark[e.getX()] && !subGraphMark[e.getY()])
            newG->addEdge(e);

    // Adding nodes before start
    for (int i = 0; i < q_start; i++) {
        int v = queryNodes[i];
        for (Edge e: g->neighbors(v)) {
            int u = e.other(v);
            if (!subGraphMark[u])
                newG->addEdge(e);
        }
        subGraphMark[v] = false;
    }

    // Truss
    KTrussIndex *index = nullptr;
    {
        ProcessTimer decompositionTimer;
        decompositionTimer.start();
        KTrussDecomposition ktdG = KTrussDecomposition(newG);
        ktdG.createIndex();
        index = ktdG.getFinalIndex();
        decompositionTimer.end();
        cout << "Decomposition " << setprecision(9) << fixed << decompositionTimer.getDuration().count() << endl;
        clog << "Decomposition " << setprecision(9) << fixed << decompositionTimer.getDuration().count() << endl;
    }

    // Experiment
    ProcessTimer::TimeIntervalType runTotalTime = ProcessTimer::TimeIntervalType::zero();
    for (int i = q_start; i < q_end; i++) {
        int v = queryNodes[i];
        vector<int> neighbors;
        for (Edge e: g->neighbors(v)) {
            int u = e.other(v);
            if (!subGraphMark[u])
                neighbors.push_back(u);
        }

        ProcessTimer runTimer;
        runTimer.start();
        if (nodeBased) {
            for (int u: neighbors)
                newG->addEdge(u, v);
            KTrussUpdate::updateNodeInsertion(v, neighbors, index);

        } else {
            for (int u: neighbors) {
                newG->addEdge(u, v);
                KTrussUpdate::updateEdgeInsertion(Edge(u, v), index);
            }
        }
        runTimer.end();

        auto vDuration = runTimer.getDuration();
        clog << "   Insertion of node " << v << " d: " << neighbors.size()
             << ": " << std::setprecision(9) << fixed << vDuration.count() << endl;

        cout << v << ", " << neighbors.size() << ", " << std::setprecision(9) << fixed << vDuration.count() << endl;

        runTotalTime += vDuration;
        subGraphMark[v] = false;
    }
    clog << type << " running time: " << runTotalTime.count() << endl;
}

int SubGraphTrussExperiment::subGraphTrussMain(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Few arguments. run as ./k-truss.out [CommandType] [arg1] [arg2] ..." << endl;
        return 0;
    }
    string action(argv[1]);
    if (action == "sample")
        doSampling(argc, argv);
    else if (action == "run")
        doRun(argc, argv);
    else
        cout << "Action not found" << endl;
    return 0;
}
