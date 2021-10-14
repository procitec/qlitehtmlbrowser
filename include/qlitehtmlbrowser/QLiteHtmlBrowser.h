#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>
#include <QtCore/QByteArray>

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

  /// propery to access the url that is currently shown in the browser
  Q_PROPERTY( QUrl source READ source WRITE setSource )

  // todo QTextBrowser
  //  Q_PROPERTY(QTextDocument::ResourceType sourceType READ sourceType)

  // this will not be supported, because we use no QTextEdit and litehtml
  // does not support editing, and we use it as viewer only (e.g. no Form input)
  //  Q_OVERRIDE(bool modified SCRIPTABLE false)
  //  Q_OVERRIDE(bool readOnly DESIGNABLE false SCRIPTABLE false)
  //  Q_OVERRIDE(bool undoRedoEnabled DESIGNABLE false SCRIPTABLE false)

  /// set flag to false to disable link navigation. Default is enabled.
  Q_PROPERTY( bool openLinks READ openLinks WRITE setOpenLinks )
  /// set flag to true to enable link navigation of non-local url schemes via external programs
  /// determined by QDesktopServices
  Q_PROPERTY( bool openExternalLinks READ openExternalLinks WRITE setOpenExternalLinks )
  /// Set additional paths to resolve relative urls or resources. These paths are considered
  /// if the url could not get resolved against the given base url.
  Q_PROPERTY( QStringList searchPaths READ searchPaths WRITE setSearchPaths )

public:
  /// identifier for the type of resouce that should get loaded as hint
  /// for the implementation where to look for the resource
  /// In case of Unknown the resource should be loaded due to url scheme
  /// or url path properly
  enum class ResourceType : int
  {
    Unknown,
    Html,
    Image,
    Css
  };

  /// create the widget with given parent or nullptr if used
  /// without any parent.
  QLiteHtmlBrowser( QWidget* parent = nullptr );

  /// if setHtml is called with the html content,
  /// it is require to set the base directory if not in current working directory
  /// for the given html code to resolve file system dependencies. To avoid issues
  /// with CleanUrls the baseUrl must end with a '/' if baseurl is a directory
  /// If no baseurl is set, the baseurl will be set to the current working directory
  void setHtml( const QString& html, const QString& baseurl = QString() );

  /// return current url
  QUrl source() const;

  /// set scaling factor for the widgets html content.
  /// 1.0 means normal view, 2.0 zooms in to 150%, 0.5 zooms out to 50%.
  /// The scaling is internally limited for max and min scaling.
  /// Unlike QTextBrowser the scaling is not limited to Text only
  void setScale( double scale );

  /// return current scaling.
  double scale() const;

  /// Used to load binary resources from given url. If your implementation needs
  /// to handle resources that are not located in the current filesystem, you
  /// have to reimplement this method to handle resource loading e.g. of qthelp
  /// urls via QHelpEngine or http urls with QNetworkRequest
  virtual QByteArray loadResource( int, const QUrl& );

  /// returns true if link navigation is enabled ( clicked links will be opened )
  /// anchorClicked is emitted in any case, sourceChanged is emitted only if enabled.
  /// Default is enabled.
  bool openLinks() const;

  /// set flag to false to disable link navigation
  void setOpenLinks( bool );

  /// returns true if links without support schemes (i.e. no file, qrc, qthelp scheme)
  /// should be loaded via external program from QDesktopServices
  bool openExternalLinks() const;

  /// enable opening of urls with external program from QDesktopService.
  /// default is disabled.
  void setOpenExternalLinks( bool );

  /// return list of paths in the file system, there relative urls or resources are resolved
  /// if the content is set via setSource, the given base url is always considered first
  const QStringList& searchPaths() const;

  /// set file system paths where relative urls or resources are resolved
  void setSearchPaths( const QStringList& );

  // todo QTextBrowser navigation

  //  bool isBackwardAvailable() const;
  //  bool isForwardAvailable() const;
  //  void clearHistory();
  //  QString historyTitle(int) const;
  //  QUrl historyUrl(int) const;
  //  int backwardHistoryCount() const;
  //  int forwardHistoryCount() const;

public Q_SLOTS:
  /// set URL to given url. The URL may be an url to local file, qthelp, http etc.
  /// The URL could contain an anchor element. Currently parameters to URL like
  /// '?name=value are not explicitly supported.
  /// Unlike QTextBrowser no automatic detection for different types (markdown, css)
  /// is supported, HTML only is supported.
  virtual void setSource( const QUrl& url, const ResourceType& type = ResourceType::Unknown );

  //  virtual void backward();
  //  virtual void forward();

  /// home url is the first url set via setSource for this instance.
  /// If not empty, home() reloads the url.
  virtual void home();

  /// reload current url
  virtual void reload();

protected:
Q_SIGNALS:
  //  void backwardAvailable(bool);
  //  void forwardAvailable(bool);
  //  void historyChanged();

  /// emited when the url changed due to user interaction, e.g. link activation, @see setOpenLinks
  void sourceChanged( const QUrl& );

  /// send when an anchor is clicked in the document, even if link is not activated, @see setOpenLinks
  void anchorClicked( const QUrl& );

  // selection, copy, paste etc currently not implemented
  //  void highlighted(const QUrl &);

private:
  QLiteHtmlBrowserImpl* mImpl = nullptr;
};
