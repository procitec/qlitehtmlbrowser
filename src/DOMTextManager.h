#pragma once

#include <litehtml.h>
#include <QtCore/QDebug>

class DOMTextManager
{
public:
  struct TextFragment
  {
    std::string            text;
    litehtml::element::ptr element;
    litehtml::position     pos;
    int                    start_char_offset; // Offset in element text
    int                    end_char_offset;   // end Offset in element text
    int                    global_start_offset;
    int                    global_end_offset;
    bool                   is_leaf;

    bool hasValidPosition() const { return pos.width > 0 && pos.height > 0; }
  };

  struct TextFindMatch
  {
    std::string               matched_text;
    std::vector<TextFragment> fragments;    // may contains multiple elements
    litehtml::position        bounding_box; // bounding box over all elements

    bool isEmpty() const { return fragments.empty(); }
  };

  struct TextPosition
  {
    litehtml::element::ptr element;
    int                    char_offset; // Zeichenposition im Element-Text
    litehtml::position     element_pos;

    bool operator<( const TextPosition& other ) const
    {
      // Vergleich basierend auf Position im Dokument
      if ( element_pos.y != other.element_pos.y )
      {
        return element_pos.y < other.element_pos.y;
      }
      return element_pos.x < other.element_pos.x;
    }

    bool isValid() const { return element != nullptr; }
  };

  struct SelectionRange
  {
    TextPosition              start;
    TextPosition              end;
    std::vector<TextFragment> fragments; // Alle Textfragmente in der Selektion

    bool isEmpty() const { return !start.isValid() || !end.isValid(); }

    void clear()
    {
      start = TextPosition();
      end   = TextPosition();
      fragments.clear();
    }
  };

private:
  std::vector<TextFragment> m_fragments;
  std::string               m_fullText;

public:
  void buildFromDocument( litehtml::document::ptr doc )
  {
    m_fragments.clear();
    m_fullText.clear();

    if ( doc && doc->root() )
    {
      collectTextFragments( doc->root() );

      qDebug() << "\n=== Gesammelte Text-Fragmente ===";
      qDebug() << "Anzahl Fragmente:" << m_fragments.size();
      for ( size_t i = 0; i < m_fragments.size(); ++i )
      {
        const auto& frag = m_fragments[i];
        qDebug() << "Fragment" << i << ":"
                 << "Text:" << QString::fromStdString( frag.text ).left( 30 ) << "| Leaf:" << frag.is_leaf << "| Pos:" << frag.pos.x << frag.pos.y
                 << "| Size:" << frag.pos.width << "x" << frag.pos.height << "| Valid:" << frag.hasValidPosition();
      }
      qDebug() << "================================\n";
    }
  }

  // Findet die Text-Position basierend auf Pixel-Koordinaten
  TextPosition getPositionAtCoordinates( int x, int y )
  {
    TextPosition  result;
    TextFragment* bestMatch    = nullptr;
    int           smallestArea = INT_MAX;

    qDebug() << "\n>>> Suche Element bei:" << x << y;

    // Suche das kleinste Element, das den Punkt enthält (spezifischstes Element)
    for ( auto& fragment : m_fragments )
    {
      // Nur Leaf-Nodes und Elemente mit gültiger Position prüfen
      if ( !fragment.is_leaf || !fragment.hasValidPosition() )
      {
        continue;
      }

      const auto& pos = fragment.pos;

      // Prüfe ob Punkt im Element liegt
      if ( x >= pos.x && x <= pos.x + pos.width && y >= pos.y && y <= pos.y + pos.height )
      {

        int area = pos.width * pos.height;

        qDebug() << "  Kandidat:" << QString::fromStdString( fragment.text ).left( 20 ) << "| Box:" << pos.x << pos.y << pos.width << pos.height
                 << "| Area:" << area;

        // Wähle das kleinste umschließende Element
        if ( area < smallestArea )
        {
          smallestArea = area;
          bestMatch    = &fragment;
        }
      }
    }

    if ( bestMatch )
    {
      result.element     = bestMatch->element;
      result.element_pos = bestMatch->pos;

      // Berechne Zeichen-Offset basierend auf x-Position
      const auto& pos = bestMatch->pos;
      if ( pos.width > 0 && bestMatch->text.length() > 0 )
      {
        float relativeX    = static_cast<float>( x - pos.x ) / pos.width;
        result.char_offset = static_cast<int>( relativeX * bestMatch->text.length() );
        result.char_offset = std::max( 0, std::min( result.char_offset, static_cast<int>( bestMatch->text.length() ) ) );
      }
      else
      {
        result.char_offset = 0;
      }

      qDebug() << ">>> TREFFER:" << QString::fromStdString( bestMatch->text ).left( 30 ) << "| Offset:" << result.char_offset;
    }
    else
    {
      qDebug() << ">>> KEIN TREFFER!";
    }

    return result;
  }

  // Extrahiert Text und Fragmente zwischen zwei Positionen
  SelectionRange getSelectionBetween( const TextPosition& start, const TextPosition& end )
  {
    SelectionRange selection;
    selection.start = start;
    selection.end   = end;

    if ( !start.isValid() || !end.isValid() )
    {
      return selection;
    }

    // Sicherstellen dass start vor end liegt
    TextPosition actualStart = start;
    TextPosition actualEnd   = end;
    if ( end < start )
    {
      std::swap( actualStart, actualEnd );
    }

    bool inSelection = false;

    // Nur durch Leaf-Nodes iterieren
    for ( const auto& fragment : m_fragments )
    {
      if ( !fragment.is_leaf || !fragment.hasValidPosition() )
      {
        continue;
      }

      bool isStartFragment = ( fragment.element == actualStart.element );
      bool isEndFragment   = ( fragment.element == actualEnd.element );

      if ( isStartFragment )
      {
        inSelection = true;
      }

      if ( inSelection )
      {
        TextFragment selectedFragment = fragment;
        int          startOffset      = 0;
        int          endOffset        = fragment.text.length();

        if ( isStartFragment )
        {
          startOffset = actualStart.char_offset;
        }

        if ( isEndFragment )
        {
          endOffset = actualEnd.char_offset;
        }

        selectedFragment.start_char_offset = startOffset;
        selectedFragment.end_char_offset   = endOffset;

        selection.fragments.push_back( selectedFragment );

        if ( isEndFragment )
        {
          break;
        }
      }
    }

    qDebug() << "Selektion umfasst" << selection.fragments.size() << "Fragmente";

    return selection;
  }

  const std::vector<TextFragment>& getFragments() const { return m_fragments; }

  std::vector<TextFindMatch> findText( const std::string& search_term, bool case_sensitive = true )
  {
    std::vector<TextFindMatch> results;

    if ( search_term.empty() || m_fullText.empty() )
    {
      return results;
    }

    // Normalisiere Text
    std::string normalizedFullText   = ( m_fullText );
    std::string normalizedSearchTerm = normalizeWhitespace( search_term );

    std::string searchText = normalizedFullText;
    std::string searchFor  = normalizedSearchTerm;

    if ( !case_sensitive )
    {
      std::transform( searchText.begin(), searchText.end(), searchText.begin(), ::tolower );
      std::transform( searchFor.begin(), searchFor.end(), searchFor.begin(), ::tolower );
    }

    // Alle Vorkommen finden
    size_t pos = 0;
    while ( ( pos = searchText.find( searchFor, pos ) ) != std::string::npos )
    {
      TextFindMatch result;
      result.matched_text = normalizedFullText.substr( pos, searchFor.length() );

      int searchStart = static_cast<int>( pos );
      int searchEnd   = static_cast<int>( pos + searchFor.length() );

      // Finde alle Fragmente in diesem Bereich
      result.bounding_box = calculateBoundingBox( searchStart, searchEnd, result.fragments );
      // result.bounding_box = calculatePreciseBoundingBox( searchStart, searchEnd, result.fragments );

      results.push_back( result );
      pos += searchFor.length();
    }

    qDebug() << "Suche nach '" << QString::fromStdString( search_term ) << "': " << results.size() << " Treffer";

    return results;
  }

private:
  std::string normalizeWhitespace( const std::string& text )
  {
    std::string result;
    bool        lastWasSpace = false;

    for ( char c : text )
    {
      if ( std::isspace( static_cast<unsigned char>( c ) ) )
      {
        if ( !lastWasSpace && !result.empty() )
        {
          result += ' ';
          lastWasSpace = true;
        }
      }
      else
      {
        result += c;
        lastWasSpace = false;
      }
    }

    return result;
  }

  void collectTextFragments( litehtml::element::ptr el )
  {
    if (!el) return;

    // Prüfe ob Element Kinder hat
    bool hasChildren = !el->children().empty();

    // Text aus dem Element holen
    std::string text;
    el->get_text( text );

    // NUR Leaf-Nodes (Elemente ohne Kinder) mit Text speichern
    // Dadurch vermeiden wir, dass Parent-Elemente den gesamten
    // aggregierten Text ihrer Kinder zurückgeben
    if ( !hasChildren && !text.empty() )
    {
      litehtml::position pos = el->get_placement();

      // Nur wenn Element eine gültige Position hat
      if ( pos.width > 0 && pos.height > 0 )
      {
        TextFragment fragment;
        fragment.text                = normalizeWhitespace( text );
        fragment.element             = el;
        fragment.pos                 = pos;
        fragment.start_char_offset   = 0;
        fragment.end_char_offset     = fragment.text.length();
        fragment.global_start_offset = m_fullText.length();
        fragment.is_leaf             = true;

        m_fullText += fragment.text + " ";
        fragment.global_end_offset = m_fullText.length();

        m_fragments.push_back( fragment );

        qDebug() << "Leaf-Fragment:" << QString::fromStdString( fragment.text ).left( 30 ) << "| Pos:" << pos.x << pos.y << "| Size:" << pos.width
                 << "x" << pos.height;
      }
    }

    // Rekursiv durch alle Kindelemente
    for ( auto it = el->children().begin(); it != el->children().end(); ++it )
    {
      collectTextFragments( *it );
    }
  }

  litehtml::position calculateBoundingBox( int searchStart, int searchEnd, std::vector<TextFragment>& matchedFragments )
  {
    litehtml::position boundingBox   = { 0, 0, 0, 0 };
    bool               firstFragment = true;

    qDebug() << "\n=== Berechne Bounding Box ===";
    qDebug() << "Suchbereich: global offset" << searchStart << "bis" << searchEnd;
    qDebug() << "Gesuchter Text:"
             << QString::fromStdString(
                  m_fullText.substr( searchStart, std::min( (size_t)( searchEnd - searchStart ), m_fullText.length() - searchStart ) ) );

    for ( const auto& fragment : m_fragments )
    {
      if ( !fragment.is_leaf || !fragment.hasValidPosition() )
      {
        continue;
      }

      // Überlappung zwischen Suchbereich und Fragment
      int overlapStart = std::max( searchStart, fragment.global_start_offset );
      int overlapEnd   = std::min( searchEnd, fragment.global_end_offset );

      if ( overlapStart < overlapEnd )
      {
        // Kopie des Fragments erstellen
        TextFragment matchedFragment = fragment;

        // Lokale Offsets im Fragment-Text berechnen
        int localStart = overlapStart - fragment.global_start_offset;
        int localEnd   = overlapEnd - fragment.global_start_offset;

        // WICHTIG: Bounds checking
        int maxLen = static_cast<int>( fragment.text.length() );
        localStart = std::clamp( localStart, 0, maxLen );
        localEnd   = std::clamp( localEnd, 0, maxLen );

        if ( localEnd <= localStart )
        {
          qDebug() << "Warnung: Ungültige Offsets für Fragment";
          continue;
        }

        matchedFragment.start_char_offset = localStart;
        matchedFragment.end_char_offset   = localEnd;

        // WICHTIG: Position des matched Fragments anpassen
        litehtml::position adjustedPos = fragment.pos;

        if ( maxLen > 0 && fragment.pos.width > 0 )
        {
          float charWidth = static_cast<float>( fragment.pos.width ) / static_cast<float>( maxLen );

          // X-Position anpassen basierend auf localStart
          adjustedPos.x = fragment.pos.x + static_cast<int>( charWidth * localStart );

          // Breite anpassen basierend auf Anzahl der Zeichen
          adjustedPos.width = static_cast<int>( charWidth * ( localEnd - localStart ) );
        }

        // Speichere die angepasste Position IM Fragment
        matchedFragment.pos = adjustedPos;

        matchedFragments.push_back( matchedFragment );

        qDebug() << "Fragment gefunden:" << QString::fromStdString( fragment.text.substr( localStart, localEnd - localStart ) )
                 << "| Local:" << localStart << "-" << localEnd << "| Pos:" << adjustedPos.x << adjustedPos.y << "| Size:" << adjustedPos.width << "x"
                 << adjustedPos.height;

        // Bounding Box erweitern
        if ( firstFragment )
        {
          boundingBox   = adjustedPos;
          firstFragment = false;
        }
        else
        {
          int minX = std::min( boundingBox.x, adjustedPos.x );
          int minY = std::min( boundingBox.y, adjustedPos.y );
          int maxX = std::max( boundingBox.x + boundingBox.width, adjustedPos.x + adjustedPos.width );
          int maxY = std::max( boundingBox.y + boundingBox.height, adjustedPos.y + adjustedPos.height );

          boundingBox.x      = minX;
          boundingBox.y      = minY;
          boundingBox.width  = maxX - minX;
          boundingBox.height = maxY - minY;
        }
      }
    }

    qDebug() << "Finale Bounding Box:" << boundingBox.x << boundingBox.y << boundingBox.width << "x" << boundingBox.height;
    qDebug() << "=========================\n";

    return boundingBox;
  }
};
