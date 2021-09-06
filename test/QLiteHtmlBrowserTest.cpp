#include "QLiteHtmlBrowserTest.h"
#include "QLiteHtmlBrowser.h"

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>
#include <memory>

QTEST_MAIN( QLiteHtmlBrowserTest )

QLiteHtmlBrowser* QLiteHtmlBrowserTest::createMainWindow( const QSize& size )
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

void QLiteHtmlBrowserTest::testCreation()
{
  createMainWindow( QSize( 800, 600 ) );
  // QVERIFY( mWnd->centralWidget() );
}

void QLiteHtmlBrowserTest::testHtml_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "simple body with text" ) << "<html><body>This is just text</body></html>";
}

void QLiteHtmlBrowserTest::testHtml()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( QSize( 800, 600 ) );
  browser->setHtml( html );
  QTest::qWait( 1000 );
}
