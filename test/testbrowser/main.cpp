
#include "testbrowser.h"
#include <QtWidgets/QApplication>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  TestBrowser mWnd;
  mWnd.show();
  app.setQuitOnLastWindowClosed( true );
  auto ret = app.exec();

  return ret;
}
