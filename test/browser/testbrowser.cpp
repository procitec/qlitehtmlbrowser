#include "testbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtCore/QFileInfo>
#include <QtCore/QSignalBlocker>
#include <QtWidgets/QApplication>

TestBrowser::TestBrowser()
{
  mBrowser = new QHelpBrowser( this );
  setCentralWidget( mBrowser );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setMinimumSize( { 1024, 768 } );
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
    //  QFile f( html_file );
    //  if ( f.open( QIODevice::ReadOnly ) )
    //  {
    //    auto html = f.readAll();
    //    f.close();
    //    mBrowser->setHtml( html, mLastDirectory.absolutePath() );

    //    mBrowser->update();
    //    update();
    //  }
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

void TestBrowser::openHelp()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open Help File" ), mLastDirectory.absolutePath(), tr( "Qt Help (*.qch)" ) );

  if ( !fileName.isEmpty() )
  {
    auto ns = QHelpEngineCore::namespaceName( fileName );
    mHelpEngine->unregisterDocumentation( ns );
    auto reg = mHelpEngine->registerDocumentation( fileName );
    if ( reg )
    {
      loadHelp();
    }
  }
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
