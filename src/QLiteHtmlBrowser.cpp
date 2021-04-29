#include "QLiteHtmlBrowser.h"
#include "container_qt.h"

QLiteHtmlBrowser::QLiteHtmlBrowser( QWidget* parent )
  : QWidget( parent )
{
  mContainer = new container_qt( parent );
}
