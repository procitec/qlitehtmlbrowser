#pragma once

#include "container_qt.h"

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>

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

  using ResourceHandlerType = container_qt::ResourceHandlerType;

  void        setUrl( const QUrl& url );
  void        setHtml( const QString& html, const QUrl& source_url = {} );
  void        setScale( double scale );
  double      scale() const;
  const QUrl& url() const { return mUrl; }
  void        loadStyleSheet();
  void        setResourceHandler( const ResourceHandlerType& rh );
  QByteArray  loadResource( int type, const QUrl& url );

protected:
  void wheelEvent( QWheelEvent* ) override;
  void changeEvent( QEvent* ) override;

  //  virtual QByteArray loadResource( const QUrl& /*url*/ ) { return {}; }

  //  void         resizeEvent( QResizeEvent* ) override;

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );

private:
  using ResourceType = container_qt::ResourceType;
  QString findFile( const QUrl& name ) const;
  int     mapToResourceType( const QString& scheme ) const;

  Q_DISABLE_COPY( QLiteHtmlBrowserImpl );
  Q_DISABLE_MOVE( QLiteHtmlBrowserImpl );

  container_qt*       mContainer = nullptr;
  QUrl                mSource;
  QString             mCSS;
  QUrl                mUrl;
  ResourceHandlerType mResourceHandler;
};
