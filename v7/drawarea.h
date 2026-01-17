#pragma once

#include <QWidget>
#include <QTimer>

#include "Context.h"
#include "Vec2.h"

class DrawArea : public QWidget
{
    Q_OBJECT

public:
    explicit DrawArea(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void animate();

private:
    QPointF worldToScreen(const Vec2& p) const;
    float metersToPixels(float m) const;

private:
    Context ctx_;
    QTimer timer_;
    float dt_ = 1.f / 240.f;

    float pixelsPerMeter_ = 200.f;
    float marginPx_ = 30.f;
    bool estEnTrainDeViser_ = false;
    Vec2 positionSouris;
    int indexBilleBlanche_ = -1;
};
