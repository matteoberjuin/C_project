#include "drawarea.h"
#include "Context.h"
#include <QPainter>
#include <algorithm>
#include <QMouseEvent>
#include "Particle.h"
#include <cmath>


DrawArea::DrawArea(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(700, 600);

    float rayonBille = 0.03f;
    float departX = 1.8f;
    float departY = 0.63f;
    for (int col = 0; col < 5; ++col) {
        for (int ligne = 0; ligne <= col; ++ligne) {
            Particle p;
            p.name = "boule rouge";
            float x = departX + col * (rayonBille * 2.0f * 0.866f);
            float y = departY + (ligne - col * 0.5f) * (rayonBille * 2.02f);

            p.position = Vec2(x, y);
            p.predictedPosition = p.position;
            p.velocity = Vec2(0.f, 0.f);
            p.radius = rayonBille;
            p.mass = 1.0f;

            ctx_.addParticle(p);
        }
    }

    Particle p2;
    p2.name="boule blanche";
    p2.position = Vec2(0.6f, 0.63f);
    p2.predictedPosition = p2.position;
    p2.velocity = Vec2(0.f, 0.f);
    p2.mass = 1.f;
    p2.radius = 0.03f;
    p2.mammamia = 0;
    p2.contactNormal = Vec2(0.f, 0.f);
    ctx_.addParticle(p2);

    connect(&timer_, &QTimer::timeout, this, &DrawArea::animate);
    timer_.start(16);
}

void DrawArea::animate()
{
    const float tableWidth = 2.54f;
    const float tableHeight = 1.27f;
    const float leftX = 0.f;
    const float bottomY = 0.f;
    const float rightX = tableWidth;
    const float topY   = tableHeight;

    ctx_.setWorldBounds(leftX, rightX, bottomY, topY);
    if (ctx_.poches().empty()) {
        ctx_.ajtPoche(Vec2(leftX, bottomY), 0.12f);
        ctx_.ajtPoche(Vec2(rightX, bottomY), 0.12f);
        ctx_.ajtPoche(Vec2(leftX, topY), 0.12f);
        ctx_.ajtPoche(Vec2(rightX, topY), 0.12f);
        ctx_.ajtPoche(Vec2(tableWidth/2.f, bottomY), 0.12f);
        ctx_.ajtPoche(Vec2(tableWidth/2.f, topY), 0.12f);}
    ctx_.updatePhysicalSystem(dt_);
    if (ctx_.blanche_en_attente_ && !ctx_.est_ce_que_ca_bouge_encore()) {
        ctx_.replacerBlanche(Vec2(0.6f, 0.63f));
    }
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

        // 1. Fond de la fenêtre
        painter.fillRect(rect(), QColor(30, 30, 30));

        // 2. On définit les 4 coins exacts de la table en PIXELS
        QPointF basGauche  = worldToScreen(Vec2(0.f, 0.f));
        QPointF basDroite   = worldToScreen(Vec2(2.54f, 0.f));
        QPointF hautGauche = worldToScreen(Vec2(0.f, 1.27f));
        QPointF hautDroite  = worldToScreen(Vec2(2.54f, 1.27f));

        // 3. On remplit l'intérieur en vert (Le tapis)
        // On crée un polygone avec les 4 coins pour être sûr que ça s'aligne
        QPolygonF tapis;
        tapis << hautGauche << hautDroite << basDroite << basGauche;
        painter.setBrush(QColor(20, 100, 40));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(tapis);

        // 4. On dessine les murs blancs PILE sur les limites physiques
        painter.setPen(QPen(Qt::white, 3));
        painter.drawLine(basGauche, basDroite);   // Bas
        painter.drawLine(hautGauche, hautDroite); // Haut
        painter.drawLine(basGauche, hautGauche);  // Gauche
        painter.drawLine(basDroite, hautDroite);  // Droite

        // 5. On dessine les poches (Elles seront maintenant parfaitement sur les coins)
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        for (const auto& poche : ctx_.poches()) {
            QPointF centrePoche = worldToScreen(poche.position);
            float rayonPoche = metersToPixels(poche.rayon);
            painter.drawEllipse(centrePoche, rayonPoche, rayonPoche);
        }
        painter.setPen(Qt::yellow); // Couleur qui ressort bien sur le vert
        painter.setFont(QFont("Arial", 20));
        painter.drawText(50, 50, "Score : " + QString::number(ctx_.score));


    for (const Particle& part : ctx_.particles())
    {
        if (part.name == "boule blanche")
            painter.setBrush(Qt::white);
        else
            painter.setBrush(Qt::red);

        const QPointF c = worldToScreen(part.position);
        const float r = std::max(1.f, metersToPixels(part.radius));

        if (!std::isfinite(c.x()) || !std::isfinite(c.y()) || !std::isfinite(r))
            continue;

        painter.drawEllipse(c, r, r);

        painter.drawEllipse(c, r, r);
    }
    if (estEnTrainDeViser_ && indexBilleBlanche_ != -1) {

        QPointF depart = worldToScreen(ctx_.particles()[indexBilleBlanche_].position);

        QPointF fin = worldToScreen(positionSouris);

        painter.setPen(QPen(Qt::white, 2, Qt::DashLine));

        painter.drawLine(depart, fin);
    }
}

void DrawArea::mousePressEvent(QMouseEvent* event)
{

    Vec2 positionClic( (event->position().x() - marginPx_) / pixelsPerMeter_,
                      (height() - marginPx_ - event->position().y()) / pixelsPerMeter_ );


    const auto& billes = ctx_.particles();

    for (int i = 0; i < (int)billes.size(); ++i) {

        if (billes[i].name == "boule blanche") {


            float distance = (billes[i].position - positionClic).length();

            if (distance < billes[i].radius * 2.0f) {
                estEnTrainDeViser_ = true;
                indexBilleBlanche_ = i;
                break;
            }
        }
    }

}


void DrawArea::mouseMoveEvent(QMouseEvent* event)
{
    if (estEnTrainDeViser_) {


        positionSouris = Vec2((event->position().x() - marginPx_) / pixelsPerMeter_,
                                    (height() - marginPx_ - event->position().y()) / pixelsPerMeter_);

        update();
    }
}

void DrawArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (estEnTrainDeViser_ && indexBilleBlanche_ != -1) {


        Vec2 posBlanche = ctx_.particles()[indexBilleBlanche_].position;
        Vec2 vecteurTir = posBlanche - positionSouris;


        float puissance = 5.0f;
        ctx_.appliquerImpulsion(indexBilleBlanche_, vecteurTir * puissance);

        estEnTrainDeViser_ = false;
        indexBilleBlanche_ = -1;

        update();
    }
}


