#include "main_widget.h"

#include <QMovie>
#include <QLabel>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QStateMachine>
#include <QAction>
#include <QThread>
#include <QSlider>

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

main_widget::main_widget(QWidget *parent) 
   : QWidget{parent} 
{
//! \todo 2019-04-04 action: play/pause / state / space
//! \todo 2019-04-04 action: left -> next left
//! \todo 2019-04-04 action: right -> next right
//! \todo 2019-04-04 load image and filelist in a second thread -> enable action if thread is done

   movie = new QMovie(this);
   movie->setCacheMode(QMovie::CacheAll);

   canvas = new QLabel(tr("no picture loaded"), this);
   canvas->setAlignment(Qt::AlignCenter);
   canvas->setBackgroundRole(QPalette::Dark);
   canvas->setAutoFillBackground(true);

   frame_slider = new QSlider(Qt::Horizontal, this);
   frame_slider->setVisible(false);
   frame_slider->setTickPosition(QSlider::TicksBelow);
   frame_slider->setTickInterval(10);

   //! \todo 2019-04-04 unn�tig?
   auto mainLayout = new QVBoxLayout;
   mainLayout->addWidget(canvas);
   mainLayout->addWidget(frame_slider);
   setLayout(mainLayout);

   // clang-format off

   // actions
   auto switch_image_size = new QAction{this};
   switch_image_size->setShortcut(QKeySequence{Qt::Key::Key_R});

   //auto switch_speed = new QAction{this};
   //switch_speed->setShortcuts({ QKeySequence{Qt::Key::Key_Minus}, QKeySequence{Qt::Key::Key_Plus} });

   play_pause = new QAction{this};
   play_pause->setShortcut(QKeySequence{Qt::Key::Key_Space});

   previous_image = new QAction{this};
   previous_image->setShortcut(QKeySequence{Qt::Key::Key_Left});

   next_image = new QAction{this};
   next_image->setShortcut(QKeySequence{Qt::Key::Key_Right});

   canvas->addAction(switch_image_size);
   canvas->addAction(play_pause);
   canvas->addAction(previous_image);
   canvas->addAction(next_image);

   // root state
   auto state_main = new QState(QState::ParallelStates);
   state_main->assignProperty(frame_slider, "visible", false);

   // child states
   auto state_image_size = new QState(state_main);
   //auto state_animation = new QState(state_main);
   //auto state_speed = new QState(state_main);

   // state_image_size
   {
      auto state_image_fit_to_window = new QState(state_image_size);
      //! \todo 2019-04-05 scroll if imagesize is greater than windowsize
      state_image_fit_to_window->assignProperty(canvas, "scaledContents", true);

      auto state_image_orginal_size = new QState(state_image_size);
      state_image_orginal_size->assignProperty(canvas, "scaledContents", false);

      state_image_size->setInitialState(state_image_fit_to_window);

      state_image_fit_to_window->addTransition(switch_image_size, &QAction::triggered, state_image_orginal_size);
      state_image_orginal_size->addTransition(switch_image_size, &QAction::triggered, state_image_fit_to_window);
   }

   // state_animation
   //{
   //   auto state_animation_play = new QState(state_animation);
   //   state_animation_play->assignProperty(frame_slider, "visible", false);
   //   QObject::connect(state_animation_play, &QState::entered, [this] { 
   //      
   //            std::cout << "animation play" << std::endl;

   //      movie->setPaused(false); });

   //   auto state_animation_pause = new QState(state_animation);
   //   state_animation_pause->assignProperty(frame_slider, "visible", true);
   //   //! \todo 2019-04-05 show frameslider (overlay)
   //   QObject::connect(state_animation_play, &QState::entered, [this] { 
   //      
   //      
   //   std::cout << "animation paused" << std::endl;
   //      
   //      movie->setPaused(true); });
   //
   //   state_animation->setInitialState(state_animation_play);
   //
   //   state_animation_play->addTransition(play_pause, &QAction::triggered, state_animation_pause);
   //   state_animation_pause->addTransition(play_pause, &QAction::triggered, state_animation_play);
   //}

   //QObject::connect(movie, &QMovie::frameChanged, this, &main_widget::update_frame_slider);
   //QObject::connect(frame_slider, &QSlider::valueChanged, [this](auto frame){ movie->jumpToFrame(frame); });
   QObject::connect(previous_image, &QAction::triggered, [this]{ open_file(*std::prev(current_image)); });
   QObject::connect(next_image, &QAction::triggered, [this]{ open_file(*std::next(current_image)); });

   auto state_machine = new QStateMachine(this);
   state_machine->addState(state_main);
   state_machine->setInitialState(state_main);
   state_machine->start();

   resize(400, 400);
}

void main_widget::open_file(std::string const& filename)
{
   //!\todo 2019-04-05 unneccessary casts
   const auto current_directory = QFileInfo(QString::fromStdString(filename)).absolutePath().toStdString();
   const auto purged_filename = QFileInfo(QString::fromStdString(filename)).fileName().toStdString();
   //! \todo 2019-04-05 filter images from filelist
   const auto rng = boost::make_iterator_range(fs::directory_iterator{current_directory}, {});

   //! \todo 2019-04-05 only do stuff if it is neccessary -> always clear and reload make no sense
   current_directory_image_list.clear();
   std::transform(
      std::begin(rng), 
      std::end(rng), 
      std::back_inserter(current_directory_image_list), 
      [](auto && e){ return e.path().u8string(); });

   const auto it = std::find_if(
      std::begin(current_directory_image_list), 
      std::end(current_directory_image_list),
      [fn = purged_filename](auto && e){ return boost::ends_with(e, fn); });

   current_image = it;

   auto n = std::distance(std::begin(current_directory_image_list), it);
   
   // enable actions
   play_pause->setEnabled(boost::ends_with(purged_filename, ".gif"));
   previous_image->setEnabled(n > 0);
   next_image->setEnabled(n + 1 < std::size(current_directory_image_list));

   movie->stop();
   canvas->setMovie(movie);
   movie->setFileName(QString::fromStdString(*it));
   movie->start();
}
