#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QUrl>
#include <functional>

namespace Browser
{
using ResourceHandlerType = std::function<QByteArray( int, const QUrl& )>;
enum class ResourceType : int
{
  Unknown,
  Html,
  Image,
  Css
};
} // namespace Browser
