#include "distbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtCore/QFileInfo>
#include <QtWidgets/QApplication>

DistBrowser::DistBrowser()
{
  mBrowser = new QDistBrowser( this );
  setCentralWidget( mBrowser );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setMinimumSize( { 1024, 768 } );
  mBrowser->show();
}

DistBrowser::~DistBrowser() {}
