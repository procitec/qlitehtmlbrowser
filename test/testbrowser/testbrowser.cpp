#include "testbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtCore/QFileInfo>

TestBrowser::TestBrowser()
{
  mBrowser = new QLiteHtmlBrowser( this );
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
  action->setText( tr( "Quit" ) );
  file_menu->addAction( action );
  connect( action, &QAction::triggered, this, [this]() { close(); } );

  mUrl = new QLineEdit( this );
  mToolBar.addWidget( mUrl );
  connect( mUrl, &QLineEdit::editingFinished, this, [this]() { loadHtml( mUrl->text() ); } );
  connect( mBrowser, &QLiteHtmlBrowser::urlChanged, mUrl, [this]( const QUrl& url ) {
    mUrl->blockSignals( true );
    mUrl->setText( url.toString() );
    mUrl->blockSignals( false );
  } );

  setMenuBar( &mMenu );
  addToolBar( Qt::TopToolBarArea, &mToolBar );
  mLastDirectory = QDir::current();
}

TestBrowser::~TestBrowser()
{
  delete mBrowser;
  mBrowser = nullptr;
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
    mBrowser->setUrl( url );
  }
}

void TestBrowser::openHtml()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open File" ), mLastDirectory.absolutePath(), tr( "HTML (*.html *.htm)" ) );

  if ( !fileName.isEmpty() )
  {
    loadHtml( fileName );
    mUrl->setText( mBrowser->url().toString() );
  }
}
