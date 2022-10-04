#pragma once

class container_qt;

#include "browserdefinitions.h"
#include <QtWidgets/QWidget>
#include <QtCore/QUrl>
#include <QtCore/QStack>

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

  void           setUrl( const QUrl& url, int resourceType = static_cast<int>( Browser::ResourceType::Unknown ), bool clearFWHist = true);
  void           setHtml( const QString& html, const QUrl& source_url = {} );
  QString        html() const;
  void           setScale( double scale );
  double         scale() const;
  const UrlType& url() const { return mUrl; }
  void           loadStyleSheet();
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

  // todo from there is the title?
  QString historyTitle( int ) const { return {}; }

  QUrl historyUrl( int ) const;
  int  backwardHistoryCount() const { return ( 1 < mBWHistStack.count() ) ? mBWHistStack.count() - 1 : 0; }
  int  forwardHistoryCount() const { return ( 0 < mFWHistStack.count() ) ? mFWHistStack.count() : 0; }

  const QString& caption() const;

protected:
  void changeEvent( QEvent* ) override;
  void mousePressEvent( QMouseEvent* ) override;

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );
  void anchorClicked( const QUrl& );

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
  void    onAnchorClicked( const QUrl& );
  QUrl    baseUrl( const QUrl& url ) const;
  void    parseUrl( const QUrl& url );
  QString readResourceCss( const QString& ) const;

  Q_DISABLE_COPY( QLiteHtmlBrowserImpl );
#if ( QT_VERSION >= QT_VERSION_CHECK( 5, 13, 0 ) )
  Q_DISABLE_MOVE( QLiteHtmlBrowserImpl );
#endif

  container_qt*                  mContainer = nullptr;
  QUrl                           mBaseUrl   = {};
  QString                        mCSS       = {};
  UrlType                        mUrl       = {};
  Browser::ResourceHandlerType   mResourceHandler;
  Browser::UrlResolveHandlerType mUrlResolveHandler;
  QStack<HistoryEntry>           mBWHistStack  = {};
  QStack<HistoryEntry>           mFWHistStack  = {};
  UrlType                        mHome         = {};
  QStringList                    mSearchPaths  = {};
  QStringList                    mValidSchemes = { "file", "qrc", "qthelp" };
};
