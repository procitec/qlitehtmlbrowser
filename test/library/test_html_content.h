
#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>
#include "test_base.h"

class QLiteHtmlBrowser;

class HTMLContentTest : public TestBase
{
  Q_OBJECT
public:
  HTMLContentTest()
    : TestBase( qApp->arguments() ) {};
  virtual ~HTMLContentTest() = default;

private Q_SLOTS:
  void init() { TestBase::init(); }
  void cleanup()
  {
    TestBase::cleanup();
    mWnd.reset();
    mWnd = nullptr;
  }
  void test_creation();
  void test_lists_data();
  void test_lists();
  void test_html_data();
  void test_html();
  //  void test_fonts_data();
  //  void test_fonts();
  void test_img_data();
  void test_img();
  void test_img_scale_data();
  void test_img_scale();
  void test_tables_data();
  void test_tables();
  void test_qstyles_data();
  void test_qstyles();
  void test_zoom_reflow_data();
  void test_zoom_reflow();

private:
  QLiteHtmlBrowser*            createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd         = nullptr;
  QSize                        mBrowserSize = { 800, 600 };
};
