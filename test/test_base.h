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
  TestBase( const QStringList& args )
  {
    auto ag = args;
    if ( ag.contains( "--interactive" ) )
    {
      ag.removeAll( "--interactive" );
      mArgs = ag;
    }
  }

  TestBase()
  {
    auto args = qApp->arguments();
    if ( args.contains( "--interactive" ) )
    {
      args.removeAll( "--interactive" );
      mArgs = args;
    }
  }
  virtual ~TestBase() {}

  //  QArgsToStdArgs& args() { return mArgs; };
  QStringList& args() { return mArgs; };

protected:
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
