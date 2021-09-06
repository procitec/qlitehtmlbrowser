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
  void setHtml( const QString& html );

protected:
  virtual void _setSource( const QUrl& name );
  //  void         resizeEvent( QResizeEvent* ) override;

private:
  container_qt* mContainer = nullptr;
  QUrl          mSource;
};
