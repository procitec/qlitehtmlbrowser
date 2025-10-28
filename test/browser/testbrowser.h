#pragma once

#include "QHelpBrowser.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAction>
#include <QtCore/QStringList>
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
  void findText( const QString& text );
  void nextFindMatch();
  void previousFindMatch();

private:
  QHelpBrowser* mBrowser;
  QMenuBar      mMenu;
  QDir          mLastDirectory;
  QLineEdit*    mUrl      = nullptr;
  QLineEdit*    mFindText = nullptr;
  QToolBar      mToolBar;
  QHelpEngine*  mHelpEngine        = nullptr;
  QAction*      mNextFindMatch     = nullptr;
  QAction*      mPreviousFindMatch = nullptr;
  QLabel*       mScale             = nullptr;
  QString       mScaleText         = tr( "Zoom: %1%" );
};
