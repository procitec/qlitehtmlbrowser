#include "test_selection.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>
#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"
#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  SelectionTest mContentTest;
  return QTest::qExec( &mContentTest, mContentTest.args() );
}

QLiteHtmlBrowser* SelectionTest::createMainWindow( const QSize& size )
{
  mWnd         = std::make_unique<QMainWindow>();
  auto browser = new QLiteHtmlBrowser( nullptr );
  mWnd->setCentralWidget( browser );
  browser->setMinimumSize( size );
  browser->update();
  browser->show();
  mWnd->show();
  return browser;
}

void SelectionTest::testSelectionText_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::addColumn<QString>( "selection" );

  QTest::addRow( "%d", 0 ) << QString( "simple_text.html" ) << QString( "This is pure text only in one sentence" );
  QTest::addRow( "%d", 1 ) << QString( "simple_inline.html" ) << QString( "This is bold text with one strong word" );
  QTest::addRow( "%d", 2 ) << QString( "simple_paragraph.html" )
                           << QString( "Abschnitt 1: Erste Zeile\n\nAbschnitt 2: Zweite Zeile\n\nAbschnitt 3: Dritte Zeile" );
  QTest::addRow( "%d", 3 )
    << QString( "simple_table.html" )
    << QString( "Company Contact Country\nAlfreds Futterkiste Maria Anders Germany\nCentro comercial Moctezuma Francisco Chang Mexico" );
}

void SelectionTest::testSelectionText()
{
  QFETCH( QString, html );
  QFETCH( QString, selection );

  auto browser = createMainWindow( mBrowserSize );
  auto base    = QString{ TEST_SOURCE_DIR } + "/selection/";

  browser->setSource( QUrl::fromLocalFile( base + "/" + html ) );

  auto container = browser->mImpl->container();

  container->selectAll();

  auto text = container->selectedText();
  QCOMPARE( text, selection );
}
