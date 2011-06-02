#include "clock.h"

Clock::Clock(Vec3d center, int radius)
{
    m_center = center;
    m_radius = radius;
}

void Clock::update(int elapsed)
{

}

Vec3d Clock::getPoints() //liefert die Punkte zurück die glbox dann in paintGL zeichnen soll
{

}

Vec3d Clock::getCenter()
{
    return m_center;
}

int Clock::getRadius()
{
    return m_radius;
}
