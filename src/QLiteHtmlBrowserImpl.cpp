#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QStackedLayout>
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
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>
#include <QtCore/QBuffer>
#include <QtGui/QScreen>
#include <QtWidgets/QScrollBar>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QMimeType>
#include <QTimer>

namespace
{

QString normalizedUrlPath( const QUrl& url )
{
  if ( url.isLocalFile() )
  {
    return url.toLocalFile();
  }

  return url.path();
}

QString urlSuffix( const QUrl& url )
{
  // return QFileInfo( normalizedUrlPath( url ) ).suffix().toLower();
  return QFileInfo( url.path() ).suffix().toLower();
}

static bool hasHtmlExtension( const QUrl& url )
{
  const auto ext = urlSuffix( url );
  return ext == "html" || ext == "htm" || ext == "xhtml";
}

static bool hasImageExtension( const QUrl& url )
{
  static const QSet<QString> exts = { "png", "jpg", "jpeg", "gif", "svg", "bmp", "webp" };
  return exts.contains( urlSuffix( url ) );
}

bool isImageData( const QByteArray& data )
{
  if ( data.isEmpty() )
  {
    return false;
  }

  QBuffer buffer;
  buffer.setData( data );
  if ( !buffer.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  QImageReader reader( &buffer );
  reader.setDecideFormatFromContent( true );
  return reader.canRead();
}

bool looksLikeSvgData( const QByteArray& data )
{
  const auto head = QString::fromUtf8( data.left( 512 ) ).trimmed().toLower();
  return head.contains( "<svg" );
}

bool looksLikeHtmlData( const QByteArray& data )
{
  if ( data.isEmpty() )
  {
    return false;
  }

  const auto head = QString::fromUtf8( data.left( 1024 ) ).trimmed().toLower();

  return head.startsWith( "<!doctype html" ) || head.startsWith( "<html" ) || head.contains( "<html" ) || head.contains( "<head" ) ||
         head.contains( "<body" );
}

Browser::ResourceType detectResourceType( const QUrl& url, const QByteArray& content, int requestedType )
{
  const auto requested = static_cast<Browser::ResourceType>( requestedType );
  if ( requested != Browser::ResourceType::Unknown )
  {
    return requested;
  }

  if ( isImageData( content ) || looksLikeSvgData( content ) )
  {
    return Browser::ResourceType::Image;
  }

  if ( looksLikeHtmlData( content ) )
  {
    return Browser::ResourceType::Html;
  }

  if ( hasImageExtension( url ) )
  {
    return Browser::ResourceType::Image;
  }

  if ( hasHtmlExtension( url ) )
  {
    return Browser::ResourceType::Html;
  }

  if ( url.isLocalFile() )
  {
    QMimeDatabase db;
    const auto    mime = db.mimeTypeForFile( url.toLocalFile(), QMimeDatabase::MatchContent );

    if ( mime.inherits( "text/html" ) || mime.inherits( "application/xhtml+xml" ) )
    {
      return Browser::ResourceType::Html;
    }

    if ( mime.name().startsWith( "image/" ) )
    {
      return Browser::ResourceType::Image;
    }
  }

  return Browser::ResourceType::Unknown;
}

bool isSvgUrl( const QUrl& url )
{
  return urlSuffix( url ) == "svg";
}

} // namespace

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
  connect( mContainer, &container_qt::selectionChanged, this, &QLiteHtmlBrowserImpl::selectionChanged );

  mImageScroll = new QScrollArea( this );
  mImageScroll->setWidgetResizable( false );
  mImageScroll->setAlignment( Qt::AlignCenter );
  mImageLabel = new QLabel( mImageScroll );
  mImageLabel->setAlignment( Qt::AlignCenter );
  mImageLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  mImageLabel->installEventFilter( this );
  mImageScroll->setWidget( mImageLabel );

  auto* layout = new QVBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  mViewStack = new QStackedLayout;
  mViewStack->setContentsMargins( 0, 0, 0, 0 );
  mViewStack->addWidget( mContainer );
  mViewStack->addWidget( mImageScroll );
  layout->addLayout( mViewStack );
  setLayout( layout );
  showHtmlView();
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

void QLiteHtmlBrowserImpl::resizeEvent( QResizeEvent* ev )
{
  QWidget::resizeEvent( ev );

  if ( mViewStack && mImageScroll && mViewStack->currentWidget() == mImageScroll && mImageFitToView )
  {
    updateImageView();
  }
}

bool QLiteHtmlBrowserImpl::eventFilter( QObject* watched, QEvent* event )
{
  if ( watched == mImageLabel && event )
  {
    if ( event->type() == QEvent::MouseButtonRelease )
    {
      auto* mouseEvent = static_cast<QMouseEvent*>( event );
      if ( mouseEvent->button() == Qt::LeftButton && !mCurrentImagePixmap.isNull() )
      {
        toggleImageZoomMode();
        return true;
      }
    }
  }

  return QWidget::eventFilter( watched, event );
}

// void QLiteHtmlBrowser::resizeEvent( QResizeEvent* ev )
//{
//  if ( ev )
//  {
//    QWidget::resizeEvent( ev );
//  }
//}

bool QLiteHtmlBrowserImpl::isImageUrl( const QUrl& u ) const
{
  return hasImageExtension( u );
}

bool QLiteHtmlBrowserImpl::isHtmlUrl( const QUrl& u ) const
{
  return hasHtmlExtension( u );
}

void QLiteHtmlBrowserImpl::setUrl( const QUrl& url, int type, bool clearFWHist )
{
  if ( !mContainer )
  {
    return;
  }

  QUrl pureUrl( url );
  pureUrl.setFragment( {} );

  QByteArray content;

  if ( pureUrl.isLocalFile() )
  {
    QFile f( pureUrl.toLocalFile() );
    if ( f.open( QIODevice::ReadOnly ) )
    {
      content = f.readAll();
      f.close();
    }
  }
  else
  {
    // NICHT über findFile/loadResource laufen lassen.
    // qthelp:, http:, custom schemes etc. müssen hier bleiben.
    content = mResourceHandler( type, url );

    // optional: falls type Unknown ist und dein Handler den Typ braucht:
    if ( content.isEmpty() && type == static_cast<int>( Browser::ResourceType::Unknown ) )
    {
      if ( hasHtmlExtension( pureUrl ) )
      {
        content = mResourceHandler( static_cast<int>( Browser::ResourceType::Html ), url );
        type    = static_cast<int>( Browser::ResourceType::Html );
      }
      else if ( hasImageExtension( pureUrl ) )
      {
        content = mResourceHandler( static_cast<int>( Browser::ResourceType::Image ), url );
        type    = static_cast<int>( Browser::ResourceType::Image );
      }
    }
  }

  if ( content.isEmpty() )
  {
    // emit urlChanged( url );
    return;
  }

  const bool isHtml  = hasHtmlExtension( pureUrl ) || looksLikeHtmlData( content );
  const bool isImage = hasImageExtension( pureUrl ) || isImageData( content ) || looksLikeSvgData( content );

  if ( isHtml )
  {
    parseUrl( url );
    mContainer->setHtml( QString::fromUtf8( content ), url );
    mCurrentCaption = mContainer->caption();
    showHtmlView();
  }
  else if ( isImage )
  {
    if ( !showImageFromData( url, content ) )
    {
      return;
    }
  }
  else
  {
    return;
  }

  const int storedType = isHtml ? static_cast<int>( Browser::ResourceType::Html ) : isImage ? static_cast<int>( Browser::ResourceType::Image ) : type;

  mUrl = UrlType( url, storedType );

  auto [home_url, home_type] = mHome;
  if ( home_url.isEmpty() )
  {
    mHome = UrlType( url, storedType );
  }

  QUrl hist_url;
  if ( !mBWHistStack.isEmpty() )
  {
    hist_url = mBWHistStack.top().url;
  }

  if ( hist_url != url )
  {
    mBWHistStack.push( { url, storedType, caption() } );
  }

  if ( clearFWHist )
  {
    mFWHistStack.clear();
  }

  update();
  emit urlChanged( url );
}

QSize QLiteHtmlBrowserImpl::imageViewportSize() const
{
  if ( !mImageScroll )
  {
    return {};
  }

  QSize viewportSize = mImageScroll->viewport()->size();
  viewportSize -= QSize( 4, 4 );
  return viewportSize.expandedTo( QSize( 1, 1 ) );
}

bool QLiteHtmlBrowserImpl::imageFitsViewport( const QSize& imageSize ) const
{
  const QSize viewportSize = imageViewportSize();
  return imageSize.width() <= viewportSize.width() && imageSize.height() <= viewportSize.height();
}

void QLiteHtmlBrowserImpl::updateImageView()
{
  if ( !mImageLabel || !mImageScroll || mCurrentImagePixmap.isNull() )
  {
    return;
  }

  QPixmap displayPixmap = mCurrentImagePixmap;
  if ( mImageFitToView )
  {
    const QSize viewportSize = imageViewportSize();
    displayPixmap            = mCurrentImagePixmap.scaled( viewportSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
  }

  mImageLabel->setPixmap( displayPixmap );
  mImageLabel->resize( displayPixmap.size() );
  mImageLabel->setMinimumSize( displayPixmap.size() );
  mImageLabel->setMaximumSize( displayPixmap.size() );
  mImageScroll->horizontalScrollBar()->setValue( 0 );
  mImageScroll->verticalScrollBar()->setValue( 0 );
}

void QLiteHtmlBrowserImpl::toggleImageZoomMode()
{
  if ( mCurrentImagePixmap.isNull() || imageFitsViewport( mCurrentImagePixmap.size() ) )
  {
    return;
  }

  mImageFitToView = !mImageFitToView;
  updateImageView();
}

bool QLiteHtmlBrowserImpl::showImageFromData( const QUrl& url, const QByteArray& imageData )
{
  if ( !mImageLabel || !mImageScroll )
  {
    return false;
  }

  QImage img;
  if ( !imageData.isEmpty() )
  {
    img.loadFromData( imageData );
    if ( img.isNull() && ( isSvgUrl( url ) || looksLikeSvgData( imageData ) ) )
    {
      img = loadSvgFromData( imageData );
    }
  }

  if ( img.isNull() && url.isLocalFile() )
  {
    QFileInfo f( url.toLocalFile() );
    if ( f.exists() )
    {
      if ( !img.load( f.absoluteFilePath() ) && isSvgUrl( url ) )
      {
        img = loadSvgFromFile( f.absoluteFilePath() );
      }
    }
  }

  if ( img.isNull() )
  {
    return false;
  }

  mCurrentImagePixmap = QPixmap::fromImage( img );

  QFileInfo info( normalizedUrlPath( url ) );
  mCurrentCaption = info.fileName().isEmpty() ? url.fileName() : info.fileName();
  if ( mCurrentCaption.isEmpty() )
  {
    mCurrentCaption = url.toDisplayString();
  }

  showImageView();

  mImageFitToView = !imageFitsViewport( mCurrentImagePixmap.size() );
  updateImageView();

  // A second deferred update avoids using a stale viewport size directly after
  // switching the stacked widget page.
  QTimer::singleShot( 0, this,
                      [this]()
                      {
                        if ( mViewStack && mImageScroll && mViewStack->currentWidget() == mImageScroll && !mCurrentImagePixmap.isNull() )
                        {
                          if ( mImageFitToView )
                          {
                            updateImageView();
                          }
                        }
                      } );

  return true;
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
    mCurrentCaption = mContainer->caption();
    showHtmlView();
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

QImage QLiteHtmlBrowserImpl::loadSvgFromFile( const QString& filename )
{
  QSvgRenderer renderer;
  QImage       img;
  renderer.load( filename );
  if ( renderer.isValid() )
  {
    QSize size = renderer.defaultSize();
    if ( !size.isValid() || size.isEmpty() )
    {
      size = QSize( 512, 512 );
    }
    QImage svgImg( size, QImage::Format_ARGB32_Premultiplied );
    svgImg.fill( Qt::transparent );
    QPainter p( &svgImg );
    renderer.render( &p );
    img = svgImg;
  }
  return img;
}

QImage QLiteHtmlBrowserImpl::loadSvgFromData( const QByteArray& data )
{
  QSvgRenderer renderer;
  QImage       img;
  renderer.load( data );
  if ( renderer.isValid() )
  {
    QSize size = renderer.defaultSize();
    if ( !size.isValid() || size.isEmpty() )
    {
      size = QSize( 512, 512 );
    }
    QImage svgImg( size, QImage::Format_ARGB32_Premultiplied );
    svgImg.fill( Qt::transparent );
    QPainter p( &svgImg );
    renderer.render( &p );
    img = svgImg;
  }
  return img;
}

QByteArray QLiteHtmlBrowserImpl::loadResource( int type, const QUrl& url )
{
  QByteArray data;

  auto resource_type = static_cast<Browser::ResourceType>( type );

  QString fileName = findFile( url );
  if ( !fileName.isEmpty() )
  {
    if ( resource_type == Browser::ResourceType::Image && fileName.toLower().endsWith( ".svg" ) )
    {
      auto    img    = loadSvgFromFile( fileName );
      auto    pixmap = QPixmap::fromImage( img );
      QBuffer buffer( &data );
      buffer.open( QIODevice::WriteOnly );
      pixmap.save( &buffer, "PNG" );
      buffer.close();
    }
    else
    {
      QFile f( fileName );
      if ( f.open( QFile::ReadOnly ) )
      {
        data = f.readAll();
        f.close();
      }
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

  if ( QFileInfo( resolved.toLocalFile() ).isReadable() )
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
  return mCurrentCaption;
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
QString QLiteHtmlBrowserImpl::selectedText() const
{
  QString text;
  if ( mContainer )
  {
    text = mContainer->selectedText();
  }
  return text;
}

void QLiteHtmlBrowserImpl::showHtmlView()
{
  if ( mViewStack && mContainer )
  {
    mViewStack->setCurrentWidget( mContainer );
  }
}

void QLiteHtmlBrowserImpl::showImageView()
{
  if ( mViewStack && mImageScroll )
  {
    mViewStack->setCurrentWidget( mImageScroll );

    // When switching from HTML to image view, the scroll area's viewport size can
    // still reflect the previously hidden state. Re-apply the scaling once the
    // layout has settled so fit-to-view uses the final viewport size.
    if ( !mCurrentImagePixmap.isNull() && mImageFitToView )
    {
      QTimer::singleShot( 0, this, [this]() { updateImageView(); } );
    }
  }
}

bool QLiteHtmlBrowserImpl::onImageClicked( const QUrl& url )
{
  QByteArray imageData = loadResource( static_cast<int>( Browser::ResourceType::Image ), url );

  if ( imageData.isEmpty() && url.isLocalFile() )
  {
    QFile f( url.toLocalFile() );
    if ( f.open( QIODevice::ReadOnly ) )
    {
      imageData = f.readAll();
      f.close();
    }
  }

  return showImageFromData( url, imageData );
}
