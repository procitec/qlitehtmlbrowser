#include "test_css_content.h"
#include "QLiteHtmlBrowser.h"
#include "test_base.h"

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  HTMLCssTest mContentTest;
  QTest::qExec( &mContentTest, mContentTest.args() );
}

QLiteHtmlBrowser* HTMLCssTest::createMainWindow( const QSize& size )
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

void HTMLCssTest::testCSS_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "Centered single line of text" ) << R"-(<html><body><p style = "text-align:center">This is just text</body></html>)-";
}

void HTMLCssTest::testCSS()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}
