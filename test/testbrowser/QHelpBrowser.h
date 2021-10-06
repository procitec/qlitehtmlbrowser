#pragma once

#include "QLiteHtmlBrowser.h"

#include <QtHelp/QHelpEngineCore>

class QHelpBrowser : public QLiteHtmlBrowser
{
  Q_OBJECT
public:
  explicit QHelpBrowser( QWidget* parent );
  void setHelpEnginge( QHelpEngineCore* engine ) { mHelpEngine = engine; }

protected:
  virtual QByteArray loadResource( const QUrl& ) override;

private:
  QHelpEngineCore* mHelpEngine = nullptr;
};
