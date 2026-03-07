#include "mainwindow.h"
#include <gdal_priv.h>
#include <cpl_conv.h>
#include <vector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QIcon>
#include <QTextStream>
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <limits>
#include "dialog.h"
#include "rainfall_dialog.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QFile>
#include"mapcanvas2d.h"

#include"intro_window.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    connect(ui->runButton, &QAction::triggered,
            this, &MainWindow::runSimulation);
    netManager = new QNetworkAccessManager(this);
    // uploadToAzure("D:/test.txt");
    mapCanvas = new MapCanvas2D(this);
    mapCanvas3D  = new MapCanvas3D(this);
    QVBoxLayout* layout = new QVBoxLayout(ui->map2DWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mapCanvas);
    connect(mapCanvas,
            &MapCanvas2D::cellClicked,
            this,
            &MainWindow::onCellClicked);
    QVBoxLayout* layout3D = new QVBoxLayout(ui->map3DWidget);
    layout3D->setContentsMargins(0, 0, 0, 0);
    layout3D->addWidget(mapCanvas3D);
mapCanvas->setFrameShape(QFrame::NoFrame);

    bool ok = engine.init();
    if (!ok) {
        // log("❌ FloodEngine DLL init failed");
    }

}

MainWindow::~MainWindow()
{

    delete ui;

}

void MainWindow::log(const QString& msg)
{
    QString colored = msg;

    if (msg.contains("❌"))
        colored = "<span style='color:red;'>" + msg + "</span>";
    else if (msg.contains("⚠"))
        colored = "<span style='color:orange;'>" + msg + "</span>";
    else if (msg.contains("AI"))
        colored = "<span style='color:#b84cff;'>" + msg + "</span>";
    else if (msg.contains("DEM"))
        colored = "<span style='color:#00bcd4;'>" + msg + "</span>";
    else if (msg.contains("Rain"))
        colored = "<span style='color:#4caf50;'>" + msg + "</span>";
    else
        colored = "<span style='color:white;'>" + msg + "</span>";

    ui->consoleTextEdit->appendHtml(colored);
}
void MainWindow::on_loadDemAction_triggered()
{
    QString filepath = QFileDialog :: getOpenFileName(
        this,
        "Load DEM CSV",
        "",
        "CSV Files(*.csv)"
        );
    if(filepath.isEmpty()){
          return;
}

    QFile file(filepath);
if(!file.open(QIODevice::ReadOnly | QIODevice :: Text)){
        ui->consoleTextEdit->appendPlainText("DEM file open failed");
    return;
}
QTextStream in(&file);
int rows =0;
int cols =0;
double minElev = std::numeric_limits<double>::max();
double maxElev = std::numeric_limits<double>::lowest();
demGrid.clear();
while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    if (line.isEmpty()) continue;

    QStringList values = line.split(",", Qt::SkipEmptyParts);
    std::vector<float>row;
    if (rows == 0)
        cols = values.size();

    for (const QString& v : values) {
        double elev = v.toDouble();
        row.push_back(v.toFloat());
        minElev = std::min(minElev, elev);
        maxElev = std::max(maxElev, elev);

    }
    demGrid.push_back(row);
    rows++;
}
demRows = demGrid.size();
demCols = demGrid[0].size();
file.close();
log(" DEM Loaded");
ui->layersListWidget->addItem("DEM (CSV)");
// log("Rows: " + QString::number(rows));
// log("Cols: " + QString::number(cols));
// log("Min Elev: " + QString::number(minElev));
// log("Max Elev: " + QString::number(maxElev));
if(showDEM){
    mapCanvas->setDEM(demGrid);
    mapCanvas3D->setDEM(demGrid);

}

}
void MainWindow::saveGridAsCSV(
    const QString& csvPath,
    const std::vector<std::vector<float>>& grid)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // log(" CSV save failed");
        return;
    }

    QTextStream out(&file);

    for (const auto& row : grid) {
        for (int j = 0; j < row.size(); j++) {
            out << row[j];
            if (j != row.size() - 1)
                out << ",";
        }
        out << "\n";
    }

    file.close();
    // log(" CSV saved: " + csvPath);
}



void MainWindow::on_generateDemAction_triggered()
{
    Dialog dlg(this);

    connect(&dlg, &Dialog::generateRequested,
            this, &MainWindow::startDemGeneration);

    dlg.exec();
}
void MainWindow::loadGeoTiff(const QString& tifPath)
{
    GDALAllRegister();

    GDALDataset* ds = (GDALDataset*)GDALOpen(
        tifPath.toStdString().c_str(),
        GA_ReadOnly
        );

    if (!ds) {
        // log(" GDAL failed to open GeoTIFF");
        return;
    }

    int width  = ds->GetRasterXSize();
    int height = ds->GetRasterYSize();

    GDALRasterBand* band = ds->GetRasterBand(1);

    std::vector<float> buffer(width * height);
    band->RasterIO(
        GF_Read,
        0, 0,
        width, height,
        buffer.data(),
        width, height,
        GDT_Float32,
        0, 0
        );

    demGrid.clear();
    demGrid.resize(height, std::vector<float>(width));

    float minE = 1e9, maxE = -1e9;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float e = buffer[i * width + j];
            demGrid[i][j] = e;
            minE = std::min(minE, e);
            maxE = std::max(maxE, e);
        }
    }

    GDALClose(ds);

    // log("GDAL GeoTIFF Loaded");
    log("Rows: " + QString::number(height));
    log("Cols: " + QString::number(width));
    log("Min Elev: " + QString::number(minE));
    log("Max Elev: " + QString::number(maxE));

    ui->layersListWidget->addItem("DEM (GDAL)");
    if (!engine.isReady()) {
        log("❌ Engine not ready, DEM not sent");
        return;
    }

    engine.loadDEM(demGrid);
    mapCanvas->setDEM(demGrid);
    mapCanvas3D->setDEM(demGrid);
    ui->DEM_CHECKBOX->setChecked(true);
    demReady = true;
}

void MainWindow::startDemGeneration(double minLat, double maxLat,
                                    double minLon, double maxLon)
{
    QString saveDir = QFileDialog::getExistingDirectory(this , "Select folder to save DEM");
    if(saveDir.isEmpty()) return;
    QString tifPath = saveDir + "/dem.tif";
    QString url =
        "https://portal.opentopography.org/API/globaldem?"
        "demtype=SRTMGL1"
        "&south=" + QString::number(minLat) +
        "&north=" + QString::number(maxLat) +
        "&west="  + QString::number(minLon) +
        "&east="  + QString::number(maxLon) +
        "&outputFormat=GTiff"
        "&API_Key=3a099296641fbf4a0cfdc1a990d08f3c";
    log("Dowloading DEM (GeoTiff)");
    GminLat = minLat;
    GmaxLat = maxLat;
    GminLon = minLon;
    GmaxLon = maxLon;
    QNetworkReply* reply =
        netManager->get(QNetworkRequest(QUrl(url)));

    connect(reply, &QNetworkReply::finished, this, [=]() {

        if (reply->error() != QNetworkReply::NoError) {
            ui->consoleTextEdit->appendPlainText(
                " Download failed: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QFile file(tifPath);
        if (!file.open(QIODevice::WriteOnly)) {
            ui->consoleTextEdit->appendPlainText("❌ Cannot save file");
            reply->deleteLater();
            return;
        }

        file.write(reply->readAll());
        file.close();

        ui->consoleTextEdit->appendPlainText(
            " GeoTIFF saved at: " + tifPath);

        loadGeoTiff(tifPath);

        QString csvPath = saveDir + "/dem.csv";
        saveGridAsCSV(csvPath, demGrid);


        // -------- CREATE SESSION FOLDER --------
        QString dateFolder =
            QDate::currentDate().toString("yyyy-MM-dd");

        QString sessionFolder =
            "session_" +
            QDateTime::currentDateTime().toString("HHmmss");

        currentSessionPrefix =
            "runs/" + dateFolder + "/" + sessionFolder + "/";

        // -------- UPLOAD DEM --------
        uploadToAzure(csvPath, currentSessionPrefix + "dem.csv");

        // log("DEM uploaded to Azure: " + currentSessionPrefix);
        if(showDEM){

            mapCanvas->setDEM(demGrid);
        mapCanvas3D->setDEM(demGrid);
        }

        reply->deleteLater();
    });
}
void MainWindow :: on_loadFloodAction_triggered()
{
    if (demGrid.empty()) {
        log(" Load DEM before loading flood data");
        return;
    }

    QString filepath = QFileDialog::getOpenFileName(
        this,
        "Load Flood CSV",
        "",
        "CSV Files (*.csv)"
        );

    if (filepath.isEmpty())
        return;

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log(" Flood CSV open failed");
        return;
    }

    QTextStream in(&file);

    floodGrid.clear();
    int rows = 0;
    int cols = demGrid[0].size();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList values = line.split(",", Qt::SkipEmptyParts);
        std::vector<float> row;

        if (values.size() != cols) {
            log(" Flood CSV column mismatch with DEM");
            return;
        }

        for (const QString& v : values)
            row.push_back(v.toFloat());

        floodGrid.push_back(row);
        rows++;
    }

    file.close();

    if (rows != demGrid.size()) {
        log(" Flood CSV row mismatch with DEM");
        floodGrid.clear();
        return;
    }

    log("Flood CSV Loaded");
    mapCanvas->setFlood(floodGrid);
}

void MainWindow::on_DEM_CHECKBOX_checkStateChanged(Qt::CheckState state)
{
    bool show = (state == Qt::Checked);

    if (show && !demGrid.empty()) {
        mapCanvas->setDEM(demGrid);
        mapCanvas3D->setDEM(demGrid);
    } else {
        mapCanvas->clearDEM();
        // 3D me abhi clear nahi, bas redraw mat kar
    }
}

void MainWindow::on_actionFetchRain_triggered() {
    if (!demReady) {
        log("⚠ Generate DEM first");
        return;
    }

    rainFall_Dialog dlg(this);
    connect(&dlg, &rainFall_Dialog::rainSelected,
            this, &MainWindow::onRainSelected);
    dlg.exec();
}

void MainWindow::onRainSelected(QDateTime start, QDateTime end)
{
    rainStart = start;
    rainEnd   = end;
    rainReady = true;
    // log("Rainfall time selected:");
    // log(start.toString(Qt::ISODate) + " → " +
    //     end.toString(Qt::ISODate));

    fetchRainfallFromAPI();

    //  Climate API abhi nahi
    //  Avg rainfall abhi nahi
}
void MainWindow::fetchRainfallFromAPI()
{
    // Center of DEM bbox
    double lat = (GminLat + GmaxLat) / 2.0;
    double lon = (GminLon + GmaxLon) / 2.0;

    QString startDate = rainStart.date().toString("yyyy-MM-dd");
    QString endDate   = rainEnd.date().toString("yyyy-MM-dd");

    QString url = QString(
                      "https://meteostat.p.rapidapi.com/point/daily?"
                      "lat=%1&lon=%2&start=%3&end=%4"
                      ).arg(lat)
                      .arg(lon)
                      .arg(startDate)
                      .arg(endDate);


    // log("Fetching rainfall from Meteostat (RapidAPI)...");
    // log(url);

    QNetworkRequest request{QUrl(url)};
    QString rapidKey = qEnvironmentVariable("RAPIDAPI_KEY");

    request.setRawHeader(
        "X-RapidAPI-Key",
        rapidKey.toUtf8());
    request.setRawHeader("X-RapidAPI-Host", "meteostat.p.rapidapi.com");

    QNetworkReply* reply = netManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleRainfallReply(reply);
    });

}
void MainWindow::handleRainfallReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        log("❌ Rainfall API failed: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();

    // log("RAW RESPONSE:");
    // log(QString(response));
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (!doc.isObject()) {
        log("❌ Invalid JSON from Meteostat API");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray data = root["data"].toArray();

    if (data.isEmpty()) {
        log("❌ No rainfall data received");
        return;
    }

    double sum = 0.0;
    int count = 0;

    for (const auto& v : data) {
        QJsonObject obj = v.toObject();

        // Meteostat monthly precipitation (mm)
        if (obj.contains("prcp") && !obj["prcp"].isNull()) {
            sum += obj["prcp"].toDouble();
            count++;
        }
    }

    if (count == 0) {
        log("❌ Rainfall values missing in API response");
        return;
    }
// log(response);
    avgRainfall = sum / count;   // mm per month (average)
    rainReady = true;

    log(QString("✅ Avg Monthly Rainfall = %1 mm")
            .arg(avgRainfall));
    // avgRainfall *= 30;
    // floodGrid = engine.predictFlood(avgRainfall);

    // QString outDir = QFileDialog::getExistingDirectory(
    //     this, "Select folder to save flood output");

    // if (outDir.isEmpty()) return;

    // QString floodCsv = outDir + "/flood_depth.csv";
    // QString metaJson = outDir + "/metadata.json";

    // saveGridAsCSV(floodCsv, floodGrid);
    // saveFloodMetadata(metaJson, avgRainfall, floodGrid);

    // // ---- ANALYTICS ----
    // double maxF = 0.0;
    // for (const auto& r : floodGrid)
    //     for (float v : r)
    //         maxF = std::max(maxF, (double)v);

    // log("Max Flood Depth = " + QString::number(maxF));

    // // ---- VISUALIZE ----
    // mapCanvas->setFlood(floodGrid);
    // ui->FLOOD_CHECKBOX->setChecked(true);
    double avg1 = avgRainfall;
    avgRainfall *= 30;
    // log(QString("Final Rainfall Used = %1 mm").arg(avgRainfall));
    ui->rainfallLabel->setText(
        "🌧 Avg Rainfall: " + QString::number(avg1, 'f', 2) + " mm");
    ui->avgrainfall->setText(
        "Total Used: " + QString::number(avgRainfall, 'f', 2) + " mm");

}
void MainWindow::on_FLOOD_CHECKBOX_checkStateChanged(Qt::CheckState state)
{
    bool show = (state == Qt::Checked);

    if (show && !floodGrid.empty()) {
        mapCanvas->setFlood(floodGrid);
    } else {
        mapCanvas->clearFlood();   // ye function add karenge
    }

    mapCanvas->update();
}
void MainWindow::saveFloodMetadata(
    const QString& jsonPath,
    double rainfall,
    const std::vector<std::vector<float>>& floodGrid)
{
    int rows = floodGrid.size();
    int cols = rows ? floodGrid[0].size() : 0;

    double maxFlood = 0.0;
    int low = 0, medium = 0, high = 0;


    for (const auto& r : floodGrid) {
        for (float v : r) {
            maxFlood = std::max(maxFlood, (double)v);
        }
    }
    if (maxFlood <= 0.0) maxFlood = 1.0;

    for (const auto& r : floodGrid) {
        for (float v : r) {
            double norm = v / maxFlood;   // 0 → 1

            if (norm < 0.3)
                low++;
            else if (norm < 0.6)
                medium++;
            else
                high++;
        }
    }

    QJsonObject meta;
    meta["rainfall_mm"] = rainfall;
    meta["rows"] = rows;
    meta["cols"] = cols;
    meta["max_flood_depth"] = maxFlood/1000.0;

    meta["severity_low_cells"] = low;
    meta["severity_medium_cells"] = medium;
    meta["severity_high_cells"] = high;

    meta["timestamp"] =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(meta);

    QFile file(jsonPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        log("❌ Metadata save failed");
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    log("✅ Metadata saved: " + jsonPath);
}void MainWindow::runSimulation()
{
    if (!demReady) {
        log("Load DEM first");
        return;
    }

    if (!rainReady) {
        log("Fetch rainfall first");
        return;
    }

    log("Running simulation...");
    statusBar()->showMessage("Running simulation...");

    auto future = QtConcurrent::run([=]() {
        return engine.predictFlood(avgRainfall);
    });

    auto* watcher =
        new QFutureWatcher<std::vector<std::vector<float>>>(this);

    connect(watcher,
            &QFutureWatcher<std::vector<std::vector<float>>>::finished,
            this,
            [=]() {

                floodGrid = watcher->result();

                mapCanvas->setFlood(floodGrid);
                ui->FLOOD_CHECKBOX->setChecked(true);

                QString dateFolder =
                    QDate::currentDate().toString("yyyy-MM-dd");

                QString sessionFolder =
                    "session_" +
                    QDateTime::currentDateTime().toString("HHmmss");

                QString basePrefix =
                    "runs/" + dateFolder + "/" + sessionFolder + "/";

                QString tempDir = QDir::tempPath();

                QString floodCsv =
                    tempDir + "/flood_depth.csv";

                QString metaJson =
                    tempDir + "/metadata.json";

                saveGridAsCSV(floodCsv, floodGrid);
                saveFloodMetadata(metaJson, avgRainfall, floodGrid);
                double maxFlood = 0.0;
                int low = 0, medium = 0, high = 0;

                for (const auto& r : floodGrid)
                {
                    for (float v : r)
                    {
                        maxFlood = std::max(maxFlood, (double)v);
                    }
                }

                if (maxFlood <= 0) maxFlood = 1;

                for (const auto& r : floodGrid)
                {
                    for (float v : r)
                    {
                        double norm = v / maxFlood;

                        if (norm < 0.3)
                            low++;
                        else if (norm < 0.6)
                            medium++;
                        else
                            high++;
                    }
                }
                generateAIAlert(avgRainfall, maxFlood, low, medium, high);
                generateImpactReport(
                    avgRainfall,
                    maxFlood,
                    low,
                    medium,
                    high);
                QString mapPath =
                    QDir::tempPath() + "/risk_map.png";
                // ---- UPDATE LAYERS PANEL WITH REAL STATS ----

                ui->layersListWidget->addItem("");

                QListWidgetItem *statsTitle = new QListWidgetItem("Simulation Stats");
                statsTitle->setForeground(QColor(200,200,200));
                statsTitle->setFlags(Qt::NoItemFlags);
                ui->layersListWidget->addItem(statsTitle);

                ui->layersListWidget->addItem(
                    "Rainfall Used: " + QString::number(avgRainfall, 'f', 2) + " mm");

                ui->layersListWidget->addItem(
                    "Max Flood Depth: " + QString::number(maxFlood/1000.0, 'f', 2) + " m");

                ui->layersListWidget->addItem(
                    "High Risk Cells: " + QString::number(high));

                ui->layersListWidget->addItem(
                    "Medium Risk Cells: " + QString::number(medium));

                ui->layersListWidget->addItem(
                    "Low Risk Cells: " + QString::number(low));
                if (mapCanvas->saveRiskMap(mapPath))
                {
                    log("Risk map exported to azure");

                    uploadToAzure(
                        mapPath,
                        currentSessionPrefix + "risk_map.png");
                }
                else
                {
                    log("Risk map export failed");
                }
                // log("Files saved locally (temp)");

                if (currentSessionPrefix.isEmpty()) {
                    log("Session not initialized (Generate DEM first)");
                    return;
                }

                uploadToAzure(floodCsv,
                              currentSessionPrefix + "flood_depth.csv");

                uploadToAzure(metaJson,
                              currentSessionPrefix + "metadata.json");

                QString tempDir1 = QDir::tempPath();

                QString mapImage =
                    tempDir1 + "/risk_map.png";

                QPixmap pix = mapCanvas->grab();

                pix.save(mapImage);
                uploadToAzure(
                    mapImage,
                    currentSessionPrefix + "risk_map.png");
                // log("Risk map image saved");
                statusBar()->showMessage(
                    "Simulation Completed & Uploaded");

                watcher->deleteLater();
            });

    QListWidgetItem *low = new QListWidgetItem("■ Low Risk");
    low->setForeground(QColor(0,120,255));

    QListWidgetItem *medium = new QListWidgetItem("■ Medium Risk");
    medium->setForeground(QColor(255,165,0));

    QListWidgetItem *high = new QListWidgetItem("■ High Risk");
    high->setForeground(QColor(255,0,0));

    QListWidgetItem *title = new QListWidgetItem("Risk Legend");
    title->setForeground(QColor(200,200,200));
    title->setFlags(Qt::NoItemFlags);
    ui->layersListWidget->addItem(title);
    ui->layersListWidget->addItem(low);
    ui->layersListWidget->addItem(medium);
    ui->layersListWidget->addItem(high);
    watcher->setFuture(future);
}
void MainWindow::uploadToAzure(
    const QString &filePath,
    const QString &blobPath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "File open failed";
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QString sasContainerUrl =
        qEnvironmentVariable("AZURE_BLOB_SAS");
    int index = sasContainerUrl.indexOf('?');
    QString baseUrl = sasContainerUrl.left(index);
    QString sasToken = sasContainerUrl.mid(index);

    QString fullUrl = baseUrl + "/" + blobPath + sasToken;

    qDebug() << "Uploading to:" << fullUrl;
QNetworkRequest request(QUrl::fromUserInput(fullUrl));
    request.setHeader(QNetworkRequest::ContentLengthHeader, fileData.size());
    request.setRawHeader("x-ms-blob-type", "BlockBlob");

    QNetworkReply *reply = netManager->put(request, fileData);

    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError)
            qDebug() << "Upload successful!";
        else
            qDebug() << "Upload failed:" << reply->errorString();

        reply->deleteLater();
    });
}
void MainWindow::generateAIAlert(
    double rainfall,
    double maxFlood,
    int low,
    int medium,
    int high)
{
    QString prompt =
        "You are a disaster management AI.\n"
        "Generate a short flood warning.\n\n"
        "Rainfall: " + QString::number(rainfall) + " mm\n"
                                      "Max Flood Depth: " + QString::number(maxFlood) + " m\n"
                                      "Low Severity Cells: " + QString::number(low) + "\n"
                                 "Medium Severity Cells: " + QString::number(medium) + "\n"
                                    "High Severity Cells: " + QString::number(high) + "\n\n"
                                  "Output format:\n"
                                  "⚠️ Flood Alert: <risk level>\n"
                                  "Water depth: <value>\n"
                                  "Recommended action: <short advice>";

    QUrl url("https://api.groq.com/openai/v1/chat/completions");

    QNetworkRequest request(url);

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json");

    QString apiKey = qEnvironmentVariable("GROQ_API_KEY");

    request.setRawHeader(
        "Authorization",
        ("Bearer " + apiKey).toUtf8());

    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;

    QJsonArray messages;
    messages.append(message);

    QJsonObject body;
    body["model"] = "llama-3.1-8b-instant";
    body["messages"] = messages;
    body["temperature"] = 0.3;

    QJsonDocument doc(body);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = netManager->post(request, data);

    connect(reply, &QNetworkReply::finished,
            this, [=]() {
                handleAIReply(reply);
            });

    // log("AI alert generation started...");
}
void MainWindow::handleAIReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        log("AI API failed: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(response);

    if (!doc.isObject())
    {
        log("Invalid AI JSON response");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray choices = root["choices"].toArray();

    if (choices.isEmpty())
    {
        log("AI returned empty response");
        return;
    }

    QJsonObject msg =
        choices[0].toObject()["message"].toObject();

    QString alertText = msg["content"].toString();

    log(" AI Flood Alert Generated:");
    QString alertBox =
        "<div style='border-left:4px solid #ff9800;"
        "background:#1e1e1e;padding:6px;margin:6px 0;'>"
        "<b style='color:#ff9800;'>⚠ AI FLOOD ALERT</b><br>" +
        alertText +
        "</div>";

    ui->consoleTextEdit->appendHtml(alertBox);

    QString alertPath =
        QDir::tempPath() + "/flood_alert.txt";

    QFile file(alertPath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << alertText;
        file.close();

        log("AI Alert and impact saved to azure storage");
    }

    uploadToAzure(
        alertPath,
        currentSessionPrefix + "flood_alert.txt");
}
void MainWindow::generateImpactReport(
    double rainfall,
    double maxFlood,
    int low,
    int medium,
    int high)
{
    QString prompt =
        "You are a flood simulation analyst for a research prototype.\n"
        "The data comes from a small DEM grid simulation, not a real city.\n"
        "Do NOT assume real world population or billions of damage.\n"
        "Only analyze the grid severity levels.\n\n"
        "Simulation data:\n"
        "Total accumulated rainfall of the area: " + QString::number(rainfall) + " mm\n"
                                      "Max simulated flood depth: " + QString::number(maxFlood/1000.0) + " m\n"
                                               "Low severity cells: " + QString::number(low) + "\n"
                                 "Medium severity cells: " + QString::number(medium) + "\n"
                                    "High severity cells: " + QString::number(high) + "\n\n"
                                  "Grid size: " + QString::number(demRows) + " x " + QString::number(demCols) + "\n"
                                  "Write a in detail technical impact summary including:\n"
                                  "- Flood severity distribution\n"
                                  "- Possible infrastructure stress\n"
                                  "- Suggested mitigation actions\n"
                                  "Do NOT invent population numbers or economic losses.\n"
                                  ;

    QUrl url("https://api.groq.com/openai/v1/chat/completions");

    QNetworkRequest request(url);

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json");
    QString apiKey = qEnvironmentVariable("GROQ_API_KEY");

    request.setRawHeader(
        "Authorization",
        ("Bearer " + apiKey).toUtf8());

    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;

    QJsonArray messages;
    messages.append(message);

    QJsonObject body;
    body["model"] = "llama-3.1-8b-instant";
    body["messages"] = messages;

    QJsonDocument doc(body);

    QNetworkReply* reply =
        netManager->post(request, doc.toJson());

    connect(reply,
            &QNetworkReply::finished,
            this,
            [=]()
            {
                QByteArray response = reply->readAll();
                reply->deleteLater();

                QJsonDocument doc =
                    QJsonDocument::fromJson(response);

                QJsonObject root = doc.object();
                QJsonArray choices =
                    root["choices"].toArray();

                QString text =
                    choices[0]
                        .toObject()["message"]
                        .toObject()["content"]
                        .toString();

                log("Impact report saved on azure");

                QString path =
                    QDir::tempPath() +
                    "/impact_report.txt";

                QFile file(path);

                if (file.open(QIODevice::WriteOnly))
                {
                    QTextStream out(&file);
                    out << text;
                    file.close();
                }

                uploadToAzure(
                    path,
                    currentSessionPrefix +
                        "impact_report.txt");
            });
}
void MainWindow::onCellClicked(int row, int col)
{
    if (demGrid.empty()) return;

    float elev = demGrid[row][col];
    float flood = 0;

    if (!floodGrid.empty())
        flood = floodGrid[row][col];

    QString msg =
        "<div style='background:#1e1e1e;"
        "border-left:4px solid #00bcd4;"
        "padding:6px;margin:4px 0;'>"
        "<b style='color:#00e5ff;'>CELL INFO</b><br>"
        "<span style='color:#bbbbbb;'>Row:</span> "
        "<span style='color:white;'>" + QString::number(row) + "</span><br>"
                                 "<span style='color:#bbbbbb;'>Col:</span> "
                                 "<span style='color:white;'>" + QString::number(col) + "</span><br>"
                                 "<span style='color:#bbbbbb;'>Elevation:</span> "
                                 "<span style='color:#4caf50;'>" + QString::number(elev) + " m</span><br>"
                                  "<span style='color:#bbbbbb;'>Flood Depth:</span> "
                                  "<span style='color:#ff9800;'>" + QString::number(flood/1000.0) + " m</span>"
                                            "</div>";

    ui->consoleTextEdit->appendHtml(msg);
}
