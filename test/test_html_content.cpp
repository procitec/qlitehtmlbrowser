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

  // litehtml seems to support this via css only
  QTest::newRow( "ordered list uppercase Roman WILL_FAIL" ) << R"-(
  <html><body>
  <ol type="I">
    <li>Coffee</li>
    <li>Tea</li>
    <li>Milk</li>
  </ol> 
  </body></html>)-";

  // litehtml seems to support this via css only
  QTest::newRow( "ordered list lowercase alphabet WILL_FAIL " ) << R"-(
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

  QTest::newRow( "Simple local image <execute in SOURCE_DIR required>" ) << R"-(
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
  browser->setHtml( html, QUrl::fromLocalFile( QDir::currentPath() ).path() );
}

void HTMLContentTest::test_tables_data()
{
  /// https://www.w3schools.com/html/html_tables.asp
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "example table with header and rows" ) << R"-(
<!DOCTYPE html>
<html>
<body>
  
<h2>HTML Table</h2>
  
<table>
  <th>
    <th>Company</th>
    <th>Contact</th>
    <th>Country</th>
  </th>
  <tr>
    <td>Alfreds Futterkiste</td>
    <td>Maria Anders</td>
    <td>Germany</td>
  </tr>
  <tr>
    <td>Centro comercial Moctezuma</td>
    <td>Francisco Chang</td>
    <td>Mexico</td>
  </tr>
  <tr>
    <td>Ernst Handel</td>
    <td>Roland Mendel</td>
    <td>Austria</td>
  </tr>
  <tr>
    <td>Island Trading</td>
    <td>Helen Bennett</td>
    <td>UK</td>
  </tr>
  <tr>
    <td>Laughing Bacchus Winecellars</td>
    <td>Yoshi Tannamuri</td>
    <td>Canada</td>
  </tr>
  <tr>
    <td>Magazzini Alimentari Riuniti</td>
    <td>Giovanni Rovelli</td>
    <td>Italy</td>
  </tr>
</table>
  
</body>
</html>)-";
}

void HTMLContentTest::test_tables()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}
void HTMLContentTest::test_qstyles_data()
{
  /// https://www.w3schools.com/html/html_tables.asp
  QTest::addColumn<QString>( "html" );
  QTest::addColumn<QString>( "style" );
  QString fixed_html = R"-(
<!DOCTYPE html>
<html><body>
<h1>Header One</h1>
<p>This is the text</p>
</body></html>)-";

  // QTest::newRow( "Normal without qstyle " ) << fixed_html << QString( R"-()-" );
  QTest::newRow( "QWidget with darkmode " ) << fixed_html << QString( R"-(
QWidget {
  background-color: #19232D;
  border: 0px solid #32414B;
  padding: 0px;
  color: #F0F0F0;
  selection-background-color: #1464A0;
  selection-color: #F0F0F0;
})-" );
}

void HTMLContentTest::test_qstyles()
{
  QFETCH( QString, html );
  QFETCH( QString, style );
  qApp->setStyleSheet( style );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}
