#include <boost/graph/adjacency_list.hpp>
namespace meshtools {
template <typename EdgeProp>
using DirectedGraph =
    boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
                          boost::no_property, EdgeProp>;

struct MatchingEdge {
  int matching_index = -1;
  double diff = 0.0;
};
using EdgeProperty = boost::property<boost::edge_weight_t, MatchingEdge>;
using MatchingGraph = DirectedGraph<EdgeProperty>;
typedef MatchingGraph::edge_descriptor EdgeIter;
typedef MatchingGraph::vertex_descriptor VertexIter;
} // namespace meshtools
