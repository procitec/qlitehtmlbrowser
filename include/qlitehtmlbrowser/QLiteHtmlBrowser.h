#pragma once

#include <QtWidgets/QWidget>

class container_qt;

class QLiteHtmlBrowser : public QWidget
{
public:
  QLiteHtmlBrowser( QWidget* parent );

private:
  container_qt* mContainer = nullptr;
};
