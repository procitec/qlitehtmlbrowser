#include "test_history.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  HistoryTest mContentTest;
  return QTest::qExec( &mContentTest, mContentTest.args() );
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
}

void HistoryTest::test_history()
{
  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/history/";

  browser->setSource( QUrl::fromLocalFile( base + "/url1.htm" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), false );
  QCOMPARE( browser->caption(), QLatin1String( "Url 1" ) );

  browser->setSource( QUrl::fromLocalFile( base + "/url2.htm" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 1 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );
  QCOMPARE( browser->caption(), QLatin1String( "Url 2" ) );

  browser->setSource( QUrl::fromLocalFile( base + "/url3.htm" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );
  QCOMPARE( browser->caption(), QLatin1String( "Url 3" ) );

  browser->reload();
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );
  QCOMPARE( browser->caption(), QLatin1String( "Url 3" ) );

  browser->backward(); // url2
  QCOMPARE( browser->forwardHistoryCount(), 1 );
  QCOMPARE( browser->backwardHistoryCount(), 1 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), true );
  QCOMPARE( browser->caption(), QLatin1String( "Url 2" ) );

  browser->backward(); // url1
  QCOMPARE( browser->forwardHistoryCount(), 2 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), true );
  QCOMPARE( browser->caption(), QLatin1String( "Url 1" ) );

  // this should not be possible!
  browser->backward(); // url1
  QCOMPARE( browser->forwardHistoryCount(), 2 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  QCOMPARE( browser->isBackwardAvailable(), false );
  QCOMPARE( browser->isForwardAvailable(), true );
  QCOMPARE( browser->caption(), QLatin1String( "Url 1" ) );

  browser->forward();
  QCOMPARE( browser->forwardHistoryCount(), 1 );
  QCOMPARE( browser->backwardHistoryCount(), 1 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), true );
  QCOMPARE( browser->caption(), QLatin1String( "Url 2" ) );

  browser->forward();
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );
  QCOMPARE( browser->caption(), QLatin1String( "Url 3" ) );

  // this should not be possible
  browser->forward();
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 2 );
  QCOMPARE( browser->isBackwardAvailable(), true );
  QCOMPARE( browser->isForwardAvailable(), false );
  QCOMPARE( browser->caption(), QLatin1String( "Url 3" ) );

  browser->home();
  QCOMPARE( browser->caption(), QLatin1String( "Url 1" ) );
  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 3 ); // increased due to home() call, this affects history
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
