
#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>
#include "test_base.h"

class QLiteHtmlBrowser;

class HTMLContentTest : public TestBase
{
  Q_OBJECT
public:
  HTMLContentTest()
    : TestBase( qApp->arguments() ){};
  virtual ~HTMLContentTest() = default;

private Q_SLOTS:
  void init() { TestBase::init(); }
  void cleanup()
  {
    TestBase::cleanup();
    mWnd.reset();
    mWnd = nullptr;
  }
  void testCreation();
  void testHtml_data();
  void testHtml();
  void testHtml_Scroll_data();
  void testHtml_Scroll();

private:
  QLiteHtmlBrowser*            createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd         = nullptr;
  QSize                        mBrowserSize = { 800, 600 };
};
