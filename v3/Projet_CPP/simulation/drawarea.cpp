#include "drawarea.h"

#include <QPainter>
#include <algorithm>

#include "simulation/Particle.h"

DrawArea::DrawArea(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 600);

    // Particule 1
    Particle p1;
    p1.position = Vec2(1.f, 1.5f);
    p1.predictedPosition = p1.position;
    p1.velocity = Vec2(0.f, 0.f);
    p1.mass = 1.f;
    p1.radius = 0.08f;
    ctx_.addParticle(p1);

    // Particule 2
    Particle p2;
    p2.position = Vec2(1.25f, 2.2f);
    p2.predictedPosition = p2.position;
    p2.velocity = Vec2(0.f, 0.f);
    p2.mass = 1.f;
    p2.radius = 0.08f;
    ctx_.addParticle(p2);

    // Timer 60 Hz
    connect(&timer_, &QTimer::timeout, this, &DrawArea::animate);
    timer_.start(16);
}

void DrawArea::animate()
{
    ctx_.updatePhysicalSystem(dt_);
    update();
}

float DrawArea::metersToPixels(float m) const
{
    return m * pixelsPerMeter_;
}

QPointF DrawArea::worldToScreen(const Vec2& p) const
{
    const float x = marginPx_ + metersToPixels(p.x);
    const float y = height() - marginPx_ - metersToPixels(p.y);
    return QPointF(x, y);
}

void DrawArea::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(20, 20, 20));

    painter.setPen(QPen(QColor(180, 180, 180), 2));
    painter.drawLine(
        QPointF(marginPx_, height() - marginPx_),
        QPointF(width() - marginPx_, height() - marginPx_)
        );

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(80, 180, 255));

    for (const Particle& part : ctx_.particles())
    {
        const QPointF c = worldToScreen(part.position);
        const float r = std::max(1.f, metersToPixels(part.radius));
        painter.drawEllipse(c, r, r);
    }
}
