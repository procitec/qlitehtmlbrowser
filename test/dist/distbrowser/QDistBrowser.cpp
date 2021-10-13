#include "QDistBrowser.h"

QDistBrowser::QDistBrowser( QWidget* parent )
  : QLiteHtmlBrowser( parent )
{
}

QByteArray QDistBrowser::loadResource( int, const QUrl& )
{
  QByteArray ret;
  return ret;
}
