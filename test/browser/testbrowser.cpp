#include "testbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtCore/QFileInfo>
#include <QtCore/QSignalBlocker>
#include <QtWidgets/QApplication>
#include <QtGui/QKeySequence>
#include <QtGui/QIcon>
#include <QtGui/QPalette>
// #include <QtPrintSupport/QPrinter>
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>
#include <QtCore/QDebug>
#include <QtSvg/QSvgRenderer>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStatusBar>

#include <cmath>

namespace
{

constexpr const char* SVG_COLOR_TOKEN = "#000000";

const QByteArray HOME_SVG = QByteArrayLiteral(
  R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2.1" stroke-linecap="round" stroke-linejoin="round"><path d="M3.75 10.5L12 4l8.25 6.5"/><path d="M6.75 9.5V20h10.5V9.5"/></svg>)svg" );

const QByteArray BACK_SVG = QByteArrayLiteral(
  R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2.1" stroke-linecap="round" stroke-linejoin="round"><path d="M10.5 6L4.5 12l6 6"/><path d="M5 12h14.5"/></svg>)svg" );

const QByteArray FORWARD_SVG = QByteArrayLiteral(
  R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2.1" stroke-linecap="round" stroke-linejoin="round"><path d="M13.5 6l6 6-6 6"/><path d="M19 12H4.5"/></svg>)svg" );

const QByteArray UP_SVG = QByteArrayLiteral(
  R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2.1" stroke-linecap="round" stroke-linejoin="round"><path d="M6 10.5l6-6 6 6"/><path d="M12 19.5v-14"/></svg>)svg" );

const QByteArray DOWN_SVG = QByteArrayLiteral(
  R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2.1" stroke-linecap="round" stroke-linejoin="round"><path d="M6 13.5l6 6 6-6"/><path d="M12 4.5v14"/></svg>)svg" );

QByteArray recolorSvg( QByteArray svg, const QColor& color )
{
  svg.replace( SVG_COLOR_TOKEN, color.name( QColor::HexRgb ).toUtf8() );
  return svg;
}

QPixmap renderSvgPixmap( const QByteArray& svgTemplate, const QColor& color, const QSize& logicalSize, qreal devicePixelRatio )
{
  const QSize deviceSize( qMax( 1, qRound( logicalSize.width() * devicePixelRatio ) ), qMax( 1, qRound( logicalSize.height() * devicePixelRatio ) ) );

  QPixmap pixmap( deviceSize );
  pixmap.fill( Qt::transparent );
  pixmap.setDevicePixelRatio( devicePixelRatio );

  QSvgRenderer renderer;
  renderer.load( recolorSvg( svgTemplate, color ) );
  if ( !renderer.isValid() )
  {
    return pixmap;
  }

  QPainter painter( &pixmap );
  painter.setRenderHint( QPainter::Antialiasing, true );
  painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
  renderer.render( &painter, QRectF( QPointF( 0.0, 0.0 ), QSizeF( logicalSize ) ) );

  return pixmap;
}

QIcon createSvgIcon( const QByteArray& svgTemplate, const QWidget* widget )
{
  const QPalette palette = widget->palette();

  QColor normalColor = palette.color( QPalette::ButtonText );
  if ( !normalColor.isValid() )
  {
    normalColor = palette.color( QPalette::WindowText );
  }

  QColor disabledColor = palette.color( QPalette::Disabled, QPalette::ButtonText );
  if ( !disabledColor.isValid() )
  {
    disabledColor = palette.color( QPalette::Disabled, QPalette::WindowText );
  }
  if ( !disabledColor.isValid() )
  {
    disabledColor = normalColor;
  }

  QIcon              icon;
  const QList<int>   iconExtents       = { 16, 20, 24, 32 };
  const QList<qreal> devicePixelRatios = { 1.0, 1.25, 1.5, 2.0 };

  for ( const int iconExtent : iconExtents )
  {
    const QSize logicalSize( iconExtent, iconExtent );
    for ( const qreal devicePixelRatio : devicePixelRatios )
    {
      icon.addPixmap( renderSvgPixmap( svgTemplate, normalColor, logicalSize, devicePixelRatio ), QIcon::Normal, QIcon::Off );
      icon.addPixmap( renderSvgPixmap( svgTemplate, normalColor, logicalSize, devicePixelRatio ), QIcon::Active, QIcon::Off );
      icon.addPixmap( renderSvgPixmap( svgTemplate, disabledColor, logicalSize, devicePixelRatio ), QIcon::Disabled, QIcon::Off );
    }
  }

  return icon;
}

int toolbarIconExtent( const QWidget* widget )
{
  return widget->style()->pixelMetric( QStyle::PM_ToolBarIconSize, nullptr, widget );
}

} // namespace

TestBrowser::TestBrowser()
{

  const auto homeIcon    = createSvgIcon( HOME_SVG, this );
  const auto backIcon    = createSvgIcon( BACK_SVG, this );
  const auto forwardIcon = createSvgIcon( FORWARD_SVG, this );
  const auto upIcon      = createSvgIcon( UP_SVG, this );
  const auto downIcon    = createSvgIcon( DOWN_SVG, this );

  mBrowser = new QHelpBrowser( this );
  setCentralWidget( mBrowser );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setMinimumSize( { 200, 150 } );
  resize( 1024, 768 );
  mBrowser->show();

  const int iconExtent = toolbarIconExtent( this );
  mToolBar.setIconSize( QSize( iconExtent, iconExtent ) );

  auto file_menu = mMenu.addMenu( "File" );

  auto action = new QAction( this );
  action->setText( tr( "Open File" ) );
  action->setShortcut( QKeySequence( QKeySequence::StandardKey::Open ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, &TestBrowser::openHtml );

  action = new QAction( this );
  action->setText( tr( "Open QtHelp" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, &TestBrowser::openHelp );

  action = new QAction( this );
  action->setText( tr( "Quit" ) );
  action->setShortcut( QKeySequence( QKeySequence::StandardKey::Quit ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, [this]() { close(); } );

  action = new QAction( this );
  action->setText( tr( "Reload" ) );
  action->setShortcut( QKeySequence::Refresh );
  connect( action, &QAction::triggered, mBrowser, &QHelpBrowser::reload );
  mToolBar.addAction( action );

  action = new QAction( this );
  action->setText( tr( "Print PDF" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, &TestBrowser::export2pdf );

  mUrl = new QLineEdit( this );
  mToolBar.addWidget( mUrl );
  connect( mUrl, &QLineEdit::editingFinished, this,
           [this]()
           {
             if ( mUrl )
               loadHtml( mUrl->text() );
           } );
  connect( mBrowser, &QLiteHtmlBrowser::sourceChanged, mUrl,
           [this]( const QUrl& url )
           {
             if ( mUrl )
             {
               QSignalBlocker signalBlocker{ mUrl };
               mUrl->setText( url.toString() );
             }
           } );

  mActHome = new QAction( homeIcon, tr( "home" ), this );
  connect( mActHome, &QAction::triggered, this, [this]() { home(); } );
  mToolBar.addAction( mActHome );

  mActBackward = new QAction( backIcon, tr( "backward" ), this );
  connect( mActBackward, &QAction::triggered, this, [this]() { backward(); } );
  mToolBar.addAction( mActBackward );

  mActForward = new QAction( forwardIcon, tr( "forward" ), this );
  connect( mActForward, &QAction::triggered, this, [this]() { forward(); } );
  mToolBar.addAction( mActForward );

  mToolBar.addSeparator();

  mFindText = new QLineEdit( this );
  mToolBar.addWidget( mFindText );
  connect( mFindText, &QLineEdit::returnPressed, this,
           [this]()
           {
             if ( mFindText != nullptr )
             {
               findText( mFindText->text() );
             }
           } );

  mPreviousFindMatch = new QAction( upIcon, "Vorheriges", this );
  connect( mPreviousFindMatch, &QAction::triggered, this, [this]() { previousFindMatch(); } );
  mPreviousFindMatch->setEnabled( false );
  mToolBar.addAction( mPreviousFindMatch );

  mNextFindMatch = new QAction( downIcon, "Nächstes", this );
  mNextFindMatch->setEnabled( false );
  connect( mNextFindMatch, &QAction::triggered, this, [this]() { nextFindMatch(); } );

  mToolBar.addAction( mNextFindMatch );

  setMenuBar( &mMenu );
  addToolBar( Qt::TopToolBarArea, &mToolBar );
  mLastDirectory = QDir::current();

  auto collectionFile = QString( "%1/%2-%3.qhc" ).arg( QDir::tempPath(), qApp->applicationName(), QString::number( qApp->applicationPid() ) );
  mHelpEngine         = new QHelpEngine( collectionFile );
  mHelpEngine->setupData();
  mBrowser->setHelpEnginge( mHelpEngine );

  auto* statusbar = statusBar();
  mScale          = new QLabel();
  mScale->setText( mScaleText.arg( static_cast<int>( std::round( mBrowser->scale() * 100.0 ) ) ) );
  statusbar->addPermanentWidget( mScale );

  mSelection = new QLabel();
  mSelection->setText( QString() );

  statusbar->addWidget( mSelection );

  connect( mBrowser, &QHelpBrowser::scaleChanged, this,
           [this]() { mScale->setText( mScaleText.arg( static_cast<int>( std::round( mBrowser->scale() * 100.0 ) ) ) ); } );

  connect( mBrowser, &QHelpBrowser::selectionChanged, this,
           [this]()
           {
             auto text = mBrowser->selectedText();
             mSelection->setText( mSelectionText.arg( text.length() ) );
           } );
}

TestBrowser::~TestBrowser()
{
  mBrowser->setHelpEnginge( nullptr );
  delete mHelpEngine;
  mHelpEngine = nullptr;
}

void TestBrowser::setSearchPaths( const QStringList& paths )
{
  if ( mBrowser )
  {
    mBrowser->setSearchPaths( paths );
  }
}

void TestBrowser::loadHtml( const QString& html_file )
{
  if ( !html_file.isEmpty() )
  {
    QFileInfo fi( html_file );
    mLastDirectory.setPath( fi.absoluteDir().absolutePath() );
    auto url = QUrl::fromUserInput( html_file );
    if ( mBrowser )
    {
      mBrowser->setSource( url );
      setWindowTitle( QString( "%1 <%2>" ).arg( qApp->applicationDisplayName(), mBrowser->caption() ) );
      mUrl->setText( mBrowser->source().toString() );
    }
  }
}

void TestBrowser::openHtml()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open HTML File" ), mLastDirectory.absolutePath(), tr( "HTML (*.html *.htm)" ) );

  if ( !fileName.isEmpty() )
  {
    loadHtml( fileName );
  }
}

void TestBrowser::loadQtHelp( const QString& qch_file )
{
  if ( !qch_file.isEmpty() )
  {
    auto ns = QHelpEngineCore::namespaceName( qch_file );
    mHelpEngine->unregisterDocumentation( ns );
    auto reg = mHelpEngine->registerDocumentation( qch_file );
    if ( reg )
    {
      loadHelp();
    }
  }
}

void TestBrowser::openHelp()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open Help File" ), mLastDirectory.absolutePath(), tr( "Qt Help (*.qch)" ) );
  loadQtHelp( fileName );
}
void TestBrowser::loadHelp()
{
  if ( mHelpEngine )
  {
    auto nss = mHelpEngine->registeredDocumentations();
    if ( !nss.isEmpty() )
    {
      auto ns    = nss.first();
      auto files = mHelpEngine->files( ns, QStringList() );
      if ( !files.isEmpty() )
      {
        auto f = files.first();
        mBrowser->setSource( f );
        mUrl->setText( f.toString() );
      }
    }
  }
}

void TestBrowser::export2pdf()
{
#ifndef QT_NO_PRINTER
  //! [0]
  QFileDialog fileDialog( this, tr( "Export PDF" ) );
  fileDialog.setAcceptMode( QFileDialog::AcceptSave );
  fileDialog.setMimeTypeFilters( QStringList( "application/pdf" ) );
  fileDialog.setDefaultSuffix( "pdf" );
  if ( fileDialog.exec() )
  {
    auto files = fileDialog.selectedFiles();
    if ( !files.empty() )
    {
      QString fileName = files.first();

      QPdfWriter pdf( fileName );
      pdf.setPageSize( QPageSize( QPageSize::A4 ) );
      pdf.setPageOrientation( QPageLayout::Portrait );
      pdf.setPageMargins( { 10, 10, 10, 10 }, QPageLayout::Millimeter );
      mBrowser->print( &pdf );
    }
  }
#endif
}
void TestBrowser::findText( const QString& text )
{
  auto found = mBrowser->findText( text );
  mPreviousFindMatch->setEnabled( found > 0 );
  mNextFindMatch->setEnabled( found > 0 );
}

void TestBrowser::previousFindMatch()
{
  mBrowser->findPreviousMatch();
}

void TestBrowser::nextFindMatch()
{
  mBrowser->findNextMatch();
}

void TestBrowser::forward()
{
  mBrowser->forward();
}

void TestBrowser::backward()
{
  mBrowser->backward();
}

void TestBrowser::home()
{
  mBrowser->home();
}
