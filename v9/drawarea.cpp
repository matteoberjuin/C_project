#include "drawarea.h"
#include "Context.h"
#include <QPainter>
#include <algorithm>
#include <QMouseEvent>
#include "Particle.h"
#include <cmath>
#include <QColor>


DrawArea::DrawArea(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(700, 600);
    std::vector<int> pattern = {
        1,
        2, 1,
        1, 8, 2,
        2, 1, 2, 1,
        1, 2, 1, 2, 1
    };

    int indexPattern = 0;
    float rayonBille = 0.05f;
    float departX = 3.3f;
    float departY = 1.27f;
    for (int col = 0; col < 5; ++col) {
        for (int ligne = 0; ligne <= col; ++ligne) {
            Particle p;
            int type = pattern[indexPattern++];

            if (type == 1) {
                p.name = "rouge";
                p.color = Qt::red;
            } else if (type == 2) {
                p.name = "jaune";
                p.color = Qt::yellow;
            } else if (type == 8) {
                p.name = "noire";
                p.color = Qt::black;
            }
            float x = departX + col * (rayonBille * 2.2f * 0.866f);
            float y = departY + (ligne - col * 0.5f) * (rayonBille * 2.2f);

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
    p2.color = Qt::white;
    p2.position = Vec2(0.8f, 1.27f);
    p2.predictedPosition = p2.position;
    p2.velocity = Vec2(0.f, 0.f);
    p2.mass = 1.f;
    p2.radius = 0.05f;
    p2.mammamia = 0;
    p2.contactNormal = Vec2(0.f, 0.f);
    ctx_.addParticle(p2);

    connect(&timer_, &QTimer::timeout, this, &DrawArea::animate);
    timer_.start(16);
}

void DrawArea::animate()
{
    const float tableWidth = 5.08f;
    const float tableHeight = 2.54f;

    // 1. Configuration du monde (murs et poches)
    ctx_.setWorldBounds(0.f, tableWidth, 0.f, tableHeight);

    if (ctx_.poches().empty()) {
        ctx_.ajtPoche(Vec2(0.f, 0.f), 0.12f);
        ctx_.ajtPoche(Vec2(tableWidth, 0.f), 0.12f);
        ctx_.ajtPoche(Vec2(0.f, tableHeight), 0.12f);
        ctx_.ajtPoche(Vec2(tableWidth, tableHeight), 0.12f);
        ctx_.ajtPoche(Vec2(tableWidth/2.f, 0.f), 0.12f);
        ctx_.ajtPoche(Vec2(tableWidth/2.f, tableHeight), 0.12f);
    }

    // --- LOGIQUE DE TOUR ---

    // A. On regarde si ça bouge AVANT de calculer la physique
    bool ca_bougeait_avant = ctx_.est_ce_que_ca_bouge_encore();

    // B. On fait avancer la physique (UNE SEULE FOIS !)
    ctx_.updatePhysicalSystem(dt_);

    // C. On regarde si ça bouge APRÈS le calcul
    bool ca_bouge_plus_maintenant = !ctx_.est_ce_que_ca_bouge_encore();

    // D. Si ça bougeait juste avant et que ça vient de s'arrêter (Transition)
    if (ca_bougeait_avant && ca_bouge_plus_maintenant)
    {
        // Si aucune bille rouge n'est rentrée pendant ce coup
        if (!ctx_.une_bille_est_rentree)
        {
            // Changement de joueur (1 -> 2 ou 2 -> 1)
            ctx_.joueur_actuel = (ctx_.joueur_actuel == 1) ? 2 : 1;
        }

        // On remet le flag à faux pour le prochain coup
        ctx_.une_bille_est_rentree = false;
    }

    // E. Gestion de la bille blanche (Respawn quand tout est arrêté)
    if (ctx_.blanche_en_attente_ && ca_bouge_plus_maintenant)
    {
        // Attention : J'ai mis 0.8f/1.27f pour correspondre à ton constructeur
        ctx_.replacerBlanche(Vec2(0.8f, 1.27f));
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
        QPointF basDroite   = worldToScreen(Vec2(5.08f, 0.f));
        QPointF hautGauche = worldToScreen(Vec2(0.f, 2.54f));
        QPointF hautDroite  = worldToScreen(Vec2(5.08f, 2.54f));

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


        // On parcourt toutes les particules
        for (const Particle& part : ctx_.particles())
        {
            // --- C'EST ICI QUE LA MAGIE OPÈRE ---
            // On prend la couleur définie dans le constructeur (Rouge, Jaune, Noire ou Blanche)
            painter.setBrush(part.color);

            // On ajoute un contour noir fin pour que les boules jaunes et la blanche ressortent bien
            painter.setPen(QPen(Qt::black, 1));

            // Calcul de la position à l'écran
            const QPointF c = worldToScreen(part.position);
            const float r = std::max(1.f, metersToPixels(part.radius));

            // Sécurité pour éviter de dessiner des valeurs infinies (bug physique)
            if (!std::isfinite(c.x()) || !std::isfinite(c.y()) || !std::isfinite(r))
                continue;

            // Dessin final de la bille
            painter.drawEllipse(c, r, r);
        }
    if (estEnTrainDeViser_ && indexBilleBlanche_ != -1) {

        QPointF depart = worldToScreen(ctx_.particles()[indexBilleBlanche_].position);

        QPointF fin = worldToScreen(positionSouris);

        painter.setPen(QPen(Qt::white, 2, Qt::DashLine));

        painter.drawLine(depart, fin);
    }
    painter.setPen(Qt::white);
    painter.drawText(50, 40, "J1 : " + QString::number(ctx_.score_j1));
    painter.drawText(width() - 100, 40, "J2 : " + QString::number(ctx_.score_j2));

    QString texteTour = QString("Tour du Joueur %1").arg(ctx_.joueur_actuel);
    painter.setPen(ctx_.joueur_actuel == 1 ? Qt::cyan : Qt::magenta);
    painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignTop, texteTour);

    if (ctx_.partieTerminee)
    {
        // On assombrit l'écran
        painter.setBrush(QColor(0, 0, 0, 150)); // Noir transparent
        painter.drawRect(rect());

        // On écrit GAME OVER
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 40, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER");

        painter.setFont(QFont("Arial", 20));
        painter.drawText(rect().adjusted(0, 80, 0, 0), Qt::AlignCenter, "La noire est tombée !");
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


