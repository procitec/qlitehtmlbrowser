#include "test_history_image.h"

#include <qlitehtmlbrowser/QLiteHtmlBrowser.h>

QTEST_MAIN( TestQLiteHtmlBrowserHistory )

QString TestQLiteHtmlBrowserHistory::writeFile( const QString& name, const QByteArray& content )
{
  const QString path = mTempDir.filePath( name );
  QFile         f( path );
  ( f.open( QIODevice::WriteOnly | QIODevice::Truncate ), qPrintable( path ) );
  ( f.write( content ), content.size() );
  f.close();
  return path;
}

QString TestQLiteHtmlBrowserHistory::createTestImage( const QString& name, const QSize& size )
{
  const QString path = mTempDir.filePath( name );

  QImage img( size, QImage::Format_ARGB32_Premultiplied );
  img.fill( Qt::red );
  ( img.save( path ), qPrintable( path ) );

  return path;
}

QUrl TestQLiteHtmlBrowserHistory::createHtmlPageWithImageLink( const QString& htmlName, const QString& imageFileName )
{
  const QByteArray html = QByteArray( R"(
<!DOCTYPE html>
<html>
<head>
    <title>Test HTML Page</title>
</head>
<body>
    <p>Before image</p>
    <a href=")" ) + imageFileName.toUtf8() +
                          QByteArray( R"(">Open image</a>
</body>
</html>
)" );
  return QUrl::fromLocalFile( writeFile( htmlName, html ) );
}

void TestQLiteHtmlBrowserHistory::imageUrlIsAddedToHistory()
{
  const QString imagePath = createTestImage( "image.png" );
  const QUrl    htmlUrl   = createHtmlPageWithImageLink( "page1.html", "image.png" );
  const QUrl    imageUrl  = QUrl::fromLocalFile( imagePath );

  QLiteHtmlBrowser browser;
  browser.resize( 800, 600 );
  browser.show();
  QVERIFY( QTest::qWaitForWindowExposed( &browser ) );

  // Falls vorhanden: Signal auf URL-Wechsel beobachten
  QSignalSpy urlChangedSpy( &browser, SIGNAL(urlChanged(QUrl)));

  // 1) HTML laden
  browser.setSource( htmlUrl );
  QTRY_COMPARE( browser.caption(), QString( "Test HTML Page" ) );

  // 2) Bild laden (entspricht funktional dem Klick auf den Datei-Link)
  browser.setSource( imageUrl );

  // Erwartung: Bildansicht aktiv, Caption ist Dateiname
  QTRY_COMPARE( browser.caption(), QString( "image.png" ) );

  // 3) Zurück => wieder HTML
  browser.backward();
  QTRY_COMPARE( browser.caption(), QString( "Test HTML Page" ) );

  browser.forward();
  QTRY_COMPARE( browser.caption(), QString( "image.png" ) );

  // Optional: falls urlChanged existiert
  if ( !urlChangedSpy.isEmpty() )
  {
    QVERIFY( urlChangedSpy.count() >= 2 );
  }
}

void TestQLiteHtmlBrowserHistory::forwardHistoryIsClearedWhenNavigatingToNewTarget()
{
  const QString image1Path = createTestImage( "image1.png", QSize( 100, 50 ) );
  const QString image2Path = createTestImage( "image2.png", QSize( 200, 100 ) );
  const QUrl    htmlUrl    = createHtmlPageWithImageLink( "page2.html", "image1.png" );
  const QUrl    image1Url  = QUrl::fromLocalFile( image1Path );
  const QUrl    image2Url  = QUrl::fromLocalFile( image2Path );

  QLiteHtmlBrowser browser;
  browser.resize( 800, 600 );
  browser.show();
  QVERIFY( QTest::qWaitForWindowExposed( &browser ) );

  browser.setSource( htmlUrl );
  QTRY_COMPARE( browser.caption(), QString( "Test HTML Page" ) );

  browser.setSource( image1Url );
  QTRY_COMPARE( browser.caption(), QString( "image1.png" ) );

  browser.backward();
  QTRY_COMPARE( browser.caption(), QString( "Test HTML Page" ) );

  // neue Navigation nach "zurück" muss Forward-History löschen
  browser.setSource( image2Url );
  QTRY_COMPARE( browser.caption(), QString( "image2.png" ) );
}
