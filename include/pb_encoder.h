#ifndef PB_ENCODER_H
#define PB_ENCODER_H

#include "encoder.h"
#include "pkgversion.h"
#include <fstream>
#include <stdexcept>
#include <map>
#include <set>
#include <list>

class PB_Encoder : public Encoder
{
public:
    PB_Encoder(const char* outputFilename,
               const lemon::ListDigraph& givenGraph,
               const lemon::ListDigraph::NodeMap<std::string>& name,
               const lemon::ListDigraph::NodeMap<std::string>& constraintAddOn,
               const lemon::ListDigraph::NodeMap<PkgVersion>& version,
               const lemon::ListDigraph::NodeMap<std::string>& priority,
               const lemon::ListDigraph::NodeMap<std::string>& architecture,
               const lemon::ListDigraph::NodeMap<std::string>& dependencies,
               const lemon::ListDigraph::NodeMap<std::string>& conflicts,
               const lemon::ListDigraph::NodeMap<std::string>& size,
               const lemon::ListDigraph::NodeMap<std::string>& section,
               const lemon::ListDigraph::ArcMap<std::string>& arcTypeMap);

    std::map<std::string, int> weightMap;

    virtual void encode();
//private:
    void outEdgesToPseudoBooleanConstraint(
            std::ofstream& outFile,
            lemon::ListDigraph::Node& sourceNode);
    std::string collectAllAlternatives(
            lemon::ListDigraph::Node& sourceNode,
            std::string variableName,
            std::set<std::string>& alreadyCheckedNames);
    void collectAllAlternatives(
            lemon::ListDigraph::Node& sourceNode,
            std::string variableName,
            std::list<PkgVersion>& alternatives);


    void setWeight(std::string variable, int weight);
    void initalizeWeights();

    void userPolicyToObjectiveFunction(
            std::ofstream& outFile,
            const std::string& userPolicy);
    void numberAsUserPolicy(
            std::ofstream& outFile);
    void removalAsUserPolicy(
            std::ofstream& outFile);
    void freshnessAsUserPolicy(
            std::ofstream& outFile);
    int calculateNormalizedDistance(
            lemon::ListDigraph::Node sourceNode,
            std::string name,
            PkgVersion version);
};

#endif // PB_ENCODER_H
