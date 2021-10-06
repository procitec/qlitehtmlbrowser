#include "QHelpBrowser.h"

QHelpBrowser::QHelpBrowser( QWidget* parent )
  : QLiteHtmlBrowser( parent )
{
}

QByteArray QHelpBrowser::loadResource( const QUrl& url )
{
  QByteArray ret;
  if ( mHelpEngine && url.scheme() == "qthelp" )
  {
    ret = mHelpEngine->fileData( url );
  }
  return ret;
}
