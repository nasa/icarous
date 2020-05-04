#include "scheduler.h"

double  within(int n,double  x,std::vector<std::vector<double> > &F,bool closed,bool min){

    if(closed){
        for(int i=0;i<n;i++){
            if ( (x >= F[i][0]) && (x <= F[i][1]) ){
                if(min){
                    return F[i][0]>F[i][1]?F[i][1]:F[i][0];
                }else{
                    return F[i][0]>F[i][1]?F[i][0]:F[i][1];
                }
            }
        }
    }else{
        for(int i=0;i<n;i++){
            if ( (x > F[i][0]) && (x < F[i][1]) ){
                if(min){
                    return F[i][0]>F[i][1]?F[i][1]:F[i][0];
                }else{
                    return F[i][0]>F[i][1]?F[i][0]:F[i][1];
                }
            }
        }
    }

    return DBL_MAX;
}

// A utility function to swap two elements
void swapD(double * a, double * b)
{
    double  t = *a;
    *a = *b;
    *b = t;
}

void swapI(uint8_t * a, uint8_t * b)
{
    uint8_t t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (int low, int high,std::vector<double> &arrA,std::vector<double> &arrB,std::vector<uint8_t> &arrC)
{
    double pivot = arrA[high];    // pivot
    int i = (low - 1);         // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arrA[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swapD(&arrA[i], &arrA[j]);
            swapD(&arrB[i],&arrB[j]);
            swapI(&arrC[i],&arrC[j]);
        }
    }
    swapD(&arrA[i + 1], &arrA[high]);
    swapD(&arrB[i + 1], &arrB[high]);
    swapI(&arrC[i + 1], &arrC[high]);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSort( int low, int high,std::vector<double>  &arrA,std::vector<double>  &arrB,std::vector<uint8_t> &arrC)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(low, high,arrA,arrB,arrC);

        // Separately sort elements before
        // partition and after partition
        quickSort(low, pi - 1,arrA,arrB,arrC);
        quickSort(pi + 1, high,arrA,arrB,arrC);
    }
}

int8_t LeastDeadlineAtT(int n,std::vector<double>  &R,std::vector<double>  &D,std::vector<uint8_t> &ids,double  t, uint8_t* ScheduledJobs,int numSchJobs){
    std::vector<double> newR(n);
    std::vector<double> newD(n);
    std::vector<uint8_t> newID(n);

    int num = 0;
    for(int i=0;i<n;++i){
        if(R[i] <= t){
            newR[i] = R[i];
            newD[i] = D[i];
            newID[i] = ids[i];
            num++;
        }
    }

    quickSort(0,num-1,newD,newR,newID);

    if(numSchJobs == 0)
        return newID[0];

    for(int i=0;i<num;++i){
        bool available = false;
        for(int j=0;j<numSchJobs;++j){
            if (newID[i] == ScheduledJobs[j]){
                available = true;
                break;
            }
        }

        if(!available)
            return newID[i];
    }

    return -1;
}

bool Scheduler(int n,double *Rtimes,double *Dtimes,double * ScheduledT,uint8_t* ScheduledJobs){
    std::vector<std::vector<double> > F;
    std::vector<double> c(n);
    std::vector<uint8_t> ids(n);
    std::vector<double> R;
    std::vector<double> D;

    for(int i=0;i<n;++i){
        std::vector<double> f(2);
        F.push_back(f);
        R.push_back(Rtimes[i]); 
        D.push_back(Dtimes[i]); 
    }


    for(int i=0;i<n;++i){
        ids[i] = (uint8_t)i;
        c[i] = DBL_MAX;
    }

    quickSort(0,n-1,R,D,ids);

    for(int i=n-1;i>=0;i--){
        for (int j=0;j<n;j++){
            if(D[j] >= D[i]){
                if(c[j] >= DBL_MAX - 0.001){
                    c[j] = D[j] - 1;
                }else{
                    c[j] = c[j] - 1;
                }
            }

            double _c = 0;
            while (_c < DBL_MAX) {
                _c = within(n,c[j],F,false,true);
                if(_c < DBL_MAX)
                    c[j] = _c;
            }
        }

        if (i==0){
            double C = DBL_MAX;
            for(int j=0;j<n;++j){
                if (c[j] < C){
                    C = c[j];
                }
            }

            if(C < R[i]){
                return false;
            }

            double _c = 0;
            std::vector<std::vector<double> > _F(1);
            _F[0].push_back(R[i]);
            _F[0].push_back(R[i] + 1);
            _c = within(1,C,_F,true,true);
            if(_c < DBL_MAX){
                F[i][0] = C-1;
                F[i][1] = R[i];
            }
        }else if(R[i-1] < R[i]){
            double C = DBL_MAX;
            for(int j=0;j<n;++j){
                if (c[j] < C){
                    C = c[j];
                }
            }

            if (C < R[i]){
                return false;
            }

            double _c = 0;
            std::vector<std::vector<double> > _F(1);
            _F[0].push_back(R[i]);
            _F[0].push_back(R[i] + 1);
            _c = within(1,C,_F,true,true);
            if(_c < DBL_MAX){
                F[i][0] = C-1;
                F[i][1] = R[i];
            }
        }
    }

    double t = 0;

    for(int i=0;i<n;++i){
        ScheduledJobs[i] = INTMAX;
        ScheduledT[i] = 0;
    }

    for(int i=0;i<n;++i){
        double rmin = DBL_MAX;
        double RMIN[MAX_SLOTS];
        int count = 0;

        for(int j=0;j<n;++j){
            bool in = false;
            for(int k=0;k<n;k++){
                if(ids[j] == ScheduledJobs[k]){
                    in = true;
                }
            }
            if(!in){
                RMIN[count] = R[j];
                count++;
            }
        }

        for(int j=0;j<count;j++){
            if(RMIN[j] < rmin)
                rmin = RMIN[j];
        }

        t = t>rmin?t:rmin;
        double _c = 0;
        while(_c < DBL_MAX){
            _c = within(n,t,F,false,false);
            if(_c < DBL_MAX)
                t = _c;
        }

        uint8_t minDJobId = LeastDeadlineAtT(n,R,D,ids,t,ScheduledJobs,i);
        ScheduledT[minDJobId] = t;
        t = t+1;
        ScheduledJobs[i] = minDJobId;
    }

    return true;
}
