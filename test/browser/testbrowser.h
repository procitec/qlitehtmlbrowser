#pragma once

#include "QHelpBrowser.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLineEdit>
#include <QAction>
#include <QtCore/QDir>
#include <QtHelp/QHelpEngine>

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

  void setSearchPaths( const QStringList& );
  void loadQtHelp( const QString& qch_file );

protected:
  void openHtml();
  void openHelp();
  void loadHelp();
  void export2pdf();
  void searchText( const QString& text );
  void nextSearchResult();
  void previousSearchResult();

private:
  QHelpBrowser* mBrowser;
  QMenuBar      mMenu;
  QDir          mLastDirectory;
  QLineEdit*    mUrl        = nullptr;
  QLineEdit*    mSearchText = nullptr;
  QToolBar      mToolBar;
  QHelpEngine*  mHelpEngine           = nullptr;
  QAction*      mNextSearchResult     = nullptr;
  QAction*      mPreviousSearchResult = nullptr;
};
