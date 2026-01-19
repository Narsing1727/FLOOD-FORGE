#include<iostream>
using namespace std;
#include "raster.h"
#include<fstream>


double Raster :: FloodMax(int t) const {
auto fa = FloodDepthGrid(t);
double maxF = fa[0][0];

for(int i = 0 ;i<rows ; i++){
    for(int j = 0 ; j<cols ; j++){
        if(fa[i][j] > maxF){
            maxF = fa[i][j];
        }
    }
}
return maxF * 1.0;
}



double Raster :: FloodMin(int t) const {
auto fa = FloodDepthGrid(t);
double minF = fa[0][0];

for(int i = 0 ;i<rows ; i++){
    for(int j = 0 ; j<cols ; j++){
        if(fa[i][j] <  minF){
            minF = fa[i][j];
        }
    }
}
return minF * 1.0;
}


int Raster :: FloodCells(int t) const {
int cells = 0;
auto fa = FloodDepthGrid(t);
for(int i = 0 ; i<rows ; i++){
    for(int j = 0 ; j<cols ; j++){
        if(fa[i][j] > 0){
            cells++;
        }
    }
}
return cells;
}


double Raster :: FloodAvg(int t) const {
double avg = 0;
auto fa = FloodDepthGrid(t);
for(int i = 0 ; i<rows ; i++){
    for(int j = 0 ; j<cols ; j++){
       avg += fa[i][j];
    }
}
return avg/((rows)*(cols)*1.0);
}


double Raster::FloodVolume(int t) const {
    auto fa = FloodDepthGrid(t);
    double volume = 0.0;

    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            volume += fa[i][j];

    return volume;
}
void Raster :: exportFloodStat(const string& filename , int t) const {
    static bool firstWrite = false;

ofstream file(filename , ios :: app);

if(!file.is_open()){
    cout<<"Error while opening the file"<<endl;
    return;

}
if(!firstWrite){
file << "time,max,avg,cells,volume\n";
firstWrite = true;
}
 file << t << ","
        << FloodMax(t) << ","
        << FloodAvg(t) << ","
        << FloodCells(t) << ","
        << FloodVolume(t)
        << "\n";
        file.close();

cout<<"Writting file succesfully "<<filename<<endl;        
}

vector<vector<double>> Raster :: OutflowInflow(int t) const {
    double k = 0.3;
    vector<vector<double>> flood(rows , vector<double>(cols , 0.0));
    auto fa = FloodDepthGrid(t);
    for(int i = 0 ; i<rows ; i++){
        for(int j = 0 ; j<cols ; j++){
            int currentDepth = fa[i][j];
            int bestdir = getFlowDirection(i , j);
            pair<int , int>p = nextCell(i , j , bestdir);
           
            int outflow = currentDepth*k;
            flood[i][j] += currentDepth - outflow;
            flood[p.first][p.second] += outflow; 
        }
    }
    return flood;
}