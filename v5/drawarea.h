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
};
