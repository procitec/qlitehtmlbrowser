#pragma once

#include <QtWidgets/QWidget>

class QLiteHtmlBrowserImpl;
///
/// \brief The QLiteHtmlBrowser class
///
/// QLiteHtmlBrowser is the main widget to be embedded
/// inside other QWidgets / QMainWindow to show HTML/URL
/// content

class QLiteHtmlBrowser : public QWidget
{
  Q_OBJECT
  //  Q_PROPERTY( QUrl source MEMBER mSource )
public:
  /// create the widget with given parent or nullptr if used
  /// without any parent.
  QLiteHtmlBrowser( QWidget* parent = nullptr );

  /// set URL to given url. The URL may be an url to local file, qthelp, http etc.
  /// The URL could contain an anchor element. Currently parameters to URL like
  /// '?name=value are not explicitly supported.
  void setUrl( const QUrl& url );

  /// if setHtml is called with the html content,
  /// it is require to set the base directory if not in current working directory
  /// for the given html code to resolve file system dependencies.
  void setHtml( const QString& html, const QString& baseurl = QString() );

  /// set scaling factor for the widgets html content.
  /// 1.0 means normal view, 2.0 zooms in to 150%, 0.5 zooms out to 50%.
  /// The scaling is internally limited for max and min scaling.
  void setScale( double scale );

  /// return current scaling.
  double scale() const;

  /// return current url
  const QUrl& url() const;

protected:
  /// Used to load binary resources from given url. If your implementation needs
  /// to handle resources that are not located in the current filesystem, you
  /// have to reimplement this method to handle resource loading e.g. of qthelp
  /// urls via QHelpEngine or http urls with QNetworkRequest
  virtual QByteArray loadResource( const QUrl& /*url*/ );

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );

private:
  QLiteHtmlBrowserImpl* mImpl = nullptr;
};
