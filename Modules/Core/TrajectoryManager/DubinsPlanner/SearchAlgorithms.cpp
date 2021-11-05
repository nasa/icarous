#include <memory>
#include <algorithm>
#include <queue>
#include <set>
#include <functional>
#include "DubinsPlanner.hpp"

bool DubinsPlanner::AstarSearch(node_t* root,node_t* goal){
    auto cmp = [] (std::shared_ptr<node_t> A, std::shared_ptr<node_t> B) { 
        return (A->g+A->h) > (B->g+B->h);
    };
    root->source = nullptr;
    root->g = 0;
    root->h = 0;
    std::priority_queue<std::shared_ptr<node_t>,std::vector<std::shared_ptr<node_t>>,decltype(cmp)> frontier(cmp);
    std::set<std::shared_ptr<node_t>> visited;
    std::shared_ptr<node_t> q = std::make_shared<node_t>();
    *q = *root;
    visited.insert(q);
    while(!q->goal){
        bool visitedAlready = true;
        for(auto child: q->children){
            std::shared_ptr<node_t> next = std::make_shared<node_t>();
            *(next) = *child;
            bool status = GetDubinsParams(q.get(),next.get());
            if(status){
                next->source = q.get();
                frontier.push(next);
            }
        }

        // This while loop checks for already visited node.
        // Note: this shouldn't be used if you enable reflexive transitions
        while(visitedAlready && frontier.size() > 0){
            q = frontier.top();
            frontier.pop();
            if(visited.find(q) == visited.end()){
                visitedAlready = false;
                visited.insert(q);
            }else if( (*(visited.find(q)))->g > q->g ){
                visitedAlready = false; 
            }else{
                visitedAlready = true;
            }
        }

        if(frontier.size() == 0){
            break;
        }
    }
    std::list<node_t> astarPath;
    node_t* p = q.get();
    if(q->goal){
        while (p->source != nullptr)
        {
            astarPath.push_front(*p);
            p = p->source;
        }
    }
    astarPath.push_front(*root);
    path = std::vector<node_t>(astarPath.begin(),astarPath.end());


    if(path.size() > 1){
        return true;
    }else{
        return false;
    }
}