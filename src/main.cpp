#include <QApplication>

#include "main_widget.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv)
{
   bool maximized{ false };
   std::string image_filename{};

   namespace po = boost::program_options;
   namespace fs = std::experimental::filesystem;

   //! \todo 2019-04-05 messages and options
   po::options_description desc("Allowed options");
   desc.add_options()
      ("version,v", "print version string"),
      ("help", "produce help message (this page)"),
      ("maximized", po::bool_switch(&maximized)->default_value(maximized), "Shows the window maximized"),
      ("image", po::value<decltype(image_filename)>(&image_filename), "file that will be shown")
      ;

   po::variables_map vm;
   po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
   po::notify(vm);

   if (vm.count("help"))
   {
      std::cout << desc << "\n";
      return 1;
   }

   auto current_directory = fs::absolute({ image_filename });
   auto ss = fs::absolute({ image_filename }).u8string();
   // const auto current_directory = QFileInfo(QString::fromStdString(filename)).absolutePath().toStdString();
   // const auto purged_filename = QFileInfo(QString::fromStdString(filename)).fileName().toStdString();
   //! \todo 2019-04-05 filter images from filelist
   const auto rng = boost::make_iterator_range(fs::directory_iterator{current_directory}, {});

   //! \todo 2019-04-05 only do stuff if it is neccessary -> always clear and reload make no sense
   // current_directory_image_list.clear();
   // std::transform(
   //    std::begin(rng), 
   //    std::end(rng), 
   //    std::back_inserter(current_directory_image_list), 
   //    [](auto && e){ return e.path().u8string(); });

   QApplication app(argc, argv);
   //! \todo 2019-04-04 cmdline arguments for file 
   main_widget wnd{};
   wnd.open_file("data/sam.png");
   maximized ? wnd.showMaximized() : wnd.show();
   return app.exec();
}