#include "test_find.h"
#include <qlitehtmlbrowser/QLiteHtmlBrowser>
#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"
#include <QtWidgets/QMainWindow>
#include <QtTest/QTest>

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  FindTest mContentTest;
  return QTest::qExec( &mContentTest, mContentTest.args() );
}

bool FindTest::skipUITest() const
{
  auto skip = false;
  auto qpa  = qgetenv( "QT_QPA_PLATFORM" );
  if ( !qpa.isEmpty() )
  {
    auto value = QString::fromLocal8Bit( qpa );
    skip       = value != "xcb";
  }
  return skip;
}

QLiteHtmlBrowser* FindTest::createMainWindow( const QSize& size )
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

void FindTest::test_find_single_word_data()
{
  QTest::addColumn<QString>( "find_text" );
  QTest::addColumn<int>( "matches" );
  QTest::addRow( "%d", 0 ) << QString( "Sätze" ) << 40;
  QTest::addRow( "%d", 1 ) << QString( "Er" ) << 46;
  QTest::addRow( "%d", 2 ) << QString( "" ) << 0;
}

void FindTest::test_find_single_word()
{
  QFETCH( QString, find_text );
  QFETCH( int, matches );

  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );

  auto found = browser->findText( ( find_text ) );
  qApp->processEvents();
  QCOMPARE( found, matches );
}

void FindTest::test_find_phrase_data()
{
  QTest::addColumn<QString>( "find_text" );
  QTest::addColumn<int>( "matches" );
  QTest::addColumn<QList<QRect>>( "bounding_boxes" );
  QList<QRect> boxes = { { 252, 995, 151, 17 } };
  QTest::addRow( "%d", 0 ) << QString( "allgemeine Beobachtungen" ) << 1 << boxes;
  boxes = { { 0, 0, 0, 0 } };
  QTest::addRow( "%d", 0 ) << QString( "schliesst ab" ) << 0 << boxes;
}

void FindTest::test_find_phrase()
{
  QFETCH( QString, find_text );
  QFETCH( int, matches );
  QFETCH( QList<QRect>, bounding_boxes );

  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  QCOMPARE( browser->forwardHistoryCount(), 0 );
  QCOMPARE( browser->backwardHistoryCount(), 0 );
  auto base = QString{ TEST_SOURCE_DIR } + "/search/";

  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );
  auto found = browser->findText( find_text );
  qApp->processEvents();
  QCOMPARE( found, matches );

  if ( 0 < found )
  {
    if ( skipUITest() )
    {
      QSKIP( "skip tests due QT_QPA_PLATFORM_PLUGIN" );
    }

    auto container    = browser->mImpl->container();
    auto find_machtes = container->mFindMatches;
    for ( size_t i = 0; i < find_machtes.size(); ++i )
    {
      const auto& match = find_machtes[i];
      auto        bb    = QRect( match.bounding_box.x, match.bounding_box.y, match.bounding_box.width, match.bounding_box.height );
      if ( bb != bounding_boxes[i] )
      {
        auto img = container->grab(); // QMainWindow, QWidget usw.
        img.save( QString( "screenshot_ui_find_phrase_%1.png" ).arg( i ) );
      }

      QCOMPARE( bb, bounding_boxes[i] );
    }
  }
}

void FindTest::test_find_phrase_multi_element_data()
{
  QTest::addColumn<QString>( "find_text" );
  QTest::addColumn<int>( "matches" );
  QTest::addColumn<QList<QRect>>( "bounding_boxes" );
  QList<QRect> boxes = { { 8, 2145, 64, 46 } };
  QTest::addRow( "%d", 0 ) << QString( "schließt ab.Absatz 48" ) << 1 << boxes;
}

void FindTest::test_find_phrase_multi_element()
{
  QFETCH( QString, find_text );
  QFETCH( int, matches );
  QFETCH( QList<QRect>, bounding_boxes );

  auto browser = createMainWindow( mBrowserSize );
  // QVERIFY( mWnd->centralWidget() );

  auto base = QString{ TEST_SOURCE_DIR } + "/search/";
  browser->setSource( QUrl::fromLocalFile( base + "/LongGermanText.html" ) );

  auto found = browser->findText( find_text );
  qApp->processEvents();
  // QTest::qWaitForWindowExposed( browser->mImpl->container() );
  QCOMPARE( found, matches );
  if ( 0 < found )
  {
    if ( skipUITest() )
    {
      QSKIP( "skip tests due QT_QPA_PLATFORM_PLUGIN" );
    }

    auto container = browser->mImpl->container();
    qApp->processEvents();
    auto find_machtes = container->mFindMatches;
    for ( size_t i = 0; i < find_machtes.size(); ++i )
    {
      const auto& match = find_machtes[i];
      auto        bb    = QRect( match.bounding_box.x, match.bounding_box.y, match.bounding_box.width, match.bounding_box.height );
      if ( bb != bounding_boxes[i] )
      {
        auto img = container->grab(); // QMainWindow, QWidget usw.
        img.save( QDir::currentPath() + QString( "/screenshot_ui_multi_element_%1.png" ).arg( i ) );
      }

      QCOMPARE( bb, bounding_boxes[i] );
    }
  }
}
