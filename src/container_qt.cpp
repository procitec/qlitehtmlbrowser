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
#include <QtGui/QKeySequence>
#include <QtGui/QShortcut>
#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtCore/QTextBoundaryFinder>

#include <algorithm>
#include <cmath>
#include <string>

container_qt::container_qt( QWidget* parent )
  : QAbstractScrollArea( parent )
{
  auto* shortcut = new QShortcut( QKeySequence{ QKeySequence::ZoomIn }, this );
  shortcut->setContext( Qt::WidgetShortcut );
  connect( shortcut, &QShortcut::activated, this, [this]() { setScale( scale() + 0.1 ); } );

  shortcut = new QShortcut( QKeySequence{ QKeySequence::ZoomOut }, this );
  shortcut->setContext( Qt::WidgetShortcut );
  connect( shortcut, &QShortcut::activated, this, [this]() { setScale( scale() - 0.1 ); } );

  shortcut = new QShortcut( tr( "Ctrl+0" ), this );
  shortcut->setContext( Qt::WidgetShortcut );
  connect( shortcut, &QShortcut::activated, this, [this]() { setScale( 1.0 ); } );

  setMouseTracking( true );
}

void container_qt::setCSS( const QString& master_css, const QString& user_css )
{
  mMasterCSS = master_css;
  mUserCSS   = user_css;
}

void container_qt::setHtml( const QString& html, const QUrl& source_url )
{

  if ( !html.isEmpty() )
  {
    if ( !source_url.isEmpty() && !source_url.isValid() )
      qCritical( "%s", qPrintable( QString( "container_qt.::setHtml(): `source_url` is not valid: %1" ).arg( source_url.toString() ) ) );
    auto pure_url = source_url;

    pure_url.setFragment( {} );
    mSourceUrl      = pure_url;
    mDocumentSource = html.toUtf8();
    mCaption.clear();

    mDocument = litehtml::document::createFromString(
      mDocumentSource, this, mMasterCSS.isEmpty() ? litehtml::master_css : mMasterCSS.toUtf8().constData(), mUserCSS.toUtf8().constData() );
    verticalScrollBar()->setValue( 0 );
    horizontalScrollBar()->setValue( 0 );
    render();

    auto frag = source_url.fragment();
    if ( !frag.isEmpty() )
    {
      scrollToAnchor( frag );
    }
  }
}

QPoint container_qt::scrollBarPos() const
{
  return { horizontalScrollBar()->value(), verticalScrollBar()->value() };
}

void container_qt::render()
{
  if ( !mDocument )
    return;

  mDocument->render( scaled( this->viewport()->width() ) );

  auto* hBar = horizontalScrollBar();
  hBar->setPageStep( scaled( viewport()->width() ) );
  hBar->setSingleStep( hBar->pageStep() / 20 );
  hBar->setRange( 0, std::max( 0, mDocument->width() - scaled( viewport()->width() ) ) );

  auto* vBar = verticalScrollBar();
  vBar->setPageStep( scaled( viewport()->height() ) );
  vBar->setSingleStep( vBar->pageStep() / 20 );
  vBar->setRange( 0, std::max( 0, mDocument->height() - scaled( viewport()->height() ) ) );

  viewport()->update();
}

void container_qt::paintEvent( QPaintEvent* event )
{
  if ( event )
  {
    if ( mDocument )
    {
      QPainter p( viewport() );
      p.setWorldTransform( QTransform().scale( mScale, mScale ) );
      p.setRenderHint( QPainter::SmoothPixmapTransform, true );
      p.setRenderHint( QPainter::Antialiasing, true );

      const litehtml::position clipRect   = { scaled( event->rect().x() ), scaled( event->rect().y() ), scaled( event->rect().width() ),
                                              scaled( event->rect().height() ) };
      auto                     margins    = viewport()->contentsMargins();
      auto                     scroll_pos = -scrollBarPos();

      mDocument->draw( reinterpret_cast<litehtml::uint_ptr>( &p ), margins.left() + scroll_pos.x(), margins.top() + scroll_pos.y(), &clipRect );
    }
  }
}

litehtml::uint_ptr container_qt::create_font(
  const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm )
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

  font->setFixedPitch( true );
  font->setPixelSize( size );

  font->setItalic( litehtml::font_style_italic == italic ? true : false );

  // thinner, thicker? @see https://www.w3schools.com/cssref/pr_font_weight.asp
  font->setWeight( static_cast<QFont::Weight>( 100 * std::clamp( weight / 100, 1, 9 ) ) );

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

int container_qt::text_width( const char* text, litehtml::uint_ptr hFont )
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
  litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );
  QFont*    f = reinterpret_cast<QFont*>( hFont );
  p->save();
  p->setFont( *f );
  auto web_color = toColor( color );
  p->setPen( web_color );
  p->drawText( QRect( pos.x, pos.y, pos.width, pos.height ), 0, QString::fromUtf8( text ) );
  p->restore();
}

int container_qt::pt_to_px( int pt ) const
{
  return this->viewport()->physicalDpiY() * pt / this->viewport()->logicalDpiY();
}

int container_qt::get_default_font_size() const
{
  return mFontSize;
}

const char* container_qt::get_default_font_name() const
{
  return mFontInfo.constData();
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
  pm.loadFromData( loadResource( Browser::ResourceType::Image, url ) );
  return pm;
}

// hint: from QTextBrowser Private
QUrl container_qt::resolveUrl( const char* src, const char* baseurl ) const
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

  if ( resolved_url.isRelative() )
  {
    resolved_url = mUrlResolverHandler( _url.toString() );
  }

  return resolved_url;
}

QByteArray container_qt::loadResource( Browser::ResourceType type, const QUrl& url )
{
  return mResourceHandler( static_cast<int>( type ), url );
}

container_qt::MousePos container_qt::convertMousePos( const QMouseEvent* event )
{
  MousePos pos;
  pos.client = { scaled( mapFromGlobal( event->globalPosition().toPoint() ) ) };
  pos.html   = { pos.client + scrollBarPos() };

  return pos;
}

void container_qt::load_image( const char* src, const char* baseurl, bool redraw_on_ready )
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

void container_qt::get_image_size( const char* src, const char* baseurl, litehtml::size& sz )
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
void container_qt::draw_background( litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bgs )
{
  QPainter* p( reinterpret_cast<QPainter*>( hdc ) );
  p->save();
  p->setPen( Qt::NoPen );
  for ( auto bg : bgs )
  {
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

void container_qt::set_caption( const char* caption )
{
  mCaption = QString::fromStdString( caption );
}

void container_qt::set_base_url( const char* base_url )
{
  auto url = QUrl( base_url );
  url.setFragment( {} );
  mBaseUrl = base_url;
}

void container_qt::link( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el ) {}
void container_qt::on_anchor_click( const char* url, const litehtml::element::ptr& el )
{
  if ( mDocument )
  {
    QUrl resolved_url;

    if ( url && *url == '#' )
    {
      // assert( std::string( el->get_attr( "class", "" ) ) == std::string( "reference internal" ) );
      resolved_url = mSourceUrl;
      resolved_url.setFragment( QString( url + 1 ) );
    }
    else
    {
      resolved_url = resolveUrl( url, nullptr );
    }

    emit anchorClicked( resolved_url );
  }
}
void container_qt::set_cursor( const char* cursor )
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
void container_qt::transform_text( litehtml::string& text, litehtml::text_transform tt )
{
  switch ( tt )
  {
    case litehtml::text_transform_capitalize:
    {
      auto                str = QString::fromStdString( text );
      QTextBoundaryFinder finder( QTextBoundaryFinder::Word, str );
      auto                position = finder.toNextBoundary();

      while ( 0 <= position )
      {
        if ( finder.boundaryReasons() & QTextBoundaryFinder::EndOfItem )
        {
          str.replace( 0, 1, str[0].toUpper() );
        }
        else if ( finder.boundaryReasons() & QTextBoundaryFinder::StartOfItem )
        {
          str.replace( position, 1, str[position].toUpper() );
        }
        position = finder.toNextBoundary();
      }

      text = str.toStdString();
    }
    break;
    case litehtml::text_transform_lowercase:
      text = QString::fromStdString( text ).toLower().toStdString();
      break;
    case litehtml::text_transform_uppercase:
      text = QString::fromStdString( text ).toUpper().toStdString();
      break;
    case litehtml::text_transform_none:
      break;
  }
}
void container_qt::import_css( litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl )
{
  auto resolved_url = resolveUrl( url.c_str(), baseurl.c_str() );
  auto content      = loadResource( Browser::ResourceType::Css, resolved_url );
  if ( !content.isEmpty() )
  {
    text = QString::fromUtf8( content.constData() ).toStdString();
  }
}
void container_qt::set_clip( const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius /*,bool valid_x, bool valid_y */ ) {}
void container_qt::del_clip() {}

void container_qt::get_client_rect( litehtml::position& client ) const
{
  client.width  = scaled( this->viewport()->width() );
  client.height = scaled( this->viewport()->height() );
  client.x      = contentsMargins().left();
  client.y      = contentsMargins().top();
}

std::shared_ptr<litehtml::element>
container_qt::create_element( const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc )
{
  return {};
}

void container_qt::setScale( double scale )
{
  auto relH =
    ( 0 < horizontalScrollBar()->maximum() ) ? static_cast<float>( horizontalScrollBar()->value() ) / horizontalScrollBar()->maximum() : 0.0;
  auto relV = ( 0 < verticalScrollBar()->maximum() ) ? static_cast<float>( verticalScrollBar()->value() ) / verticalScrollBar()->maximum() : 0.0;
  mScale    = std::clamp( scale, mMinScale, mMaxScale );
  render();
  horizontalScrollBar()->setValue( std::floor( relH * horizontalScrollBar()->maximum() ) );
  verticalScrollBar()->setValue( std::floor( relV * verticalScrollBar()->maximum() ) );
}

void container_qt::get_media_features( litehtml::media_features& media ) const
{
  litehtml::position client;
  get_client_rect( client );
  media.type        = litehtml::media_type_screen;
  media.width       = client.width;
  media.height      = client.height;
  media.color       = 8;
  media.monochrome  = 0;
  media.color_index = 256;
  auto w_screen     = screen();
  if ( w_screen )
  {
    media.device_width  = w_screen->availableGeometry().width();
    media.device_height = w_screen->availableGeometry().height();
    media.resolution    = w_screen->devicePixelRatio();
  }
}
void container_qt::get_language( litehtml::string& language, litehtml::string& culture ) const
{
  language = "en";
  culture  = "";
}

void container_qt::resizeEvent( QResizeEvent* event )
{
  auto relH =
    ( 0 < horizontalScrollBar()->maximum() ) ? static_cast<float>( horizontalScrollBar()->value() ) / horizontalScrollBar()->maximum() : 0.0;
  auto relV = ( 0 < verticalScrollBar()->maximum() ) ? static_cast<float>( verticalScrollBar()->value() ) / verticalScrollBar()->maximum() : 0.0;
  QAbstractScrollArea::resizeEvent( event );
  render();
  horizontalScrollBar()->setValue( std::floor( relH * horizontalScrollBar()->maximum() ) );
  verticalScrollBar()->setValue( std::floor( relV * verticalScrollBar()->maximum() ) );
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

bool container_qt::event( QEvent* event )
{
  if ( event->type() == QEvent::Type::ApplicationFontChange )
  {
    mFontInfo = fontInfo().family().toLocal8Bit();
  }

  return QAbstractScrollArea::event( event );
}

QSize container_qt::scaled( const QSize& size ) const
{
  return QSize( std::floor( size.width() / mScale ), std::floor( size.height() / mScale ) );
}
QRect container_qt::scaled( const QRect& rect ) const
{
  return QRect( scaled( rect.topLeft() ), scaled( rect.size() ) );
}
QPoint container_qt::scaled( const QPoint& point ) const
{
  return QPoint( std::floor( point.x() / mScale ), std::floor( point.y() / mScale ) );
}

int container_qt::scaled( int i ) const
{
  return std::floor( i / mScale );
}

int container_qt::inv_scaled( int i ) const
{
  return std::floor( i * mScale );
}

void container_qt::mouseMoveEvent( QMouseEvent* e )
{

  if ( e && mDocument )
  {

    litehtml::position::vector redraw_boxes;
    const auto                 mousePos = convertMousePos( e );
    if ( mDocument->on_mouse_over( mousePos.html.x(), mousePos.html.y(), mousePos.client.x(), mousePos.client.y(), redraw_boxes ) )
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
      litehtml::position::vector redraw_boxes;
      const auto                 mousePos = convertMousePos( e );
      if ( mDocument->on_lbutton_up( mousePos.html.x(), mousePos.html.y(), mousePos.client.x(), mousePos.client.y(), redraw_boxes ) )
      {
        // something changed, redraw of boxes required;
        e->accept();
      }
      else
        e->ignore();
    }
    else
      e->ignore();
  }
}
void container_qt::mousePressEvent( QMouseEvent* e )
{
  if ( e && mDocument )
  {
    if ( Qt::LeftButton == e->button() )
    {
      litehtml::position::vector redraw_boxes;
      const auto                 mousePos = convertMousePos( e );
      if ( mDocument->on_lbutton_down( mousePos.html.x(), mousePos.html.y(), mousePos.client.x(), mousePos.client.y(), redraw_boxes ) )
      {
        // something changed, redraw of boxes required;
        e->accept();
      }
      else
        e->ignore();
    }
    else
      e->ignore();
  }
}

void container_qt::print( QPagedPaintDevice* paintDevice )
{
  QPainter painter( paintDevice );

  painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
  painter.setRenderHint( QPainter::Antialiasing, true );

  auto resolutionX = paintDevice->physicalDpiX();

  const auto pageLayout = paintDevice->pageLayout();
  const auto pageRect   = pageLayout.paintRectPixels( resolutionX );

  auto         width  = mDocument->width();
  const double xscale = static_cast<int>( std::floor( pageRect.width() / double( width ) ) );

  auto       scaled_document_height       = mDocument->height() * xscale;
  const auto printable_page_height        = ( pageRect.height() );
  const auto scaled_printable_page_height = static_cast<int>( std::floor( printable_page_height / xscale ) );
  const auto scaled_printable_page_width  = mDocument->width();
  auto       number_of_pages              = static_cast<int>( std::ceil( scaled_document_height / printable_page_height ) );

  painter.translate( pageRect.width() / 2., pageRect.height() / 2. );
  painter.scale( xscale, xscale );
  painter.translate( -scaled_printable_page_width / 2., ( -scaled_printable_page_height / 2. ) );

  litehtml::position clipRect = { 0, 0, scaled_printable_page_width, scaled_printable_page_height };
  painter.setClipRect( QRect{ 0, 0, scaled_printable_page_width, scaled_printable_page_height } );

  for ( auto page = 0; page < number_of_pages; page++ )
  {
    mDocument->draw( reinterpret_cast<litehtml::uint_ptr>( &painter ), 0, -page * ( scaled_printable_page_height ), &clipRect );
    if ( page != number_of_pages - 1 )
    {
      paintDevice->newPage();
    }
  }
}
