#include "mapcanvas2d.h"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QImage>
#include <QPixmap>
#include <QWheelEvent>
#include <QMouseEvent>
MapCanvas2D::MapCanvas2D(QWidget* parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QColor(25,25,25));
}

void MapCanvas2D::setDEM(const std::vector<std::vector<float>>& dem)
{
    demGrid = dem;
    renderDEM();
}
void MapCanvas2D::renderDEM()
{
    scene->clear();

    if (demGrid.empty()) return;

    int rows = demGrid.size();
    int cols = demGrid[0].size();

    float minE = 1e9, maxE = -1e9;
    for (auto& r : demGrid)
        for (float v : r) {
            minE = std::min(minE, v);
            maxE = std::max(maxE, v);
        }

    if (maxE - minE < 0.0001f)
        maxE = minE + 1.0f;

    QImage img(cols, rows, QImage::Format_ARGB32);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            float norm = (demGrid[i][j] - minE) / (maxE - minE);

            QColor color;
            if (norm < 0.4)
                color = QColor(120, 85, 60);
            else if (norm < 0.7)
                color = QColor(90, 60, 40);
            else
                color = QColor(30, 30, 30);

            img.setPixelColor(j, i, color);
        }
    }

    if (showFlood && !floodGrid.empty()) {

        float maxD = 0;
        for (auto& r : floodGrid)
            for (float d : r)
                maxD = std::max(maxD, d);

        if (maxD > 0) {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {

                    float d = floodGrid[i][j];
                    if (d <= 0) continue;
                    float norm = d / maxD;

                    QColor water;

                    if (norm < 0.3)
                        water = QColor(30, 144, 255, 150);   // Blue → Low flood
                    else if (norm < 0.6)
                        water = QColor(255, 165, 0, 170);    // Orange → Medium
                    else
                        water = QColor(255, 0, 0, 200);      // Red → Severe

                    int alpha = water.alpha();
                    QColor base = img.pixelColor(j, i);

                    // simple blend
                    int r = (base.red()   * (255 - alpha) + water.red()   * alpha) / 255;
                    int g = (base.green() * (255 - alpha) + water.green() * alpha) / 255;
                    int b = (base.blue()  * (255 - alpha) + water.blue()  * alpha) / 255;

                    img.setPixelColor(j, i, QColor(r, g, b));
                }
            }
        }
    }

  QPixmap pix = QPixmap::fromImage(img);

    scene->addPixmap(pix);

    scene->setSceneRect(pix.rect());
fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}
void MapCanvas2D::wheelEvent(QWheelEvent* event)
{
    const double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0)
        scale(scaleFactor, scaleFactor);
    else
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}
void MapCanvas2D::setFlood(const std::vector<std::vector<float>>& flood){
    floodGrid = flood;
    showFlood = true;
    renderDEM();
}
void MapCanvas2D::clearDEM()
{
    demGrid.clear();
    hasDEM = false;
    scene->clear();
}
void MapCanvas2D::clearFlood()
{
    floodGrid.clear();
    showFlood = false;
    renderDEM();
}
void MapCanvas2D::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    if (!scene->sceneRect().isEmpty())
        fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}
bool MapCanvas2D::saveRiskMap(const QString& path)
{
    if (!scene)
        return false;

    QRectF rect = scene->sceneRect();

    QImage image(rect.size().toSize(),
                 QImage::Format_ARGB32);

    image.fill(Qt::transparent);

    QPainter painter(&image);

    scene->render(&painter);

    painter.end();

    return image.save(path);
}
void MapCanvas2D::mousePressEvent(QMouseEvent *event)
{
    if (demGrid.empty())
        return;

    QPointF scenePos = mapToScene(event->pos());

    int col = scenePos.x();
    int row = scenePos.y();

    if(row >= 0 && row < demGrid.size() &&
        col >= 0 && col < demGrid[0].size())
    {
        emit cellClicked(row, col);
    }

     QGraphicsView::mousePressEvent(event);
}
