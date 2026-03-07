#include <iostream>
#include "raster.h"
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include "randomForest.h"
using namespace std;

int main() {
    
    cout << "Program started\n";

    Raster dem;
    RandomForest rf;
    if (!dem.loadCSVFile("dem3.csv")) {
        cout << "CSV load failed\n";
        return 1;
    }
    // if(!rf.loadModel("rf_model.txt")){
    //     cout<<"RF model load failed";
    //     cout<<endl;
    //     return 1;
    // }
int rows = dem.getRows();
int cols = dem.getCols();
auto grid = dem.gridReturn();
vector<vector<double>> mlDepth(rows, vector<double>(cols, 0.0));
vector<vector<double>> errorGrid(rows, vector<double>(cols, 0.0));

    // cout << "CSV loaded"<<endl;
    // cout << "Max value: " << dem.getMax() << endl;
    // cout << "Min value: " << dem.getMin() << endl;
    // cout << "Average value: " << dem.getAverage() << endl;
    // cout << "Number of Rows : " << dem.getRows() << endl;
    // cout << "Number of Columns : " << dem.getCols() << endl;
//     auto nbs = dem.getNeighbours(1,1);
//          for (auto &p : nbs) {
//                cout << "(" << p.first << "," << p.second << ") ";
// }
// int i = 1;
// int j = 1;
// cout << endl;
// int dir = dem.getFlowDirection(i,j);
// cout<<"For Cell ("<<i<<","<<j<<") Flow Direction is"<<" "<<dir<<endl;
// auto fa = dem.computeFlowAccumulation();

// for(int i = 0 ; i<dem.getRows() ; i++){
//     for(int j = 0 ; j<dem.getCols() ; j++){
//         cout<<fa[i][j]<<"\t";
//     }
//     cout<<endl;
// }

// auto rg = dem.RiverGrid();
// for(int i = 0 ; i<dem.getRows() ; i++){
//     for(int j = 0 ; j<dem.getCols() ; j++){
//          cout<<rg[i][j]<<" ";
//     }
//     cout<<endl;
// }

// for(int i = 0 ; i<5 ; i++){
//     auto fd = dem.FloodDepthGrid(i);
//     string filename =   "output/flood_" +
//         to_string(i).insert(0, 5 - to_string(i).length(), '0') +
//         ".csv";
//     dem.exportCSV(fd , filename);
// }

// for(int i = 0 ; i<5 ; i++){
//     dem.exportFloodStat("flood_stat.csv" , i);
// }
//            ofstream csv("flood_ml_dataset.csv");
// csv << "cell_id,i,j,t,rain_t,rain_t_1,flow_acc,elevation,depth_t1\n";
// for (int t = 0; t < 7; t++) {

//     double rain_t   = dem.rainTime(t);
//     double rain_t_1 = (t == 0) ? 0 : dem.rainTime(t-1);

//     auto depth_next = dem.FloodDepthGrid(t+1);
//     auto flow_acc = dem.computeFlowAccumulation();
//     for (int i = 0; i < rows; i++) {
//         for (int j = 0; j < cols; j++) {

//             int cell_id = i * cols + j;

// csv
//     << cell_id << ","
//     << i << ","
//     << j << ","
//     << t << ","
//     << rain_t << ","
//     << rain_t_1 << ","
//     << flow_acc[i][j] << ","
//     << grid[i][j] << ","
//     << depth_next[i][j]
//     << "\n";


//         }
//     }
// }
ifstream file("flood_ml_predictions.csv");
if(!file.is_open()){
    cout<<"ML csv not openend\n";
    return 1;
}
string line;
getline(file , line);
while(getline(file , line)){
    stringstream ss(line);
       int cell_id, i, j, t;
    double rain_t, rain_t_1, flow_acc, elevation;
    double depth_t1, ml_depth_t1;
    char comma;
        ss >> cell_id >> comma
       >> i >> comma
       >> j >> comma
       >> t >> comma
       >> rain_t >> comma
       >> rain_t_1 >> comma
       >> flow_acc >> comma
       >> elevation >> comma
       >> depth_t1 >> comma
       >> ml_depth_t1;

           mlDepth[i][j] = ml_depth_t1;
}
file.close();
// cout << "ML depth at (0,0): " << mlDepth[0][0] << endl;
// cout << "ML depth at (10,10): " << mlDepth[10][10] << endl;

auto physicsDepth = dem.FloodDepthGrid(6);
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        errorGrid[i][j] = mlDepth[i][j] - physicsDepth[i][j];
    }
}
ofstream err("error_grid.csv");
err << "i,j,error\n";

for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        err << i << "," << j << "," << errorGrid[i][j] << "\n";
    }
}

err.close();
//   int T;
//     cout << "Enter timestep T: ";
//     cin >> T;

//     auto mlFlood = dem.MLFloodDepthGrid(T, rf);


//     std::cout << rows << " " << cols << "\n";
//     for (int i = 0; i < rows; i++) {
//         for (int j = 0; j < cols; j++) {
//             cout << mlFlood[i][j] << " ";
//         }
//         cout << "\n";
//     }

rf.loadModel("models/rf_model.txt");
int T = 3;
auto phys = dem.FloodDepthGrid(T);
auto ml   = dem.MLFloodDepthGrid(T, rf);

std::vector<std::vector<double>> error(rows,
    std::vector<double>(cols, 0.0));

for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
        error[i][j] = ml[i][j] - phys[i][j];

dem.exportGridCSV(error, "error_grid_cpp.csv");

    return 0;
}
