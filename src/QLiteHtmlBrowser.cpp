#include "QLiteHtmlBrowser.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QPalette>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>

extern const litehtml::tchar_t master_css[] = {
#include "master.css.inc"
};

QByteArray master_css_x = { master_css };

QLiteHtmlBrowser::QLiteHtmlBrowser( QWidget* parent )
  : QWidget( parent )
{
  setMinimumSize( 200, 200 );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer = new container_qt( this );
  connect( mContainer, &container_qt::anchorClicked, this, [this]( const QUrl& url ) {
    setUrl( url );
    emit urlChanged( url );
  } );
  mContainer->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer->show();

  auto* layout = new QVBoxLayout;
  layout->addWidget( mContainer );

  setLayout( layout );

  mCSS = QString::fromUtf8( master_css_x );
  loadStyleSheet();
}

void QLiteHtmlBrowser::changeEvent( QEvent* e )
{
  QWidget::changeEvent( e );
  if ( e && e->type() == QEvent::PaletteChange )
  {
    auto lightness = this->palette().color( QPalette::Window ).lightnessF();
    bool dark_mode = ( lightness > 0.5 ) ? false : true;
    if ( dark_mode )
    {
      mCSS = QString::fromUtf8( master_css_x );
      QFile dark_css( ":/styles/dark.css" );
      dark_css.open( QIODevice::ReadOnly );
      if ( dark_css.isOpen() )
      {
        auto css = QString::fromUtf8( dark_css.readAll() );
        css.replace( "@QPalette::Window@", palette().color( QPalette::Window ).name() );
        css.replace( "@QPalette::Text@", palette().color( QPalette::Text ).name() );
        mCSS += css;
        loadStyleSheet();
      }
    }
  }
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
void QLiteHtmlBrowser::setUrl( const QUrl& name )
{
  _setSource( name );
}

void QLiteHtmlBrowser::_setSource( const QUrl& url )
{
  mSource = url;
  if ( mContainer )
  {
    auto fragment = url.fragment();
    auto pure_url = QUrl( url );
    pure_url.setFragment( {} );
    QString html;
    QString baseUrl;

    if ( pure_url.isLocalFile() )
    {
      // get the content of the url and display the html

      QFile f( pure_url.toLocalFile() );
      if ( f.open( QIODevice::ReadOnly ) )
      {
        html = f.readAll();
        f.close();
        baseUrl = pure_url.adjusted( QUrl::RemoveFilename ).toString();
      }
    }
    else
    {
      // todo
      qWarning() << "url scheme is not supported: " << url.toString();
    }

    if ( !html.isEmpty() )
    {
      mUrl = pure_url;
      mUrl.setFragment( fragment );
      mContainer->setHtml( html, baseUrl );
      if ( !fragment.isEmpty() )
      {
        mContainer->scrollToAnchor( fragment );
      }
      update();
    }
  }
}

void QLiteHtmlBrowser::setHtml( const QString& html, const QString& baseurl )
{
  if ( mContainer )
  {
    mContainer->setHtml( html, baseurl );
  }
}

// void QLiteHtmlBrowser::setUrl( const QUrl& url )
//{
//  if ( mContainer )
//  {
//    mContainer->setBaseDirectory( url.path() );
//    mContainer->setHtml()
//  }
//}

void QLiteHtmlBrowser::loadStyleSheet()
{
  mContainer->setCSS( mCSS );
}

void QLiteHtmlBrowser::setScale( double scale )
{
  if ( mContainer )
  {
    mContainer->setScale( scale );
  }
}

double QLiteHtmlBrowser::scale() const
{
  double scale = 0.0;
  if ( mContainer )
  {
    scale = mContainer->scale();
  }
  return scale;
}

void QLiteHtmlBrowser::wheelEvent( QWheelEvent* e )
{
  if ( mContainer && ( e->modifiers() & Qt::ControlModifier ) )
  {
    e->setAccepted( true );
    float delta = e->angleDelta().y() / 120.f;
    auto  scale = mContainer->scale();
    scale += delta / 10.0; // get scaling in 10 percent steps;

    mContainer->setScale( scale );
  }
}
