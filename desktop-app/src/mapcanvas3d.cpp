#include "mapcanvas3d.h"
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtMath>
#include <algorithm>
#include <cmath>

MapCanvas3D::MapCanvas3D(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(400, 400);
}

MapCanvas3D::~MapCanvas3D()
{
    makeCurrent();
    vbo.destroy();
    vao.destroy();
    delete program;
    doneCurrent();
}

/* ================= INITIALIZATION ================= */

void MapCanvas3D::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Disable culling to see both sides of triangles
    // This fixes gaps caused by inconsistent winding
    glDisable(GL_CULL_FACE);

    program = new QOpenGLShaderProgram(this);

    // Vertex shader source
    const char* vertSource = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fragNormal;
out float fragHeight;
out vec3 fragWorldPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    fragWorldPos = worldPos.xyz;
    fragHeight = position.y;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    fragNormal = normalize(normalMatrix * normal);

    gl_Position = projection * view * worldPos;
}
)";

    // Fragment shader source
    const char* fragSource = R"(
#version 330 core
in vec3 fragNormal;
in float fragHeight;
in vec3 fragWorldPos;

out vec4 FragColor;

uniform vec3 lightDir;
uniform float minHeight;
uniform float maxHeight;

void main()
{
    vec3 normal = normalize(fragNormal);

    // 🥇 SLOPE-BASED COLORING (GAME CHANGER!)
    // Calculate slope: 0 = flat, 1 = vertical
    float slope = 1.0 - abs(dot(normal, vec3(0, 1, 0)));

    // Normalize height to 0-1 range
    float heightNorm = (fragHeight - minHeight) / max(maxHeight - minHeight, 0.001);

    // Base terrain colors
    vec3 grassColor = vec3(0.25, 0.55, 0.25);  // Rich green
    vec3 dirtColor = vec3(0.45, 0.35, 0.25);   // Brown dirt
    vec3 rockColor = vec3(0.50, 0.48, 0.45);   // Gray rock
    vec3 snowColor = vec3(0.95, 0.95, 0.98);   // White snow

    // 🎨 Height-based base color
    vec3 baseColor;
    if (heightNorm < 0.3) {
        // Low elevation: grass to dirt
        float t = heightNorm / 0.3;
        baseColor = mix(grassColor, dirtColor, t);
    } else if (heightNorm < 0.6) {
        // Mid elevation: dirt to rock
        float t = (heightNorm - 0.3) / 0.3;
        baseColor = mix(dirtColor, rockColor, t);
    } else if (heightNorm < 0.85) {
        // High elevation: rock to snow
        float t = (heightNorm - 0.6) / 0.25;
        baseColor = mix(rockColor, snowColor, t);
    } else {
        // Very high: pure snow
        baseColor = snowColor;
    }

    // 🏔️ SLOPE OVERRIDE: Steep areas = rock (regardless of height)
    // This is the secret sauce for realistic terrain!
    if (slope > 0.5) {
        // Steep slope: force rocky appearance
        float steepness = (slope - 0.5) / 0.5; // 0 to 1
        baseColor = mix(baseColor, rockColor, steepness * 0.7);
    } else if (slope < 0.2 && heightNorm < 0.5) {
        // Flat low areas: more grass
        float flatness = 1.0 - (slope / 0.2);
        baseColor = mix(baseColor, grassColor, flatness * 0.4);
    }

    // ☀️ ENHANCED LIGHTING with ambient occlusion approximation
    vec3 L = normalize(lightDir);
    float diffuse = max(dot(normal, L), 0.0);

    // Fake ambient occlusion from slope (valleys darker)
    float ao = 1.0 - (slope * 0.3);

    // Add subtle specular on steep wet rocks
    vec3 viewDir = normalize(-fragWorldPos);
    vec3 halfDir = normalize(L + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    float specular = spec * slope * 0.15; // Only on steep slopes

    float ambient = 0.35;
    float lighting = (ambient + diffuse * 0.65) * ao + specular;

    vec3 litColor = baseColor * lighting;

    // 🌫️ EXPONENTIAL FOG for depth and atmosphere
    float dist = length(fragWorldPos);
    float fogDensity = 0.0008;
    float fogFactor = exp(-dist * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 fogColor = vec3(0.5, 0.6, 0.7); // Atmospheric blue-gray
    vec3 finalColor = mix(fogColor, litColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
)";

    qDebug() << "Compiling shaders...";

    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertSource)) {
        qCritical() << "Vertex shader error:" << program->log();
        return;
    }
    qDebug() << "Vertex shader compiled";

    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource)) {
        qCritical() << "Fragment shader error:" << program->log();
        return;
    }
    qDebug() << "Fragment shader compiled";

    if (!program->link()) {
        qCritical() << "Shader linking error:" << program->log();
        return;
    }
    qDebug() << "Shaders linked successfully";

    // Setup VAO/VBO
    vao.create();
    vao.bind();

    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    program->bind();

    // Position attribute
    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, pos), 3, sizeof(Vertex));

    // Normal attribute
    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

    program->release();
    vbo.release();
    vao.release();
}

/* ================= DEM PROCESSING ================= */

void MapCanvas3D::smoothDEM(int iterations)
{
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<std::vector<float>> smoothed = demGrid;

        for (int z = 1; z < demRows - 1; ++z) {
            for (int x = 1; x < demCols - 1; ++x) {
                smoothed[z][x] = (
                                     demGrid[z][x] +
                                     demGrid[z-1][x] + demGrid[z+1][x] +
                                     demGrid[z][x-1] + demGrid[z][x+1]
                                     ) / 5.0f;
            }
        }
        demGrid = smoothed;
    }
}

QVector3D MapCanvas3D::calculateNormal(int x, int z)
{
    float heightL = (x > 0) ? demGrid[z][x-1] : demGrid[z][x];
    float heightR = (x < demCols-1) ? demGrid[z][x+1] : demGrid[z][x];
    float heightD = (z > 0) ? demGrid[z-1][x] : demGrid[z][x];
    float heightU = (z < demRows-1) ? demGrid[z+1][x] : demGrid[z][x];

    QVector3D normal(heightL - heightR, 2.0f, heightD - heightU);
    return normal.normalized();
}

void MapCanvas3D::generateTerrainMesh()
{
    std::vector<Vertex> vertices;
    vertices.reserve((demRows - 1) * (demCols - 1) * 6);

    float heightRange = maxHeight - minHeight;
    if (heightRange < 0.001f) heightRange = 1.0f;

    // Adaptive height scaling
    float heightScale;
    if (heightRange < 50.0f) {
        heightScale = 5.0f;
    } else if (heightRange < 200.0f) {
        heightScale = 1.5f;
    } else {
        heightScale = 0.5f;
    }

    float horizontalScale = 100.0f / std::max(demRows, demCols);

    // Track actual vertex height range
    float minY = 1e9f, maxY = -1e9f;

    for (int z = 0; z < demRows - 1; ++z) {
        for (int x = 0; x < demCols - 1; ++x) {
            float h00 = (demGrid[z][x] - minHeight) * heightScale;
            float h10 = (demGrid[z][x+1] - minHeight) * heightScale;
            float h01 = (demGrid[z+1][x] - minHeight) * heightScale;
            float h11 = (demGrid[z+1][x+1] - minHeight) * heightScale;

            minY = std::min({minY, h00, h10, h01, h11});
            maxY = std::max({maxY, h00, h10, h01, h11});

            float fx0 = (x - demCols/2.0f) * horizontalScale;
            float fx1 = (x+1 - demCols/2.0f) * horizontalScale;
            float fz0 = (z - demRows/2.0f) * horizontalScale;
            float fz1 = (z+1 - demRows/2.0f) * horizontalScale;

            QVector3D p00(fx0, h00, fz0);
            QVector3D p10(fx1, h10, fz0);
            QVector3D p01(fx0, h01, fz1);
            QVector3D p11(fx1, h11, fz1);

            QVector3D n00 = calculateNormal(x, z);
            QVector3D n10 = calculateNormal(x+1, z);
            QVector3D n01 = calculateNormal(x, z+1);
            QVector3D n11 = calculateNormal(x+1, z+1);

            // First triangle (counter-clockwise winding)
            vertices.push_back({p00, n00});
            vertices.push_back({p10, n10});
            vertices.push_back({p01, n01});

            // Second triangle (counter-clockwise winding)
            vertices.push_back({p10, n10});
            vertices.push_back({p11, n11});
            vertices.push_back({p01, n01});
        }
    }

    vertexCount = vertices.size();

    // Store scaled heights for shader
    minHeight = minY;
    maxHeight = maxY;

    qDebug() << "Mesh generated:" << vertexCount << "vertices";
    qDebug() << "Vertex Y range:" << minHeight << "to" << maxHeight;

    vao.bind();
    vbo.bind();
    vbo.allocate(vertices.data(), vertexCount * sizeof(Vertex));
    vbo.release();
    vao.release();
}

void MapCanvas3D::setDEM(const std::vector<std::vector<float>>& dem)
{
    if (dem.empty() || dem[0].empty()) {
        qWarning() << "Empty DEM";
        return;
    }

    demGrid = dem;
    demRows = demGrid.size();
    demCols = demGrid[0].size();

    minHeight = 1e9f;
    maxHeight = -1e9f;

    for (const auto& row : demGrid) {
        for (float h : row) {
            if (std::isfinite(h)) {
                minHeight = std::min(minHeight, h);
                maxHeight = std::max(maxHeight, h);
            }
        }
    }

    for (auto& row : demGrid) {
        for (float& h : row) {
            if (!std::isfinite(h)) h = minHeight;
        }
    }

    qDebug() << "DEM loaded:" << demCols << "x" << demRows;
    qDebug() << "Original height range:" << minHeight << "-" << maxHeight;

    // Increase smoothing iterations for less spiky terrain
    // 0 = raw data, 1-2 = slight smoothing, 3-5 = very smooth
    smoothDEM(3);
    generateTerrainMesh();

    update();
}

/* ================= RENDERING ================= */

void MapCanvas3D::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void MapCanvas3D::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (vertexCount == 0 || !program) return;

    program->bind();
    vao.bind();

    QMatrix4x4 proj;
    proj.perspective(45.0f, float(width())/float(height()), 0.1f, 10000.0f);

    float yawRad = qDegreesToRadians(yaw);
    float pitchRad = qDegreesToRadians(pitch);

    QVector3D camPos(
        distance * qCos(pitchRad) * qCos(yawRad),
        distance * qSin(pitchRad),
        distance * qCos(pitchRad) * qSin(yawRad)
        );

    QMatrix4x4 view;
    view.lookAt(camPos + targetOffset, targetOffset, QVector3D(0,1,0));

    QMatrix4x4 model;
    model.setToIdentity();

    program->setUniformValue("projection", proj);
    program->setUniformValue("view", view);
    program->setUniformValue("model", model);
    program->setUniformValue("lightDir", QVector3D(0.5f, 0.8f, 0.3f).normalized());
    program->setUniformValue("minHeight", minHeight);
    program->setUniformValue("maxHeight", maxHeight);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    vao.release();
    program->release();
}

/* ================= INPUT ================= */

void MapCanvas3D::mousePressEvent(QMouseEvent* e)
{
    lastMousePos = e->pos();
}

void MapCanvas3D::mouseMoveEvent(QMouseEvent* e)
{
    QPoint delta = e->pos() - lastMousePos;
    lastMousePos = e->pos();

    if (e->buttons() & Qt::LeftButton) {
        // Left button: rotate
        yaw += delta.x() * 0.3f;
        pitch += delta.y() * 0.3f;

        // Allow full rotation: -89 to +89 degrees
        // Negative = looking down, Positive = looking up
        pitch = qBound(-89.0f, pitch, 89.0f);
        update();
    }
    else if (e->buttons() & Qt::RightButton) {
        // Right button: pan
        float panSpeed = distance * 0.002f;
        QVector3D right(qCos(qDegreesToRadians(yaw)), 0, qSin(qDegreesToRadians(yaw)));
        QVector3D up(0, 1, 0);
        targetOffset -= right * delta.x() * panSpeed;
        targetOffset += up * delta.y() * panSpeed;
        update();
    }
}

void MapCanvas3D::wheelEvent(QWheelEvent* e)
{
    distance -= e->angleDelta().y() * 0.1f;
    distance = qBound(10.0f, distance, 2000.0f);
    update();
}

void MapCanvas3D::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_R) {
        // Reset camera
        yaw = -90.0f;
        pitch = -30.0f;  // Start looking down at 30 degrees
        distance = 200.0f;
        targetOffset = QVector3D(0, 0, 0);
        qDebug() << "Camera reset";
        update();
    }
}
