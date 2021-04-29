#include "QLiteHtmlBrowserTest.h"

#include "QLiteHtmlBrowser.h"

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>
#include <memory>

QTEST_MAIN( QLiteHtmlBrowserTest )

void QLiteHtmlBrowserTest::createMainWindow( const QSize& size )
{
  mWnd         = std::make_unique<QMainWindow>();
  auto browser = new QLiteHtmlBrowser( nullptr );
  browser->setMinimumSize( size );
  mWnd->setCentralWidget( browser );
  mWnd->show();
}

void QLiteHtmlBrowserTest::testCreation()
{
  createMainWindow( QSize( 800, 600 ) );
  QTest::qWait( 1000 );
  // QVERIFY( mWnd->centralWidget() );
}
