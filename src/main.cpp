#include <QApplication>

#include "any_of.h"
#include "config.h"
#include "main_widget.h"

#include <docopt.h>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <random>
#include <string>
#include <tuple>
#include <utility>

inline constexpr auto USAGE =
   R"(imgv

    Usage:
      imgv --path <path> [ --shuffle ] [ --maximized ]
      imgv (-h | --help)
      imgv --version

    Options:
      --path <path>   Images in the given directory will be shown
      --shuffle       Shows a random image in the filelist
      --maximized     Shows the window maximized
      -h --help       Show this screen.
      --version       Show version.
)";

int main(int argc, char** argv)
{
   try {
      spdlog::set_level(spdlog::level::debug);
      spdlog::info("imgv started ...");

      auto const args =
         docopt::docopt(USAGE, {argv + 1, argv + argc}, true,
                        std::format("{} {}.{}", IMGV_BINARY_NAME, IMGV_VERSION_MAJOR, IMGV_VERSION_MINOR));

      const auto is_png = [](auto const& e) { return e.path().extension() == ".png"; };
      const auto is_gif = [](auto const& e) { return e.path().extension() == ".gif"; };
      const auto is_jpg = [](auto const& e) { return e.path().extension() == ".jpg"; };
      const auto is_jpeg = [](auto const& e) { return e.path().extension() == ".jpeg"; };

      auto input_path = args.at("--path").asString();

      std::vector<std::filesystem::path> found_images{};
      std::copy_if(std::filesystem::directory_iterator{input_path}, {}, std::back_inserter(found_images),
                   imgv::any_of(is_png, is_gif, is_jpg, is_jpeg));

      spdlog::info("{} images found in {}", std::size(found_images), input_path);

      if (std::empty(found_images)) {
         std::runtime_error{"No images found"};
      }

      if (args.at("--shuffle").asBool()) {
         std::random_device rd;
         std::mt19937 g(rd());
         std::shuffle(std::begin(found_images), std::end(found_images), g);
      }

      QApplication app(argc, argv);
      imgv::main_widget wnd{std::move(found_images)};
      args.at("--maximized").asBool() ? wnd.showMaximized() : wnd.show();
      return app.exec();
   } catch (std::exception const& e) {
      spdlog::error(e.what());
      return 1;
   }
   return 0;
}