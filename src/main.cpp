#include <QApplication>

#include "main_widget.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>

int main(int argc, char** argv)
{
   bool maximized{ false };
   std::string image_filename{};

   namespace po = boost::program_options;

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
      //! \todo show help message
//      std::cout << desc << "\n";
      return 1;
   }

   QApplication app(argc, argv);
   //! \todo 2019-04-04 cmdline arguments for file 
   main_widget wnd{};
   wnd.open_file("data/sam.png");
   maximized ? wnd.showMaximized() : wnd.show();
   return app.exec();
}