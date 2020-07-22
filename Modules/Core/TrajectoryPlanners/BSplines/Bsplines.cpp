//
// Created by research133 on 3/27/18.
//

#include "Bsplines.hpp"
#include "math.h"


void Bsplines::SetSplineProperties(int _ndim, int _lenT, double *_tVec, int _lenK, double *_KntVec, int _order) {
    lenT = _lenT;
    lenK = _lenK;
    tVec = _tVec;
    KntVec = _KntVec;
    order = _order;
    ndim = _ndim;
}

void Bsplines::SetObstacleProperties(double _dx, double _dy, double _threshold, double _obsWeight) {
    dx = _dx;
    dy = _dy;
    OBS_THRESH = _threshold;
    obsWeight = _obsWeight;
}

void Bsplines::SetInitControlPts(double *ctrlPt) {
    ctrlPt0 = ctrlPt;
}

void Bsplines::SetObstacles(larcfm::Poly3D& obs) {

    KDTREE *kdtree = new KDTREE(5);
    int numVertices = obs.size();
    for(int i=0;i<numVertices;++i){
        int j = (i+1)%numVertices;
        larcfm::Vect2 vertex1 = obs.poly2D().getVertex(i);
        larcfm::Vect2 vertex2 = obs.poly2D().getVertex(j);
        double distH = (vertex2 - vertex1).norm();
        double distV = obs.getTop() - obs.getBottom();
        int numNodesH = (int) distH/dx;
        int numNodesV = (int) distV/dy;
        double nH = 1.0/(double)numNodesH;
        double nV = distV/(double)numNodesV;
        numNodesV = 0;
        for(int q=0;q<=numNodesV;++q) {
            for (int k = 0; k <= numNodesH; ++k) {
                larcfm::Vect2 vertex = vertex1.Scal(1 - nH*k) + vertex2.Scal(nH*k);
                double h = obs.getBottom() + nV*q;
                std::vector<double> val({vertex.x, vertex.y,h});
                kdtree->points.push_back(val);
            }
        }
    }

    std::vector<double> val = kdtree->points[10];
    kdtree->ConstructTree(kdtree->points,NULL,0);
    kdtreeList.push_back(kdtree);
    obslist.push_back(obs);
}

double Bsplines::Nfac(double t,int i,int k){
    if (k==0){
        if ((t >= KntVec[i]) && (t < KntVec[i+1])){
            return 1;
        }else{
            return 0;
        }
    }else{
        double facA,facB;
        double facAN = t - KntVec[i];
        double facAD = KntVec[i+k] - KntVec[i];
        double facBN = KntVec[i+k+1] - t;
        double facBD = KntVec[i+k+1] - KntVec[i+1];
        fabs(facAD)<1e-5?facA=0:facA=facAN/facAD;
        fabs(facBD)<1e-5?facB=0:facB=facBN/facBD;
        double Nikm1   = Nfac(t,i,k-1);
        double Nip1km1 = Nfac(t,i+1,k-1);
        return facA*Nikm1 + facB*Nip1km1;
    };
}

void Bsplines::GetPoint(double t,double *x,double *output) {
    double sumX=0,sumY=0;
    int numPts = ndim/2;
    for(int j=0;j<numPts;j++){
        double pt[2] = {x[2*j],x[2*j+1]};
        double nfac = Nfac(t,j,order);
        sumX += pt[0]*nfac;
        sumY += pt[1]*nfac;
    }

    output[0] = sumX;
    output[1] = sumY;
}

double Bsplines::Beta(double x){
    double val;
    x>=OBS_THRESH?val=0:val=x*1.0/OBS_THRESH + (1.0/x)*OBS_THRESH - 2.0;
    return val;
}

double Bsplines::HingeLoss(double x){
    if (x > OBS_THRESH){
        return 0;
    }else if( x > 0 && x < OBS_THRESH){
        return 0.5*(1-x)*(1-x);
    }else{
        return 0.5 - x;
    }
}

double Bsplines::HingeLossGrad(double x) {
    if(x > OBS_THRESH){
        return 0;
    }else if(x > 0 && x < OBS_THRESH){
        return x-1;
    }else{
        return -1;
    }
}

double Bsplines::dist2D(double x1,double y1,double x2,double y2){
    return sqrt( pow((x1 - x2),2) + pow((y1 - y2),2));
}

double Bsplines::Objective2D(const double *x){
    double cost = 0;
    double oldVal[2] = {0.0,0.0};
    int numPts = ndim/2;
    for(int i=0;i<lenT;i++){
        double sumX = 0;
        double sumY = 0;
        for(int j=0;j<numPts;j++){
            double pt[2] = {x[2*j],x[2*j+1]};
            double nfac = Nfac(tVec[i],j,order);
            sumX += pt[0]*nfac;
            sumY += pt[1]*nfac;
        }

        double sign;
        larcfm::Vect3 point(sumX,sumY,0);
        bool val = polycarp.definitelyInside(point,obslist.front());
        (val)?sign=-1:sign=1;

        double obsCost = 0;
        for(it = kdtreeList.begin();it != kdtreeList.end(); ++it) {
            KDTREE *kdtree = *it;
            std::vector<double> qval({sumX,sumY,0});
            spline_node_t qnode{0,qval,NULL,NULL,NULL};
            double _distval = MAXDOUBLE;
            spline_node_t *nn = kdtree->KNN(kdtree->root,&qnode,_distval);
            obsloc[0] = nn->val[0];
            obsloc[1] = nn->val[1];
            double dist2obs = dist2D(obsloc[0], obsloc[1], sumX, sumY)*sign;
            obsCost += HingeLoss(dist2obs)*obsWeight;
        }

        double pathlen;
        i>0?pathlen=dist2D(oldVal[0],oldVal[1],sumX,sumY):pathlen = 0;

        cost += pathlen*pathlen + obsCost;
        oldVal[0] = sumX;
        oldVal[1] = sumY;
    }

    return cost;
}

void Bsplines::Gradient2D(const double* x0,double *grad){

    int numPts = ndim/2;

    for(int k=0;k<numPts;++k){

        double sumLPx = 0;
        double sumLPy = 0;
        double sumOPx = 0;
        double sumOPy = 0;

        for(int i=0;i<lenT-1;++i){
            double Ptx1 = 0;
            double Ptx2 = 0;
            double Pty1 = 0;
            double Pty2 = 0;
            for(int j=0;j<numPts;++j){
                double P1[2] = {x0[2*j],x0[2*j+1]};
                double nfac1 = Nfac(tVec[i],j,order);
                double nfac2 = Nfac(tVec[i+1],j,order);

                Ptx1 += nfac1*P1[0];
                Pty1 += nfac1*P1[1];

                Ptx2 += nfac2*P1[0];
                Pty2 += nfac2*P1[1];
            }
            sumLPx += 2*(Ptx2 - Ptx1)*(Nfac(tVec[i+1],k,order) - Nfac(tVec[i],k,order));
            sumLPy += 2*(Pty2 - Pty1)*(Nfac(tVec[i+1],k,order) - Nfac(tVec[i],k,order));
        }


        for(int i=0;i<lenT;++i){
            double Ptx1 = 0;
            double Pty1 = 0;

            for(int j=0;j<numPts;++j){
                double P1[2] = {x0[2*j],x0[2*j+1]};
                double nfac1 = Nfac(tVec[i],j,order);
                Ptx1 += nfac1*P1[0];
                Pty1 += nfac1*P1[1];
            }

            double nfac  = Nfac(tVec[i],k,order);
            double dist2obsgradX = 0;
            double dist2obsgradY = 0;

            for(it = kdtreeList.begin();it != kdtreeList.end(); ++it) {
                KDTREE *kdtree = *it;
                std::vector<double> qval({Ptx1,Pty1,0});
                spline_node_t qnode{0,qval,NULL,NULL,NULL};
                double _distval = MAXDOUBLE;
                spline_node_t *nn = kdtree->KNN(kdtree->root,&qnode,_distval);

                obsloc[0] = nn->val[0];
                obsloc[1] = nn->val[1];
                double dist2obs = dist2D(obsloc[0], obsloc[1], Ptx1, Pty1);

                double sign;
                larcfm::Vect3 point(obsloc[0],obsloc[1],0);
                bool val = polycarp.definitelyInside(point,obslist.front());
                val?sign=-1:sign=1;

                dist2obs = dist2obs*sign;

                if (fabs(dist2obs) < 0.0001){
                    dist2obs = 0.0001*sign;
                }


                //double dbeta = (1.0 / OBS_THRESH - OBS_THRESH / (dist2obs * dist2obs));
                double hlossgrad = HingeLossGrad(dist2obs);
                dist2obsgradX += (hlossgrad * (1 / dist2obs) * (Ptx1 - obsloc[0]) * nfac)*obsWeight;
                dist2obsgradY += (hlossgrad * (1 / dist2obs) * (Pty1 - obsloc[1]) * nfac)*obsWeight;

            }

            sumOPx += dist2obsgradX;
            sumOPy += dist2obsgradY;
        }

        grad[2*k] = sumLPx + sumOPx;
        grad[2*k+1] = sumLPy + sumOPy;

        if( (k==0) || (k==numPts-1)){
            grad[2*k] = 0;
            grad[2*k+1] = 0;
        }
        
    }
}