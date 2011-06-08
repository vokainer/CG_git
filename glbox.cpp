//
// GLBox
//
// Widget to display an OpenGL scene. The scene is bounded by the [-1,1]^3 volume.
//
// Author: Peter Salz, TU Kaiserslautern, salz@rhrk.uni-kl.de
// Based on the original work by Burkhard Lehner <lehner@informatik.uni-kl.de> and Gerd Reis.

#include <math.h> // for sqrt

#include "glbox.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>

GLBox::GLBox( QWidget* parent, const QGLWidget* shareWidget )
    : QGLWidget( parent,  shareWidget )
{
    scale = 100;
    m_texID = 0;
    m_winWidth = 700;
    m_winHeight = 700;
    m_Ygravity = 1.0;
    m_Xgravity = 0.2;
    double m_focus = 0; //Brennweite, distance Viewer to Bildfläche

    // Initialize the texture buffer.
    m_buffer = new unsigned char[3*TEX_RES];

    // Set new Clock
    m_clock = Clock(Vec3d(-1.0,1.0,1.0),100);

    // Set the timeout to 50 milliseconds, corresponding to 20 FPS.
    m_timeout = 50; // 50 msecs
    m_timer = new QTimer(this);
    // Connect the timeout signal of the timer to the animate slot.
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
    // Start the timer.
    m_timer->start(m_timeout);
    m_elapsed = 0;

}

GLBox::~GLBox()
{
    delete [] m_buffer;
    m_buffer = NULL;
}

void GLBox::manageTexture()
{
    glEnable(GL_TEXTURE_2D);

    if (m_texID == 0)
        glGenTextures(1, &m_texID);

    glBindTexture(GL_TEXTURE_2D, m_texID);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_RES_X, TEX_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, m_buffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
}

void GLBox::clearImage(Color c)
{
    for (int i = 0; i < TEX_RES; i++)
    {
        m_buffer[3*i  ] = (unsigned char)(255.0*c.r);
        m_buffer[3*i+1] = (unsigned char)(255.0*c.g);
        m_buffer[3*i+2] = (unsigned char)(255.0*c.b);
    }
}

void GLBox::setPoint(Point2D p, Color c)
{
    // Transform coordinates from [-TEX_HALF,TEX_HALF] to [0, TEX_RES]
    int x = p.x + TEX_HALF_X;
    int y = p.y + TEX_HALF_Y;
    if (x < 0 || y < 0 || x >= TEX_RES_X || y >= TEX_RES_Y)
    {
        qDebug() << "Illegal point coordinates (" << p.x << "," << p.y << ")";
        return;
    }

    m_buffer[3*TO_LINEAR(x,y)  ] = (unsigned char)(255.0*c.r);
    m_buffer[3*TO_LINEAR(x,y)+1] = (unsigned char)(255.0*c.g);
    m_buffer[3*TO_LINEAR(x,y)+2] = (unsigned char)(255.0*c.b);
}

bool GLBox::CheckInBoundX(int point){
    int x = point + TEX_HALF_X;
    if (x < 0 || x >= TEX_RES_X)
    {
        return false;
    } else
        return true;
}

bool GLBox::CheckInBoundY(int point){
    int y = point + TEX_HALF_Y;
    if (y < 0 || y >= TEX_RES_Y)
    {
        return false;
    } else
        return true;
}


/*
  Draw a line with bresenham Algo from p1 to p2 with color
*/
void GLBox::bresenhamLine(Point2D p1, Point2D p2, Color color)
{
    int x,x1,x2,y,y1,y2,d,dx,dx2,dy,dy2,dy2_minus_dx2,dy2_plus_dx2;

    if(p1.x > p2.x){ //Swap points if p1 is on the right of p2
        x2 = p1.x;
        y2 = p1.y;
        x1 = p2.x;
        y1 = p2.y;
    } else {
        x1 = p1.x;
        y1 = p1.y;
        x2 = p2.x;
        y2 = p2.y;
    }

    dy = y2 - y1;  //y increment p1 to p2
    dx = x2 - x1;  //x increment p1 to p2


    dy2 = 2*dy; //Or dy2 = (dy << 1);  dy << 1 == 2*dy (1 bit to the left shift)
    dx2 = 2*dx;
    dy2_minus_dx2 = dy2 - dx2;
    dy2_plus_dx2 = dy2 + dx2;

    if(dy >= 0) //m >= 0
    {
        if(dy <= dx){
            //Case 1:   0 <= m <= 1       1.Oktant (4.Oktant with swap)

            d = dy2 - dx; //Initial d

            x = x1;
            y = y1;

            while(x <= x2){
                setPoint(Point2D(x,y),color);

                if(d <= 0){
                    d += dy2;
                } else {
                    y++;
                    d += dy2_minus_dx2; //dy2 <= dx2 -> dy2 - dx2 = 0 or negativ
                }
                x++;
            }
        } else {
            //Case 2:   1 < m < inf     2.Oktant (3.Oktant with swap)

            d = dx2 -dy; //Initial d

            x = x1;
            y = y1;

            while(y <= y2){
                setPoint(Point2D(x,y),color);

                if(d <= 0){
                    d += dx2;
                } else {
                    x++;
                    d -= dy2_minus_dx2; //dy2 > dx2 -> dy2 - dx2 > 0
                }
                y++;
            }
        }
    }
    else { //m<0

        if(dx >= -dy){
            //Case 3:   -1 <= m < 0     8.Oktant (5.Oktant with swap)

            d = -dy2 - dx; //Initial d

            x = x1;
            y = y1;

            while(x <= x2){

                setPoint(Point2D(x,y),color);
                if(d <= 0){
                    d -= dy2;
                } else {
                    y--; // -1 <= m < 0
                    d -= dy2_plus_dx2; //dx2 >= -dy2 -> dy2 + dx2 > 0
                }
                x++;

            }
        }
        //Case 4:  -inf < m < -1    7.Oktant (6.Oktant with swap)
        else {
            d = dx2 + dy; //Initial d

            y = y1;
            x = x1;
            while(y >= y2){
                setPoint(Point2D(x,y),color);
                if(d <= 0){
                    d += dx2;
                } else {
                    x++;
                    d += dy2_plus_dx2; //dx2 < dy2 -> dy2 + dx2 <=0
                }
                y--; //-inf < m < -1
            }
        }
    }

}

/*
  Draw a line with bresenham Algo from v1 to v2 with color
*/
void GLBox::bresenhamLine(Vec3d v1, Vec3d v2, Color color)
{
    double d1[3];
    double d2[3];
    v1.getData(d1);
    v2.getData(d2);

    Point2D p1 (int(d1[0] + 0.5), int(d1[1] + 0.5));
    Point2D p2 (int(d2[0] + 0.5), int(d2[1] + 0.5));

    bresenhamLine(p1, p2, color);
}





void GLBox::bresenhamCircle(Point2D center, int radius, Color color)
{
    int x = 0;
    int y = radius;
    int d = 5 - 4*radius;

    setPoint(Point2D(x + center.x,y + center.y),color);
    setPoint(Point2D(x + center.x,-y + center.y),color);
    setPoint(Point2D(y + center.x,-x + center.y), color);
    setPoint(Point2D(-y + center.x,-x + center.y),color);
    setPoint(Point2D(-x + center.x,-y + center.y),color);
    setPoint(Point2D(-x + center.x,y + center.y),color);
    setPoint(Point2D(-y + center.x,x + center.y),color);
    setPoint(Point2D(y + center.x,x + center.y),color);

    while(y > x){

        if (d <= 0){
            d += 4*(2*x + 3);
        }
        else {
            d += 4*(2*(x - y) + 5);
            y--;
        }
        x++;
        setPoint(Point2D(x + center.x,y + center.y),color);
        setPoint(Point2D(x + center.x,-y + center.y),color);
        setPoint(Point2D(y + center.x,-x + center.y), color);
        setPoint(Point2D(-y + center.x,-x + center.y),color);
        setPoint(Point2D(-x + center.x,-y + center.y),color);
        setPoint(Point2D(-x + center.x,y + center.y),color);
        setPoint(Point2D(-y + center.x,x + center.y),color);
        setPoint(Point2D(y + center.x,x + center.y),color);
    }

}

void GLBox::bresenhamCircle(Vec3d center, int radius, Color color)
{
    double d[3];
    center.getData(d);

    Point2D p1 (int(d[0] + 0.5), int(d[1] + 0.5));

    bresenhamCircle(p1,radius,color);
}

void GLBox::initializeGL()
{
    // this method is called exactly once on program start
    clearImage();

    glViewport(0, 0, m_winWidth, m_winHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-m_winWidth/2, m_winWidth/2, -m_winHeight/2, m_winHeight/2, 0, 1);

    glClear (GL_COLOR_BUFFER_BIT);

}


void GLBox::resizeGL(int w, int h)
{
    m_winWidth = w;
    m_winHeight = h;

    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w/2, w/2, -h/2, h/2, 0, 1);

    updateGL();
}

void GLBox::paintGL()
{
    // this method draws the scene into the OpenGL widget
    // usually you do not call this method directly, instead call updateGL(), which in turn calls paintGL()

    clearImage(Color(1.0, 1.0, 1.0));

    //----- Clock
    bresenhamCircle(m_clock.getCenter(), m_clock.getRadius());

    //draw Hour Pointer
    bresenhamLine(m_clock.getCenter(), Mat3d::getTranslationmatix(m_clock.getCenter()) * m_clock.getHours(), Color(1,0,0));

    //draw Minute Pointer
    bresenhamLine(m_clock.getCenter(), Mat3d::getTranslationmatix(m_clock.getCenter()) *  m_clock.getMinutes(), Color(0,0,1));

    //draw Seconds Pointer
    bresenhamLine(m_clock.getCenter(), Mat3d::getTranslationmatix(m_clock.getCenter()) *  m_clock.getSeconds(), Color(0,1,0));


    //-------------
    manageTexture();

    glClear( GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, m_texID);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(-m_winWidth/2, -m_winHeight/2);
    glTexCoord2i(1, 0);
    glVertex2i( m_winWidth/2, -m_winHeight/2);
    glTexCoord2i(1, 1);
    glVertex2i( m_winWidth/2,  m_winHeight/2);
    glTexCoord2i(0, 1);
    glVertex2i(-m_winWidth/2, m_winHeight/2);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    // perform all output operations
    glFlush();
}

void GLBox::animate()
{
//    // Gravity
//    m_clock.SpeedY = m_clock.SpeedY - m_Ygravity;
//    m_clock.SpeedX = m_clock.SpeedX - m_Xgravity;

//    // Get Clocks (min x, min y), (max x, max y) of Axis aligned bounding box (AABB)
//    Point2D min(m_clock.getCenter().getX() - m_clock.getRadius() + m_clock.SpeedX, m_clock.getCenter().getY() - m_clock.getRadius() + m_clock.SpeedY);
//    Point2D max(m_clock.getCenter().getX() + m_clock.getRadius() + m_clock.SpeedX, m_clock.getCenter().getY() + m_clock.getRadius() + m_clock.SpeedY);

//    if(!CheckInBoundX(min.x))
//        m_clock.SpeedX *= -1;

//    if(!CheckInBoundX(max.x))
//        m_clock.SpeedX *= -1;

//    if(!CheckInBoundY(min.y))
//        m_clock.SpeedY *= -1;

//    if(!CheckInBoundY(max.y))
//        m_clock.SpeedY *= -1;

//    // Prevent Clock from gaining to much Speed
//    if(m_clock.SpeedX > 20)
//        m_clock.SpeedX *= 0.5;

//    if(m_clock.SpeedY > 20)
//        m_clock.SpeedY *= 0.5;

//    // Set new Clock Position based on her Speed
//    m_clock.setCenter(Vec3d(m_clock.getCenter().getX(), m_clock.getCenter().getY() + m_clock.SpeedY, 1));
//    m_clock.setCenter(Vec3d(m_clock.getCenter().getX()  + m_clock.SpeedX, m_clock.getCenter().getY(), 1));

//    // At each timeout, increase the elapsed time until it reaches 100. Then it is set to zero and the hands of the clock are moved.
//    m_elapsed = (m_elapsed + qobject_cast<QTimer*>(sender())->interval()) % 100;
//    m_clock.update(m_elapsed);
    updateGL();
}

void GLBox::focusChanged(int focus){
    m_focus = focus;
    qDebug() << m_focus;
}

void GLBox::mousePressEvent( QMouseEvent *e )
{
    // a mouse button was pressed

    // if we are in moving mode, store the current mouse position (in viewport coordinates)
    // for calculating the cursor movement later (see mouseMoveEvent)
    x0 = e->x();
    y0 = e->y();
}

void GLBox::mouseMoveEvent( QMouseEvent *e )
{
    // mouse was moved while a mouse button was pressed (dragging)

    // so we are in moving mode
    // get the current cursor position (viewport coordinates)
    int x = e->x();
    int y = e->y();

    // calculate the difference to the last cursor position, relative to the dimension of the viewport
    double dx = (x - x0) / height();
    double dy = (y - y0) / height();
    // calculate the difference to the center of the viewport, relative to the dimension of the viewport
    double rx = (x - .5 * width()) / height();
    double ry = (y - .5 * height()) / height();
    // store the cursor position for the next move event
    x0 = x;
    y0 = y;

    int state = e->buttons ();
    // check for left mouse button => rotation
    if ((state & Qt::LeftButton) != 0)
    {
        //double dist = sqrt(dx*dx + dy*dy); //späteres Blatt?
        Vec4d axis(dy, dx, 4.0*(ry*dx - rx*dy), 0.0);
    }

    // check for right mouse button => translation
    if ((state & Qt::RightButton) != 0)
    {
        Vec4d trans(-dx*1.5*scale, dy*1.5*scale, 0.0, 0.0);
    }

    // repaint the scene
    updateGL();
}

void GLBox::mouseReleaseEvent( QMouseEvent * )
{
    // nothing to be done here
}

void GLBox::wheelEvent (QWheelEvent *e)
{
    // turning the mouse wheel will change the scale and therefore zoom in and out
    // double dist = e->delta() / 120.0;  // one wheel "tick" counts for 120 späteres Blatt?
    //scale *= exp (dist * log (1.05));
    updateGL();
}

void GLBox::keyPressEvent(QKeyEvent *e)
{
    //int key = e->key(); //Hier noch Sinnlos, späteres Blatt?
    qDebug() << "keyPressEvent()";

    e->accept();
    updateGL();
}
