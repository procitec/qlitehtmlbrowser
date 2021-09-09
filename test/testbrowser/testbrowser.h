#pragma once

#include "QLiteHtmlBrowser.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtCore/QDir>

class TestBrowser : public QMainWindow
{
  Q_OBJECT

public:
  TestBrowser();
  virtual ~TestBrowser();

protected:
  void loadHtml();

private:
  QLiteHtmlBrowser* mBrowser;
  QMenuBar          mMenu;
  QAction           mLoadAction;
  QAction           mExitAction;
  QDir              mLastDirectory;
};
