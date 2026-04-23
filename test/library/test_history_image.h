#pragma once

#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QImage>
#include <QSignalSpy>
#include <QUrl>

#include <qlitehtmlbrowser/QLiteHtmlBrowser>

class TestQLiteHtmlBrowserHistory : public QObject
{
  Q_OBJECT

private:
  QTemporaryDir mTempDir;

  QString writeFile( const QString& name, const QByteArray& content );

  QString createTestImage( const QString& name, const QSize& size = QSize( 320, 200 ) );

  QUrl createHtmlPageWithImageLink( const QString& htmlName, const QString& imageFileName );

private slots:
  void initTestCase() { QVERIFY( mTempDir.isValid() ); }

  void imageUrlIsAddedToHistory();
  void forwardHistoryIsClearedWhenNavigatingToNewTarget();
};
