
#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>

class QLiteHtmlBrowser;

class QLiteHtmlBrowserTest : public QObject
{
  Q_OBJECT
public:
  QLiteHtmlBrowserTest()          = default;
  virtual ~QLiteHtmlBrowserTest() = default;

private Q_SLOTS:
  void init() {}
  void cleanup()
  {
    mWnd.reset();
    mWnd = nullptr;
  }
  void testCreation();
  void testHtml_data();
  void testHtml();

private:
  QLiteHtmlBrowser*            createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd = nullptr;
};
