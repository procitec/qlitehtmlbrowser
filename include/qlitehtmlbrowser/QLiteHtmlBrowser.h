#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>

class container_qt;

class QLiteHtmlBrowser : public QWidget
{
  Q_OBJECT
  Q_PROPERTY( QUrl source MEMBER mSource )
public:
  QLiteHtmlBrowser( QWidget* parent );

  QUrl source() const;
  void setUrl( const QUrl& name );
  /// if setHtml is called with the html content,
  /// it is require to set the base directory if not in current working directory
  /// for the given html code to resolve file system dependencies
  void setHtml( const QString& html, const QString& baseurl = QString() );

  void   setScale( double scale );
  double scale() const;

protected:
  void wheelEvent( QWheelEvent* ) override;

  virtual void _setSource( const QUrl& name );
  //  void         resizeEvent( QResizeEvent* ) override;
  void loadStyleSheet();

private:
  container_qt* mContainer = nullptr;
  QUrl          mSource;
  QString       mCSS;
};
