
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
  void testCSS_data();
  void testCSS();

private:
  QLiteHtmlBrowser*            createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd         = nullptr;
  QSize                        mBrowserSize = { 800, 600 };
};
