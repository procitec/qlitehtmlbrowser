#pragma once

#include <memory>
#include <iostream>
#include <QtCore/QStringList>
#include <QtCore/QObject>
#include <QtTest/QTest>

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

private:
  QStringList mArgs        = {};
  QStringList mSearchPaths = {};
  bool        mInteractive = true;
};
