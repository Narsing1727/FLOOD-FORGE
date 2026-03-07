#ifndef FLOODENGINECLIENT_H
#define FLOODENGINECLIENT_H

#include <vector>
#include <QLibrary>

class FloodEngineClient
{
public:
    FloodEngineClient();
    ~FloodEngineClient();

    bool init();
    void loadDEM(const std::vector<std::vector<float>>& dem);
    std::vector<std::vector<float>> predictFlood(double rainfall);
    bool isReady() const;

private:
    QLibrary engineLib;
    void* engineHandle = nullptr;

    // ===== DLL function pointer types =====
    typedef void* (*createEngine_t)();
    typedef void  (*destroyEngine_t)(void*);
    typedef void  (*setDEM_t)(void*, const double*, int, int);
    typedef void  (*setRainfall_t)(void*, double);
    typedef void  (*runFlood_t)(void*);
    typedef const double* (*getFloodGrid_t)(void*);
    typedef int (*getRows_t)(void*);
    typedef int (*getCols_t)(void*);

    createEngine_t  createEngine  = nullptr;
    destroyEngine_t destroyEngine = nullptr;
    setDEM_t        setDEM        = nullptr;
    setRainfall_t   setRainfall   = nullptr;
    runFlood_t      runFlood      = nullptr;
    getFloodGrid_t  getFloodGrid  = nullptr;
    getRows_t       getRows       = nullptr;
    getCols_t       getCols       = nullptr;

    int rows = 0;
    int cols = 0;
};

#endif
