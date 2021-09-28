#pragma once

#include "QLiteHtmlBrowser.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QAction>
#include <QtCore/QDir>

class TestBrowser : public QMainWindow
{
  Q_OBJECT

public:
  TestBrowser();
  virtual ~TestBrowser();

  void loadHtml( const QString& html_file );
  void setLastDirectory( const QString& dir )
  {
    auto d = QDir( dir );
    if ( d.exists() )
    {
      mLastDirectory.setPath( d.absolutePath() );
    }
  }

protected:
  void openHtml();

private:
  QLiteHtmlBrowser* mBrowser;
  QMenuBar          mMenu;
  QDir              mLastDirectory;
  QLineEdit*        mUrl = nullptr;
  QToolBar          mToolBar;
};
