#include "QHelpBrowser.h"

QHelpBrowser::QHelpBrowser( QWidget* parent )
  : QLiteHtmlBrowser( parent )
{
}

QByteArray QHelpBrowser::loadResource( int resourceType, const QUrl& url )
{
  QByteArray ret;
  if ( mHelpEngine && url.scheme() == "qthelp" )
  {
    ret = mHelpEngine->fileData( url );
  }
  if ( ret.isEmpty() )
  {
    ret = QLiteHtmlBrowser::loadResource( resourceType, url );
  }
  return ret;
}
