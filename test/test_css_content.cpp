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

void HTMLCssTest::testCSS_align_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "Centered single line of text" ) << R"-(<html><body><p style = "text-align:center">This text should be centered</body></html>)-";
}

void HTMLCssTest::testCSS_align()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLCssTest::testCSS_font_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "Red foreground color" ) << R"-(<html><body><p style="color:red">This should be red text.</p></body></html>)-";
  QTest::newRow( "Blue foreground color" ) << R"-(<html><body><p style="color:blue">This should be blue text.</p></body></html>)-";
  QTest::newRow( "Font size 11px to 20px" ) << R"-(
    <html><body>
      <p style="font-size:11px">This should be 11px text.</p>
      <p style="font-size:12px">This should be 12px text.</p>
      <p style="font-size:13px">This should be 13px text.</p>
      <p style="font-size:14px">This should be 14px text.</p>
      <p style="font-size:15px">This should be 15px text.</p>
      <p style="font-size:16px">This should be 16px text.</p>
      <p style="font-size:17px">This should be 17px text.</p>
      <p style="font-size:18px">This should be 18px text.</p>
      <p style="font-size:19px">This should be 19px text.</p>
      <p style="font-size:20px">This should be 20px text.</p>
    </body></html>
    )-";
}

void HTMLCssTest::testCSS_font()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}
