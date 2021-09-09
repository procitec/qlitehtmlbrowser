#include "test_html_content.h"
#include "QLiteHtmlBrowser.h"
#include "test_base.h"

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  HTMLContentTest mContentTest;
  QTest::qExec( &mContentTest, mContentTest.args() );
}

QLiteHtmlBrowser* HTMLContentTest::createMainWindow( const QSize& size )
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

void HTMLContentTest::testCreation()
{
  createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );
}

void HTMLContentTest::testHtml_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "Show a single line of text" ) << "<html><body>This is just text</body></html>";
  QTest::newRow( "Show 2 lines of text" ) << R"-(<html><body>this is first line<br>
  this is second line</body></html>)-";
  QTest::newRow( "Text formating text" ) << R"-(
<!DOCTYPE html>
<html>
<body>

<p><b>This text is bold</b></p>
<p><i>This text is italic</i></p>
<strong>This test is strong</strong></p>

</body>
</html>
)-";
}

void HTMLContentTest::testHtml()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLContentTest::testHtml_Scroll_data()
{
  QTest::addColumn<QString>( "html" );

  auto    lines = 1000;
  QString html  = QStringLiteral( "<html><body>" );
  for ( auto line = 0; line < lines; line++ )
  {
    html += QString( "<p>This is line %1</p>" ).arg( line );
  }
  html += QStringLiteral( "</body></html>" );

  QTest::newRow( QString( "Show %1 lines html with scroll bars" ).arg( lines ).toLocal8Bit().constData() ) << html.toLocal8Bit().constData();
}

void HTMLContentTest::testHtml_Scroll()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}
