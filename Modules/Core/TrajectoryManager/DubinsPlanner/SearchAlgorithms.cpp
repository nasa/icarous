#include <memory>
#include "DubinsPlanner.hpp"

bool DubinsPlanner::DijkstraSearch(node_t* root,node_t* goal){

    std::list<node_t*> Q; 

    for(auto &node: potentialFixes){
        if(node.pos.distanceH(goal->pos) < 1e-3){
           node.dist2goal = 0.0;
        }else{
           node.dist2goal = MAXDOUBLE;
        }
        Q.push_back(&node);
    }

    // Lambda function to check if a node is available in Q
    auto inQ = [&] (node_t* nd) { for (auto node: Q) { if( node->id == nd->id) return true;} return false;};

    while(Q.size() > 0){
        auto cmp = [] (node_t* i,node_t* j){return (i->dist2goal>j->dist2goal);};
        Q.sort(cmp);
        node_t *nd = Q.back();
        Q.pop_back();

        std::list<node_t*> *children;
        children = &nd->parents;

        for(auto node: *children){
            if (inQ(node) && (nd->id != node->id)) {
                double distX = nd->pos.distanceH(node->pos);
                double distY = nd->pos.distanceV(node->pos);
                double alt = nd->dist2goal + (distX*distX + distY*distY);
                if (alt < node->dist2goal) {
                    node->dist2goal = alt;
                }
            }
        }
    }
    // Add root node to path list
    path.push_back(*root);

    // Expand tree depth first greedily by connecting nodes with dubins curves
    bool status = GreedyDepthFirst(*root,goal);
    return status;
}

bool DubinsPlanner::GreedyDepthFirst(node_t q,node_t* g){
    auto cmp = [&] (node_t* i, node_t*j) { 
        double h1 = q.pos.distanceH(i->pos);
        double h2 = q.pos.distanceH(j->pos);
        return (h1 + i->dist2goal) < (h2 + j->dist2goal);
    };

    if(q.id != g->id){
        if(q.children.size() > 0){
            q.children.sort(cmp);
            bool status = false;
            for (auto child : q.children)
            {
                node_t next = *child;
                status = GetDubinsParams(&q, &next);
                if (status)
                {
                    path.push_back(next);
                    status = GreedyDepthFirst(next,g);
                    if(status){
                        return true;
                    }else{
                        path.pop_back();
                    }
                }
            }
            return status;
        }else{
            return false;
        }
    }else{
        return true;
    }
}

bool DubinsPlanner::AstarSearch(node_t* root,node_t* goal){
    auto cmp = [] (std::shared_ptr<node_t> A, std::shared_ptr<node_t> B) { 
        node* i = A.get();
        node* j = B.get();
        return (i->g+i->h) < (j->g+j->h);
    };
    root->source = nullptr;
    root->g = 0;
    root->h = 0;
    std::list<std::shared_ptr<node_t>> frontier;
    std::list<std::shared_ptr<node_t>> visited;
    std::shared_ptr<node_t> q = std::make_shared<node_t>();
    *q = *root;
    visited.push_back(q);
    while(!q->goal){
        for(auto child: q->children){
            std::shared_ptr<node_t> next = std::make_shared<node_t>();
            *(next) = *child;
            bool status = GetDubinsParams(q.get(),next.get());
            if(status){
                next->source = q.get();
                frontier.push_back(next);
            }
        }
        if(frontier.size() == 0){
            break;
        }
        frontier.sort(cmp);
        q = frontier.front();
        frontier.pop_front();
        visited.push_back(q);
    }
    std::list<node_t> astarPath;
    node_t* p = q.get();
    while(p->source != nullptr){
        astarPath.push_front(*p);
        p = p->source;
    }
    astarPath.push_front(*root);
    path = std::vector<node_t>(astarPath.begin(),astarPath.end());


    if(path.size() > 1){
        return true;
    }else{
        return false;
    }
}