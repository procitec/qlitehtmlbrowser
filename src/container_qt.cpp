#include "container_qt.h"

#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QScrollBar>

container_qt::container_qt( QWidget* parent )
  : QAbstractScrollArea( parent )
{
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

  //  auto* layout = new QVBoxLayout();

  //  layout->addWidget( new QPushButton( "text" ) );
  //  setLayout( layout );
}

void container_qt::setCSS( const QString& css )
{
  mContext.load_master_stylesheet( css.toUtf8().constData() );
}

void container_qt::setSource( const char* url )
{
  // mDocument = litehtml::document::createFromString( url, this, );
}

void container_qt::setHtml( const char* html )
{
  if ( html )
  {
    mDocument = litehtml::document::createFromUTF8( html, this, &mContext );
    mDocument->render( this->viewport()->width(), litehtml::render_all );
    resetScrollBars();
    viewport()->update();
  }
}

void container_qt::resetScrollBars()
{
  horizontalScrollBar()->setValue( 0 );
  verticalScrollBar()->setValue( 0 );
  horizontalScrollBar()->setMaximum( mDocument->width() );
  verticalScrollBar()->setMaximum( mDocument->height() );
  horizontalScrollBar()->setPageStep( viewport()->width() );
  verticalScrollBar()->setPageStep( viewport()->height() );
}

QPoint container_qt::scrollBarPos() const
{
  return { horizontalScrollBar()->value(), verticalScrollBar()->value() };
}

void container_qt::paintEvent( QPaintEvent* event )
{
  if ( event )
  {
    if ( mDocument )
    {
      /*auto     width = */
      mDocument->render( this->viewport()->width() );
      QPainter p( viewport() );
      p.setRenderHint( QPainter::SmoothPixmapTransform, true );
      p.setRenderHint( QPainter::Antialiasing, true );
      const litehtml::position clipRect = { event->rect().x(), event->rect().y(), event->rect().width(), event->rect().height() };
      // auto                     margins  = contentsMargins();
      auto margins    = viewport()->contentsMargins();
      auto scroll_pos = -scrollBarPos();

      mDocument->draw( reinterpret_cast<litehtml::uint_ptr>( &p ), margins.left() + scroll_pos.x(), margins.top() + scroll_pos.y(), &clipRect );
    }
  }
}

litehtml::uint_ptr container_qt::create_font(
  const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm )
{
  auto* font = new QFont();
  font->setFamily( "Monospace" );
  font->setFixedPitch( true );
  font->setPixelSize( size );

  font->setItalic( litehtml::fontStyleItalic == italic ? true : false );

  // thinner, thicker? @see https://www.w3schools.com/cssref/pr_font_weight.asp
  font->setWeight( ( weight - 1 ) / 10 );

  /// handle fonts decoration
  if ( litehtml::font_decoration_underline == decoration )
  {
    font->setUnderline( true );
  }
  if ( litehtml::font_decoration_overline == decoration )
  {
    font->setOverline( true );
  }
  if ( litehtml::font_decoration_linethrough == decoration )
  {
    font->setStrikeOut( true );
  }

  if ( fm )
  {
    const QFontMetrics _fm( *font );
    fm->height      = _fm.height();
    fm->ascent      = _fm.ascent();
    fm->descent     = _fm.descent();
    fm->x_height    = _fm.xHeight();
    fm->draw_spaces = true;
  }

  return reinterpret_cast<litehtml::uint_ptr>( font );
}
void container_qt::delete_font( litehtml::uint_ptr hFont )
{
  if ( hFont )
  {
    auto* font = reinterpret_cast<QFont*>( hFont );
    delete font;
  }
}
int container_qt::text_width( const litehtml::tchar_t* text, litehtml::uint_ptr hFont )
{
  int text_width = -1;
  if ( hFont )
  {
    auto* font = reinterpret_cast<QFont*>( hFont );

    QFontMetrics fm( *font );
    text_width = fm.horizontalAdvance( QString::fromUtf8( text ) );
  }
  return text_width;
}
void container_qt::draw_text(
  litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );
  QFont*    f = reinterpret_cast<QFont*>( hFont );
  p->save();
  p->setFont( *f );
  p->setPen( QColor( color.red, color.green, color.blue, color.alpha ) );
  //  litehtml::position clientPos;
  //  get_client_rect( clientPos );
  //  QRect rect;
  //  auto  brect = p->boundingRect( rect, /*Qt::AlignTop | Qt::AlignLeft*/ 0, QString::fromUtf8( text ) );
  //  p->drawText( pos.x, pos.y + brect.height(), QString::fromUtf8( text ) );
  p->drawText( QRect( pos.x, pos.y, pos.width, pos.height ), 0, QString::fromUtf8( text ) );
  p->restore();
}
int container_qt::pt_to_px( int pt )
{
  return this->viewport()->physicalDpiY() * pt / this->viewport()->logicalDpiY();
}
int container_qt::get_default_font_size() const
{
  return mFontSize;
}
const litehtml::tchar_t* container_qt::get_default_font_name() const
{
#ifdef Q_OS_LINUX
  return "Monospace";
#else
  return "Courier New";
#endif
}
void container_qt::draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker ) {}
void container_qt::load_image( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready ) {}
void container_qt::get_image_size( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz ) {}
void container_qt::draw_background( litehtml::uint_ptr hdc, const litehtml::background_paint& bg )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );
  p->save();
  p->setBrush( Qt::green );
  p->drawRect( bg.border_box.x, bg.border_box.y, bg.border_box.width, bg.border_box.height );
  p->restore();
}
void container_qt::draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root ) {}

void container_qt::set_caption( const litehtml::tchar_t* caption ) {}

void container_qt::set_base_url( const litehtml::tchar_t* base_url )
{
  mBaseUrl = base_url;
}

void container_qt::link( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el ) {}
void container_qt::on_anchor_click( const litehtml::tchar_t* url, const litehtml::element::ptr& el ) {}
void container_qt::set_cursor( const litehtml::tchar_t* cursor ) {}
void container_qt::transform_text( litehtml::tstring& text, litehtml::text_transform tt ) {}
void container_qt::import_css( litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl ) {}
void container_qt::set_clip( const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y ) {}
void container_qt::del_clip() {}

void container_qt::get_client_rect( litehtml::position& client ) const
{
  client.width  = this->viewport()->width();
  client.height = this->viewport()->height();
  //  auto clientPos = mapToParent( { contentsMargins().left(), contentsMargins().top() } );
  //  client.x       = clientPos.x();
  //  client.y       = clientPos.y();
  client.x = contentsMargins().left();
  client.y = contentsMargins().top();
}

std::shared_ptr<litehtml::element> container_qt::create_element( const litehtml::tchar_t*                   tag_name,
                                                                 const litehtml::string_map&                attributes,
                                                                 const std::shared_ptr<litehtml::document>& doc )
{
  return {};
}

void container_qt::get_media_features( litehtml::media_features& media ) const
{
  litehtml::position client;
  get_client_rect( client );
  media.type          = litehtml::media_type_screen;
  media.width         = client.width;
  media.height        = client.height;
  media.device_width  = 1920;
  media.device_height = 1080;
  media.color         = 8;
  media.monochrome    = 0;
  media.color_index   = 256;
  media.resolution    = 96;
}
void container_qt::get_language( litehtml::tstring& language, litehtml::tstring& culture ) const
{
  language = _t( "en" );
  culture  = _t( "" );
}
