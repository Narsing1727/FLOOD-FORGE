#pragma once
#include "randomForest.h"
#include <vector>
#include <string>
#include<utility>
class Raster {
public:
    Raster();

    bool loadCSVFile(const std::string& filename);
    bool ValidCell() const;


    double rainTime(int t) const;
    int getRows() const;
    int getCols() const;
    int FloodCells(int t) const;
    int getFlowDirection(int i , int j) const;
    int flowAccumulator(int i , int j , std ::vector< std :: vector<int>>& fa , std :: vector<std ::vector<bool>>& visited) const;

    double FloodMax(int t) const;
    double FloodMin(int t) const;
    double FloodAvg(int t) const;
    double FloodPercentage() const;
    double FloodVolume(int t) const;
    double getMax() const;
    double getMin() const;
    double getAverage() const;

    void printGrid() const;




std :: vector<std::pair<int ,int>>getNeighbours(int i , int j) const;
std :: vector<std::pair<int ,int>>traceFlowPath(int i , int j , std :: vector<std :: vector<bool>>& visited) const;
std :: vector<std :: vector<int>>computeFlowAccumulation() const;
std :: vector<std :: vector<int>>RiverGrid() const;
std :: vector<std :: vector<double>>FloodDepthGrid(int t) const;
   std::vector<std::vector<double>> MLFloodDepthGrid(int t,RandomForest& rf) const;
std :: vector<std :: vector<double>>OutflowInflow(int t) const;
std::vector<std::vector<double>> gridReturn() const;
std::vector<std::vector<double>>FloodDepthGridWithRain(int t, double rainfall) const;





std ::pair<int , int> nextCell(int i , int j , int dir) const;

void loadFromGrid(const std::vector<std::vector<double>>& g);
void exportFlowPathCSV(const std :: string& filename,int si, int sj) const; 
void exportFloodStat(const std :: string& filename , int t) const ;
void exportCSV(std :: vector<std :: vector<double>>& grid , const std :: string& filename);
void exportGridCSV(const std::vector<std::vector<double>>& grid,const std::string& filename) const ;
private:
    std::vector<std::vector<double>> grid;
    int rows;
    int cols;
};
