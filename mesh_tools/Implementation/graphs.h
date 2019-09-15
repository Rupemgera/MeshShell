#include <boost/graph/adjacency_list.hpp>
namespace meshtools{
  template<typename EdgeProp>
  using DirectGraph = boost::adjacency_list<boost::vecS,boost::vecS,boost::directedS,boost::no_property,EdgeProp>;
}
