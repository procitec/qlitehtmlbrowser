#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QPalette>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>

#include <functional>

extern const litehtml::tchar_t master_css[] = {
#include "master.css.inc"
};

QByteArray master_css_x = { master_css };

QLiteHtmlBrowserImpl::QLiteHtmlBrowserImpl( QWidget* parent )
  : QWidget( parent )
{
  setMinimumSize( 200, 200 );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer = new container_qt( this );
  connect( mContainer, &container_qt::anchorClicked, this, &QLiteHtmlBrowserImpl::onUrlChanged, Qt::QueuedConnection );

  connect(
    mContainer, &container_qt::urlChanged, this, [this]( const QUrl& url ) { emit urlChanged( url ); }, Qt::QueuedConnection );

  mContainer->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer->show();

  auto* layout = new QVBoxLayout;
  layout->addWidget( mContainer );

  setLayout( layout );

  mCSS = QString::fromUtf8( master_css_x );
  loadStyleSheet();
}

QLiteHtmlBrowserImpl::~QLiteHtmlBrowserImpl() {}

void QLiteHtmlBrowserImpl::onUrlChanged( const QUrl& url )
{
  if ( mContainer->openLinks() )
  {
    if ( mContainer->openExternalLinks() && mValidSchemes.contains( url.scheme() ) )
    {
      setUrl( url );
      emit urlChanged( url );
      emit anchorClicked( url );
    }
    else
    {
      // todo open with QDesktopServices
    }
  }
  else
  {
    emit anchorClicked( url );
  }
}

void QLiteHtmlBrowserImpl::setResourceHandler( const Browser::ResourceHandlerType& rh )
{
  mResourceHandler = rh;
  mContainer->setResourceHandler( rh );
}

void QLiteHtmlBrowserImpl::changeEvent( QEvent* e )
{
  QWidget::changeEvent( e );
  if ( e && e->type() == QEvent::PaletteChange )
  {
    auto lightness = this->palette().color( QPalette::Window ).lightnessF();
    bool dark_mode = ( lightness > 0.5 ) ? false : true;
    if ( dark_mode )
    {
      mCSS = QString::fromUtf8( master_css_x );
      QFile dark_css( ":/styles/dark.css" );
      dark_css.open( QIODevice::ReadOnly );
      if ( dark_css.isOpen() )
      {
        auto css = QString::fromUtf8( dark_css.readAll() );
        css.replace( "@QPalette::Window@", palette().color( QPalette::Window ).name() );
        css.replace( "@QPalette::Text@", palette().color( QPalette::Text ).name() );
        mCSS += css;
        loadStyleSheet();
      }
    }
  }
}

// void QLiteHtmlBrowser::resizeEvent( QResizeEvent* ev )
//{
//  if ( ev )
//  {
//    QWidget::resizeEvent( ev );
//  }
//}

void QLiteHtmlBrowserImpl::setUrl( const QUrl& url, int type )
{
  mUrl                       = UrlType( url, type );
  auto [home_url, home_type] = mHome;
  if ( home_url.isEmpty() )
  {
    mHome = UrlType( url, type );
  }

  if ( mContainer )
  {
    auto pure_url = QUrl( url );
    pure_url.setFragment( {} );
    QString html;

    if ( pure_url.isLocalFile() )
    {
      // get the content of the url and display the html

      QFile f( pure_url.toLocalFile() );
      if ( f.open( QIODevice::ReadOnly ) )
      {
        html = f.readAll();
        f.close();
      }
    }
    else
    {
      // eg. if ( url.scheme() == "qthelp" )
      html = mResourceHandler( type, url );
    }

    if ( !html.isEmpty() )
    {
      mContainer->setHtml( html, url );
      mBWHistStack.push( { url, type, mContainer->caption() } );

      update();
    }
  }
}

void QLiteHtmlBrowserImpl::setHtml( const QString& html, const QUrl& source_url )
{
  if ( mContainer )
  {
    mContainer->setHtml( html, source_url );
  }
}

QString QLiteHtmlBrowserImpl::html() const
{
  return ( mContainer ) ? mContainer->html() : QString();
}

// void QLiteHtmlBrowser::setUrl( const QUrl& url )
//{
//  if ( mContainer )
//  {
//    mContainer->setBaseDirectory( url.path() );
//    mContainer->setHtml()
//  }
//}

void QLiteHtmlBrowserImpl::loadStyleSheet()
{
  mContainer->setCSS( mCSS );
}

void QLiteHtmlBrowserImpl::setScale( double scale )
{
  if ( mContainer )
  {
    mContainer->setScale( scale );
  }
}

double QLiteHtmlBrowserImpl::scale() const
{
  double scale = 0.0;
  if ( mContainer )
  {
    scale = mContainer->scale();
  }
  return scale;
}

void QLiteHtmlBrowserImpl::wheelEvent( QWheelEvent* e )
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

QByteArray QLiteHtmlBrowserImpl::loadResource( int type, const QUrl& url )
{
  QByteArray data;

  QString fileName = findFile( url );
  if ( !fileName.isEmpty() )
  {
    QFile f( fileName );
    if ( f.open( QFile::ReadOnly ) )
    {
      data = f.readAll();
      f.close();
    }
  }

  return data;
}

QString QLiteHtmlBrowserImpl::findFile( const QUrl& name ) const
{
  QString fileName;
  if ( name.scheme().isEmpty() )
  {
    fileName = name.path();
  }
  else
  {
    fileName = name.toLocalFile();
  }

  if ( fileName.isEmpty() )
    return fileName;

  if ( QFileInfo( fileName ).isAbsolute() )
    return fileName;

  // TODO search paths relative to baseurl or document source

  //  for ( QString path : qAsConst( searchPaths ) )
  //  {
  //    if ( !path.endsWith( QLatin1Char( '/' ) ) )
  //      path.append( QLatin1Char( '/' ) );
  //    path.append( fileName );
  //    if ( QFileInfo( path ).isReadable() )
  //      return path;
  //  }

  return fileName;
}

bool QLiteHtmlBrowserImpl::openLinks() const
{
  return ( mContainer ) ? mContainer->openLinks() : false;
}

bool QLiteHtmlBrowserImpl::openExternalLinks() const
{
  return ( mContainer ) ? mContainer->openExternalLinks() : false;
}

void QLiteHtmlBrowserImpl::setOpenLinks( bool open )
{
  if ( mContainer )
    mContainer->setOpenLinks( open );
}

void QLiteHtmlBrowserImpl::setOpenExternalLinks( bool open )
{
  if ( mContainer )
  {
    mContainer->setOpenExternalLinks( open );
  }
}

QUrl QLiteHtmlBrowserImpl::historyUrl( int ) const
{
  // todo implement
  return {};
}

void QLiteHtmlBrowserImpl::forward()
{
  if ( !mFWHistStack.isEmpty() )
  {
    auto entry = mFWHistStack.pop();
    setUrl( entry.url, entry.urlType );
  }
}

void QLiteHtmlBrowserImpl::backward()
{
  if ( 1 < mBWHistStack.count() )
  {
    // this is current site
    auto entry = mBWHistStack.pop();
    mFWHistStack.push( { entry.url, entry.urlType, entry.urlTitle } );
    entry = mBWHistStack.pop();
    setUrl( entry.url, entry.urlType );
  }
}
void QLiteHtmlBrowserImpl::reload()
{
  if ( !mBWHistStack.isEmpty() )
  {
    auto entry = mBWHistStack.pop();
    setUrl( entry.url, entry.urlType );
  }
}
