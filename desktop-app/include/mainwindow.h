#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "floodengineclient.h"
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "mapcanvas2d.h"
#include "mapcanvas3d.h"
#include <QDateTime>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
       void log(const QString& msg);
    std::vector<std::vector<float>>demGrid;
       int demRows =0;
    int demCols =0;
           QNetworkAccessManager* netManager;
    void loadGeoTiff(const QString& tifPath);

           void saveGridAsCSV(
               const QString& csvPath,
        const std::vector<std::vector<float>>& grid);
    MapCanvas2D* mapCanvas;
           std::vector<std::vector<float>> floodGrid;
    void saveFloodMetadata(
        const QString& jsonPath,
        double rainfall,
        const std::vector<std::vector<float>>& floodGrid);
    bool showDEM = false;
           bool demReady = false;
           //rain state
           bool rainReady = false;
           QDateTime rainStart;
           QDateTime rainEnd;
           double avgRainfall = 0.0;
           double GminLat;
            double GmaxLat;
            double GminLon;
             double GmaxLon;
            QString currentSessionPrefix;
            //Rainfall api
             void fetchRainfallFromAPI();
             void handleRainfallReply(QNetworkReply* reply);
             FloodEngineClient engine;
             // 3d logic
             MapCanvas3D* mapCanvas3D = nullptr;
private slots:

             void onCellClicked(int row, int col);
    void on_loadDemAction_triggered();
    void on_generateDemAction_triggered();
    void on_loadFloodAction_triggered();
    // void on_DEM_CHECKBOX_checkStateChanged();
    void startDemGeneration(double minLat, double maxLat,
                            double minLon, double maxLon);
    void on_DEM_CHECKBOX_checkStateChanged(Qt::CheckState state);

    void onRainSelected(QDateTime start, QDateTime end);
    void on_actionFetchRain_triggered();
 void on_FLOOD_CHECKBOX_checkStateChanged(Qt::CheckState state);
        void runSimulation();
 void uploadToAzure(const QString& filePath , const QString &blobPath);
        void generateAIAlert(
            double rainfall,
            double maxFlood,
            int low,
            int medium,
            int high);
void handleAIReply(QNetworkReply* reply);
        void generateImpactReport(
            double rainfall,
            double maxFlood,
            int low,
            int medium,
            int high);
// enum LogSection
// {
//     DEM,
//     RAIN,
//     SIM,
//     AI,
//     SYSTEM
// };
};
#endif // MAINWINDOW_H
