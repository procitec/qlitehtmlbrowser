#pragma once

#include "litehtml.h"
#include "browserdefinitions.h"

#include <QtWidgets/QAbstractScrollArea>
#include <QtCore/QHash>
#include <QtCore/QUrl>

class container_qt : public QAbstractScrollArea, protected litehtml::document_container
{
  Q_OBJECT
public:
  container_qt( QWidget* parent = nullptr );

  void    setHtml( const QString& html, const QUrl& source_url = {} );
  QString html() const { return QString::fromUtf8( mDocumentSource ); }
  void    setCSS( const QString& css );
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

protected:
  void paintEvent( QPaintEvent* ) override;
  void wheelEvent( QWheelEvent* ) override;
  void keyPressEvent( QKeyEvent* ) override;
  void mouseMoveEvent( QMouseEvent* ) override;
  void mouseReleaseEvent( QMouseEvent* ) override;
  void mousePressEvent( QMouseEvent* ) override;

Q_SIGNALS:
  void anchorClicked( const QUrl& );
  void anchorClickedInfo( const QUrl& );
  void urlChanged( const QUrl& );

protected:
  litehtml::uint_ptr       create_font( const litehtml::tchar_t* faceName,
                                        int                      size,
                                        int                      weight,
                                        litehtml::font_style     italic,
                                        unsigned int             decoration,
                                        litehtml::font_metrics*  fm ) override;
  void                     delete_font( litehtml::uint_ptr hFont ) override;
  int                      text_width( const litehtml::tchar_t* text, litehtml::uint_ptr hFont ) override;
  void                     draw_text( litehtml::uint_ptr        hdc,
                                      const litehtml::tchar_t*  text,
                                      litehtml::uint_ptr        hFont,
                                      litehtml::web_color       color,
                                      const litehtml::position& pos ) override;
  int                      pt_to_px( int pt ) const override;
  int                      get_default_font_size() const override;
  const litehtml::tchar_t* get_default_font_name() const override;
  void                     draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker ) override;
  void                     load_image( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready ) override;
  void                     get_image_size( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz ) override;
  void                     draw_background( litehtml::uint_ptr hdc, const litehtml::background_paint& bg ) override;
  void draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root ) override;

  void set_caption( const litehtml::tchar_t* caption ) override;
  void set_base_url( const litehtml::tchar_t* base_url ) override;
  void link( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el ) override;
  void on_anchor_click( const litehtml::tchar_t* url, const litehtml::element::ptr& el ) override;
  void set_cursor( const litehtml::tchar_t* cursor ) override;
  void transform_text( litehtml::tstring& text, litehtml::text_transform tt ) override;
  void import_css( litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl ) override;
  void set_clip( const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y ) override;
  void del_clip() override;
  void get_client_rect( litehtml::position& client ) const override;
  std::shared_ptr<litehtml::element> create_element( const litehtml::tchar_t*                   tag_name,
                                                     const litehtml::string_map&                attributes,
                                                     const std::shared_ptr<litehtml::document>& doc ) override;

  void get_media_features( litehtml::media_features& media ) const override;
  void get_language( litehtml::tstring& language, litehtml::tstring& culture ) const override;

  void resizeEvent( QResizeEvent* event ) override;

private:
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
  QUrl                   resolveUrl( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl ) const;
  void                   render();
  QColor                 toColor( const litehtml::web_color& color ) const { return QColor( color.red, color.green, color.blue, color.alpha ); };
  Qt::PenStyle           toPenStyle( const litehtml::border_style& style ) const;
  std::pair<int, int>    findAnchorPos( const QString& anchor );
  litehtml::element::ptr findAnchor( const QString& anchor );
  QByteArray             loadResource( Browser::ResourceType type, const QUrl& url );
  MousePos               convertMousePos( const QMouseEvent* event );

private:
  std::shared_ptr<litehtml::document> mDocument;
  QByteArray                          mDocumentSource;
  litehtml::context                   mContext;
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
};
