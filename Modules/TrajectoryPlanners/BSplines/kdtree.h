//
// Created by swee on 4/2/18.
//

#ifndef BSPLINES_KDTREE_H
#define BSPLINES_KDTREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <values.h>

typedef struct spline_node{
    int depth;
    std::vector<double> val;
    spline_node* parent;
    spline_node* rightChild;
    spline_node* leftChild;
}spline_node_t;

class KDTREE{
public:
    std::vector<std::vector<double>> points;
    std::vector<std::vector<double>> kneighbors;
    double neighborhood;
    
    spline_node_t* root;
    
    KDTREE(double eps);
    double dist(spline_node_t* A,spline_node_t* B);
    spline_node_t* ConstructTree(std::vector<std::vector<double>> list,spline_node_t* parent,int depth);
    spline_node_t* traverse(spline_node_t* root,spline_node_t* qnode);
    spline_node_t* unwind(spline_node_t* nodeA,spline_node_t* qnode,double& bestdist,spline_node_t* root);
    spline_node_t* KNN(spline_node_t* root,spline_node_t* qnode,double& distance);
    static bool dimX(std::vector<double> val1, std::vector<double> val2);
    static bool dimY(std::vector<double> val1, std::vector<double> val2);

};

#endif //BSPLINES_KDTREE_H
