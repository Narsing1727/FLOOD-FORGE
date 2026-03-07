#ifndef MAPCANVAS2D_H
#define MAPCANVAS2D_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <vector>

class MapCanvas2D : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MapCanvas2D(QWidget* parent = nullptr);
    void setFlood(const std::vector<std::vector<float>>& flood);
    void setDEM(const std::vector<std::vector<float>>& dem);
      void clearDEM();
    void clearFlood();
      bool saveRiskMap(const QString& path);
     void  mousePressEvent(QMouseEvent *event) override;
  signals:
      void cellClicked(int row, int col);
protected:
    void wheelEvent(QWheelEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    QGraphicsScene* scene;
    std::vector<std::vector<float>> demGrid;
    std::vector<std::vector<float>> floodGrid;
    bool showFlood = false;
    void renderDEM();
    bool hasDEM = false;

};

#endif
