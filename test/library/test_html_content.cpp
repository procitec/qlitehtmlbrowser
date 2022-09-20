#include "test_html_content.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>
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
  browser->setSearchPaths( searchPaths() );
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

  QTest::newRow( "Simple local image <execute in SOURCE_DIR required>" ) << R"-(
  <!DOCTYPE html>
  <html>
  <body>
  <img src="images/16x16/arrow_up_green.png"/>
  </body>
  </html>
  )-";

  QTest::newRow( "invalid image" ) << R"-(
  <!DOCTYPE html>
  <html>
  <body>
  <img src="does_not_exist.png"/>
  </body>
  </html>
  )-";
}

void HTMLContentTest::test_img()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html, QUrl::fromLocalFile( QDir::currentPath() ).path() + "/" );
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
  qApp->setStyleSheet( "" );
}

void HTMLContentTest::test_zoom_reflow_data()
{
  const QString html(R"-(<!DOCTYPE html>
<html>
  <head>
    <title>Test page</title>
  </head>
  <body style="margin: 2em;">
    <p style="font-size: 24px;">Text must occupy all availble horizontal space. Horizontal scroll must <em>not</em> be possible as line wrapping is performed.</p>
    <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. In vel ornare massa, eget tempor urna. Aenean pretium massa non iaculis consequat. In vel rutrum neque. Nulla condimentum dolor id tincidunt suscipit. Ut molestie aliquet turpis, id maximus ipsum pharetra quis. Vivamus dignissim aliquet est, ac iaculis quam auctor eget. Nam venenatis, justo a eleifend vulputate, dui lacus tristique sem, a finibus risus lectus sit amet urna. Nullam scelerisque commodo elementum. Nulla ut semper est. Donec cursus accumsan purus, nec bibendum massa porttitor eu. Ut nec lacus accumsan, vehicula tortor at, vulputate lorem.</p>
    <p>Integer nisl dolor, sagittis sed magna id, malesuada eleifend nisl. Nam pharetra nisl ac dictum lobortis. Nunc elementum mauris felis, vitae accumsan sem tincidunt nec. Ut placerat ut felis non condimentum. Nullam a arcu at ex tincidunt fermentum. Vestibulum auctor arcu a nisl molestie, sit amet efficitur nunc vestibulum. Donec placerat id purus eget consectetur. Quisque ornare leo eget lorem malesuada fermentum. Curabitur fermentum ultricies nibh, nec aliquam quam dapibus et.</p>
    <p>Etiam mollis augue risus, ac finibus felis commodo eget. Nunc nec facilisis lacus. Fusce pretium metus eu ante ullamcorper dignissim. Aliquam in ullamcorper odio, a congue diam. Proin nec enim eu metus sagittis gravida ac sit amet mi. Sed cursus urna non justo fermentum efficitur. Maecenas efficitur est ac cursus feugiat. Sed sem mauris, varius vitae ipsum in, viverra dapibus orci. Etiam vitae volutpat dolor. Curabitur tincidunt mauris quis libero pulvinar, at maximus nisl tincidunt. Vestibulum laoreet scelerisque metus, vitae ornare erat porttitor eget.</p>
    <p>Nulla aliquet lobortis est in euismod. Nullam nec varius enim. Suspendisse convallis mi venenatis neque ornare, et tempus leo semper. Vivamus vitae erat ac erat tempor congue non quis elit. Nam euismod volutpat turpis, et eleifend diam vestibulum a. Ut mattis fermentum consectetur. Vestibulum elit nulla, malesuada vitae ante sit amet, sagittis hendrerit dolor. Nullam a purus pharetra, lobortis est ac, feugiat sapien. Nulla est mauris, rutrum sit amet accumsan at, vehicula sit amet turpis. Pellentesque in purus at est sollicitudin maximus. Vestibulum ac sapien sagittis, gravida velit a, maximus dolor. Phasellus aliquam ultrices neque, eget placerat diam aliquam quis. Cras ante nulla, faucibus vitae ex sit amet, dignissim hendrerit leo. Duis rutrum iaculis odio vel venenatis.</p>
    <p>Pellentesque eget justo sollicitudin, dapibus nibh at, blandit nunc. Nulla pretium lorem nunc, in convallis diam dictum a. Sed fringilla ultrices erat non hendrerit. Curabitur non justo scelerisque, imperdiet ligula et, rutrum odio. Ut sit amet tristique ligula. Quisque felis sapien, bibendum nec lorem sed, feugiat tristique tortor. Phasellus pharetra purus quam, vel dapibus leo rutrum sed. Morbi et porta massa. In ut massa consectetur tortor facilisis vehicula ac vitae nisi. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Aliquam mattis purus sed dui sollicitudin, a varius ligula varius. Donec at magna eleifend nisi facilisis fringilla. Suspendisse vel mi non nisi consequat commodo. Quisque placerat mauris luctus tristique finibus.</p>
  </body>
</html>
)-");

  const QString html_long_word(R"-(<!DOCTYPE html>
<html>
  <head>
    <title>Test page</title>
  </head>
  <body style="margin: 2em; font-size: 24px">
    <p>Text must occupy all availble horizontal space. Horizontal scroll must be possible if the paragraph below does <em>not</em> fit.</p>
    <p>abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789</p>
  </body>
</html>
)-");

  QTest::addColumn<QString>( "html" );
  QTest::addColumn<double>( "scale" );
  QTest::addRow( "test reflow zoom 0.75" ) << html << 0.75;
  QTest::addRow( "test reflow zoom 1.00" ) << html << 1.0;
  QTest::addRow( "test reflow zoom 1.25" ) << html << 1.25;
  QTest::addRow( "test reflow long zoom 0.75" ) << html_long_word << 0.75;
  QTest::addRow( "test reflow long zoom 1.00" ) << html_long_word << 1.0;
  QTest::addRow( "test reflow long zoom 1.25" ) << html_long_word << 1.25;
}

void HTMLContentTest::test_zoom_reflow()
{
  QFETCH( QString, html );
  QFETCH( double, scale );
  auto browser = createMainWindow( mBrowserSize );
  browser->resize( 800, 600 );
  browser->setHtml( html );
  browser->setScale( scale );
}
