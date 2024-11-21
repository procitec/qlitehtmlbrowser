#include "testbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtCore/QFileInfo>
#include <QtCore/QSignalBlocker>
#include <QtWidgets/QApplication>
#include <QtGui/QKeySequence>
// #include <QtPrintSupport/QPrinter>
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>

TestBrowser::TestBrowser()
{
  mBrowser = new QHelpBrowser( this );
  setCentralWidget( mBrowser );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setMinimumSize( { 200, 150 } );
  resize( 1024, 768 );
  mBrowser->show();

  auto file_menu = mMenu.addMenu( "File" );

  auto action = new QAction( this );
  action->setText( tr( "Open File" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, &TestBrowser::openHtml );
  action = new QAction( this );
  action->setText( tr( "Open QtHelp" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, &TestBrowser::openHelp );
  action = new QAction( this );
  action->setText( tr( "Quit" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, [this]() { close(); } );

  action = new QAction( this );
  action->setText( tr( "Reload" ) );
  action->setShortcut( QKeySequence::Refresh );
  connect( action, &QAction::triggered, mBrowser, &QHelpBrowser::reload );
  action = new QAction( this );
  action->setText( tr( "Print PDF" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, &TestBrowser::export2pdf );

  mToolBar.addAction( action );

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

  setMenuBar( &mMenu );
  addToolBar( Qt::TopToolBarArea, &mToolBar );
  mLastDirectory = QDir::current();

  auto collectionFile = QString( "%1/%2-%3.qhc" ).arg( QDir::tempPath(), qApp->applicationName(), QString::number( qApp->applicationPid() ) );
  mHelpEngine         = new QHelpEngine( collectionFile );
  mHelpEngine->setupData();
  mBrowser->setHelpEnginge( mHelpEngine );
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
