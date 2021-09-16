#include "testbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QFileInfo>

TestBrowser::TestBrowser()
{
  mBrowser = new QLiteHtmlBrowser( this );
  setCentralWidget( mBrowser );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setMinimumSize( { 1024, 768 } );
  mBrowser->show();
  mLoadAction.setText( tr( "Open File" ) );
  mExitAction.setText( tr( "Quit" ) );
  connect( &mLoadAction, &QAction::triggered, this, &TestBrowser::openHtml );
  connect( &mExitAction, &QAction::triggered, this, [this]() { close(); } );

  auto file_menu = mMenu.addMenu( "File" );
  file_menu->addAction( &mLoadAction );
  file_menu->addAction( &mExitAction );

  setMenuBar( &mMenu );
  mLastDirectory = QDir::current();
}

TestBrowser::~TestBrowser()
{
  delete mBrowser;
  mBrowser = nullptr;
}
void TestBrowser::loadHtml( const QString& html_file )
{
  QFileInfo fi( html_file );
  mLastDirectory.setPath( fi.absoluteDir().absolutePath() );
  QFile f( html_file );
  if ( f.open( QIODevice::ReadOnly ) )
  {
    auto html = f.readAll();
    f.close();
    mBrowser->setBaseUrl( mLastDirectory.absolutePath() );
    mBrowser->setHtml( html );

    mBrowser->update();
    update();
  }
}

void TestBrowser::openHtml()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open File" ), mLastDirectory.absolutePath(), tr( "HTML (*.html *.htm)" ) );

  if ( !fileName.isEmpty() )
  {
    loadHtml( fileName );
  }
}
