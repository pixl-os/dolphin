// Copyright 2015 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Resources.h"

#include <QFileInfo>
#include <QIcon>
#include <QImageReader>
#include <QPixmap>
#include <QLibraryInfo>

#include "Common/Assert.h"
#include "Common/FileUtil.h"

#include "Core/Config/MainSettings.h"

#include "DolphinQt/Settings.h"
#include "Common/Logging/Log.h"

#include <stdio.h>
#include <iostream>
#include <string>

bool Resources::m_svg_supported;
QList<QIcon> Resources::m_platforms;
QList<QIcon> Resources::m_countries;
QList<QIcon> Resources::m_misc;

QIcon Resources::LoadNamedIcon(std::string_view name, const QString& dir)
{
  const QString base_path = dir + QLatin1Char{'/'} + QString::fromLatin1(name);
  const QString svg_path = base_path + QStringLiteral(".svg");
  
  std::cout << "svg_path : " << svg_path.toStdString().c_str() << std::endl;
  std::cout << "m_svg_supported : " << m_svg_supported ? "true" : "false";
  std::cout << std::endl;
  std::cout.flush(); // explicitly flush here
  
  // Prefer svg
  if (m_svg_supported && QFileInfo(svg_path).exists())
    return QIcon(svg_path);

  QIcon icon;

  auto load_png = [&](int scale) {
    QString suffix = QStringLiteral(".png");
    if (scale > 1)
      suffix = QString::fromLatin1("@%1x.png").arg(scale);

    std::cout << "base_path : " << base_path.toStdString().c_str() << std::endl;
    std::cout << "suffix : " << suffix.toStdString().c_str() << std::endl;
    std::cout.flush(); // explicitly flush here
  
    QPixmap pixmap(base_path + suffix);
    if (!pixmap.isNull())
    {
      pixmap.setDevicePixelRatio(scale);
      icon.addPixmap(pixmap);
    }
  };

  // Since we are caching the files, we need to try and load all known sizes up-front.
  // Otherwise, a dynamic change of devicePixelRatio could result in use of non-ideal image from
  // cache while a better one exists on disk.
  for (auto scale : {1, 2, 4})
    load_png(scale);

  ASSERT(icon.availableSizes().size() > 0);

  return icon;
}

static QString GetCurrentThemeDir()
{
  return QString::fromStdString(File::GetThemeDir(Config::Get(Config::MAIN_THEME_NAME)));
}

static QString GetResourcesDir()
{
  return QString::fromStdString(File::GetSysDirectory() + "Resources");
}

QIcon Resources::GetResourceIcon(std::string_view name)
{
  return LoadNamedIcon(name, GetResourcesDir());
}

QIcon Resources::GetThemeIcon(std::string_view name)
{
  return LoadNamedIcon(name, GetCurrentThemeDir());
}

void Resources::Init()
{

  //to check QT version used and paths associated
  std::cout << "Qt 'QLibraryInfo' version " << QLibraryInfo::version().toString().toStdString().c_str() << std::endl;
  std::cout << "Qt PrefixPath: " << QLibraryInfo::location(QLibraryInfo::PrefixPath).toStdString().c_str() << std::endl;        //	0	The default prefix for all paths.
  std::cout << "Qt DocumentationPath:" << QLibraryInfo::location(QLibraryInfo::DocumentationPath).toStdString().c_str() << std::endl; //	1	The location for documentation upon install.
  std::cout << "Qt HeadersPath:" << QLibraryInfo::location(QLibraryInfo::HeadersPath).toStdString().c_str() << std::endl; //	2	The location for all headers.
  std::cout << "Qt LibrariesPath:" << QLibraryInfo::location(QLibraryInfo::LibrariesPath).toStdString().c_str() << std::endl; //	3	The location of installed libraries.
  std::cout << "Qt LibraryExecutablesPath:" << QLibraryInfo::location(QLibraryInfo::LibraryExecutablesPath).toStdString().c_str() << std::endl; //	4	The location of installed executables required by libraries at runtime.
  std::cout << "Qt BinariesPath:" << QLibraryInfo::location(QLibraryInfo::BinariesPath).toStdString().c_str() << std::endl; //	5	The location of installed Qt binaries (tools and applications).
  std::cout << "Qt PluginsPath:" << QLibraryInfo::location(QLibraryInfo::PluginsPath).toStdString().c_str() << std::endl; //	6	The location of installed Qt plugins.
  std::cout << "Qt ImportsPath:" << QLibraryInfo::location(QLibraryInfo::QmlImportsPath).toStdString().c_str() << std::endl; //	7	The location of installed QML extensions to import (QML 1.x).
  std::cout << "Qt ArchDataPath:" << QLibraryInfo::location(QLibraryInfo::ArchDataPath).toStdString().c_str() << std::endl; //	9	The location of general architecture-dependent Qt data.
  std::cout << "Qt DataPath:" << QLibraryInfo::location(QLibraryInfo::DataPath).toStdString().c_str() << std::endl; //	10	The location of general architecture-independent Qt data.
  std::cout << "Qt TranslationsPath:" << QLibraryInfo::location(QLibraryInfo::TranslationsPath).toStdString().c_str() << std::endl; //	11	The location of translation information for Qt strings.
  std::cout << "Qt ExamplesPath:" << QLibraryInfo::location(QLibraryInfo::ExamplesPath).toStdString().c_str() << std::endl; //	12	The location for examples upon install.
  std::cout << "Qt TestsPath:" << QLibraryInfo::location(QLibraryInfo::TestsPath).toStdString().c_str() << std::endl; //	13	The location of installed Qt testcases.
  std::cout << "Qt SettingsPath:" << QLibraryInfo::location(QLibraryInfo::SettingsPath).toStdString().c_str() << std::endl; //	100	The location for Qt settings. Not applicable on Windows.
  std::cout.flush(); // explicitly flush here

  m_svg_supported = QImageReader::supportedImageFormats().contains("svg");

  for (std::string_view platform :
       {"Platform_Gamecube", "Platform_Wii", "Platform_Wad", "Platform_File"})
  {
    m_platforms.append(GetResourceIcon(platform));
  }

  for (std::string_view country :
       {"Flag_Europe", "Flag_Japan", "Flag_USA", "Flag_Australia", "Flag_France", "Flag_Germany",
        "Flag_Italy", "Flag_Korea", "Flag_Netherlands", "Flag_Russia", "Flag_Spain", "Flag_Taiwan",
        "Flag_International", "Flag_Unknown"})
  {
    m_countries.append(GetResourceIcon(country));
  }

  m_misc.append(GetResourceIcon("nobanner"));
  m_misc.append(GetResourceIcon("dolphin_logo"));
}

QIcon Resources::GetPlatform(DiscIO::Platform platform)
{
  return m_platforms[static_cast<int>(platform)];
}

QIcon Resources::GetCountry(DiscIO::Country country)
{
  return m_countries[static_cast<int>(country)];
}

QIcon Resources::GetMisc(MiscID id)
{
  return m_misc[static_cast<int>(id)];
}

QIcon Resources::GetAppIcon()
{
  return GetMisc(MiscID::Logo);
}
