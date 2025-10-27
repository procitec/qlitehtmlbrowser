#include "test_find.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  FindTest mContentTest;
  return QTest::qExec( &mContentTest, mContentTest.args() );
}

QLiteHtmlBrowser* FindTest::createMainWindow( const QSize& size )
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

void FindTest::test_find_single_word()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->findText( ( "Sätze" ) );
  qApp->processEvents();
  QCOMPARE( found, 40 );

  found = browser->findText( ( " Er" ) );
  qApp->processEvents();
  QCOMPARE( found, 46 );

  found = browser->findText( "" );
  qApp->processEvents();
  QCOMPARE( found, 0 );
}

void FindTest::test_find_phrase()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->findText( "allgemeine Beobachtungen" );
  qApp->processEvents();
  QCOMPARE( found, 1 );

  found = browser->findText( "schliesst ab" );
  qApp->processEvents();
  QCOMPARE( found, 0 );
}

void FindTest::test_find_phrase_multi_element()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->findText( QStringLiteral( "schließt ab. Absatz 48" ) );
  qApp->processEvents();
  QCOMPARE( found, 1 );
}
