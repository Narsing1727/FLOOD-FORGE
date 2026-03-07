#include "FloodEngineClient.h"
#include <QDebug>

FloodEngineClient::FloodEngineClient() {}

FloodEngineClient::~FloodEngineClient()
{
    if (engineHandle && destroyEngine)
        destroyEngine(engineHandle);

    if (engineLib.isLoaded())
        engineLib.unload();
}

bool FloodEngineClient::init()
{
    engineLib.setFileName("FloodEngine"); // FloodEngine.dll

    if (!engineLib.load()) {
        qDebug() << "DLL load failed:" << engineLib.errorString();
        return false;
    }

    createEngine  = (createEngine_t)engineLib.resolve("createEngine");
    destroyEngine = (destroyEngine_t)engineLib.resolve("destroyEngine");
    setDEM        = (setDEM_t)engineLib.resolve("setDEM");
    setRainfall   = (setRainfall_t)engineLib.resolve("setRainfall");
    runFlood      = (runFlood_t)engineLib.resolve("runFlood");
    getFloodGrid  = (getFloodGrid_t)engineLib.resolve("getFloodGrid");
    getRows       = (getRows_t)engineLib.resolve("getRows");
    getCols       = (getCols_t)engineLib.resolve("getCols");

    if (!createEngine || !setDEM || !runFlood || !getFloodGrid) {
        qDebug() << "❌ DLL symbols missing";
        return false;
    }

    engineHandle = createEngine();
    qDebug() << "✅ FloodEngine initialized";
    return true;
}

bool FloodEngineClient::isReady() const
{
    return engineHandle &&
           setDEM &&
           setRainfall &&
           runFlood &&
           getFloodGrid;
}

void FloodEngineClient::loadDEM(
    const std::vector<std::vector<float>>& dem)
{
    if (!engineHandle) return;

    rows = dem.size();
    cols = dem[0].size();

    std::vector<double> flat(rows * cols);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            flat[i * cols + j] = dem[i][j];

    setDEM(engineHandle, flat.data(), rows, cols);
}

std::vector<std::vector<float>>
FloodEngineClient::predictFlood(double rainfall)
{
    std::vector<std::vector<float>> grid;

    if (!engineHandle) return grid;

    setRainfall(engineHandle, rainfall);
    runFlood(engineHandle);

    int r = getRows(engineHandle);
    int c = getCols(engineHandle);

    const double* flat = getFloodGrid(engineHandle);

    grid.assign(r, std::vector<float>(c));

    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            grid[i][j] = static_cast<float>(flat[i * c + j]);

    return grid;
}
