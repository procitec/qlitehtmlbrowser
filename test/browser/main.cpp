
#include "testbrowser.h"
#include <QtWidgets/QApplication>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  QCommandLineParser parser;
  parser.setApplicationDescription( "TestBrowser for the qlitehtml browser widget" );
  parser.addHelpOption();

  QCommandLineOption fileOption( QStringList() << "f"
                                               << "file",
                                 "Load html file <file> on startup.", "file" );
  QCommandLineOption dirOption( QStringList() << "d"
                                              << "dir",
                                "Set directory to load files from to <directory> on startup.", "dir" );

  QCommandLineOption pathOption( QStringList() << "s"
                                               << "searchpath",
                                 "Add <directory> to list of search paths.", "searchpath" );
  parser.addOption( fileOption );
  parser.addOption( dirOption );
  parser.addOption( pathOption );

  parser.process( app );

  QString     html_file    = parser.value( fileOption );
  QString     html_dir     = parser.value( dirOption );
  QStringList search_paths = parser.values( pathOption );

  TestBrowser mWnd;
  mWnd.show();
  app.setQuitOnLastWindowClosed( true );
  if ( !html_file.isEmpty() )
  {
    mWnd.loadHtml( html_file );
  }
  if ( !html_dir.isEmpty() )
  {
    mWnd.setLastDirectory( html_dir );
  }

  mWnd.setSearchPaths( search_paths );
  auto ret = app.exec();

  return ret;
}
