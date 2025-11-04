#pragma once

#include <iostream>
#include <QtCore/QStringList>
#include <QtCore/QObject>
#include <QtTest/QTest>
#include <QtGui/QFontDatabase>

class TestBase : public QObject
{
  Q_OBJECT
public:
  TestBase( const QStringList& args, bool interactive = true )
    : mInteractive( interactive )
  {
    parseArgs( args );
  }

  TestBase() { parseArgs( qApp->arguments() ); }
  virtual ~TestBase() {}

  //  QArgsToStdArgs& args() { return mArgs; };
  QStringList&       args() { return mArgs; }
  const QStringList& searchPaths() const { return mSearchPaths; }

protected:
  void parseArgs( const QStringList& args )
  {
    QStringList ag;
    for ( auto idx = 0; idx < args.count(); idx++ )
    {
      if ( args[idx].startsWith( QLatin1String( "--interactive" ) ) )
      {
      }
      else if ( args[idx].startsWith( QLatin1String( "--searchpath=" ) ) )
      {
        mSearchPaths += args[idx].mid( QStringLiteral( "--searchpath=" ).length() );
      }
      else
      {
        ag.append( args[idx] );
      }
    }
    mArgs = ag;
  }
  void init() {}
  void cleanup()
  {
    QTest::qWait( 100 );
    if ( mInteractive && qApp->arguments().contains( QStringLiteral( "--interactive" ) ) )
    {
      auto* tag = QTest::currentDataTag();
      if ( tag )
      {
        std::cout << "verify test content, <return> to continue: " << tag << std::endl;
        std::cin.get();
      }
    }
  }

  bool loadTestFonts()
  {
    struct FontMetrics
    {
      QString family;
      int     pixelSize;
      int     textWidth;
      int     textHeight;

      bool operator==( const FontMetrics& other ) const
      {
        return family == other.family && pixelSize == other.pixelSize && qAbs( textWidth - other.textWidth ) <= 1 && // Toleranz von 2px
               qAbs( textHeight - other.textHeight ) <= 1;
      }
    };

    qputenv( "QT_FONT_DPI", "96" );

    // Fonts aus Qt-Resource laden
    QStringList fontResources = { ":/fonts/DejaVuSans.ttf", ":/fonts/DejaVuSans-Bold.ttf", ":/fonts/DejaVuSansMono.ttf" };

    bool    success = false;
    QString testFontFamily;
    for ( const QString& fontPath : fontResources )
    {
      // Prüfe ob Resource existiert
      QFile file( fontPath );
      if ( !file.exists() )
      {
        qWarning() << "Font resource not found:" << fontPath;
        continue;
      }

      int fontId = QFontDatabase::addApplicationFont( fontPath );

      if ( fontId != -1 )
      {
        QStringList families = QFontDatabase::applicationFontFamilies( fontId );
        // Speichere die erste geladene Familie als Test-Font
        if ( testFontFamily.isEmpty() && !families.isEmpty() )
        {
          testFontFamily = families.first();
        }

        qDebug() << "Loaded font from resource:" << fontPath << "Families:" << families;
        success = true;
      }
    }

    QFont font( testFontFamily );
    font.setPixelSize( 12 );
    font.setHintingPreference( QFont::PreferNoHinting );
    font.setKerning( false );
    font.setStyleStrategy( QFont::PreferAntialias );

    qApp->setFont( font ); // set the DejaVue Font as default font for tests

    QFontMetrics metrics( font );

    QString     testText = "The quick brown fox";
    FontMetrics measured = { font.family(), font.pixelSize(), metrics.horizontalAdvance( testText ), metrics.height() };

    // Erwartete Werte (von Referenzplattform)
    FontMetrics expected = { "DejaVu Sans", 12,
                             121, // Diese Werte von einer Referenzplattform ermitteln
                             15 };

    // qDebug() << font.family();
    // qDebug() << font.pixelSize();
    // qDebug() << metrics.horizontalAdvance( testText );
    // qDebug() << metrics.height();

    success = measured == expected;

    return success;
  }

private:
  QStringList mArgs        = {};
  QStringList mSearchPaths = {};
  bool        mInteractive = true;
};
