#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QPoint>
#include <vector>

class MapCanvas3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MapCanvas3D(QWidget* parent = nullptr);
    ~MapCanvas3D();
    void setDEM(const std::vector<std::vector<float>>& dem);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    struct Vertex {
        QVector3D pos;
        QVector3D normal;
    };

    void generateTerrainMesh();
    QVector3D calculateNormal(int x, int z);
    void smoothDEM(int iterations = 1);

    std::vector<std::vector<float>> demGrid;
    QOpenGLBuffer vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram* program = nullptr;

    int vertexCount = 0;

    // Terrain dimensions
    int demRows = 0;
    int demCols = 0;
    float minHeight = 0.0f;
    float maxHeight = 0.0f;

    // Camera controls
    float yaw = -90.0f;      // Horizontal rotation (left/right)
    float pitch = -30.0f;    // Vertical rotation (up/down) - start at 30° down
    float distance = 200.0f; // Distance from target
    QVector3D targetOffset = QVector3D(0, 0, 0);  // For panning
    QPoint lastMousePos;
};
