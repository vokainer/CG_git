#ifndef CLOCK_H
#define CLOCK_H

#include "matrix.h"
#include <time.h>

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

    // Setter Methods
    void setCenter(Vec3d center);

    int SpeedX; // Speed in X Axis
    int SpeedY; // Speed in Y Axis


private:
    int m_time; //Time since the clock was started
    tm *m_now; // Current time, when the clock was started
    unsigned int m_militime; //Time in milliseconds since last update() call
    Vec3d m_center; //Clock center Position in World Coordinates
    int m_radius; //Clock radius
    Vec3d m_hours; //Hours pointer in Local Coordinates
    Vec3d m_minutes; //Minutes pointer in Local Coordinates
    Vec3d m_seconds; //Seconds pointer in Local Coordinates

    Mat3d m_RotationSeconds; // Rotationmatrix of -6°
    Mat3d m_RotationHours; // Rotationmatrix of -30°

    unsigned int m_SCount; // Counting Seconds
    unsigned int m_MCount; // Counting Minutes
};

#endif // CLOCK_H
