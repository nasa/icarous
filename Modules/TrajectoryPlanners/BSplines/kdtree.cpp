//
// Created by research133 on 4/2/18.
//

#include "kdtree.h"


KDTREE::KDTREE(double eps) {
    neighborhood = eps;
}

bool KDTREE::dimX(std::vector<double> val1, std::vector<double> val2){
    return val1[0] < val2[0];
}

bool KDTREE::dimY(std::vector<double> val1, std::vector<double> val2){
    return val1[1] < val2[1];
}

spline_node_t* KDTREE::ConstructTree(std::vector<std::vector<double>> list, spline_node_t *parent, int depth){
    int axis = depth%2;

    if (axis == 0){
        sort(list.begin(),list.end(),dimX);
    }else{
        sort(list.begin(),list.end(),dimY);
    }

    int length = list.size();
    int median = length/2;

    spline_node_t* nd = NULL;
    std::vector<double> valinit({0.0,0.0,0.0});
    if (length > 0) {
        nd = new spline_node_t{0,valinit,NULL,NULL,NULL};
        nd->parent = parent;
        nd->depth = depth;
        nd->val.assign(3,0);
        nd->val = list[median];


        if(length > 1) {
            std::vector<std::vector<double>> left(list.begin(), list.begin() + (median));
            std::vector<std::vector<double>> right(list.begin() + median + 1, list.end());

            nd->leftChild = ConstructTree(left,nd, depth + 1);
            nd->rightChild = ConstructTree(right,nd, depth + 1);
        }else{
            nd->leftChild = NULL;
            nd->rightChild = NULL;
        }
    }
    root = nd;
    return nd;
}

double KDTREE::dist(spline_node_t* A,spline_node_t* B){
    return sqrt(pow(A->val[0]-B->val[0],2) + pow(A->val[1]-B->val[1],2));
}

spline_node_t* KDTREE::traverse(spline_node_t* root,spline_node_t* qnode){
    if(root->val.size() > 0) {
        int splitType = root->depth % 2;
        if (splitType == 0) {
            if (qnode->val[0] < root->val[0]) {
                if (root->leftChild){
                    return traverse(root->leftChild,qnode);
                }else{
                    return root;
                }
            } else {
                if (root->rightChild){
                    return traverse(root->rightChild,qnode);
                }else{
                    return root;
                }
            }
        } else {
            if (qnode->val[1] < root->val[1]) {
                if (root->leftChild){
                    return traverse(root->leftChild,qnode);
                }else{
                    return root;
                }
            } else {
                if (root->rightChild){
                    return traverse(root->rightChild,qnode);
                }else{
                    return root;
                }
            }
        }
    }
}

spline_node_t* KDTREE::unwind(spline_node_t* nodeA,spline_node_t* qnode,double& bestdist,spline_node_t* root){

    if(!nodeA){
        return NULL;
    }

    bool searchOtherHalf = false;
    double prevbestdist = bestdist;
    double val = dist(nodeA,qnode);
    spline_node_t* bestnode = NULL;
    spline_node_t* currentNode = nodeA;

    if(val < neighborhood){
        kneighbors.push_back(nodeA->val);
    }

    if (val < prevbestdist){
        bestnode = nodeA;
        prevbestdist = val;
        bestdist = prevbestdist;
    }

    int split = nodeA->depth%2;

    if(split == 0){
        if(fabs(nodeA->val[0] - qnode->val[0]) < bestdist ){
            searchOtherHalf = true;
        }
    }else{
        if(fabs(nodeA->val[1] - qnode->val[1]) < bestdist ){
            searchOtherHalf = true;
        }
    }

    if(searchOtherHalf){
        spline_node_t* otherbranch;
        if (split==0) {
            if (qnode->val[0] < nodeA->val[0])
                otherbranch = KNN(nodeA->rightChild,qnode,prevbestdist);
            else
                otherbranch = KNN(nodeA->leftChild,qnode,prevbestdist);
        }else{
            if (qnode->val[1] < nodeA->val[1])
                otherbranch = KNN(nodeA->rightChild,qnode,prevbestdist);
            else
                otherbranch = KNN(nodeA->leftChild,qnode,prevbestdist);
        }

        if(prevbestdist < bestdist){
            bestnode = otherbranch;
            bestdist = prevbestdist;
        }
    }

    if(currentNode != root) {
        spline_node_t *upbranch = unwind(currentNode->parent, qnode, prevbestdist, root);
        if (prevbestdist < bestdist) {
            bestnode = upbranch;
            bestdist = prevbestdist;
        }
    }

    return bestnode;
}

spline_node_t* KDTREE::KNN(spline_node_t* root,spline_node_t* qnode,double& distance) {

    if(root == NULL){
        return NULL;
    }

    double bestdist = distance;
    spline_node_t *bestNode = NULL;
    spline_node_t* leaf = traverse(root,qnode);
    double val = dist(leaf, qnode);

    if(val < neighborhood){
        if (root->parent==NULL){
            kneighbors.clear();
        }
        kneighbors.push_back(leaf->val);
    }

    if (val < bestdist){
        bestNode = leaf;
        bestdist = val;
        distance = val;
    }

    val = bestdist;

    spline_node_t* upbranch = NULL;
    if(leaf != root) {
        upbranch = unwind(leaf->parent, qnode, bestdist, root);

        if (bestdist < val) {
            bestNode = upbranch;
            val = bestdist;
            distance = val;
        }
    }

    return bestNode;
}

/*
int main(){

    std::vector<double> val1({1.0,2.0});
    std::vector<double> val2({-1.0,5.0});
    std::vector<double> val3({3.0,7.0});
    std::vector<double> val4({-3.0,1.0});
    std::vector<double> val5({5.0,1.0});
    std::vector<double> val6({4.0,4.0});

    KDTREE kdtree(3);


    kdtree.points.push_back(val1);
    kdtree.points.push_back(val2);
    kdtree.points.push_back(val3);
    kdtree.points.push_back(val4);
    kdtree.points.push_back(val5);
    kdtree.points.push_back(val6);


    node_t* root = kdtree.ConstructTree(kdtree.points,NULL,0);

    node_t qnode;

    std::vector<double> val({-2.5,3.0});
    qnode.val = val;
    double distance = MAXDOUBLE;

    std::vector<std::vector<double>> nearestNodes;

    node_t *nearest = kdtree.KNN(root,&qnode,distance);

    printf("nearest node distance %f\n",distance);

}*/