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
    // Initialize the texture buffer.
    m_buffer = new unsigned char[3*TEX_RES];

    //Exercise sheet 1 task 5 ----------

    //    Matrix<double, 4> test;
    //    test(0,0)= 3;
    //    test(0,1)= 5;
    //    test(0,2)= 1;
    //    test(0,3)= 7;

    //    test(1,0)= 2;
    //    test(1,1)= 4;
    //    test(1,2)= 5;
    //    test(1,3)= 4;

    //    test(2,0)= 1;
    //    test(2,1)= 2;
    //    test(2,2)= 2;
    //    test(2,3)= 3;


    //    test(3,0)= 4;
    //    test(3,1)= 8;
    //    test(3,2)= 9;
    //    test(3,3)= 1;


    //   bool sing;
    //   qDebug() << "Test Matrix:" << endl << test.toQString() << endl;
    //   qDebug() << endl << "Inverse der Test Matrix:" << endl << test.inverse(sing).toQString();

    //   Matrix<double, 4> ergebnis;
    //   ergebnis = test * test.inverse(sing);
    //   qDebug() << endl << endl << "Multiplikation: "<< endl << ergebnis.toQString();

    //-----------------------------------


    /*
    // Set the timeout to 50 milliseconds, corresponding to 20 FPS.
    m_timeout = 50; // 50 msecs
    m_timer = new QTimer(this);
    // Connect the timeout signal of the timer to the animate slot.
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
    // Start the timer.
    m_timer->start(m_timeout);
    m_elapsed = 0;
    */
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
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

/*
  Draw a line with bresenham Algo from p1 to p2 with color
*/
void GLBox::bresenhamLine(Point2D p1, Point2D p2, Color color)
{
    int x,x1,x2,y,y1,y2,d;

    if(p1.x > p2.x){ //Swap points if p1 is on the right of p2
        x2 = p1.x;
        y2 = p1.y;
        x1 = p2.x;
        y1 = p1.y;
    } else {
        x1 = p1.x;
        y1 = p1.y;
        x2 = p2.x;
        y2 = p1.y;
    }

    int dy            = y2 - y1;  // y-increment from p1 to p2
    int dx            = x2 - x1;  // x-increment from p1 to p2
    int dy2           = (dy << 1);  // dy << 1 == 2*dy
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2  = dy2 + dx2;

    if(dy >= 0) //m >= 0
    {
        if(dy <= dx){
            //Case 1: 0<=m<=1   1.Oktant
            d = dy2 -dx; //Initial d
            x = x1;
            y = y1;

            while(x <= x2){
                setPoint(Point2D(x,y),color);
                if(d <= 0){
                    d += y2;
                } else {
                    y++;
                    d += dy2_minus_dx2;
                }
            }
        }
        //Case 2: 1 < m < inf    2.Oktant
        else {
            d = dx2 -dy; //Initial d

            x = x1;
            y = y1;

            while(y <= y2){
                setPoint(Point2D(x,y),color);
                if(d <= 0){
                    d += dx2;
                } else {
                    x++;
                    d -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else { //m<0
        //Case 3: -1<=m<0
        if(dx >= -dy){
            d = -dy2 - dx; //Initial d

            x = x1;
            y = y1;

            while(x <= x2){

                setPoint(Point2D(x,y),color);
                if(d <= 0){
                    d -= dy2;
                } else {
                    y--;
                    d -= dy2_plus_dx2;
                }
                x++;

            }
        }
        //Case 4:  -inf < m < -1
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
                    d += dy2_plus_dx2;
                }
                y--;
            }
        }
    }

}

void GLBox::bresenhamCircle(Point2D center, int radius, Color color)
{

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

    Color red(1.0, 0.0, 0.0);
    Color blue(0.0, 0.0, 1.0);

    Point2D p1(0, 0);
    // Point2D p2(-10, 10);
    setPoint(p1, red);
    //setPoint(p2, red);

    //Point2D center(20,20);
    //setPoint(center, blue);

    //1.Oktant
    Point2D p3(10,1);
    bresenhamLine(p1,p3);
    setPoint(p3,blue);

    //    //2.Oktant
    //    Point2D p4(10,20);
    //    bresenhamLine(p1,p4);
    //    setPoint(p4,blue);

    //    //3.Oktant
    //    Point2D p5(-10,20);
    //    setPoint(p5);
    //   // bresenhamLine(p1,p5);

    //    //4.Oktant
    //    Point2D p6(-10,10);
    //    setPoint(p6);
    //   // bresenhamLine(p1,p6);

    //    //5.Oktant
    //    Point2D p7(-10,-7);
    //    setPoint(p7);
    //    bresenhamLine(p1,p7);

    //    //6.Oktant
    //    Point2D p8(-5,-10);
    //    setPoint(p8);
    //    bresenhamLine(p1,p8);

    //    //7.Oktant
    //    Point2D p9(10,-20);
    //    setPoint(p9);
    //    //bresenhamLine(p1,p9);

    //    //8.Oktant
    //    Point2D p10(10,-10);
    //    setPoint(p10);
    //    bresenhamLine(p1,p10);
    //    setPoint(p1, red);


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
    /*
    // At each timeout, increase the elapsed time until it reaches 100. Then it is set to zero and the hands of the clock are moved.
    m_elapsed = (m_elapsed + qobject_cast<QTimer*>(sender())->interval()) % 100;
    m_clock.update(m_elapsed);
    updateGL();
    */
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
