#pragma once

class container_qt;

#include "browserdefinitions.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStackedLayout>
#include <QtCore/QUrl>
#include <QtCore/QStack>
#include <QtGui/QPagedPaintDevice>

///
/// \brief The QLiteHtmlBrowser class
///
/// QLiteHtmlBrowser is the main widget to be embedded
/// inside other QWidgets / QMainWindow to show HTML/URL
/// content

class QLiteHtmlBrowserImpl : public QWidget
{
  Q_OBJECT
public:
  QLiteHtmlBrowserImpl( QWidget* parent = nullptr );
  virtual ~QLiteHtmlBrowserImpl();

  using UrlType = std::tuple<QUrl, int>;

  void           setUrl( const QUrl& url, int resourceType = static_cast<int>( Browser::ResourceType::Unknown ), bool clearFWHist = true );
  void           setHtml( const QString& html, const QUrl& source_url = {} );
  void           setCSS( const QString& css );
  QString        html() const;
  void           setScale( double scale );
  double         scale() const;
  const UrlType& url() const { return mUrl; }
  void           setResourceHandler( const Browser::ResourceHandlerType& rh );
  void           setUrlResolveHandler( const Browser::UrlResolveHandlerType& rh );
  QByteArray     loadResource( int type, const QUrl& url );
  QUrl           resolveUrl( const QString& );
  const UrlType& home() const { return mHome; }

  bool openLinks() const;
  void setOpenLinks( bool open );

  bool openExternalLinks() const;
  void setOpenExternalLinks( bool open );

  const QStringList& searchPaths() const { return mSearchPaths; }
  void               setSearchPaths( const QStringList& paths ) { mSearchPaths = paths; }

  bool isBackwardAvailable() const { return ( 1 < mBWHistStack.count() ); }
  bool isForwardAvailable() const { return ( 0 < mFWHistStack.count() ); }
  void clearHistory()
  {
    mBWHistStack.clear();
    mFWHistStack.clear();
  }

  void forward();
  void backward();
  void reload();

  int backwardHistoryCount() const { return ( 1 < mBWHistStack.count() ) ? mBWHistStack.count() - 1 : 0; }
  int forwardHistoryCount() const { return ( 0 < mFWHistStack.count() ) ? mFWHistStack.count() : 0; }

  const QString& caption() const;
  void           print( QPagedPaintDevice* printer ) const;

  int    findText( const QString& ); // search for given string and return number of found matches
  void   nextFindMatch();
  void   previousFindMatch();
  QColor highlightColor() const;
  void   setHighlightColor( QColor color );

  QString selectedText() const;

protected:
  void changeEvent( QEvent* ) override;
  void mousePressEvent( QMouseEvent* ) override;

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );
  void anchorClicked( const QUrl& );
  void scaleChanged();
  void selectionChanged();

#ifdef UNIT_TEST
  container_qt* container() const { return mContainer; }
#endif

private:
  class HistoryEntry
  {
  public:
    HistoryEntry() = default;
    HistoryEntry( const QUrl& name, int type, const QString& title )
    {
      url      = name;
      urlType  = type;
      urlTitle = title;
    }

    QUrl    url      = {};
    int     urlType  = static_cast<int>( Browser::ResourceType::Unknown );
    QString urlTitle = {};
    bool    operator==( const HistoryEntry& rhs ) { return ( rhs.url == url && rhs.urlType == urlType && rhs.urlTitle == urlTitle ); }
  };

  QString findFile( const QUrl& name ) const;
  void    showHtmlView();
  void    showImageView();
  void    onAnchorClicked( const QUrl& );
  QUrl    baseUrl( const QUrl& url ) const;
  void    parseUrl( const QUrl& url );
  QString readResourceCss( const QString& ) const;
  void    applyCSS();
  bool    isImageUrl( const QUrl& u ) const;
  bool    isHtmlUrl( const QUrl& u ) const;
  bool    onImageClicked( const QUrl& url );
  void    updateImageView();
  void    toggleImageZoomMode();
  // bool    imageFitsViewport( const QSize& imageSize ) const;
  QSize   imageViewportSize() const;
  qreal   imageDevicePixelRatio() const;
  QSize   imageNaturalDisplaySize() const;
  bool    imageFitsViewport( const QSize& imageSize ) const;
  QPixmap createRasterDisplayPixmap( const QSize& logicalSize, qreal devicePixelRatio ) const;
  QPixmap createSvgDisplayPixmap( const QSize& logicalSize, qreal devicePixelRatio ) const;
  QPixmap createDisplayPixmap( const QSize& logicalSize, qreal devicePixelRatio ) const;
  QImage  loadSvgFromFile( const QString& filename );
  QImage  loadSvgFromData( const QByteArray& data );
  bool    showImageFromData( const QUrl& url, const QByteArray& imageData );

  Q_DISABLE_COPY_MOVE( QLiteHtmlBrowserImpl );

  container_qt*                  mContainer             = nullptr;
  QStackedLayout*                mViewStack             = nullptr;
  QScrollArea*                   mImageScroll           = nullptr;
  QLabel*                        mImageLabel            = nullptr;
  QImage                         mCurrentImage          = {};
  QByteArray                     mCurrentSvgData        = {};
  QSize                          mCurrentSvgDefaultSize = {};
  bool                           mCurrentImageIsSvg     = false;
  bool                           mImageFitToView        = true;
  QUrl                           mBaseUrl               = {};
  QString                        mExternalCSS           = {};
  UrlType                        mUrl                   = {};
  Browser::ResourceHandlerType   mResourceHandler;
  Browser::UrlResolveHandlerType mUrlResolveHandler;
  QStack<HistoryEntry>           mBWHistStack    = {};
  QStack<HistoryEntry>           mFWHistStack    = {};
  UrlType                        mHome           = {};
  QString                        mCurrentCaption = {};
  QStringList                    mSearchPaths    = {};
  QStringList                    mValidSchemes   = { "file", "qrc", "qthelp" };
};
