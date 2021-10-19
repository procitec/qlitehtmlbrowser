
#include "distbrowser.h"
#include <QtWidgets/QApplication>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  Q_INIT_RESOURCE( QLiteHtmlBrowser );

  DistBrowser mWnd;
  mWnd.show();
  app.setQuitOnLastWindowClosed( true );
  auto ret = app.exec();

  return ret;
}
