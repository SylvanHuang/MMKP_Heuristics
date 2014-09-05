/*********************************************************
 *
 * File: MMKP_BBA.cpp
 * Author: Ken Zyma
 *
 * @All rights reserved
 * Kutztown University, PA, U.S.A
 *
 * BBA MMKP Problem
 *
 *********************************************************/

#include "MMKP_BBA.h"

/* MMKP_BBA */

//constructors
MMKP_BBA::MMKP_BBA(MMKPDataSet dataSet, BBA_parameters parameters)
:MMKP_MetaHeuristic(dataSet,parameters),parameters(parameters){}

//overloaded operators
MMKPSolution MMKP_BBA::operator()(std::vector<MMKPSolution> initialPopulation){
    return MMKP_BBA::run(initialPopulation);
}

void MMKP_BBA::quickSort(std::vector<MMKPBatSolution>& input,int p, int r){
    if ( p < r )
    {
        int j = partition(input, p, r);
        MMKP_BBA::quickSort(input, p, j-1);
        MMKP_BBA::quickSort(input, j+1, r);
    }
}

int MMKP_BBA::partition(std::vector<MMKPBatSolution>& input,int p, int r){
    //partition will sort greatest->least
    int pivot = input[r].solution.getProfit();
    while ( p < r )
    {
        while ( input[p].solution.getProfit() > pivot )
            p++;
        
        while ( input[r].solution.getProfit() < pivot )
            r--;
        
        if ( input[p].solution.getProfit() == input[r].solution.getProfit() )
            p++;
        
        else if (p < r )
        {
            MMKPSolution tmp = input[p].solution;
            input[p].solution = input[r].solution;
            input[r].solution = tmp;
        }
    }
    
    return r;
}

MMKPSolution MMKP_BBA::run(std::vector<MMKPSolution> initialPopulation){
    
    //init initial parameters
    //init MMKPBatSolution vector
    std::vector<MMKPBatSolution> population(initialPopulation.size());
    for(int i=0;i<initialPopulation.size();i++){
        population[i].solution = initialPopulation[i];
    }
    MMKP_BBA::initBatParemeters(population);
    
    bool terminationCriterion = false;
    int currentGeneration = 0;
    
    MMKP_BBA::quickSort(population,0,(population.size()-1));
    MMKPSolution bestSolution;
    for(int i=0;i<population.size();i++){
        if(this->dataSet.isFeasible(population[i].solution)){
            bestSolution =  population[i].solution;
            break;
        }
    }
    
    //main loop
    while(!terminationCriterion){
        
        MMKP_BBA::globalSearch(population);
        
        MMKP_BBA::quickSort(population,0,(population.size()-1));
        
        for(int i=0;i<population.size();i++){
            if(this->dataSet.isFeasible(population[i].solution)){
                if(population[i].solution.getProfit() > bestSolution.getProfit()){
                    bestSolution =  population[i].solution;
                    break;
                }
            }
        }

        if(currentGeneration >= this->parameters.numberOfGenerations){
            terminationCriterion = true;
        }
        currentGeneration++;
    }

    return bestSolution;
}

void MMKP_BBA::initBatParemeters(std::vector<MMKPBatSolution>& population){
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for(int i=0;i<population.size();i++){
        std::uniform_real_distribution<> dif(0, 1);
        std::uniform_real_distribution<> div(-1, 1);
        std::uniform_real_distribution<> dil(1, 2);
        
        //init frequency
        float f1;
        float f2;
        do{
            f1 = dif(gen);
            f2 = dif(gen);
        }while(f1 == f2);
        
        if(f1 > f2){
            population[i].fmax = f1;
            population[i].fmin = f2;
        }else{
            population[i].fmax = f2;
            population[i].fmin = f1;
        }
        
        //init velocity
        population[i].v = div(gen);
        
        //init rate of pulse
        population[i].r = dif(gen);
        
        //init loudness
        population[i].a = dil(gen);
    }
}

void MMKP_BBA::globalSearch(std::vector<MMKPBatSolution>& population){
    //already in sorted order in this implementation.
    //MMKP_BBA::quickSort(population,0,(population.size()-1));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dib(0, 1);
    
    MMKPBatSolution bestBat = population[0];
    
    for(int i=0;i<population.size();i++){
        
        MMKPBatSolution currentBat = population[i];
        
        //equation 1
        float B = dib(gen);
        float fTemp = currentBat.fmax + ((currentBat.fmax
                                          -currentBat.fmin)*B);
        
        //equation 2
        float averageV = 0.0;
        int averageVSize = 0;
        for(int j=0;j<currentBat.solution.size();j++){
            for(int k=0;k<currentBat.solution[j].size();k++){
                
                float vTemp = currentBat.v + ((currentBat.solution[j][k]
                                               -bestBat.solution[j][k])*fTemp);
                averageV += vTemp;
                averageVSize++;
                
                //equation "3"--binary uses 7 and 8
                float normalizedV = 1/(1+(exp(-(vTemp))));
                float r = dib(gen);
                
                if(r <= normalizedV){
                    currentBat.solution[j][k] = 1;
                }else{
                    currentBat.solution[j][k] = 0;
                }
                
            }
        }
        currentBat.v = (averageV/averageVSize);
        MMKP_MetaHeuristic::makeFeasible(currentBat.solution);
        population[i] = currentBat;
    }
}
