#include "test_css_content.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>
#include "test_base.h"

#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  Q_INIT_RESOURCE( QLiteHtmlBrowser );

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

void HTMLCssTest::test_align_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "Centered single line of text" ) << R"-(<html><body><p style = "text-align:center">This text should be centered</body></html>)-";
}

void HTMLCssTest::test_align()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLCssTest::test_fonts_data()
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

  QTest::newRow( "Font families" ) << R"-(
    <html><body>
      <p style="font-size:14px; font-family: Times">This should be 14px text times.</p>
      <p style="font-size:14px; font-family: monospace">This should be 14px text monospace.</p>
      <p style="font-size:14px; font-family: Helvetica">This should be 14px text helvetica.</p>
      <p style="font-size:14px; font-family: Arial">This should be 14px text arial.</p>
      <p style="font-size:14px; font-family: serif">This should be 14px text serif.</p>
      <p style="font-size:14px; font-family: sans-serif">This should be 14px text sans-serif.</p>
      <p style="font-size:14px; font-family: cursive">This should be 14px text cursive.</p>
      <p style="font-size:14px; font-family: typewriter">This should be 14px text typewriter.</p>
      <p style="font-size:14px; font-family: fantasy">This should be 14px text fantasy.</p>
      <p style="font-size:14px; font-family: system">This should be 14px text system.</p>
    </body></html>
    )-";

  /// @see https://www.w3schools.com/cssref/css_websafe_fonts.asp
  QTest::newRow( "Font families recommended" ) << R"-(
    <html><body>
      <p style="font-size:14px; font-family: Arial">This should be 14px text Arial.</p>
      <p style="font-size:14px; font-family: Verdana">This should be 14px text Verdana.</p>
      <p style="font-size:14px; font-family: Helvetica">This should be 14px text Helvetica.</p>
      <p style="font-size:14px; font-family: Tahoma">This should be 14px text Tahoma.</p>
      <p style="font-size:14px; font-family: 'Trebuchet MS'">This should be 14px text Trebuchet.</p>
      <p style="font-size:14px; font-family: 'Times New Roman'">This should be 14px text Times.</p>
      <p style="font-size:14px; font-family: 'Georgia'">This should be 14px text Georgia.</p>
      <p style="font-size:14px; font-family: 'Garamond'">This should be 14px text Garamond.</p>
      <p style="font-size:14px; font-family: 'Courier New'">This should be 14px text Courier New.</p>
      <p style="font-size:14px; font-family: 'Brush Script MT'">This should be 14px Brush.</p>
    </body></html>
    )-";
}

void HTMLCssTest::test_fonts()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLCssTest::test_lists_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "unordered list disc image " ) << R"-(
  <html><body>
  <ul style="list-style-type:disc">
    <li>US</li>
    <li>Australia</li>
    <li>New Zealand</li>
  </ul>
  </body></html>)-";
  QTest::newRow( "unordered list circle image " ) << R"-(
  <html><body>
  <ul style="list-style-type:circle">
    <li>US</li>
    <li>Australia</li>
    <li>New Zealand</li>
  </ul>
  </body></html>)-";
  QTest::newRow( "unordered list with square definition " ) << R"-(
  <html><body>
  <ul style="list-style-type:square">
    <li>US</li>
    <li>Australia</li>
    <li>New Zealand</li>
  </ul>
  </body></html>)-";

  QTest::newRow( "ordered list armenien type " ) << R"-(
  <html><body>
  <ol style="list-style-type:armenian">
    <li>Armenisch1</li>
    <li>Armenisch2</li>
    <li>Armenisch3</li>
  </ol>
  </body></html>)-";
}

void HTMLCssTest::test_lists()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

void HTMLCssTest::test_tables_data()
{
  /// https://www.w3schools.com/html/html_tables.asp
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "example table with alternating row cols " ) << R"-(
<!DOCTYPE html>
<html>
  <head>
  <style>
  table {
  font-family: arial, sans-serif;
  border-collapse: collapse;
  width: 100%;
}

td, th {
  border: 1px solid #dddddd;
  text-align: left;
  padding: 8px;
}

tr:nth-child(even) {
  background-color: #dddddd;
}
</style>
</head>
<body>
  
<h2>HTML Table</h2>
  
<table>
  <tr>
    <th>Company</th>
    <th>Contact</th>
    <th>Country</th>
  </tr>
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

  QTest::newRow( "example table with rounded borders " ) << R"-(
<!DOCTYPE html>
<html>
  <head>
  <style>

table {
    border-collapse:separate;
    border:solid black 1px;
    border-radius:6px;
    -moz-border-radius:6px;
}

td, th {
    border-left:solid black 1px;
    border-top:solid black 1px;
}

th {
    background-color: blue;
    border-top: none;
}

td:first-child, th:first-child {
     border-left: none;
}

</style>
</head>
<body>

<h2>HTML Table</h2>

<table>
  <tr>
    <th>Company</th>
    <th>Contact</th>
    <th>Country</th>
  </tr>
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
</html>
)-";
}

void HTMLCssTest::test_tables()
{
  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}

//<body>
//  <div style="text-transform:uppercase;">hello world (upper)</div>
//  <div style="text-transform:lowercase;">happy friday (lower)</div>
//  <div style="text-transform:capitalize;">
//    it's a nice day to code (capitalize)
//  </div>
//</body>

void HTMLCssTest::test_text_transform_data()
{
  QTest::addColumn<QString>( "html" );
  QTest::newRow( "HELLO WORLD (UPPER)" ) << R"-(<html><body>
      <div style="text-transform:uppercase;">hello world (upper)</div>
    </body></html>)-";
  QTest::newRow( "hello world (lower)" ) << R"-(<html><body>
      <div style="text-transform:lowercase;">Hello World (LOWER)</div>
    </body></html>)-";
  QTest::newRow( "It's A Nice Day To Code 01234 (Capitalize)" ) << R"-(<html><body>
    <div style="text-transform:capitalize;">it's a nice day to code 01234 (capitalize)</div>
  </body></html>)-";
}

void HTMLCssTest::test_text_transform()
{

  QFETCH( QString, html );
  auto browser = createMainWindow( mBrowserSize );
  browser->setHtml( html );
}
