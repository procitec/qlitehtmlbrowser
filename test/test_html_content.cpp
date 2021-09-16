#include "test_html_content.h"
#include "QLiteHtmlBrowser.h"
#include "test_base.h"

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  Q_INIT_RESOURCE( QLiteHtmlBrowser );

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

void HTMLContentTest::test_creation()
{
  createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );
}

void HTMLContentTest::test_html_data()
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

void HTMLContentTest::test_html()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLContentTest::test_fonts_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "Show 2 lines of text" ) << R"-(
  <!DOCTYPE html>
  <html>
  <head>
  <title>Font Face</title>
  </head>
  <body>
  <font face="Times New Roman" size="5" color="red">Times New Roman in red</font><br />
  <font face="Verdana" size="5">Verdana</font><br />
  <font face="Comic sans MS" size="5">Comic Sans MS</font><br />
  <font face="WildWest" size="5" color="blue">WildWest in blue</font><br />
  <font face="Bedrock" size="5">Bedrock</font><br />
  </body>
  </html>
  )-";
}

void HTMLContentTest::test_fonts()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLContentTest::test_lists_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "unordered list" ) << R"-(
  <html><body>
    <ul>
      <li>Coffee</li>
      <li>Tea</li>
      <li>Milk</li>
    </ul>
  </body></html>)-";
  QTest::newRow( "ordered list" ) << R"-(
  <html><body>
  <ol>
    <li>Coffee</li>
    <li>Tea</li>
    <li>Milk</li>
  </ol> 
  </body></html>)-";
  QTest::newRow( "description list" ) << R"-(
  <html><body>
   <dl>
    <dt>Coffee</dt>
    <dd>- black hot drink</dd>
    <dt>Milk</dt>
    <dd>- white cold drink</dd>
  </dl> 
  </body></html>)-";
  QTest::newRow( "ordered list uppercase Roman" ) << R"-(
  <html><body>
  <ol type="I">
    <li>Coffee</li>
    <li>Tea</li>
    <li>Milk</li>
  </ol> 
  </body></html>)-";
  QTest::newRow( "ordered list lowercase alphabet " ) << R"-(
  <html><body>
  <ol type="a">
    <li>Coffee</li>
    <li>Tea</li>
    <li>Milk</li>
  </ol> 
  </body></html>)-";
  QTest::newRow( "unordered list disc image " ) << R"-(
  <html><body>
  <ul style="list-style-type:disc">
    <li>US</li>
    <li>Australia</li>
    <li>New Zealand</li>
  </ul>
  </body></html>)-";
  QTest::newRow( "unordered list with string definition " ) << R"-(
  <html><body>
  <ul style="list-style-type:'ABC'">
    <li>US</li>
    <li>Australia</li>
    <li>New Zealand</li>
  </ul>
  </body></html>)-";
}

void HTMLContentTest::test_lists()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLContentTest::test_img_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "invalid image" ) << R"-(
  <!DOCTYPE html>
  <html>
  <body>
  <img src="does_not_exist.png"/>
  </body>
  </html>
  )-";

  QTest::newRow( "Simple local image" ) << R"-(
  <!DOCTYPE html>
  <html>
  <body>
  <img src="images/16x16/arrow_up_green.png"/>
  </body>
  </html>
  )-";
}

void HTMLContentTest::test_img()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setBaseUrl( QDir::currentPath() );
  browser->setHtml( html );
}
