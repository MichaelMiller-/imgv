#include <QApplication>

#include "config.h"
#include "main_widget.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <spdlog/spdlog.h>

#include <string>
#include <filesystem>
#include <sstream>
#include <tuple>
#include <utility>
#include <algorithm>
#include <random>

namespace po = boost::program_options;
namespace fs = std::filesystem;

template <typename... Fs>
class any_of
{
   std::tuple<Fs...> fs;

   template <std::size_t ...Is, typename... Args>
   constexpr bool invoke(std::index_sequence<Is...>, Args &&... args) const
   {
      return (... || std::get<Is>(fs)(args...));
   }
   
public:
   explicit any_of(Fs... fs) : fs{ fs... } {};

   template <typename... Args>
   constexpr bool operator()(Args &&... args) const
   {
      return invoke(std::index_sequence_for<Fs...>(), std::forward<Args>(args)...);
   }
};

class stop_application
{
   const std::string message;
public:
   template <typename... Args>
   stop_application(Args &&... args) : message{ std::forward<Args>(args)... }
   {}

   [[nodiscard]] auto what() const noexcept { return message; }
};

int main(int argc, char **argv)
{
   int exit_code{0};
   try
   {
      spdlog::set_level(spdlog::level::debug);
      spdlog::info("imgv started ...");

      bool maximized{false};
      bool random_shuffle{true};
      std::string input_path{};

      po::options_description desc("Allowed options");
      // clang-format off
      desc.add_options()
         ("help", "produce help message (this page)")
         ("version,V", "print version string")
         ("maximized", po::bool_switch(&maximized)->default_value(maximized), "Shows the window maximized")
         ("shuffle", po::bool_switch(&random_shuffle)->default_value(random_shuffle), "show a random image")
         ("path,P", po::value<decltype(input_path)>(&input_path), "images in path will be shown")
         ;
      // clang-format on
      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
      {
         std::stringstream ss;
         ss << desc;
         throw stop_application(ss.str());
      }
      if (vm.count("version"))
         throw stop_application("imgv version: " + std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR));

      if (std::empty(input_path))
         throw std::invalid_argument("argument is missing");

      const auto rng = boost::make_iterator_range(fs::directory_iterator{input_path}, {});

      using namespace boost::adaptors;

      const auto is_png = [](auto&& e){ return boost::ends_with(e, ".png"); };
      const auto is_gif = [](auto&& e){ return boost::ends_with(e, ".gif"); };
      const auto is_jpg = [](auto&& e){ return boost::ends_with(e, ".jpg"); };
      const auto is_jpeg = [](auto&& e){ return boost::ends_with(e, ".jpeg"); };

      // clang-format off
      std::vector<std::string> found_images;
      boost::copy(rng 
         | transformed([](auto&& e) { return e.path().u8string(); }) 
         | filtered(any_of(is_png, is_gif, is_jpg, is_jpeg)),
         std::back_inserter(found_images));
      // clang-format on
      
      spdlog::info("{} images found in {}", std::size(found_images), input_path);

      if (std::empty(found_images))
         throw stop_application("no image found in " + input_path);

      if (random_shuffle)
      {
         std::random_device rd;
         std::mt19937 g(rd());
         std::shuffle(std::begin(found_images), std::end(found_images), g);
      }
 
      QApplication app(argc, argv);
      main_widget wnd{std::move(found_images)};
      maximized ? wnd.showMaximized() : wnd.show();
      exit_code = app.exec();
   }
   catch (stop_application const& e)
   {
      spdlog::info(e.what());
   }
   catch (std::exception const& e)
   {
      spdlog::error(e.what());
      exit_code = -1;
   }
   return exit_code;
}