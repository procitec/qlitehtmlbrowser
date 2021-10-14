#include "test_history.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  Q_INIT_RESOURCE( QLiteHtmlBrowser );

  HistoryTest mContentTest;
  QTest::qExec( &mContentTest, mContentTest.args() );
}

QLiteHtmlBrowser* HistoryTest::createMainWindow( const QSize& size )
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

void HistoryTest::test_creation()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), false );
  for ( int idx = -3; idx < 3; idx++ )
  {
    QCOMPARE( browser->historyUrl( idx ), QUrl() );
  }
  // no url set, should not crash
  browser->home();
}

void HistoryTest::test_home()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), false );
  for ( int idx = -3; idx < 3; idx++ )
  {
    QCOMPARE( browser->historyUrl( idx ), QUrl() );
  }
}

void HistoryTest::test_history()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QDir::currentPath() + "/history/";

  browser->setSource( QUrl::fromLocalFile( base + "/url1.htm" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), false );

  browser->setSource( QUrl::fromLocalFile( base + "/url2.htm" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 1 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );

  browser->setSource( QUrl::fromLocalFile( base + "/url3.htm" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );

  browser->reload();
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );

  browser->backward(); // url2
  QCOMPARE( browser->forwardHistoryCount(), 1 );
  QCOMPARE( browser->backwardHistoryCount(), 1 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), true );

  browser->backward(); // url1
  QCOMPARE( browser->forwardHistoryCount(), 2 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), true );

  browser->forward();
  QCOMPARE( browser->forwardHistoryCount(), 1 );
  QCOMPARE( browser->backwardHistoryCount(), 1 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), true );

  browser->forward();
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );

  browser->clearHistory();
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), false );
}

// bool    isBackwardAvailable() const;
// bool    isForwardAvailable() const;
// void    clearHistory();
// QString historyTitle( int ) const;
// QUrl    historyUrl( int ) const;
// int     backwardHistoryCount() const;
// int     forwardHistoryCount() const;
