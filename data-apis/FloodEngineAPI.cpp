#define FLOODENGINE_EXPORTS
#include "FloodEngineAPI.h"
#include "raster.h"

struct FloodEngine {
    Raster raster;
    std::vector<double> flood;
    int rows = 0;
    int cols = 0;
    double rainfall = 0.0;
};

extern "C" {

void* createEngine() {
    return new FloodEngine();
}

void destroyEngine(void* e) {
    delete static_cast<FloodEngine*>(e);
}

void setDEM(void* e, const double* dem, int rows, int cols) {
    auto* engine = static_cast<FloodEngine*>(e);
    engine->rows = rows;
    engine->cols = cols;

    std::vector<std::vector<double>> grid(
        rows, std::vector<double>(cols));

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            grid[i][j] = dem[i * cols + j];

    engine->raster.loadFromGrid(grid);
}

void setRainfall(void* e, double rainfall) {
    static_cast<FloodEngine*>(e)->rainfall = rainfall;
}

void runFlood(void* e) {
    auto* engine = static_cast<FloodEngine*>(e);

    auto floodGrid = engine->raster.FloodDepthGridWithRain(
        0, engine->rainfall
    );

    engine->flood.clear();
    for (auto& row : floodGrid)
        for (double v : row)
            engine->flood.push_back(v);
}

const double* getFloodGrid(void* e) {
    return static_cast<FloodEngine*>(e)->flood.data();
}

int getRows(void* e) {
    return static_cast<FloodEngine*>(e)->rows;
}

int getCols(void* e) {
    return static_cast<FloodEngine*>(e)->cols;
}

}
