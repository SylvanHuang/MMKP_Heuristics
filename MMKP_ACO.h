/***************************************************************************
 *
 * File: MMKP_BBA.cpp
 * Author: Ken Zyma
 *
 * Any Colony Optimization metaheuristic from Ren et al "An ant colony 
 * optimization approach to the multiple-choice multidimensional knapsack problem".
 *
 * The MIT License (MIT)
 *
 * Copyright (c) [2015] [Kutztown University]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************/

#ifndef ____MMKP_ACO__
#define ____MMKP_ACO__

#include <iostream>
#include <vector>
#include <time.h>
#include <cmath>
#include <math.h>
#include <assert.h>
#include <iomanip>
#include <exception>
#include <cfloat>
#include <algorithm> //for "random shuffle"
#include <random>

#include "MMKP_MetaHeuristic.h"
#include "MMKPSolution.h"
#include "MMKPDataSet.h"

/**
 * Parameters for customizing the BBA algorithm.
 */
class ACO_parameters:public MetaHeuristic_parameters{
public:
    float B;    //B is the weighting of pheramone to heuristic
    float p;    //p and e in pheramone update rule
    float e;
    
    ACO_parameters(){
        this->B = 25.0;
        this->p = 0.98;
        this->e = 0.005;
    }
};

typedef struct aco_additions{
    float pheramone;
    float heuristic;
}ACO_Additions;

class ACO_DataSetAdditions{
private:
    std::vector<std::vector<ACO_Additions> > classList;
    std::vector<float> denominators;
    float B;        //pheramone/heuristic ratio
    float Lstar;    //tight upper bound
    bool isUpdated;
    void update();
    MMKPSolution getLMV(std::vector<float> U);
    float getS_k(MMKPSolution sol, int r);
    MMKPDataSet dataSet;
public:
    ACO_DataSetAdditions(MMKPDataSet dataSet, float B,float e);
    
    void initParameters();
    
    std::vector<ACO_Additions>& operator[](int index);
    
    float getProbability(int classNum, int itemNum);
    
    int returnItemIndex(int classNum);
    
    int size();
    
    float getLStar();
};

/**
 * Any Colony Optimization adapted to the
 * multiple-choice, multiple-dimensional knapsack problem.
 */
class MMKP_ACO:public MMKP_MetaHeuristic{
private:
    ACO_parameters parameters;
    ACO_DataSetAdditions solDesirability;
public:
    
    MMKP_ACO(MMKPDataSet dataSet, ACO_parameters parameters);
    
    MMKPSolution operator()(std::vector<MMKPSolution> initialPopulation);
    
    MMKPSolution run(std::vector<MMKPSolution> initialPopulation);
    
    std::vector<MMKPSolution> runOneGeneration
    (std::vector<MMKPSolution> population);
    
    void constructSolutions(std::vector<MMKPSolution>& population);
    
    void updatePheramone(MMKPSolution bestSol);

};


#endif /* defined(____MMKP_ACO__) */
