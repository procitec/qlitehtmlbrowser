#pragma once

#include "litehtml.h"
#include "browserdefinitions.h"

#include <QtWidgets/QAbstractScrollArea>
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtGui/QPagedPaintDevice>
#include <QtCore/QStack>

class container_qt : public QAbstractScrollArea, protected litehtml::document_container
{
  Q_OBJECT
public:
  container_qt( QWidget* parent = nullptr );

  void    setHtml( const QString& html, const QUrl& source_url = {} );
  QString html() const { return QString::fromUtf8( mDocumentSource ); }
  void    setCSS( const QString& master_css, const QString& user_css );
  void    setScale( double scale );
  double  scale() const { return mScale; }
  void    scrollToAnchor( const QString& anchor );

  void setResourceHandler( const Browser::ResourceHandlerType& rh ) { mResourceHandler = rh; };
  void setUrlResolveHandler( const Browser::UrlResolveHandlerType& rh ) { mUrlResolverHandler = rh; }
  bool openLinks() const { return mOpenLinks; }
  bool openExternalLinks() const { return mOpenExternLinks; }

  void           setOpenLinks( bool open ) { mOpenLinks = open; }
  void           setOpenExternalLinks( bool open ) { mOpenExternLinks = open; }
  const QString& caption() const { return mCaption; }
  void           print( QPagedPaintDevice* paintDevice );
  int            searchText( const QString& text );
  void           scrollToNextSearchResult();
  void           scrollToPreviousSearchResult();

protected:
  void paintEvent( QPaintEvent* ) override;
  void wheelEvent( QWheelEvent* ) override;
  void mouseMoveEvent( QMouseEvent* ) override;
  void mouseReleaseEvent( QMouseEvent* ) override;
  void mousePressEvent( QMouseEvent* ) override;

Q_SIGNALS:
  void anchorClicked( const QUrl& );

protected:
  litehtml::uint_ptr create_font(
    const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm ) override;
  void delete_font( litehtml::uint_ptr hFont ) override;
  int  text_width( const char* text, litehtml::uint_ptr hFont ) override;
  void
  draw_text( litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos ) override;
  int         pt_to_px( int pt ) const override;
  int         get_default_font_size() const override;
  const char* get_default_font_name() const override;
  void        draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker ) override;
  void        load_image( const char* src, const char* baseurl, bool redraw_on_ready ) override;
  void        get_image_size( const char* src, const char* baseurl, litehtml::size& sz ) override;
  void        draw_background( litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bgs ) override;
  void        draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root ) override;

  void set_caption( const char* caption ) override;
  void set_base_url( const char* base_url ) override;
  void link( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el ) override;
  void on_anchor_click( const char* url, const litehtml::element::ptr& el ) override;
  void set_cursor( const char* cursor ) override;
  void transform_text( litehtml::string& text, litehtml::text_transform tt ) override;
  void import_css( litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl ) override;
  void set_clip( const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius ) override;
  void del_clip() override;
  void get_client_rect( litehtml::position& client ) const override;
  std::shared_ptr<litehtml::element>
  create_element( const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc ) override;

  void get_media_features( litehtml::media_features& media ) const override;
  void get_language( litehtml::string& language, litehtml::string& culture ) const override;
  void resizeEvent( QResizeEvent* event ) override;
  bool event( QEvent* event ) override;

private:
  void applyClip( QPainter* p );
  bool checkClipRect( QPainter* p, const QRect& rect ) const;

  struct MousePos
  {
    QPoint html;   // absolute position in HTML document
    QPoint client; // relative position in viewport
  };

  QPoint                 scrollBarPos() const;
  QSize                  scaled( const QSize& size ) const;
  QRect                  scaled( const QRect& rect ) const;
  QPoint                 scaled( const QPoint& point ) const;
  int                    scaled( int i ) const;
  int                    inv_scaled( int i ) const;
  QPixmap                load_image_data( const QUrl& url );
  QPixmap                load_pixmap( const QUrl& url );
  QUrl                   resolveUrl( const char* src, const char* baseurl ) const;
  void                   render();
  QColor                 toColor( const litehtml::web_color& color ) const { return QColor( color.red, color.green, color.blue, color.alpha ); };
  Qt::PenStyle           toPenStyle( const litehtml::border_style& style ) const;
  std::pair<int, int>    findAnchorPos( const QString& anchor );
  litehtml::element::ptr findAnchor( const QString& anchor );
  QByteArray             loadResource( Browser::ResourceType type, const QUrl& url );
  MousePos               convertMousePos( const QMouseEvent* event );

private:
  struct TextFragment
  {
    std::string            text;
    litehtml::element::ptr element;
    litehtml::position     pos;
    int                    start_offset; // Offset in element text
    int                    end_offset;   // end Offset in element text
  };

  struct TextSearchResult
  {
    std::string               matched_text;
    std::vector<TextFragment> fragments;    // may contains multiple elements
    litehtml::position        bounding_box; // bounding box over all elements
  };

  int                                  search_text( litehtml::document::ptr doc, const std::string& search_term, bool case_sensitive = true );
  bool                                 next_search_result();
  bool                                 previous_search_result();
  const TextSearchResult*              get_current_result() const;
  void                                 highlight_text_at_position( litehtml::uint_ptr hdc, const litehtml::position& pos, const std::string& text );
  void                                 draw_highlights( litehtml::uint_ptr hdc );
  void                                 clear_highlights() { m_search_results.clear(); }
  std::string                          normalizeWhitespace( const std::string& text );
  void                                 searchTextInDocument( litehtml::document::ptr        doc,
                                                             const std::string&             search_term,
                                                             std::vector<TextSearchResult>& results,
                                                             bool                           case_sensitive = true );
  void                                 collectTextFragments( litehtml::element::ptr el, std::vector<TextFragment>& fragments, std::string& fullText );
  litehtml::position                   calculatePreciseBoundingBox( const std::vector<TextFragment>& allFragments,
                                                                    int                              searchStart,
                                                                    int                              searchEnd,
                                                                    std::vector<TextFragment>&       matchedFragments );
  void                                 scrollToSearchResult( const TextSearchResult* );

  std::shared_ptr<litehtml::document> mDocument;
  QByteArray                          mDocumentSource;
  QUrl                                mBaseUrl;
  QUrl                                mSourceUrl;
  int                                 mFontSize           = 12;
  double                              mScale              = 1.0;
  double                              mMinScale           = 0.1;
  double                              mMaxScale           = 4.0;
  QHash<QUrl, QPixmap>                mPixmapCache        = {};
  Browser::ResourceHandlerType        mResourceHandler    = {};
  bool                                mOpenLinks          = true;
  bool                                mOpenExternLinks    = false;
  QByteArray                          mFontInfo           = {};
  QString                             mCaption            = {};
  Browser::UrlResolveHandlerType      mUrlResolverHandler = {};
  QString                             mMasterCSS;
  QString                             mUserCSS;
  QStack<litehtml::position>          mClipStack;
  litehtml::position                  mClip = {};
  std::vector<TextSearchResult>       m_search_results       = {};
  int                                 m_current_result_index = -1;
  const QColor                        mHighlightColor        = QColor( 255, 255, 0, 30 );
};
