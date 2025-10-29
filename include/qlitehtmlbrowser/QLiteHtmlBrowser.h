#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>
#include <QtCore/QByteArray>

#include <QtCore/QtGlobal>
#include <QtGui/QPagedPaintDevice>

#if defined( QLITEHTMLBROWSER_LIBRARY )
#define QLITEHTMLBROWSER_EXPORT Q_DECL_EXPORT
#else
#define QLITEHTMLBROWSER_EXPORT Q_DECL_IMPORT
#endif

class QLiteHtmlBrowserImpl;
///
/// \brief The QLiteHtmlBrowser class
///
/// QLiteHtmlBrowser is the main widget to be embedded
/// inside other QWidgets / QMainWindow to show HTML/URL
/// content

class QLITEHTMLBROWSER_EXPORT QLiteHtmlBrowser : public QWidget
{
  Q_OBJECT

  /// property to access the url that is currently shown in the browser
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
  /// configure highlight color e.g. for search results
  Q_PROPERTY( QColor highlightColor READ highlightColor WRITE setHighlightColor DESIGNABLE true )

public:
  /// identifier for the type of resource that should get loaded as hint
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

  /// set a css as string. This css would be appended to the library provided css
  /// so the user can customize the default css by its own css.
  void setCSS( const QString& css );

  /// if setHtml is called with the html content,
  /// it is require to set the base directory if not in current working directory
  /// for the given html code to resolve file system dependencies. To avoid issues
  /// with CleanUrls the baseUrl must end with a '/' if baseurl is a directory
  /// If no baseurl is set, the baseurl will be set to the current working directory
  void setHtml( const QString& html, const QString& baseurl = QString() );

  /// return the loaded document source html
  QString html() const;

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
  /// have to re-implement this method to handle resource loading e.g. of QtHelp
  /// urls via QHelpEngine or http urls with QNetworkRequest
  virtual QByteArray loadResource( int, const QUrl& );

  /// used to resolve a given url. The base implementation tries to load relatives url
  /// 1. From base_url given in html element or base tag
  /// 2. From base_url of given url in setSource() or setHtml methods
  /// 3. From current directory
  /// 4. From given search paths via setSearchPaths()
  virtual QUrl resolveUrl( const QString& url );

  /// returns true if link navigation is enabled ( clicked links will be opened )
  /// anchorClicked is emitted in any case, sourceChanged is emitted only if enabled.
  /// Default is enabled.
  bool openLinks() const;

  /// set flag to false to disable link navigation
  void setOpenLinks( bool );

  /// returns true if links without support schemes (i.e. no file, qrc, QtHelp scheme)
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

  /// return true if history stack can go backward
  bool isBackwardAvailable() const;
  /// return true if history stack go go forward
  bool isForwardAvailable() const;
  /// clear the history
  void clearHistory();
  /// number of pages in history stack backwards
  int backwardHistoryCount() const;
  /// number of pages in history stack forwards
  int forwardHistoryCount() const;

  /// return title for url if available, else returns empty string
  const QString& caption() const;

  /// print document into paged paint device like a printer or pdf
  void print( QPagedPaintDevice* printer ) const;

  /// find text in document
  int findText( const QString& );
  ///  move to next match
  void findNextMatch();
  /// move to previous match
  void findPreviousMatch();

  /// configure interface for highlight color, i.e. to highlight found text
  QColor highlightColor() const;
  void   setHighlightColor( QColor color );

public Q_SLOTS:
  /// set URL to given url. The URL may be an url to local file, QtHelp, http etc.
  /// The URL could contain an anchor element. Currently parameters to URL like
  /// '?name=value are not explicitly supported.
  /// Unlike QTextBrowser no automatic detection for different types (markdown, css)
  /// is supported, HTML only is supported.
  virtual void setSource( const QUrl& name );
  void         setSource( const QUrl& url, ResourceType type );

  /// go backward in history stack
  virtual void backward();
  /// go forward in history stack
  virtual void forward();

  /// home url is the first url set via setSource for this instance.
  /// If not empty, home() reloads the url.
  virtual void home();

  /// reload current url
  virtual void reload();

protected:
Q_SIGNALS:

  /// emitted when the url changed due to user interaction, e.g. link activation, @see setOpenLinks
  void sourceChanged( const QUrl& );

  /// send when an anchor is clicked in the document, even if link is not activated, @see setOpenLinks
  void anchorClicked( const QUrl& );

  /// send when scale changes
  void scaleChanged();

  /// send when selection changes
  void selectionChanged( const QString& selectedText );

protected:
private:
  QLiteHtmlBrowserImpl* mImpl = nullptr;
};
