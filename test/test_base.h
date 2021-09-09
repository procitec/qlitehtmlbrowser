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
  TestBase( const QStringList& args ) { parseArgs( args ); }

  TestBase() { parseArgs( qApp->arguments() ); }
  virtual ~TestBase() {}

  //  QArgsToStdArgs& args() { return mArgs; };
  QStringList& args() { return mArgs; };

protected:
  void parseArgs( const QStringList& args )
  {
    auto ag = args;
    if ( ag.contains( "--interactive" ) )
    {
      ag.removeAll( "--interactive" );
    }
    mArgs = ag;
  }
  void init() {}
  void cleanup()
  {
    QTest::qWait( 100 );
    if ( qApp->arguments().contains( QStringLiteral( "--interactive" ) ) )
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
  // QArgsToStdArgs mArgs;
  QStringList mArgs;
};
