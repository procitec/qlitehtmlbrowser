#pragma once

#include <qlitehtmlbrowser/QLiteHtmlBrowser>

#include <QtHelp/QHelpEngineCore>

class QDistBrowser : public QLiteHtmlBrowser
{
  Q_OBJECT
public:
  explicit QDistBrowser( QWidget* parent );
  void setHelpEnginge( QHelpEngineCore* engine ) { mHelpEngine = engine; }

protected:
  virtual QByteArray loadResource( int, const QUrl& ) override;

private:
  QHelpEngineCore* mHelpEngine = nullptr;
};
