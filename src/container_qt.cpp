#include "container_qt.h"

container_qt::container_qt( QWidget* parent )
  : QWidget( parent )
{
}

litehtml::uint_ptr container_qt::create_font(
  const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm )
{
}
void container_qt::delete_font( litehtml::uint_ptr hFont ) {}
int  container_qt::text_width( const litehtml::tchar_t* text, litehtml::uint_ptr hFont ) {}
void container_qt::draw_text(
  litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos )
{
}
int                      container_qt::pt_to_px( int pt ) {}
int                      container_qt::get_default_font_size() const {}
const litehtml::tchar_t* container_qt::get_default_font_name() const {}
void                     container_qt::draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker ) {}
void                     container_qt::load_image( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready ) {}
void                     container_qt::get_image_size( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz ) {}
void                     container_qt::draw_background( litehtml::uint_ptr hdc, const litehtml::background_paint& bg ) {}
void container_qt::draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root ) {}

void container_qt::set_caption( const litehtml::tchar_t* caption ) {}
void container_qt::set_base_url( const litehtml::tchar_t* base_url ) {}
void container_qt::link( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el ) {}
void container_qt::on_anchor_click( const litehtml::tchar_t* url, const litehtml::element::ptr& el ) {}
void container_qt::set_cursor( const litehtml::tchar_t* cursor ) {}
void container_qt::transform_text( litehtml::tstring& text, litehtml::text_transform tt ) {}
void container_qt::import_css( litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl ) {}
void container_qt::set_clip( const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y ) {}
void container_qt::del_clip() {}
void container_qt::get_client_rect( litehtml::position& client ) const {}
std::shared_ptr<litehtml::element> container_qt::create_element( const litehtml::tchar_t*                   tag_name,
                                                                 const litehtml::string_map&                attributes,
                                                                 const std::shared_ptr<litehtml::document>& doc )
{
}

void container_qt::get_media_features( litehtml::media_features& media ) const {}
void container_qt::get_language( litehtml::tstring& language, litehtml::tstring& culture ) const {}
