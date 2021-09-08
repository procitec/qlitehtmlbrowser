#pragma once

#include <memory>
#include <cstring>
#include <iostream>
#include <functional>
#include <QtCore/QStringList>
#include <QtCore/QObject>
#include <QtTest/QTest>

class QArgsToStdArgs
{
public:
  explicit QArgsToStdArgs( const QStringList& args )
  {
    mArgsS.reserve( args.count() );
    for ( const auto& str : args )
    {
      std::cout << str.toStdString() << std::endl;
      mArgsS.push_back( str.toStdString() );
    }
    std::transform( mArgsS.begin(), mArgsS.end(), std::back_inserter( mArgsC ), convert );
  }
  int size() const { return mArgsC.size(); }

  operator char* *() { return &mArgsC[0]; }

private:
  QArgsToStdArgs( const QArgsToStdArgs& );
  QArgsToStdArgs& operator=( const QArgsToStdArgs& );

  static char* convert( const std::string& s )
  {
    //  char* pc = new char[s.size() + 1];
    //  std::strcpy( pc, s.c_str() );
    //  return pc;
    return const_cast<char*>( s.c_str() );
  }

  std::vector<std::string>( mArgsS );
  std::vector<char*> mArgsC;
};

class TestBase : public QObject
{
  Q_OBJECT
public:
  TestBase( const QStringList& args )
    : mArgs( args )
  {
  }
  virtual ~TestBase() {}

  QArgsToStdArgs& args() { return mArgs; };

protected:
  void init() {}
  void cleanup()
  {
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
  TestBase();

  QArgsToStdArgs mArgs;
};
