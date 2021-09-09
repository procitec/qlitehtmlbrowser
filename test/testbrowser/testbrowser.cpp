#include "testbrowser.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QFileInfo>

TestBrowser::TestBrowser()
{
  mBrowser = new QLiteHtmlBrowser( nullptr );
  setCentralWidget( mBrowser );
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setMinimumSize( { 1024, 768 } );
  //  browser->setMinimumSize( size );
  //  browser->update();
  //  browser->show();
  //  mWnd->show();
  mLoadAction.setText( tr( "Load Html" ) );
  mExitAction.setText( tr( "Quit" ) );
  connect( &mLoadAction, &QAction::triggered, this, &TestBrowser::loadHtml );
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

void TestBrowser::loadHtml()
{
  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open File" ), mLastDirectory.absolutePath(), tr( "HTML (*.html *.htm)" ) );

  if ( !fileName.isEmpty() )
  {
    QFileInfo fi( fileName );
    mLastDirectory.setPath( fi.absoluteDir().absolutePath() );
    QFile f( fileName );
    if ( f.open( QIODevice::ReadOnly ) )
    {
      auto html = f.readAll();
      f.close();
      mBrowser->setHtml( html );
    }
  }
}
