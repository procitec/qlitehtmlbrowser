#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>

class container_qt;

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

  QUrl source() const;

  void        setUrl( const QUrl& url );
  void        setHtml( const QString& html, const QString& baseurl = QString() );
  void        setScale( double scale );
  double      scale() const;
  const QUrl& url() const { return mUrl; }
  void        setSource( const QUrl& name );
  void        loadStyleSheet();
  void        setResourceHandler( const std::function<QByteArray( QUrl )>& rh );

protected:
  void wheelEvent( QWheelEvent* ) override;
  void changeEvent( QEvent* ) override;

  //  virtual QByteArray loadResource( const QUrl& /*url*/ ) { return {}; }

  //  void         resizeEvent( QResizeEvent* ) override;

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );

private:
  Q_DISABLE_COPY( QLiteHtmlBrowserImpl );
  Q_DISABLE_MOVE( QLiteHtmlBrowserImpl );

  container_qt*                            mContainer = nullptr;
  QUrl                                     mSource;
  QString                                  mCSS;
  QUrl                                     mUrl;
  std::function<QByteArray( const QUrl& )> mResourceHandler;
};
