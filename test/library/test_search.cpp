#include "test_search.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  SearchTest mContentTest;
  return QTest::qExec( &mContentTest, mContentTest.args() );
}

QLiteHtmlBrowser* SearchTest::createMainWindow( const QSize& size )
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

void SearchTest::test_creation()
{
  auto browser = createMainWindow( mBrowserSize );
  browser->home();
}

void SearchTest::test_search_single_word()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->searchText( QStringLiteral( "Sätze" ) );
  QCOMPARE( found, 40 );

  found = browser->searchText( " Er" );
  QCOMPARE( found, 46 );
}

void SearchTest::test_search_phrase()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->searchText( "allgemeine Beobachtungen" );
  QCOMPARE( found, 1 );

  found = browser->searchText( "schliesst ab" );
  QCOMPARE( found, 0 );
}

void SearchTest::test_search_phrase_multi_element()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->searchText( QStringLiteral( "schließt ab. Absatz 48" ) );
  QCOMPARE( found, 1 );
}
