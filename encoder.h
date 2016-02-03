#ifndef ENCODER_H
#define ENCODER_H

#include <lemon/list_graph.h>
#include "pkgversion.h"
#include <fstream>
#include <string>

class Encoder
{
public:
    Encoder(const lemon::ListDigraph& givenGraph,
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

    const lemon::ListDigraph& graph;
    const lemon::ListDigraph::NodeMap<std::string>& name;
    const lemon::ListDigraph::NodeMap<std::string>& constraintAddOn;
    const lemon::ListDigraph::NodeMap<PkgVersion>& version;
    const lemon::ListDigraph::NodeMap<std::string>& priority;
    const lemon::ListDigraph::NodeMap<std::string>& architecture;
    const lemon::ListDigraph::NodeMap<std::string>& dependencies;
    const lemon::ListDigraph::NodeMap<std::string>& conflicts;
    const lemon::ListDigraph::NodeMap<std::string>& size;
    const lemon::ListDigraph::NodeMap<std::string>& section;
    const lemon::ListDigraph::ArcMap<std::string>& arcTypeMap;

    const char* outputFile;
    void setOutputFile(const char* filename);
    virtual void encode() = 0;
};

#endif // ENCODER_H
