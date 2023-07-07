#ifndef NODE_GRAPH_HPP_INCLUDED
#define NODE_GRAPH_HPP_INCLUDED

#include <map>


class NodeData {

};


template <typename id_type>
class NodeGraph {
    
    std::map<id_type, NodeData> datas;

public:

    NodeGraph() = default;
};

#endif // NODE_GRAPH_HPP_INCLUDED

/**
 *
 * - Stocker le graphe en le représentant par ses sommets pour pouvoir le parcourir rapidement.
 *   Les vraies données peuvent être stockées dans une map qu'on accède par l'ID du node.
 * 
 */