#include "container_qt.h"

#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QScrollBar>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>
#include <QtGui/QPalette>

#include <cmath>
container_qt::container_qt( QWidget* parent )
  : QAbstractScrollArea( parent )
{
  setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

  //  auto* layout = new QVBoxLayout();

  //  layout->addWidget( new QPushButton( "text" ) );
  //  setLayout( layout );
  setMouseTracking( true );
}

void container_qt::setCSS( const QString& css )
{
  mContext.load_master_stylesheet( css.toUtf8().constData() );
  render();
}

void container_qt::setHtml( const QString& html, const QUrl& source_url )
{
  if ( !html.isEmpty() )
  {
    auto pure_url = source_url;
    pure_url.setFragment( {} );
    mBaseUrl   = pure_url.adjusted( QUrl::RemoveFilename );
    mSourceUrl = pure_url;
    mDocument  = litehtml::document::createFromUTF8( html.toUtf8(), this, &mContext );
    mDocument->render( this->viewport()->width(), litehtml::render_all );
    resetScrollBars();
    viewport()->update();
    auto frag = source_url.fragment();
    if ( !frag.isEmpty() )
    {
      scrollToAnchor( frag );
    }
  }
}

void container_qt::resetScrollBars()
{
  horizontalScrollBar()->setValue( 0 );
  verticalScrollBar()->setValue( 0 );
  horizontalScrollBar()->setMaximum( inv_scaled( mDocument->width() ) );
  verticalScrollBar()->setMaximum( inv_scaled( mDocument->height() ) );
  horizontalScrollBar()->setPageStep( ( viewport()->width() ) );
  verticalScrollBar()->setPageStep( ( viewport()->height() ) );

  const QFontMetrics _fm( this->font() );
  horizontalScrollBar()->setSingleStep( _fm.averageCharWidth() );
  verticalScrollBar()->setSingleStep( _fm.height() );
}

QPoint container_qt::scrollBarPos() const
{
  return { horizontalScrollBar()->value(), verticalScrollBar()->value() };
}

void container_qt::render()
{
  if ( mDocument )
  {
    mDocument->render( scaled( this->viewport()->width() ) );
    viewport()->update();
  }
}

void container_qt::paintEvent( QPaintEvent* event )
{
  if ( event )
  {
    if ( mDocument )
    {
      /*auto     width = */
      mDocument->render( scaled( this->viewport()->width() ) );
      QPainter p( viewport() );
      p.setWorldTransform( QTransform().scale( mScale, mScale ) );
      p.setRenderHint( QPainter::SmoothPixmapTransform, true );
      p.setRenderHint( QPainter::Antialiasing, true );

      const litehtml::position clipRect = { scaled( event->rect().x() ), scaled( event->rect().y() ), scaled( event->rect().width() ),
                                            scaled( event->rect().height() ) };
      // auto                     margins  = contentsMargins();
      auto margins    = viewport()->contentsMargins();
      auto scroll_pos = -scrollBarPos();

      mDocument->draw( reinterpret_cast<litehtml::uint_ptr>( &p ), scaled( margins.left() + scroll_pos.x() ),
                       scaled( margins.top() + scroll_pos.y() ), &clipRect );
    }
  }
}

litehtml::uint_ptr container_qt::create_font(
  const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm )
{
  auto* font = new QFont();

  litehtml::string_vector fonts;
  litehtml::split_string( faceName, fonts, "," );
  QStringList familyNames;

  for ( const auto& f : fonts )
  {
    familyNames.append( QString::fromStdString( f ).trimmed().remove( QRegularExpression( R"-('")-" ) ) );
  }

  font->setFamilies( familyNames );
  // font->setFamily( QString::fromStdString( fonts[0] ) );
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
  auto web_color = toColor( color );
  // todo: how to handle darkmode
  //  auto widget_color = QPalette().color( QPalette::Text );
  //  auto widget_bg_color = QPalette().color( QPalette::Window );
  p->setPen( web_color );
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
void container_qt::draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );

  // does the marker contain an image?
  if ( !marker.image.empty() )
  {
    auto url   = resolveUrl( marker.image.c_str(), marker.baseurl );
    auto image = load_pixmap( url );
    p->drawPixmap( /*scaled*/ ( QRect( marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height ) ), image );
  }
  else
  {
    switch ( marker.marker_type )
    {
      case litehtml::list_style_type_none:
        break;
      case litehtml::list_style_type_circle:
        p->setPen( QColor( marker.color.red, marker.color.green, marker.color.blue, marker.color.alpha ) );
        p->setBrush( Qt::NoBrush );
        p->drawEllipse( /*scaled*/ ( QRect( marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height ) ) );
        break;
      case litehtml::list_style_type_disc: // filled circle
        p->setPen( QColor( marker.color.red, marker.color.green, marker.color.blue, marker.color.alpha ) );
        p->setBrush( QColor( marker.color.red, marker.color.green, marker.color.blue, marker.color.alpha ) );
        p->drawEllipse( /*scaled*/ ( QRect( marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height ) ) );
        break;
      case litehtml::list_style_type_square:
        p->setPen( QColor( marker.color.red, marker.color.green, marker.color.blue, marker.color.alpha ) );
        p->setBrush( QColor( marker.color.red, marker.color.green, marker.color.blue, marker.color.alpha ) );
        p->drawRect( /*scaled*/ ( QRect( marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height ) ) );
        break;
      case litehtml::list_style_type_decimal:
      case litehtml::list_style_type_decimal_leading_zero:
      case litehtml::list_style_type_lower_alpha:
      case litehtml::list_style_type_lower_greek:
      case litehtml::list_style_type_lower_latin:
      case litehtml::list_style_type_lower_roman:
      case litehtml::list_style_type_upper_alpha:
      case litehtml::list_style_type_upper_latin:
      case litehtml::list_style_type_upper_roman:
      {
        // this is handled by get_list_marker_text in html_tag of litehtml
      }
      break;
      case litehtml::list_style_type_georgian:
      case litehtml::list_style_type_armenian:
      case litehtml::list_style_type_cjk_ideographic:
      case litehtml::list_style_type_hebrew:
      case litehtml::list_style_type_hiragana:
      case litehtml::list_style_type_hiragana_iroha:
      case litehtml::list_style_type_katakana:
      case litehtml::list_style_type_katakana_iroha:
        // this is NOT handled by get_list_marker_text in html_tag of litehtml
        // fallback strategy required.
        // TODO: mapping of each index into map or calculcation
        // e.g. from http://www.i18nguy.com/unicode/hebrew-numbers.html
        {
          auto* font = reinterpret_cast<QFont*>( marker.font );
          p->setPen( QColor( marker.color.red, marker.color.green, marker.color.blue, marker.color.alpha ) );
          p->setBrush( Qt::NoBrush );
          p->setFont( *font );
          auto text     = QString::fromStdString( std::to_string( marker.index ) + "." );
          auto w        = text_width( ".", marker.font );
          auto text_pos = marker.pos;
          text_pos.move_to( marker.pos.left() - w, text_pos.y );
          text_pos.width = marker.pos.width + w;
          p->drawText( /*scaled*/ ( QRect( text_pos.x, text_pos.y, text_pos.width, text_pos.height ) ), 0, text );
        }
        break;
    }
  }
}

QPixmap container_qt::load_image_data( const QUrl& url )
{
  QPixmap pm;
  pm.loadFromData( loadResource( ResourceType::Image, url ) );
  return pm;
}

// hint: from QTextBrowser Private
QUrl container_qt::resolveUrl( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl ) const
{
  QUrl _url     = ( src == nullptr ) ? QUrl() : QUrl( QString::fromStdString( src ) );
  QUrl _baseurl = ( baseurl == nullptr || std::string( baseurl ).empty() ) ? mBaseUrl : QUrl( QString::fromStdString( baseurl ) );

  if ( !_url.isRelative() )
    return _url;

  QUrl resolved_url;

  if ( !_baseurl.isEmpty() )
  {
    resolved_url = QUrl( _baseurl.toString() + ( _baseurl.path().endsWith( "/" ) ? "" : "/" ) + _url.toString() );
  }
  else
  {

    // this is our last resort when current url and new url are both relative
    // we try to resolve against the current working directory in the local
    // file system.
    QFileInfo fi( _baseurl.toLocalFile() );
    if ( fi.exists() )
    {
      resolved_url = QUrl::fromLocalFile( fi.absolutePath() + QDir::separator() ).resolved( _url );
    }
  }

  return resolved_url;
}

QByteArray container_qt::loadResource( ResourceType type, const QUrl& url )
{
  return mResourceHandler( static_cast<int>( type ), url );
}

// QString container_qt::findFile( const QUrl& name ) const
//{
//  QString fileName;
//  if ( name.scheme().isEmpty() )
//  {
//    fileName = name.path();
//  }
//  else
//  {
//    fileName = name.toLocalFile();
//  }

//  if ( fileName.isEmpty() )
//    return fileName;

//  if ( QFileInfo( fileName ).isAbsolute() )
//    return fileName;

//  // TODO search paths relative to baseurl or document source

//  //  for ( QString path : qAsConst( searchPaths ) )
//  //  {
//  //    if ( !path.endsWith( QLatin1Char( '/' ) ) )
//  //      path.append( QLatin1Char( '/' ) );
//  //    path.append( fileName );
//  //    if ( QFileInfo( path ).isReadable() )
//  //      return path;
//  //  }

//  return fileName;
//}

void container_qt::load_image( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready )
{
  auto url = resolveUrl( src, baseurl );
  if ( !mPixmapCache.contains( url ) )
  {
    auto image = load_image_data( url );
    if ( !image.isNull() )
    {
      mPixmapCache.insert( url, image );
    }
    else
    {
      auto pm = QPixmap( ":/images/broken_link.png" );
      mPixmapCache.insert( url, pm );
    }
  }
  if ( redraw_on_ready )
  {
    render();
  }
}
QPixmap container_qt::load_pixmap( const QUrl& url )
{
  if ( !url.isEmpty() && mPixmapCache.contains( url ) )
  {
    return mPixmapCache[url];
  }

  return QPixmap( ":/images/broken_link.png" );
}

void container_qt::get_image_size( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz )
{
  auto url = resolveUrl( src, baseurl );
  if ( !url.isEmpty() )
  {
    if ( mPixmapCache.contains( url ) )
    {
      const auto& pm = mPixmapCache[url];
      sz.width       = pm.width();
      sz.height      = pm.height();
    }
  }
}
void container_qt::draw_background( litehtml::uint_ptr hdc, const litehtml::background_paint& bg )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );
  p->save();
  p->setPen( Qt::NoPen );
  p->setBrush( QColor( bg.color.red, bg.color.green, bg.color.blue, bg.color.alpha ) );
  p->drawRect( bg.border_box.x, bg.border_box.y, bg.border_box.width, bg.border_box.height );
  if ( !bg.image.empty() )
  {
    auto url = resolveUrl( bg.image.c_str(), bg.baseurl.c_str() );
    auto pm  = load_pixmap( url );
    switch ( bg.repeat )
    {
      case litehtml::background_repeat_no_repeat:
        p->drawPixmap( QRect( bg.position_x, bg.position_y, bg.image_size.width, bg.image_size.height ), pm );
        break;

      case litehtml::background_repeat_repeat:
      case litehtml::background_repeat_repeat_x:
      case litehtml::background_repeat_repeat_y:
        // todo handlie this
        break;
    }
  }
  p->restore();
}

Qt::PenStyle container_qt::toPenStyle( const litehtml::border_style& style ) const
{
  Qt::PenStyle p = Qt::NoPen;

  switch ( style )
  {
    case litehtml::border_style_hidden:
    case litehtml::border_style_none:
      p = Qt::NoPen;
      break;

    case litehtml::border_style_dotted:
      p = Qt::DotLine;
      break;
    case litehtml::border_style_dashed:
      p = Qt::DashLine;
      break;
    case litehtml::border_style_solid:
      p = Qt::SolidLine;
      break;
    case litehtml::border_style_double:
    case litehtml::border_style_groove:
    case litehtml::border_style_ridge:
    case litehtml::border_style_inset:
    case litehtml::border_style_outset:
      // this must be handled with more than one pen
      break;
  }
  return p;
}
void container_qt::draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );
  p->save();
  int brd_top    = 0;
  int brd_bottom = 0;
  int brd_left   = 0;
  int brd_right  = 0;

  if ( borders.top.width != 0 && borders.top.style > litehtml::border_style_hidden )
  {
    brd_top = (int)borders.top.width;
  }
  if ( borders.bottom.width != 0 && borders.bottom.style > litehtml::border_style_hidden )
  {
    brd_bottom = (int)borders.bottom.width;
  }
  if ( borders.left.width != 0 && borders.left.style > litehtml::border_style_hidden )
  {
    brd_left = (int)borders.left.width;
  }
  if ( borders.right.width != 0 && borders.right.style > litehtml::border_style_hidden )
  {
    brd_right = (int)borders.right.width;
  }

  // draw top border
  if ( brd_top )
  {
    QPen pen( toColor( borders.top.color ) );
    pen.setWidth( brd_top );
    pen.setStyle( toPenStyle( borders.top.style ) );

    p->setPen( pen );

    double r_left  = borders.radius.top_left_x;
    double r_right = borders.radius.top_right_x;

    if ( r_left )
    {
      p->drawArc( draw_pos.left(), draw_pos.top(), 2 * r_left, 2 * borders.radius.top_left_y, 90 * 16, 90 * 16 );
    }

    p->drawLine( QPoint( draw_pos.left() + r_left, draw_pos.top() ), QPoint( draw_pos.right() - r_right - brd_right, draw_pos.top() /*+ brd_top*/ ) );

    if ( r_right )
    {
      p->drawArc( draw_pos.right() - brd_right - 2 * r_right, draw_pos.top(), 2 * r_right, 2 * borders.radius.top_right_y, 0 * 16, 90 * 16 );
    }
  }

  if ( brd_left )
  {
    QPen pen( toColor( borders.left.color ) );
    pen.setWidth( brd_left );
    pen.setStyle( toPenStyle( borders.left.style ) );

    p->setPen( pen );

    double r_top    = borders.radius.top_left_x;
    double r_bottom = borders.radius.bottom_left_x;

    p->drawLine( QPoint( draw_pos.left() /* + brd_left*/, draw_pos.top() /*+ brd_top*/ + r_top ),
                 QPoint( draw_pos.left() /*+ brd_left*/, draw_pos.bottom() - brd_bottom - r_bottom ) );
  }

  // draw right border
  if ( brd_right )
  {
    QPen pen( toColor( borders.right.color ) );
    pen.setWidth( brd_right );
    pen.setStyle( toPenStyle( borders.right.style ) );
    p->setPen( pen );

    double r_top    = borders.radius.top_right_x;
    double r_bottom = borders.radius.bottom_right_x;

    p->drawLine( QPoint( draw_pos.right() - brd_right, draw_pos.top() /* + brd_top*/ + r_top ),
                 QPoint( draw_pos.right() - brd_right, draw_pos.bottom() - brd_bottom - r_bottom ) );
  }

  // draw bottom border
  if ( brd_bottom )
  {
    QPen pen( toColor( borders.bottom.color ) );
    pen.setWidth( brd_bottom );
    pen.setStyle( toPenStyle( borders.bottom.style ) );

    p->setPen( pen );

    double r_left  = borders.radius.bottom_left_x;
    double r_right = borders.radius.bottom_right_x;

    if ( r_left )
    {
      p->drawArc( draw_pos.left(), draw_pos.bottom() - brd_bottom - 2 * borders.radius.bottom_left_y, 2 * r_left, 2 * borders.radius.bottom_left_y,
                  180 * 16, 90 * 16 );
    }

    p->drawLine( QPoint( draw_pos.left() + r_left, draw_pos.bottom() - brd_bottom ),
                 QPoint( draw_pos.right() - brd_right - r_right, draw_pos.bottom() - brd_bottom ) );

    if ( r_right )
    {
      p->drawArc( draw_pos.right() - brd_right - 2 * r_right, draw_pos.bottom() - brd_bottom - 2 * borders.radius.bottom_right_y, 2 * r_right,
                  2 * borders.radius.bottom_right_y, 270 * 16, 90 * 16 );
    }
  }

  p->restore();
}

void container_qt::scrollToAnchor( const QString& anchor )
{
  auto [x, y] = findAnchorPos( anchor );
  horizontalScrollBar()->setValue( x );
  verticalScrollBar()->setValue( y );
}

litehtml::element::ptr container_qt::findAnchor( const QString& anchor )
{
  litehtml::element::ptr element = nullptr;
  if ( mDocument )
  {
    ///@see https://github.com/litehtml/litehtml/wiki/How-to-use-litehtml#processing-named-anchors

    element = mDocument->root()->select_one( QString( "#%1" ).arg( anchor ).toStdString() );
    /// @see https://de.wikipedia.org/wiki/Anker_(HTML)
    if ( !element ) // no anchor in form of site.html#Anchor found
    {
      element = mDocument->root()->select_one( QString( "[id=%1]" ).arg( anchor ).toStdString() );
    }
    if ( !element ) // no anchor in form of id= found
    {
      element = mDocument->root()->select_one( QString( "[name=%1]" ).arg( anchor ).toStdString() );
    }
  }

  return element;
}

std::pair<int, int> container_qt::findAnchorPos( const QString& anchor )
{
  std::pair<int, int> pos     = { 0, 0 };
  auto                element = findAnchor( anchor );
  if ( element )
  {
    ///@see https://github.com/litehtml/litehtml/wiki/How-to-use-litehtml#processing-named-anchors
    /// example there with placement instead of position
    pos = { element->get_placement().x, element->get_placement().y };
  }
  return pos;
}

void container_qt::set_caption( const litehtml::tchar_t* caption ) {}

void container_qt::set_base_url( const litehtml::tchar_t* base_url )
{
  auto url = QUrl( base_url );
  url.setFragment( {} );
  mBaseUrl = base_url;
}

void container_qt::link( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el ) {}
void container_qt::on_anchor_click( const litehtml::tchar_t* url, const litehtml::element::ptr& el )
{
  if ( mDocument )
  {
    auto url_str = QString( url );
    if ( url_str.startsWith( "#" ) )
    {
      // assert( std::string( el->get_attr( "class", "" ) ) == std::string( "reference internal" ) );
      auto frag = url_str.remove( 0, 1 );
      if ( mSourceUrl.isValid() )
      {
        auto anchor_url = mSourceUrl;
        anchor_url.setFragment( frag );
        emit urlChanged( anchor_url );
      }
      scrollToAnchor( frag );
    }
    else
    {
      auto resolved_url = resolveUrl( url, nullptr );
      emit anchorClicked( resolved_url );
    }
  }
}
void container_qt::set_cursor( const litehtml::tchar_t* cursor )
{
  // todo
  /// @see https://www.w3schools.com/cssref/pr_class_cursor.asp
  ///
  /// manually switch to cursor type from string

  auto c = Qt::ArrowCursor;

  if ( cursor == QLatin1String( "pointer" ) )
  {
    c = Qt::PointingHandCursor;
  }
  viewport()->setCursor( c );
}
void container_qt::transform_text( litehtml::tstring& text, litehtml::text_transform tt ) {}
void container_qt::import_css( litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl )
{
  auto resolved_url = resolveUrl( url.c_str(), baseurl.c_str() );
  auto content      = loadResource( ResourceType::Css, resolved_url );
  if ( !content.isEmpty() )
  {
    text = QString::fromUtf8( content.constData() ).toStdString();
  }
}
void container_qt::set_clip( const litehtml::position& pos, const litehtml::border_radiuses& brd_radius, bool valid_x, bool valid_y ) {}
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

void container_qt::setScale( double scale )
{
  mScale = scale < mMinScale ? mMinScale : scale > mMaxScale ? mMaxScale : scale;
  resetScrollBars();
  update();
  viewport()->update();
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

void container_qt::wheelEvent( QWheelEvent* e )
{
  if ( e )
  {
    if ( ( e->modifiers() & Qt::ControlModifier ) )
    {
      e->setAccepted( true );
      float delta = e->angleDelta().y() / 120.f;
      auto  scale = this->scale();
      scale += delta / 10.0; // get scaling in 10 percent steps;
      setScale( scale );
    }
    else
    {
      QAbstractScrollArea::wheelEvent( e );
    }
  }
}

void container_qt::keyPressEvent( QKeyEvent* e )
{
  if ( e )
  {
    QAbstractScrollArea::keyPressEvent( e );
    if ( e->modifiers() & Qt::ControlModifier )
    {
      auto k = e->key();

      switch ( k )
      {
        case Qt::Key_Enter:
          setScale( 1.0 );
          e->setAccepted( true );
          break;

        default:
          break;
      }
    }
  }
}

QSize container_qt::scaled( const QSize& size )
{
  return QSize( std::floor( size.width() / mScale ), std::floor( size.height() / mScale ) );
}
QRect container_qt::scaled( const QRect& rect )
{
  return QRect( scaled( rect.topLeft() ), scaled( rect.size() ) );
}
QPoint container_qt::scaled( const QPoint& point )
{
  return QPoint( std::floor( point.x() / mScale ), std::floor( point.y() / mScale ) );
}

int container_qt::scaled( int i )
{
  return std::floor( i / mScale );
}

int container_qt::inv_scaled( int i )
{
  return std::floor( i * mScale );
}

void container_qt::mouseMoveEvent( QMouseEvent* e )
{

  if ( e && mDocument )
  {
    auto                       client_pos = scrollBarPos();
    auto                       mouse_pos  = mapFromGlobal( e->globalPos() );
    litehtml::position::vector redraw_boxes;
    if ( mDocument->on_mouse_over( mouse_pos.x() + client_pos.x(), mouse_pos.y() + client_pos.y(), client_pos.x(), client_pos.y(), redraw_boxes ) )
    {
      // something changed, redraw of boxes required;
    }
    //  bool litehtml::document::on_mouse_leave( position::vector& redraw_boxes );
  }
}
void container_qt::mouseReleaseEvent( QMouseEvent* e )
{
  if ( e && mDocument )
  {
    if ( Qt::LeftButton == e->button() )
    {
      auto                       client_pos = scrollBarPos();
      auto                       mouse_pos  = mapFromGlobal( e->globalPos() );
      litehtml::position::vector redraw_boxes;
      if ( mDocument->on_lbutton_up( mouse_pos.x() + client_pos.x(), mouse_pos.y() + client_pos.y(), client_pos.x(), client_pos.y(), redraw_boxes ) )
      {
        // something changed, redraw of boxes required;
      }
    }
  }
}
void container_qt::mousePressEvent( QMouseEvent* e )
{
  if ( e && mDocument )
  {
    if ( Qt::LeftButton == e->button() )
    {
      auto                       client_pos = scrollBarPos();
      auto                       mouse_pos  = mapFromGlobal( e->globalPos() );
      litehtml::position::vector redraw_boxes;
      if ( mDocument->on_lbutton_down( mouse_pos.x() + client_pos.x(), mouse_pos.y() + client_pos.y(), client_pos.x(), client_pos.y(),
                                       redraw_boxes ) )
      {
        // something changed, redraw of boxes required;
      }
    }
  }
}
