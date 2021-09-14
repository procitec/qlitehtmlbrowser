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
  void setSource( const QUrl& name );

  // methods available in QTextBrowser
  void   setHtml( const QString& html );
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
