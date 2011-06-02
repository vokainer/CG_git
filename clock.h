#ifndef CLOCK_H
#define CLOCK_H

#include "matrix.h"

class Clock
{
public:
    //Standard Constructor
    Clock(Vec3d center = Vec3d(0.0, 0.0, 1.0), int radius = 1);


    //updates Clock; Rechnet die neuen Koordinaten der Punkte aus für die Zeiger
    void update(int elapsed);


    //getter Methods
    Vec3d getCenter();
    int getRadius();
    Vec3d getHours();
    Vec3d getMinutes();
    Vec3d getSeconds();


private:
    int time; //Time since the clock was started
    unsigned int militime; //Time in milliseconds since last update() call
    Vec3d m_center; //Clock center Position in World Coordinates
    int m_radius; //Clock radius
    Vec3d m_hours; //Hours pointer in Local Coordinates
    Vec3d m_minutes; //Minutes pointer in Local Coordinates
    Vec3d m_seconds; //Seconds pointer in Local Coordinates

    Mat3d m_Rotation; // Rotationmatrix of -6°
};

#endif // CLOCK_H
