#include "encoder.h"

Encoder::Encoder(const lemon::ListDigraph& givenGraph,
                 const lemon::ListDigraph::NodeMap<std::string>& name,
                 const lemon::ListDigraph::NodeMap<std::string>&constraintAddOn,
                 const lemon::ListDigraph::NodeMap<PkgVersion>& version,
                 const lemon::ListDigraph::NodeMap<std::string>& priority,
                 const lemon::ListDigraph::NodeMap<std::string>& architecture,
                 const lemon::ListDigraph::NodeMap<std::string>& dependencies,
                 const lemon::ListDigraph::NodeMap<std::string>& conflicts,
                 const lemon::ListDigraph::NodeMap<std::string>& size,
                 const lemon::ListDigraph::NodeMap<std::string>& section,
                 const lemon::ListDigraph::ArcMap<std::string>& arcTypeMap):
    graph(givenGraph),
    name(name),
    constraintAddOn(constraintAddOn),
    version(version),
    priority(priority),
    architecture(architecture),
    dependencies(dependencies),
    conflicts(conflicts),
    size(size),
    section(section),
    arcTypeMap(arcTypeMap)
{

}

void Encoder::setOutputFile(const char *filename){
    this->outputFile = filename;
}
