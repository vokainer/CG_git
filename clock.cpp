#include "clock.h"
#include <QDebug>

Clock::Clock(Vec3d center, int radius)
{
    m_center = center;
    m_radius = radius;

    m_hours = Vec3d(0,center.getY() + radius,1);
    m_minutes = Vec3d(0,center.getY() + radius,1);
    m_seconds = Vec3d(0,center.getY() + radius,1);

    m_Rotation = Mat3d::getRotationmatrix(-6*3.141592/180);
    militime = 0;
}

void Clock::update(int elapsed)
{
    if(elapsed == 0)
        militime += 100;

    //qDebug() << militime << endl;
    if((militime % 1000) == 0 && militime != 0){
        m_seconds = m_Rotation*m_seconds;
        militime = 0;
    }

    //m_hours = Mat3d::getRotationmatrix(45)*m_hours;
}


Vec3d Clock::getCenter()
{
    return m_center;
}

int Clock::getRadius()
{
    return m_radius;
}

Vec3d Clock::getHours()
{
    return m_hours;
}

Vec3d Clock::getMinutes()
{
    return m_minutes;
}

Vec3d Clock::getSeconds()
{
    return m_seconds;
}
