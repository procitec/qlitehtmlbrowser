#include "QLiteHtmlBrowserImpl.h"
#include "container_qt.h"

#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QPalette>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>

#include <functional>

extern const litehtml::tchar_t master_css[] = {
#include "master.css.inc"
};

QByteArray master_css_x = { master_css };

QLiteHtmlBrowserImpl::QLiteHtmlBrowserImpl( QWidget* parent )
  : QWidget( parent )
{
  setMinimumSize( 200, 200 );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer = new container_qt( this );
  connect(
    mContainer, &container_qt::anchorClicked, this,
    [this]( const QUrl& url ) {
      setUrl( url );
      emit urlChanged( url );
    },
    Qt::QueuedConnection );

  mContainer->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  mContainer->show();

  auto* layout = new QVBoxLayout;
  layout->addWidget( mContainer );

  setLayout( layout );

  mCSS = QString::fromUtf8( master_css_x );
  loadStyleSheet();
}

QLiteHtmlBrowserImpl::~QLiteHtmlBrowserImpl() {}

void QLiteHtmlBrowserImpl::setResourceHandler( const std::function<QByteArray( QUrl )>& rh )
{
  mResourceHandler = rh;
  mContainer->setResourceHandler( rh );
}

void QLiteHtmlBrowserImpl::changeEvent( QEvent* e )
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

QUrl QLiteHtmlBrowserImpl::source() const
{
  return mSource;
}
void QLiteHtmlBrowserImpl::setUrl( const QUrl& name )
{
  setSource( name );
}

void QLiteHtmlBrowserImpl::setSource( const QUrl& url )
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
      // eg. if ( url.scheme() == "qthelp" )
      html    = mResourceHandler( url );
      baseUrl = pure_url.adjusted( QUrl::RemoveFilename ).toString();
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

void QLiteHtmlBrowserImpl::setHtml( const QString& html, const QString& baseurl )
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

void QLiteHtmlBrowserImpl::loadStyleSheet()
{
  mContainer->setCSS( mCSS );
}

void QLiteHtmlBrowserImpl::setScale( double scale )
{
  if ( mContainer )
  {
    mContainer->setScale( scale );
  }
}

double QLiteHtmlBrowserImpl::scale() const
{
  double scale = 0.0;
  if ( mContainer )
  {
    scale = mContainer->scale();
  }
  return scale;
}

void QLiteHtmlBrowserImpl::wheelEvent( QWheelEvent* e )
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
