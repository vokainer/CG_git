#include "MainWindow.h"
#include <qlayout.h>       // for QVBoxLayout
#include <qpushbutton.h>   // for QPushButton
#include <qcursor.h>       // for QCursor
#include <qapplication.h>  // for qApp
#include <qstatusbar.h>    // for QStatusBar
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow (QWidget *parent, const char *name)
{
    ui.setupUi(this);
    // create the GL widget to show the scene
    glbox = new GLBox (centralWidget(), 0L);

    // the layout object is responsible for controling the sizes of the two widgets "glbox" and "bottomFrame".
    QVBoxLayout *centralLayout = new QVBoxLayout (ui.widget/*, 10*/);


    // we add the glbox first, so it will cover the top part of the window
    // it gets a stretch factor of 1, so it takes all the space it can get
    centralLayout->addWidget (glbox, 1);

    //Slider
    QSlider *slider = new QSlider(Qt::Horizontal, glbox);
    connect(slider, SIGNAL(valueChanged(int)), glbox, SLOT(focusChanged(int)));

    // Layout that contains the slider widget.
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(slider);
    centralLayout->addLayout(sliderLayout);
}

GLBox *MainWindow::getGLBox ()
{
    return glbox;
}

void MainWindow::fileExit()
{
    qApp->quit();
}
