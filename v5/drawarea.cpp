#include "drawarea.h"

#include <QPainter>
#include <algorithm>

#include "Particle.h"

DrawArea::DrawArea(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(700, 600);

    Particle p1;
    p1.name="boule standard2";
    p1.position = Vec2(1.f, 0.5f);
    p1.predictedPosition = p1.position;
    p1.velocity = Vec2(0.f, 0.f);
    p1.mass = 1.f;
    p1.radius = 0.08f;
    p1.mammamia = 0;
    ctx_.addParticle(p1);

    Particle p3;
    p3.name = "boule standard1";
    p3.position = Vec2(1.f, 0.3f);
    p3.predictedPosition = p3.position;
    p3.velocity = Vec2(0.f, 0.f);
    p3.mass = 1.f;
    p3.radius = 0.08f;
    p3.mammamia = 0;
    ctx_.addParticle(p3);

    Particle p2;
    p2.name="boule blanche";
    p2.position = Vec2(3.f, 0.4f);
    p2.predictedPosition = p2.position;
    p2.velocity = Vec2(-4.f, 0.f);
    p2.mass = 1.f;
    p2.radius = 0.08f;
    p2.mammamia = 0;
    ctx_.addParticle(p2);

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
    painter.drawLine(
        QPointF(marginPx_, marginPx_),
        QPointF(width() - marginPx_, marginPx_)
        );
    painter.drawLine(
        QPointF(marginPx_, marginPx_),
        QPointF(marginPx_, height() - marginPx_)
        );
    painter.drawLine(
        QPointF(width() - marginPx_, marginPx_),
        QPointF(width() - marginPx_, height() - marginPx_)
        );

    painter.setPen(Qt::NoPen);

    for (const Particle& part : ctx_.particles())
    {
        if (part.name == "boule blanche")
            painter.setBrush(Qt::white);
        else
            painter.setBrush(Qt::red);

        const QPointF c = worldToScreen(part.position);
        const float r = std::max(1.f, metersToPixels(part.radius));
        painter.drawEllipse(c, r, r);
    }
}
