#include<iostream>
#include<utility>
#include<fstream>
#include "raster.h"
using namespace std;
inline int getDirections(int di , int dj){

    if (di == 0 && dj == 1) return 0;   // E
    if (di == 1 && dj == 1) return 1;   // SE
    if (di == 1 && dj == 0) return 2;   // S
    if (di == 1 && dj == -1) return 3;  // SW
    if (di == 0 && dj == -1) return 4;  // W
    if (di == -1 && dj == -1) return 5; // NW
    if (di == -1 && dj == 0) return 6;  // N
    if (di == -1 && dj == 1) return 7;  // NE
    return -1; 


}
int Raster :: getFlowDirection(int i , int j) const{
 
int inputVal = grid[i][j];
auto neigh = getNeighbours(i , j);
int lowVal = inputVal;
int bestDir = -1;
for(auto &p : neigh){
    int ni = p.first;
    int nj = p.second;
    if(grid[ni][nj] < lowVal){
         lowVal = grid[ni][nj];
        bestDir = getDirections(ni-i , nj-j);
    }
}
return bestDir;
}

vector<pair<int,int>> Raster::traceFlowPath(
    int i, int j,
    vector<vector<bool>>& visited
) const {

    vector<pair<int,int>> path;

    
    if(i < 0 || i >= rows || j < 0 || j >= cols)
        return path;

    
    if(visited[i][j])
        return path;

    visited[i][j] = true;
    path.push_back({i, j});

    int dir = getFlowDirection(i, j);
    if(dir == -1)
        return path;

    auto nxt = nextCell(i, j, dir);

    auto rest = traceFlowPath(nxt.first, nxt.second, visited);
    path.insert(path.end(), rest.begin(), rest.end());

    return path;
}
void Raster::exportFlowPathCSV(
    const string& filename,
    int si, int sj
) const {

    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    auto path = traceFlowPath(si, sj, visited);

    ofstream file(filename);
    if(!file.is_open()){
        cout << "Error opening flow path CSV\n";
        return;
    }

    file << "x,y,order\n";

    for(int k = 0; k < path.size(); k++){
        int i = path[k].first;
        int j = path[k].second;

        double x = j + 0.5;
        double y = -(i + 0.5);

        file << x << "," << y << "," << k << "\n";
    }

    file.close();
    cout << "Flow path CSV written: " << filename << endl;
}
