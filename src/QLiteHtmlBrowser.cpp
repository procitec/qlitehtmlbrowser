#include <qlitehtmlbrowser/QLiteHtmlBrowser>
#include "QLiteHtmlBrowserImpl.h"

#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>
//#include <QtCore/QDir>
//#include <QtCore/QDebug>
//#include <QtGui/QPalette>
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QStyle>

//#include <functional>

QLiteHtmlBrowser::QLiteHtmlBrowser( QWidget* parent )
  : QWidget( parent )
  , mImpl( new QLiteHtmlBrowserImpl( this ) )
{
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  //  setStyleSheet( "background-color:black;" );

  auto* layout = new QVBoxLayout;
  layout->addWidget( mImpl );

  setLayout( layout );

  mImpl->show();
  //  mImpl->
  connect( mImpl, &QLiteHtmlBrowserImpl::urlChanged, this, &QLiteHtmlBrowser::urlChanged );
  mImpl->setResourceHandler( std::bind( &QLiteHtmlBrowser::loadResource, this, std::placeholders::_1, std::placeholders::_2 ) );
}

QUrl QLiteHtmlBrowser::source() const
{
  return mImpl->url();
}

// void QLiteHtmlBrowser::changeEvent( QEvent* e )
//{
//  QWidget::changeEvent( e );
//  if ( e && e->type() == QEvent::PaletteChange )
//  {
//    auto lightness = this->palette().color( QPalette::Window ).lightnessF();
//    bool dark_mode = ( lightness > 0.5 ) ? false : true;
//    if ( dark_mode )
//    {
//      mCSS = QString::fromUtf8( master_css_x );
//      QFile dark_css( ":/styles/dark.css" );
//      dark_css.open( QIODevice::ReadOnly );
//      if ( dark_css.isOpen() )
//      {
//        auto css = QString::fromUtf8( dark_css.readAll() );
//        css.replace( "@QPalette::Window@", palette().color( QPalette::Window ).name() );
//        css.replace( "@QPalette::Text@", palette().color( QPalette::Text ).name() );
//        mCSS += css;
//        loadStyleSheet();
//      }
//    }
//  }
//}

// void QLiteHtmlBrowser::resizeEvent( QResizeEvent* ev )
//{
//  if ( ev )
//  {
//    QWidget::resizeEvent( ev );
//  }
//}

QByteArray QLiteHtmlBrowser::loadResource( int type, const QUrl& url )
{
  return mImpl->loadResource( type, url );
}

void QLiteHtmlBrowser::setSource( const QUrl& name )
{
  mImpl->setUrl( name );
}

void QLiteHtmlBrowser::setHtml( const QString& html, const QString& baseurl )
{
  mImpl->setHtml( html, baseurl );
}

void QLiteHtmlBrowser::setScale( double scale )
{
  mImpl->setScale( scale );
}

double QLiteHtmlBrowser::scale() const
{
  return mImpl->scale();
}

// void QLiteHtmlBrowser::wheelEvent( QWheelEvent* e )
//{
//  if ( e->modifiers() & Qt::ControlModifier )
//  {
//    e->setAccepted( true );
//    float delta = e->angleDelta().y() / 120.f;
//    auto  scale = mContainer->scale();
//    scale += delta / 10.0; // get scaling in 10 percent steps;

//    mImpl->setScale( scale );
//  }
//}
