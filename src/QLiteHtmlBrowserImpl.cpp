#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QStackedLayout>
#include <QtGui/QWheelEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWindow>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QPalette>
#include <QtGui/QKeySequence>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QShortcut>
#include <QtGui/QDesktopServices>
#include <functional>
#include <utility>
#include <algorithm>
#include <cmath>
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
#include <QtGui/QShowEvent>

#include <QByteArray>
#include <QCursor>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QString>

namespace CursorUtils
{
inline QString zoomInSvg()
{
  return QStringLiteral( R"svg(
<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 64 64" fill="none">
  <title>Zoom In</title>
  <desc>Freely usable magnifying glass with plus symbol, styled for a standard Qt desktop application.</desc>
  <g stroke="#4A4A4A" stroke-width="4" stroke-linecap="round" stroke-linejoin="round">
    <circle cx="27" cy="27" r="16"/>
    <line x1="38.5" y1="38.5" x2="52" y2="52"/>
    <line x1="27" y1="20" x2="27" y2="34"/>
    <line x1="20" y1="27" x2="34" y2="27"/>
  </g>
</svg>
)svg" );
}

inline QString zoomOutSvg()
{
  return QStringLiteral( R"svg(
<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 64 64" fill="none">
  <title>Zoom Out</title>
  <desc>Freely usable magnifying glass with minus symbol, styled for a standard Qt desktop application.</desc>
  <g stroke="#4A4A4A" stroke-width="4" stroke-linecap="round" stroke-linejoin="round">
    <circle cx="27" cy="27" r="16"/>
    <line x1="38.5" y1="38.5" x2="52" y2="52"/>
    <line x1="20" y1="27" x2="34" y2="27"/>
  </g>
</svg>
)svg" );
}

inline QCursor createSvgCursor( const QString& svg, int size = 32, int hotX = 6, int hotY = 6 )
{
  QPixmap pixmap( size, size );
  pixmap.fill( Qt::transparent );

  QSvgRenderer renderer( svg.toUtf8() );
  QPainter     painter( &pixmap );
  painter.setRenderHint( QPainter::Antialiasing, true );
  painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
  renderer.render( &painter );
  painter.end();

  return QCursor( pixmap, hotX, hotY );
}

inline QCursor createZoomInCursor( int size = 32, int hotX = 6, int hotY = 6 )
{
  return createSvgCursor( zoomInSvg(), size, hotX, hotY );
}

inline QCursor createZoomOutCursor( int size = 32, int hotX = 6, int hotY = 6 )
{
  return createSvgCursor( zoomOutSvg(), size, hotX, hotY );
}
} // namespace CursorUtils

namespace
{

class ClickableImageLabel final : public QLabel
{
public:
  using ClickHandler = std::function<void()>;

  explicit ClickableImageLabel( QWidget* parent = nullptr )
    : QLabel( parent )
  {
  }

  void setClickHandler( ClickHandler handler ) { mClickHandler = std::move( handler ); }

protected:
  void mousePressEvent( QMouseEvent* event ) override
  {
    if ( event && event->button() == Qt::LeftButton )
    {
      mLeftButtonPressed = true;
      event->accept();
      return;
    }

    QLabel::mousePressEvent( event );
  }

  void mouseReleaseEvent( QMouseEvent* event ) override
  {
#if QT_VERSION_MAJOR >= 6
    const QPoint releasePos = event ? event->position().toPoint() : QPoint();
#else
    const QPoint releasePos = event ? event->pos() : QPoint();
#endif

    const bool activate = event && event->button() == Qt::LeftButton && mLeftButtonPressed && rect().contains( releasePos );
    mLeftButtonPressed  = false;

    if ( activate )
    {
      event->accept();
      if ( mClickHandler )
      {
        mClickHandler();
      }
      return;
    }

    QLabel::mouseReleaseEvent( event );
  }

private:
  ClickHandler mClickHandler      = {};
  bool         mLeftButtonPressed = false;
};

class ImageScrollArea final : public QScrollArea
{
public:
  using LayoutChangedHandler = std::function<void()>;

  explicit ImageScrollArea( QWidget* parent = nullptr )
    : QScrollArea( parent )
  {
  }

  void setLayoutChangedHandler( LayoutChangedHandler handler ) { mLayoutChangedHandler = std::move( handler ); }

protected:
  void resizeEvent( QResizeEvent* event ) override
  {
    QScrollArea::resizeEvent( event );
    if ( mLayoutChangedHandler )
    {
      mLayoutChangedHandler();
    }
  }

  void showEvent( QShowEvent* event ) override
  {
    QScrollArea::showEvent( event );
    if ( mLayoutChangedHandler )
    {
      mLayoutChangedHandler();
    }
  }

private:
  LayoutChangedHandler mLayoutChangedHandler = {};
};

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

QSize toLogicalPixels( const QSize& pixelSize, qreal dpr )
{
  const qreal safeDpr = dpr > 0.0 ? dpr : 1.0;
  return QSize( std::max( 1, static_cast<int>( std::ceil( pixelSize.width() / safeDpr ) ) ),
                std::max( 1, static_cast<int>( std::ceil( pixelSize.height() / safeDpr ) ) ) );
}

QSize toDevicePixels( const QSize& logicalSize, qreal dpr )
{
  const qreal safeDpr = dpr > 0.0 ? dpr : 1.0;
  return QSize( std::max( 1, static_cast<int>( std::round( logicalSize.width() * safeDpr ) ) ),
                std::max( 1, static_cast<int>( std::round( logicalSize.height() * safeDpr ) ) ) );
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

  auto* imageScroll = new ImageScrollArea( this );
  imageScroll->setWidgetResizable( false );
  imageScroll->setAlignment( Qt::AlignCenter );
  imageScroll->setLayoutChangedHandler(
    [this]()
    {
      if ( mViewStack && mImageScroll && mViewStack->currentWidget() == mImageScroll && mImageFitToView )
      {
        updateImageView();
      }
    } );

  auto* imageLabel = new ClickableImageLabel( imageScroll );
  imageLabel->setAlignment( Qt::AlignCenter );
  imageLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  imageLabel->setClickHandler( [this]() { toggleImageZoomMode(); } );

  imageScroll->setWidget( imageLabel );
  mImageScroll = imageScroll;
  mImageLabel  = imageLabel;

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

qreal QLiteHtmlBrowserImpl::imageDevicePixelRatio() const
{
  if ( mImageScroll && mImageScroll->viewport() )
  {
#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
    const qreal viewportDpr = mImageScroll->viewport()->devicePixelRatioF();
#else
    const qreal viewportDpr = mImageScroll->viewport()->devicePixelRatio();
#endif
    if ( viewportDpr > 0.0 )
    {
      return viewportDpr;
    }
  }

  if ( const auto* topLevel = window() )
  {
    if ( auto* handle = topLevel->windowHandle() )
    {
      if ( auto* currentScreen = handle->screen() )
      {
        const qreal screenDpr = currentScreen->devicePixelRatio();
        if ( screenDpr > 0.0 )
        {
          return screenDpr;
        }
      }
    }
  }

  if ( auto* currentScreen = QApplication::primaryScreen() )
  {
    const qreal screenDpr = currentScreen->devicePixelRatio();
    if ( screenDpr > 0.0 )
    {
      return screenDpr;
    }
  }

  return 1.0;
}

QSize QLiteHtmlBrowserImpl::imageNaturalDisplaySize() const
{
  if ( mCurrentImageIsSvg )
  {
    if ( mCurrentSvgDefaultSize.isValid() && !mCurrentSvgDefaultSize.isEmpty() )
    {
      return mCurrentSvgDefaultSize;
    }
    return QSize( 512, 512 );
  }

  if ( !mCurrentImage.isNull() )
  {
    return toLogicalPixels( mCurrentImage.size(), imageDevicePixelRatio() );
  }

  return {};
}

bool QLiteHtmlBrowserImpl::imageFitsViewport( const QSize& imageSize ) const
{
  const QSize viewportSize = imageViewportSize();
  return imageSize.width() <= viewportSize.width() && imageSize.height() <= viewportSize.height();
}

QPixmap QLiteHtmlBrowserImpl::createRasterDisplayPixmap( const QSize& logicalSize, qreal devicePixelRatio ) const
{
  if ( mCurrentImage.isNull() || !logicalSize.isValid() || logicalSize.isEmpty() )
  {
    return {};
  }

  const QSize targetPixelSize = toDevicePixels( logicalSize, devicePixelRatio );
  QImage      displayImage    = mCurrentImage;

  if ( displayImage.size() != targetPixelSize )
  {
    displayImage = mCurrentImage.scaled( targetPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
  }

  QPixmap displayPixmap = QPixmap::fromImage( displayImage );
  displayPixmap.setDevicePixelRatio( devicePixelRatio > 0.0 ? devicePixelRatio : 1.0 );
  return displayPixmap;
}

QPixmap QLiteHtmlBrowserImpl::createSvgDisplayPixmap( const QSize& logicalSize, qreal devicePixelRatio ) const
{
  if ( !mCurrentImageIsSvg || mCurrentSvgData.isEmpty() || !logicalSize.isValid() || logicalSize.isEmpty() )
  {
    return {};
  }

  QSvgRenderer renderer( mCurrentSvgData );
  if ( !renderer.isValid() )
  {
    return {};
  }

  const QSize targetPixelSize = toDevicePixels( logicalSize, devicePixelRatio );
  QImage      displayImage( targetPixelSize, QImage::Format_ARGB32_Premultiplied );
  displayImage.fill( Qt::transparent );

  QPainter painter( &displayImage );
  renderer.render( &painter, QRect( QPoint( 0, 0 ), targetPixelSize ) );
  painter.end();

  QPixmap displayPixmap = QPixmap::fromImage( displayImage );
  displayPixmap.setDevicePixelRatio( devicePixelRatio > 0.0 ? devicePixelRatio : 1.0 );
  return displayPixmap;
}

QPixmap QLiteHtmlBrowserImpl::createDisplayPixmap( const QSize& logicalSize, qreal devicePixelRatio ) const
{
  if ( mCurrentImageIsSvg )
  {
    return createSvgDisplayPixmap( logicalSize, devicePixelRatio );
  }

  return createRasterDisplayPixmap( logicalSize, devicePixelRatio );
}

void QLiteHtmlBrowserImpl::updateImageView()
{
  if ( !mImageLabel || !mImageScroll )
  {
    return;
  }

  if ( !mCurrentImageIsSvg && mCurrentImage.isNull() )
  {
    return;
  }

  QSize displayLogicalSize = imageNaturalDisplaySize();
  auto displayNaturalSize = displayLogicalSize;
  if ( !displayLogicalSize.isValid() || displayLogicalSize.isEmpty() )
  {
    return;
  }

  if ( mImageFitToView )
  {
    const QSize viewportSize = imageViewportSize();
    if ( viewportSize.isValid() && !imageFitsViewport( displayLogicalSize ) )
    {
      displayLogicalSize    = displayLogicalSize.scaled( viewportSize, Qt::KeepAspectRatio );
    }
  }

  const qreal   devicePixelRatio = imageDevicePixelRatio();
  const QPixmap displayPixmap    = createDisplayPixmap( displayLogicalSize, devicePixelRatio );
  if ( displayPixmap.isNull() )
  {
    return;
  }

  mImageLabel->setPixmap( displayPixmap );
  mImageLabel->resize( displayLogicalSize );
  mImageLabel->setMinimumSize( displayLogicalSize );
  mImageLabel->setMaximumSize( displayLogicalSize );
  mImageScroll->horizontalScrollBar()->setValue( 0 );
  mImageScroll->verticalScrollBar()->setValue( 0 );
  if ( imageFitsViewport( displayNaturalSize ) )
  {
    mImageScroll->unsetCursor();
  }
  else if ( mImageFitToView )
  {
    mImageScroll->setCursor( CursorUtils::createZoomInCursor() );
  }
  else
  {
    mImageScroll->setCursor( CursorUtils::createZoomOutCursor() );
  }
}

void QLiteHtmlBrowserImpl::toggleImageZoomMode()
{
  const QSize naturalDisplaySize = imageNaturalDisplaySize();
  if ( naturalDisplaySize.isEmpty() || imageFitsViewport( naturalDisplaySize ) )
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

  mCurrentImage = QImage();
  mCurrentSvgData.clear();
  mCurrentSvgDefaultSize = {};
  mCurrentImageIsSvg     = false;

  auto tryLoadSvg = [this]( const QByteArray& data ) -> bool
  {
    if ( data.isEmpty() )
    {
      return false;
    }

    QSvgRenderer renderer( data );
    if ( !renderer.isValid() )
    {
      return false;
    }

    mCurrentSvgData        = data;
    mCurrentSvgDefaultSize = renderer.defaultSize();
    if ( !mCurrentSvgDefaultSize.isValid() || mCurrentSvgDefaultSize.isEmpty() )
    {
      mCurrentSvgDefaultSize = QSize( 512, 512 );
    }
    mCurrentImageIsSvg = true;
    return true;
  };

  bool loaded = false;

  if ( !imageData.isEmpty() )
  {
    if ( ( isSvgUrl( url ) || looksLikeSvgData( imageData ) ) && tryLoadSvg( imageData ) )
    {
      loaded = true;
    }
    else
    {
      QImage img;
      img.loadFromData( imageData );
      if ( !img.isNull() )
      {
        mCurrentImage      = img;
        mCurrentImageIsSvg = false;
        loaded             = true;
      }
    }
  }

  if ( !loaded && url.isLocalFile() )
  {
    QFileInfo f( url.toLocalFile() );
    if ( f.exists() )
    {
      if ( isSvgUrl( url ) )
      {
        QFile svgFile( f.absoluteFilePath() );
        if ( svgFile.open( QIODevice::ReadOnly ) )
        {
          const QByteArray svgData = svgFile.readAll();
          svgFile.close();
          loaded = tryLoadSvg( svgData );
        }
      }

      if ( !loaded )
      {
        QImage img;
        if ( img.load( f.absoluteFilePath() ) )
        {
          mCurrentImage      = img;
          mCurrentImageIsSvg = false;
          loaded             = true;
        }
      }
    }
  }

  if ( !loaded )
  {
    return false;
  }

  mImageFitToView = true;

  QFileInfo info( normalizedUrlPath( url ) );
  mCurrentCaption = info.fileName().isEmpty() ? url.fileName() : info.fileName();
  if ( mCurrentCaption.isEmpty() )
  {
    mCurrentCaption = url.toDisplayString();
  }

  showImageView();
  updateImageView();
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
  if ( mImageScroll )
  {
    mImageScroll->unsetCursor();
  }
}

void QLiteHtmlBrowserImpl::showImageView()
{
  if ( mViewStack && mImageScroll )
  {
    const bool wasImageView = mViewStack->currentWidget() == mImageScroll;
    mViewStack->setCurrentWidget( mImageScroll );

    if ( wasImageView )
    {
      updateImageView();
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
