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
  //  Q_OVERRIDE(bool modified SCRIPTABLE false)
  //  Q_OVERRIDE(bool readOnly DESIGNABLE false SCRIPTABLE false)
  //  Q_OVERRIDE(bool undoRedoEnabled DESIGNABLE false SCRIPTABLE false)
  //  Q_PROPERTY(QStringList searchPaths READ searchPaths WRITE setSearchPaths)
  //  Q_PROPERTY(bool openExternalLinks READ openExternalLinks WRITE setOpenExternalLinks)
  //  Q_PROPERTY(bool openLinks READ openLinks WRITE setOpenLinks)

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

  /// set URL to given url. The URL may be an url to local file, qthelp, http etc.
  /// The URL could contain an anchor element. Currently parameters to URL like
  /// '?name=value are not explicitly supported.
  /// Unlike QTextBrowser no automatic detection for different types (markdown, css)
  /// is supported, HTML only is supported.
  void setSource( const QUrl& url, const ResourceType& type = ResourceType::Unknown );

  /// if setHtml is called with the html content,
  /// it is require to set the base directory if not in current working directory
  /// for the given html code to resolve file system dependencies.
  void setHtml( const QString& html, const QString& baseurl = QString() );

  /// return current url
  QUrl source() const;

  /// Used to load binary resources from given url. If your implementation needs
  /// to handle resources that are not located in the current filesystem, you
  /// have to reimplement this method to handle resource loading e.g. of qthelp
  /// urls via QHelpEngine or http urls with QNetworkRequest
  virtual QByteArray loadResource( int, const QUrl& );

  // todo QTextBrowser
  //  QStringList searchPaths() const;
  //  void setSearchPaths(const QStringList &paths);

  //  bool isBackwardAvailable() const;
  //  bool isForwardAvailable() const;
  //  void clearHistory();
  //  QString historyTitle(int) const;
  //  QUrl historyUrl(int) const;
  //  int backwardHistoryCount() const;
  //  int forwardHistoryCount() const;

  //  bool openExternalLinks() const;
  //  void setOpenExternalLinks(bool open);

  //  bool openLinks() const;
  //  void setOpenLinks(bool open);

  // public Q_SLOTS:
  //#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
  //  virtual void setSource(const QUrl &name);
  //  void setSource(const QUrl &name, QTextDocument::ResourceType type);
  //#else
  //  void setSource(const QUrl &name, QTextDocument::ResourceType type = QTextDocument::UnknownResource);
  //#endif
  //  virtual void backward();
  //  virtual void forward();
  //  virtual void home();
  //  virtual void reload();

  // Q_SIGNALS:
  //  void backwardAvailable(bool);
  //  void forwardAvailable(bool);
  //  void historyChanged();
  //  void sourceChanged(const QUrl &);
  //  void highlighted(const QUrl &);
  //#if QT_DEPRECATED_SINCE(5, 15)
  //  QT_DEPRECATED_VERSION_X_5_15("Use QTextBrowser::highlighted(QUrl) instead")
  //  void highlighted(const QString &);
  //#endif
  //  void anchorClicked(const QUrl &);

  /// set scaling factor for the widgets html content.
  /// 1.0 means normal view, 2.0 zooms in to 150%, 0.5 zooms out to 50%.
  /// The scaling is internally limited for max and min scaling.
  /// Unlike QTextBrowser the scaling is not limited to Text only
  void setScale( double scale );

  /// return current scaling.
  double scale() const;

protected:
  // virtual QByteArray loadResource( const QUrl& /*url*/ );

  // todo QTextBrowser
  //  virtual void keyPressEvent(QKeyEvent *ev) override;
  //  virtual void mouseMoveEvent(QMouseEvent *ev) override;
  //  virtual void mousePressEvent(QMouseEvent *ev) override;
  //  virtual void mouseReleaseEvent(QMouseEvent *ev) override;
  //  virtual void focusOutEvent(QFocusEvent *ev) override;
  //  virtual bool focusNextPrevChild(bool next) override;
  //  virtual void paintEvent(QPaintEvent *e) override;
  //#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
  //  virtual
  //#endif
  //    void doSetSource(const QUrl &name, QTextDocument::ResourceType type = QTextDocument::UnknownResource);

Q_SIGNALS:
  /// emited when the url changed due to user interaction, e.g. link activation
  void urlChanged( const QUrl& );

private:
  QLiteHtmlBrowserImpl* mImpl = nullptr;
};
