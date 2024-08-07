
#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>
#include "test_base.h"

class QLiteHtmlBrowser;

class HTMLCssTest : public TestBase
{
  Q_OBJECT
public:
  HTMLCssTest()
    : TestBase( qApp->arguments() ){};
  virtual ~HTMLCssTest() = default;

private Q_SLOTS:
  void init() { TestBase::init(); }
  void cleanup()
  {
    TestBase::cleanup();
    mWnd.reset();
    mWnd = nullptr;
  }
  void test_align_data();
  void test_align();

  void test_fonts_data();
  void test_fonts();
  void test_lists_data();
  void test_lists();
  void test_tables_data();
  void test_tables();
  void test_text_transform_data();
  void test_text_transform();
  void test_custom_css_data();
  void test_custom_css();

private:
  QLiteHtmlBrowser*            createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd         = nullptr;
  QSize                        mBrowserSize = { 800, 600 };
};
