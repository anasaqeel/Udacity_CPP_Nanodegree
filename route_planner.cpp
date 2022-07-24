#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    RoutePlanner::start_node = &m_Model.FindClosestNode(start_x, start_y);
    RoutePlanner::end_node = &m_Model.FindClosestNode(end_x, end_y);
}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    //float distanceToEnd;
    return node->distance(*end_node);
    //return distanceToEnd;
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for(auto x : current_node->neighbors)
    {
        if(x->visited == false)
        {
            x->parent = current_node;
            x->h_value = CalculateHValue(x);
            x->g_value = current_node->g_value + current_node->distance(*x);
            x->visited = true;
            open_list.push_back(x);
        }
    }
}


bool Compare(RouteModel::Node *node1, RouteModel::Node *node2)
{
    float f1 = node1->g_value + node1->h_value;
    float f2 = node2->g_value + node2->h_value;
    return f1 > f2;
}
RouteModel::Node *RoutePlanner::NextNode() {
    sort(open_list.begin(), open_list.end(), Compare);
    RouteModel::Node *current = open_list.back();
    open_list.pop_back();
    return current;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    RouteModel::Node* pNode = current_node;
    while(pNode->parent!= nullptr)
    {
        path_found.push_back(*pNode);
        distance += pNode->distance(*pNode->parent);
        pNode = pNode->parent;
    }
    path_found.push_back(*pNode);
    std::reverse(path_found.begin(), path_found.end());

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    current_node = start_node;
    open_list.push_back(current_node);
    start_node->visited = true;
    RouteModel::Node* next;
    while(open_list.size() > 0)
    {
        next = NextNode();
        if(next == end_node)
        {
            m_Model.path = ConstructFinalPath(next);
            return;
        }
        else
            AddNeighbors(next);
    }
    m_Model.path = ConstructFinalPath(next);
    
}
