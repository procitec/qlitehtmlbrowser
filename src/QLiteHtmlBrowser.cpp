#include "QLiteHtmlBrowser.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>

extern const litehtml::tchar_t master_css[] = {
#include "master.css.inc"
  , 0 };

QLiteHtmlBrowser::QLiteHtmlBrowser( QWidget* parent )
  : QWidget( parent )
{
  setMinimumSize( 200, 200 );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer = new container_qt( this );
  mContainer->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer->show();

  auto* layout = new QVBoxLayout;
  layout->addWidget( mContainer );

  setLayout( layout );

  mCSS = master_css;
  loadStyleSheet();
}

// void QLiteHtmlBrowser::resizeEvent( QResizeEvent* ev )
//{
//  if ( ev )
//  {
//    QWidget::resizeEvent( ev );
//  }
//}

QUrl QLiteHtmlBrowser::source() const
{
  return mSource;
}
void QLiteHtmlBrowser::setUrl( const QUrl& name )
{
  _setSource( name );
}

void QLiteHtmlBrowser::_setSource( const QUrl& name )
{
  mSource = name;
  if ( mContainer )
  {
    // get the content of the url and display the html
  }
}

void QLiteHtmlBrowser::setHtml( const QString& html, const QString& baseurl )
{
  if ( mContainer )
  {
    mContainer->setHtml( html, baseurl );
  }
}

// void QLiteHtmlBrowser::setUrl( const QUrl& url )
//{
//  if ( mContainer )
//  {
//    mContainer->setBaseDirectory( url.path() );
//    mContainer->setHtml()
//  }
//}

void QLiteHtmlBrowser::loadStyleSheet()
{
  mContainer->setCSS( mCSS );
}

void QLiteHtmlBrowser::setScale( double scale )
{
  if ( mContainer )
  {
    mContainer->setScale( scale );
  }
}

double QLiteHtmlBrowser::scale() const
{
  double scale = 0.0;
  if ( mContainer )
  {
    scale = mContainer->scale();
  }
  return scale;
}

void QLiteHtmlBrowser::wheelEvent( QWheelEvent* e )
{
  if ( mContainer && ( e->modifiers() & Qt::ControlModifier ) )
  {
    e->setAccepted( true );
    float delta = e->angleDelta().y() / 120.f;
    auto  scale = mContainer->scale();
    scale += delta / 10.0; // get scaling in 10 percent steps;

    mContainer->setScale( scale );
  }
}
