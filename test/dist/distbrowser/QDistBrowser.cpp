#include "QDistBrowser.h"

QDistBrowser::QDistBrowser( QWidget* parent )
  : QLiteHtmlBrowser( parent )
{
}

QByteArray QDistBrowser::loadResource( const QUrl& url )
{
  QByteArray ret;
  return ret;
}
