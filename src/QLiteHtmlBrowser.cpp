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
  mImpl->setResourceHandler( std::bind( &QLiteHtmlBrowser::loadResource, this, std::placeholders::_1, std::placeholders::_2 ) );
  mImpl->setUrlResolveHandler( std::bind( &QLiteHtmlBrowser::resolveUrl, this, std::placeholders::_1 ) );

  connect( mImpl, &QLiteHtmlBrowserImpl::urlChanged, this, &QLiteHtmlBrowser::sourceChanged );
  connect( mImpl, &QLiteHtmlBrowserImpl::anchorClicked, this, &QLiteHtmlBrowser::anchorClicked );
}

QUrl QLiteHtmlBrowser::source() const
{
  auto [url, type] = mImpl->url();
  return url;
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

QUrl QLiteHtmlBrowser::resolveUrl( const QString& url )
{
  return mImpl->resolveUrl( url );
}

void QLiteHtmlBrowser::setSource( const QUrl& name, const ResourceType& type )
{
  mImpl->setUrl( name, static_cast<int>( type ) );
}

void QLiteHtmlBrowser::setHtml( const QString& html, const QString& baseurl )
{
  mImpl->setHtml( html, baseurl );
}

QString QLiteHtmlBrowser::html() const
{
  return mImpl->html();
}

void QLiteHtmlBrowser::home()
{
  auto [url, type] = mImpl->home();
  if ( url.isValid() )
  {
    mImpl->setUrl( url, type );
  }
}

void QLiteHtmlBrowser::reload()
{
  mImpl->reload();
}

void QLiteHtmlBrowser::setScale( double scale )
{
  mImpl->setScale( scale );
}

double QLiteHtmlBrowser::scale() const
{
  return mImpl->scale();
}

bool QLiteHtmlBrowser::openLinks() const
{
  return mImpl->openLinks();
}

bool QLiteHtmlBrowser::openExternalLinks() const
{
  return mImpl->openExternalLinks();
}

const QStringList& QLiteHtmlBrowser::searchPaths() const
{
  return mImpl->searchPaths();
}

void QLiteHtmlBrowser::setOpenLinks( bool open )
{
  mImpl->setOpenLinks( open );
}

void QLiteHtmlBrowser::setOpenExternalLinks( bool open )
{
  mImpl->setOpenExternalLinks( open );
}

void QLiteHtmlBrowser::setSearchPaths( const QStringList& paths )
{
  mImpl->setSearchPaths( paths );
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

bool QLiteHtmlBrowser::isBackwardAvailable() const
{
  return mImpl->isBackwardAvailable();
}

bool QLiteHtmlBrowser::isForwardAvailable() const
{
  return mImpl->isForwardAvailable();
}

void QLiteHtmlBrowser::clearHistory()
{
  mImpl->clearHistory();
}

QString QLiteHtmlBrowser::historyTitle( int idx ) const
{
  return mImpl->historyTitle( idx );
}

QUrl QLiteHtmlBrowser::historyUrl( int idx ) const
{
  return mImpl->historyUrl( idx );
}

int QLiteHtmlBrowser::backwardHistoryCount() const
{
  return mImpl->backwardHistoryCount();
}

int QLiteHtmlBrowser::forwardHistoryCount() const
{
  return mImpl->forwardHistoryCount();
}

void QLiteHtmlBrowser::backward()
{
  mImpl->backward();
}

void QLiteHtmlBrowser::forward()
{
  mImpl->forward();
}

const QString& QLiteHtmlBrowser::caption() const
{
  return mImpl->caption();
}

// void QLiteHtmlBrowser::initResources()
//{
//  Q_INIT_RESOURCE( QLiteHtmlBrowser );
//}
