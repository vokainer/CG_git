#include "clock.h"
#include <QDebug>


Clock::Clock(Vec3d center, int radius)
{
    m_center = center;
    m_radius = radius;

    m_hours = Vec3d(0,center.getY() + radius - 30,1);
    m_minutes = Vec3d(0,center.getY() + radius - 10,1);
    m_seconds = Vec3d(0,center.getY() + radius,1);

    m_RotationSeconds = Mat3d::getRotationmatrix(-6*3.141592/180); //Rotationmatrix of -6°
    m_RotationHours = Mat3d::getRotationmatrix(-30*3.141592/180); //Rotationmatrix of -30°

    m_militime = 0;
    m_SCount = 0;
    m_MCount = 0;

    //Get current time
    time_t timestamp;
    timestamp = time(0);
    m_now = localtime(&timestamp);

    if(m_now->tm_hour != 0)
        for(int h=0; h < m_now->tm_hour; h++)
            m_hours = m_RotationHours * m_hours;


    if(m_now->tm_min != 0)
        for(int m=0; m < m_now->tm_min; m++){
            m_MCount++;
            m_minutes = m_RotationSeconds * m_minutes;
        }

    if(m_now->tm_sec != 0)
        for(int s=0; s < m_now->tm_sec; s++){
            m_SCount++;
            m_seconds = m_RotationSeconds * m_seconds;
        }
}

void Clock::update(int elapsed)
{
    if(elapsed == 0)
        m_militime += 100;

    //qDebug() << militime << endl;
    if((m_militime % 1000) == 0 && m_militime != 0){
        m_seconds = m_RotationSeconds * m_seconds;
        m_militime = 0;
        m_SCount++;

        if(m_SCount >= 60){
            m_minutes = m_RotationSeconds * m_minutes;
            m_SCount = 0;
            m_MCount++;

            if(m_MCount >= 60){
                m_hours = m_RotationHours * m_hours;
                m_MCount = 0;
            }
        }
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
