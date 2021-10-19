#pragma once

#include "QDistBrowser.h"
#include <QtWidgets/QMainWindow>

class DistBrowser : public QMainWindow
{
  Q_OBJECT

public:
  DistBrowser();
  virtual ~DistBrowser();

protected:
private:
  QDistBrowser* mBrowser;
};
