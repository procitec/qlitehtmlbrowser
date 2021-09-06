#include "QLiteHtmlBrowser.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>

QLiteHtmlBrowser::QLiteHtmlBrowser( QWidget* parent )
  : QWidget( parent )
{
  setMinimumSize( 200, 200 );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer = new container_qt( this );
  mContainer->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer->show();

  auto* layout = new QVBoxLayout;
  layout->addWidget( mContainer );

  setLayout( layout );
}

// void QLiteHtmlBrowser::resizeEvent( QResizeEvent* ev )
//{
//  if ( ev )
//  {
//    QWidget::resizeEvent( ev );
//  }
//}

QUrl QLiteHtmlBrowser::source() const
{
  return mSource;
}
void QLiteHtmlBrowser::setSource( const QUrl& name )
{
  _setSource( name );
}

void QLiteHtmlBrowser::_setSource( const QUrl& name )
{
  mSource = name;
  if ( mContainer )
  {
    mContainer->setSource( name.toString().toUtf8().constData() );
  }
  // mContainer->set_base_url( name.toString().toStdString().c_str() );
}

void QLiteHtmlBrowser::setHtml( const QString& html )
{
  if ( mContainer )
  {
    mContainer->setHtml( html.toUtf8().constData() );
  }
}
