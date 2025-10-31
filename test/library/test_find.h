
#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>
#include "test_base.h"

class QLiteHtmlBrowser;

class FindTest : public TestBase
{
  Q_OBJECT
public:
  FindTest()
    : TestBase( qApp->arguments() ){};
  virtual ~FindTest() = default;

private Q_SLOTS:
  void init() { TestBase::init(); }
  void cleanup()
  {
    TestBase::cleanup();
    mWnd.reset();
    mWnd = nullptr;
  }
  void test_find_single_word_data();
  void test_find_single_word();
  void test_find_phrase_data();
  void test_find_phrase();
  void test_find_phrase_multi_element_data();
  void test_find_phrase_multi_element();

private:
  QLiteHtmlBrowser*            createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd         = nullptr;
  QSize                        mBrowserSize = { 800, 600 };
};
