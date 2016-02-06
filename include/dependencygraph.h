#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <lemon/list_graph.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <stack>
#include <map>
#include "pkgversion.h"


class Dependencygraph
{
public:
    lemon::ListDigraph graph;
    lemon::ListDigraph::NodeMap<std::string> name;
    lemon::ListDigraph::NodeMap<std::string> constraintAddOn;
    lemon::ListDigraph::NodeMap<PkgVersion> version;
    lemon::ListDigraph::NodeMap<std::string> priority;
    lemon::ListDigraph::NodeMap<std::string> architecture;
    lemon::ListDigraph::NodeMap<std::string> dependencies;
    lemon::ListDigraph::NodeMap<std::string> conflicts;
    lemon::ListDigraph::NodeMap<std::string> size;
    lemon::ListDigraph::NodeMap<std::string> section;
    lemon::ListDigraph::ArcMap<std::string> arcTypeMap;
    std::map <std::string, int> nameidMap;
    const char* sourceFile;

    //Konstruktor:
    Dependencygraph(char*& sourcefileName, std::vector<std::string>& packages);

    //Methoden:
    void buildGraph(
            std::vector<std::string> &packages);
    void displayGraph();

private:
    //general:
    bool cuttable(
            std::string wholeString,
            std::string delimiter);
    void cutString(
            std::vector <std::string>& singleStringVec,
            std::string wholeString,
            std::string delimiter);
    int getNumberOfLinesOfSourceFile();
    void checkForExtraConstraints(
            std::string pkgName,
            std::vector<std::string>& splitNameVector);
    void evaluateConstraintAddOn(
            std::string constraint,
            std::vector<std::string>& resultVector);

    // adding Nodes:
    void addNodeByName(
            std::vector<int>& newPkgIds,
            int sourceNodeId,
            std::string pkgname);
    void addNewNodeAndFillMetadata(
            std::string pkgName,
            std::string versionConstraint,
            bool& pkgFound,
            std::vector<int>& idOfNewPkg);
    int searchForPkg(
            std::string pkgName,
            int skipLines);
    int searchForPkg(
            std::string pkgName,
            std::string pkgVersion,
            int skipLines);
    void jumpBufferMetadata(
            lemon::ListDigraph::Node& tempNode,
            int tempLine);

    // adding Arcs:
    void addArcByType(
            const int& sourceNodeId,
            const int& targetNodeId);
};

#endif // DEPENDENCYGRAPH_H
