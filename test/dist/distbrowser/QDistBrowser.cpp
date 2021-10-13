#include "QDistBrowser.h"

QDistBrowser::QDistBrowser( QWidget* parent )
  : QLiteHtmlBrowser( parent )
{
}

QByteArray QDistBrowser::loadResource( const QUrl& url )
{
  QByteArray ret;
  if ( mHelpEngine && url.scheme() == "qthelp" )
  {
    ret = mHelpEngine->fileData( url );
  }
  return ret;
}
