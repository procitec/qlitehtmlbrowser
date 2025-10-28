#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QPalette>
#include <QtGui/QKeySequence>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QShortcut>
#include <QtGui/QDesktopServices>
#include <functional>

QLiteHtmlBrowserImpl::QLiteHtmlBrowserImpl( QWidget* parent )
  : QWidget( parent )
{
  mContainer = new container_qt( this );
  connect( mContainer, &container_qt::anchorClicked, this, &QLiteHtmlBrowserImpl::onAnchorClicked, Qt::QueuedConnection );

  auto* shortcut = new QShortcut( QKeySequence{ QKeySequence::Forward }, this );
  connect( shortcut, &QShortcut::activated, this, &QLiteHtmlBrowserImpl::forward );

  shortcut = new QShortcut( QKeySequence{ QKeySequence::Back }, this );
  connect( shortcut, &QShortcut::activated, this, &QLiteHtmlBrowserImpl::backward );
  connect( mContainer, &container_qt::scaleChanged, this, &QLiteHtmlBrowserImpl::scaleChanged );

  auto* layout = new QVBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( mContainer );
  setLayout( layout );
  applyCSS();
}

void QLiteHtmlBrowserImpl::applyCSS()
{
  auto    css = readResourceCss( QString( ":/css/master.css" ) );
  QString user_css;

  auto dark_mode = palette().color( QPalette::Text ).lightness() > palette().color( QPalette::Base ).lightness();
  if ( dark_mode )
  {
    auto dark_css = readResourceCss( ":/styles/dark.css" );
    dark_css.replace( "@QPalette::Base@", palette().color( QPalette::Base ).name() );
    dark_css.replace( "@QPalette::Text@", palette().color( QPalette::Text ).name() );
    dark_css.replace( "@QPalette::Link@", palette().color( QPalette::Link ).name() );
    dark_css.replace( "@QPalette::LinkVisited@", palette().color( QPalette::LinkVisited ).name() );
    user_css = dark_css;
  }

  user_css += mExternalCSS;

  if ( mContainer )
  {
    mContainer->setCSS( css, user_css );
  }
}

void QLiteHtmlBrowserImpl::setCSS( const QString& css )
{
  mExternalCSS = css;
  applyCSS();
}

void QLiteHtmlBrowserImpl::setHighlightColor( QColor color )
{
  if ( mContainer )
  {
    mContainer->setHighlightColor( color );
  }
}

QColor QLiteHtmlBrowserImpl::highlightColor() const
{
  QColor color;
  if ( mContainer )
  {
    color = mContainer->highlightColor();
  }
  return color;
}

QString QLiteHtmlBrowserImpl::readResourceCss( const QString& resource ) const
{
  QString css;
  QFile   f( resource );
  if ( f.open( QIODevice::ReadOnly ) )
  {
    css = QString::fromUtf8( f.readAll() );
    f.close();
  }
  return css;
}

QLiteHtmlBrowserImpl::~QLiteHtmlBrowserImpl() {}

void QLiteHtmlBrowserImpl::onAnchorClicked( const QUrl& url )
{
  if ( mContainer->openLinks() )
  {
    if ( mContainer->openExternalLinks() )
    {
      if ( mValidSchemes.contains( url.scheme() ) )
      {
#ifndef QT_NO_DESKTOPSERVICES
        // todo open with QDesktopServices
        QDesktopServices::openUrl( url );
#endif
      }
      else
      {
        emit anchorClicked( url );
      }
    }
    else
    {
      setUrl( url );
      emit anchorClicked( url );
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

void QLiteHtmlBrowserImpl::setUrlResolveHandler( const Browser::UrlResolveHandlerType& rh )
{
  mUrlResolveHandler = rh;
  mContainer->setUrlResolveHandler( rh );
}

void QLiteHtmlBrowserImpl::changeEvent( QEvent* e )
{
  QWidget::changeEvent( e );
  if ( e && e->type() == QEvent::PaletteChange )
  {
    applyCSS();
  }
}

void QLiteHtmlBrowserImpl::mousePressEvent( QMouseEvent* e )
{
  if ( const auto button = e->button(); button == Qt::ForwardButton || button == Qt::BackButton )
  {
    button == Qt::ForwardButton ? forward() : backward();
    e->accept();
  }
  else
    e->ignore();
}

// void QLiteHtmlBrowser::resizeEvent( QResizeEvent* ev )
//{
//  if ( ev )
//  {
//    QWidget::resizeEvent( ev );
//  }
//}

void QLiteHtmlBrowserImpl::setUrl( const QUrl& url, int type, bool clearFWHist )
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
      parseUrl( url );
      mContainer->setHtml( html, url );

      auto hist_url = QUrl();

      if ( !mBWHistStack.isEmpty() )
      {
        hist_url = mBWHistStack.top().url;
      }

      if ( hist_url != url )
      {
        mBWHistStack.push( { url, type, mContainer->caption() } );
      }

      if ( clearFWHist )
        mFWHistStack.clear();

      update();
    }

    emit urlChanged( url );
  }
}

QUrl QLiteHtmlBrowserImpl::baseUrl( const QUrl& url ) const
{
  // determine base for the given url
  // e.g. for file urls with filename and fragement but also for
  // pure directory urls
  // in context of clean urls this is normaly not easy do define, see
  // https://en.wikipedia.org/wiki/Clean_URL#Slug
  // we use the RemoveFilename part of QUrl to remove everything right of the last slash
  auto base = url.adjusted( QUrl::RemoveFilename );
  return base;
}

void QLiteHtmlBrowserImpl::setHtml( const QString& html, const QUrl& source_url )
{
  if ( mContainer )
  {
    parseUrl( source_url );
    mContainer->setHtml( html, source_url );
  }
}

void QLiteHtmlBrowserImpl::parseUrl( const QUrl& url )
{
  auto _url = url;
  if ( _url.isEmpty() )
  {
    _url = QUrl::fromLocalFile( QDir::currentPath() ).path() + "/";
  }

  _url.setFragment( {} );
  mBaseUrl = baseUrl( _url );
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

QByteArray QLiteHtmlBrowserImpl::loadResource( int /*type*/, const QUrl& url )
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

QUrl QLiteHtmlBrowserImpl::resolveUrl( const QString& url )
{

  QUrl resolved;

  /// used to resolve a given url. The base implementation tries to load relatives url
  /// 2. From base_url of given url in setSource() or setHtml methods
  /// 3. From current directory
  /// 4. From given search paths via setSearchPaths()

  //  else
  //  {
  //    // todo: let resourceHandler callback do this, he has search paths defined!

  auto _url = QUrl( url );
  // due to QUrl documentation, this works only if _url has no scheme!
  _url.setScheme( QString() );

  if ( !mBaseUrl.isEmpty() )
  {
    resolved = QUrl( mBaseUrl ).resolved( _url );
  }
  if ( !resolved.isRelative() )
  {
    return resolved;
  }

  else if ( QFileInfo( resolved.toLocalFile() ).isReadable() )
  {
    return QUrl::fromLocalFile( resolved.toLocalFile() );
  }

  resolved = QUrl::fromLocalFile( QDir::currentPath() + QDir::separator() ).resolved( _url );

  if ( QFileInfo( resolved.toLocalFile() ).isReadable() )
  {
    return resolved;
  }

  resolved.clear();

  for ( auto path : searchPaths() )
  {
    if ( !path.endsWith( QLatin1Char( '/' ) ) )
      path.append( QLatin1Char( '/' ) );

    QFileInfo fi( path );
    if ( fi.exists() )
    {
      resolved = QUrl::fromLocalFile( fi.absolutePath() + QDir::separator() ).resolved( _url );

      if ( QFileInfo( resolved.toLocalFile() ).isReadable() )
      {
        break;
      }
    }
  }
  return resolved;
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

  if ( QFileInfo( fileName ).isReadable() )
    return fileName;

  fileName.clear();

  for ( auto path : searchPaths() )
  {
    if ( !path.endsWith( QLatin1Char( '/' ) ) )
      path.append( QLatin1Char( '/' ) );
    path.append( fileName );
    if ( QFileInfo( path ).isReadable() )
    {
      fileName = path;
      break;
    }
  }

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

void QLiteHtmlBrowserImpl::forward()
{
  if ( !mFWHistStack.isEmpty() )
  {
    auto entry = mFWHistStack.pop();
    setUrl( entry.url, entry.urlType, false /* don’t clear forward history */ );
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
    setUrl( entry.url, entry.urlType, false /* don’t clear forward history */ );
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

const QString& QLiteHtmlBrowserImpl::caption() const
{
  return mContainer->caption();
}

void QLiteHtmlBrowserImpl::print( QPagedPaintDevice* printer ) const
{
  if ( mContainer && printer )
  {
    mContainer->print( printer );
  }
}

int QLiteHtmlBrowserImpl::findText( const QString& phrase )
{
  auto ret = 0;
  if ( mContainer )
  {
    ret = mContainer->findText( phrase );
  }

  return ret;
}

void QLiteHtmlBrowserImpl::nextFindMatch()
{
  if ( mContainer )
  {
    mContainer->findNextMatch();
  }
}

void QLiteHtmlBrowserImpl::previousFindMatch()
{
  if ( mContainer )
  {
    mContainer->findPreviousMatch();
  }
}
