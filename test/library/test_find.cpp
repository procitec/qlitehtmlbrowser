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
  browser->resize( size );
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
  QTest::addColumn<QList<QList<QRect>>>( "bounding_boxes" );
#if defined( Q_OS_LINUX ) && QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
  QList<QList<QRect>> boxes = { { { 276, 909, 66, 15 }, { 342, 909, 4, 15 }, { 346, 909, 91, 15 } } };
#else
  QList<QList<QRect>> boxes = { { { 276, 866, 66, 14 }, { 342, 866, 4, 14 }, { 346, 866, 91, 14 } } };
#endif
  QTest::addRow( "%d", 0 ) << QString( "allgemeine Beobachtungen" ) << 1 << boxes;
  boxes = { { {} } };
  QTest::addRow( "%d", 0 ) << QString( "schliesst ab" ) << 0 << boxes;
}

void FindTest::test_find_phrase()
{
  QFETCH( QString, find_text );
  QFETCH( int, matches );
  QFETCH( QList<QList<QRect>>, bounding_boxes );

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
      for ( size_t j = 0; j < match.fragments.size(); ++j )
      {
        auto bb =
          QRect( ( match.fragments[j] ).pos.x, ( match.fragments[j] ).pos.y, ( match.fragments[j] ).pos.width, ( match.fragments[j] ).pos.height );
        QCOMPARE( bb, bounding_boxes[i][j] );
      }
    }
  }
}

void FindTest::test_find_phrase_multi_element_data()
{
  QTest::addColumn<QString>( "find_text" );
  QTest::addColumn<int>( "matches" );
  QTest::addColumn<QList<QList<QRect>>>( "bounding_boxes" );
  // these boxes are currently valid, but are wrong. See issues in qlitehtml project
#if defined( Q_OS_LINUX ) && QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
  QList<QList<QRect>> boxes = {
    { { 81, 1974, 47, 15 }, { 128, 1974, 4, 15 }, { 132, 1974, 19, 15 }, { 8, 2001, 40, 15 }, { 48, 2001, 4, 15 }, { 52, 2001, 12, 15 } } };
#else
  QList<QList<QRect>> boxes = { {
    { 81, 1880, 47, 14 },
    { 128, 1880, 4, 14 },
    { 132, 1880, 19, 14 },
    { 8, 1906, 40, 14 },
    { 48, 1906, 4, 14 },
    { 52, 1906, 12, 14 },
  } };
#endif
  QTest::addRow( "%d", 0 ) << QString( "schließt ab.Absatz 48" ) << 1 << boxes;
}

void FindTest::test_find_phrase_multi_element()
{
  QFETCH( QString, find_text );
  QFETCH( int, matches );
  QFETCH( QList<QList<QRect>>, bounding_boxes );

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
      for ( size_t j = 0; j < match.fragments.size(); ++j )
      {
        auto bb =
          QRect( ( match.fragments[j] ).pos.x, ( match.fragments[j] ).pos.y, ( match.fragments[j] ).pos.width, ( match.fragments[j] ).pos.height );
        QCOMPARE( bb, bounding_boxes[i][j] );
      }
    }
  }
}
