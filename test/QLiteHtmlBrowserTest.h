
#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>

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

private:
  void                         createMainWindow( const QSize& );
  std::unique_ptr<QMainWindow> mWnd = nullptr;
};
