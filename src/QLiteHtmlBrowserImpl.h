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

  void           setUrl( const QUrl& url, int resourceType = static_cast<int>( Browser::ResourceType::Unknown ), bool addToHistory = false );
  void           setHtml( const QString& html, const QUrl& source_url = {} );
  void           setScale( double scale );
  double         scale() const;
  const UrlType& url() const { return mUrl; }
  void           loadStyleSheet();
  void           setResourceHandler( const Browser::ResourceHandlerType& rh );
  QByteArray     loadResource( int type, const QUrl& url );
  const UrlType& home() const { return mHome; }

  bool openLinks() const;
  void setOpenLinks( bool open );

  bool openExternalLinks() const;
  void setOpenExternalLinks( bool open );

  const QStringList& searchPaths() const { return mSearchPaths; }
  void               setSearchPaths( const QStringList& paths ) { mSearchPaths = paths; }

  bool isBackwardAvailable() const { return ( 1 < mHistoryStack.count() ) && mHistoryStackIter != mHistoryStack.begin(); }
  bool isForwardAvailable() const { return ( 1 < mHistoryStack.count() ) && mHistoryStackIter != mHistoryStack.end(); }
  void clearHistory()
  {
    mHistoryStack.clear();
    mHistoryStackIter = mHistoryStack.begin();
  }

  void forward();
  void backward();

  // todo from there is the title?
  QString historyTitle( int ) const { return {}; }

  QUrl historyUrl( int ) const;
  int  backwardHistoryCount() const { return ( mHistoryStackIter - mHistoryStack.begin() ); }
  int  forwardHistoryCount() const { return ( mHistoryStack.end() - mHistoryStackIter ); }

protected:
  void wheelEvent( QWheelEvent* ) override;
  void changeEvent( QEvent* ) override;

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );
  void anchorClicked( const QUrl& );

private:
  class HistoryEntry
  {
  public:
    HistoryEntry( const QUrl& name, int type )
    {
      url     = name;
      urlType = type;
    }

    QUrl url     = {};
    int  urlType = static_cast<int>( Browser::ResourceType::Unknown );
  };

  QString findFile( const QUrl& name ) const;
  void    onUrlChanged( const QUrl& );

  Q_DISABLE_COPY( QLiteHtmlBrowserImpl );
  Q_DISABLE_MOVE( QLiteHtmlBrowserImpl );

  container_qt*                  mContainer = nullptr;
  QUrl                           mSource    = {};
  QString                        mCSS       = {};
  UrlType                        mUrl       = {};
  Browser::ResourceHandlerType   mResourceHandler;
  QStack<HistoryEntry>           mHistoryStack     = {};
  QStack<HistoryEntry>::iterator mHistoryStackIter = mHistoryStack.begin();
  UrlType                        mHome             = {};
  QStringList                    mSearchPaths      = {};
  QStringList                    mValidSchemes     = { "file", "qrc", "qthelp" };
};
