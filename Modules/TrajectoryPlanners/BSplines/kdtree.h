//
// Created by swee on 4/2/18.
//

#ifndef BSPLINES_KDTREE_H
#define BSPLINES_KDTREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <values.h>

typedef struct node{
    int depth;
    std::vector<double> val;
    node* parent;
    node* rightChild;
    node* leftChild;
}node_t;

class KDTREE{
public:
    std::vector<std::vector<double>> points;
    std::vector<std::vector<double>> kneighbors;
    double neighborhood;
    
    node_t* root;
    
    KDTREE(double eps);
    double dist(node_t* A,node_t* B);
    node_t* ConstructTree(std::vector<std::vector<double>> list,node_t* parent,int depth);
    node_t* traverse(node_t* root,node_t* qnode);
    node_t* unwind(node_t* nodeA,node_t* qnode,double& bestdist,node_t* root);
    node_t* KNN(node_t* root,node_t* qnode,double& distance);
    static bool dimX(std::vector<double> val1, std::vector<double> val2);
    static bool dimY(std::vector<double> val1, std::vector<double> val2);

};

#endif //BSPLINES_KDTREE_H
