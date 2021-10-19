#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QUrl>
#include <functional>

namespace Browser
{
using ResourceHandlerType   = std::function<QByteArray( int, const QUrl& )>;
using UrlResolveHandlerType = std::function<QUrl( const QString& )>;

enum class ResourceType : int
{
  Unknown,
  Html,
  Image,
  Css
};
} // namespace Browser
