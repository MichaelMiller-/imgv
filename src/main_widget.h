#pragma once

class QLabel;
class QMovie;
class QSlider;

#include <QThread>
#include <QWidget>

#include <vector>
#include <string>
#include <filesystem>


class main_widget final : public QWidget 
{
  Q_OBJECT

private:
  QLabel *canvas{nullptr};
  QMovie *movie{nullptr};
  QSlider* frame_slider{nullptr};

  QAction* play_pause{nullptr};
  QAction* previous_image{nullptr};
  QAction* next_image{nullptr};

  //! \todo 
  QThread *directory_loader{nullptr};

  std::vector<std::string> current_directory_image_list{};
  decltype(current_directory_image_list)::iterator current_image;

public:
   main_widget(QWidget *parent = nullptr);

   void open_file(std::string const& filename);
};
