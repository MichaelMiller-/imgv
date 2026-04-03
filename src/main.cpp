#include "any_of.h"
#include "application.h"
#include "config.h"

#include <spdlog/spdlog.h>

#include <docopt.h>

#include <format>
#include <random>

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
      spdlog::info("{} starting ...", IMGV_BINARY_NAME);

      auto const args =
         docopt::docopt(USAGE, {argv + 1, argv + argc}, true,
                        std::format("{} {}.{}", IMGV_BINARY_NAME, IMGV_VERSION_MAJOR, IMGV_VERSION_MINOR));

      auto is_png = [](auto const& e) { return e.path().extension() == ".png"; };
      auto is_gif = [](auto const& e) { return e.path().extension() == ".gif"; };
      auto is_jpg = [](auto const& e) { return e.path().extension() == ".jpg"; };
      auto is_jpeg = [](auto const& e) { return e.path().extension() == ".jpeg"; };
      auto is_webp = [](auto const& e) { return e.path().extension() == ".webp"; };

      auto input_path = args.at("--path").asString();

      std::vector<std::filesystem::path> found_images{};
      std::copy_if(std::filesystem::directory_iterator{input_path}, {}, std::back_inserter(found_images),
                   imgv::any_of(is_png, is_gif, is_jpg, is_jpeg, is_webp));

      spdlog::info("{} images found in {}", std::size(found_images), input_path);

      if (std::empty(found_images)) {
         throw std::runtime_error{"No images found"};
      }

      if (args.at("--shuffle").asBool()) {
         std::random_device rd;
         std::mt19937 g(rd());
         std::shuffle(begin(found_images), end(found_images), g);
      }

      imgv::application app{found_images};
      if (not app.init()) {
         throw std::runtime_error{"Failed to initialize application"};
      }
      app.run();

   } catch (std::exception const& e) {
      spdlog::error(e.what());
      return 1;
   }
   return 0;
}