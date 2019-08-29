#include "main_widget.h"

#include <QMovie>
#include <QLabel>
#include <QVBoxLayout>
#include <QAction>
#include <QSlider>
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QScrollArea>

#include <spdlog/spdlog.h>

#include <filesystem>
#include <algorithm>

main_widget::main_widget(container_t&& img_list)
   : image_list{img_list}
{
//! \todo 2019-04-04 action: play/pause / state / space
//! \todo 2019-08-29 reload image list in a second thread if key_o is pressed -> enable action if thread is done

   movie = new QMovie(this);
   movie->setCacheMode(QMovie::CacheAll);

   canvas = new QLabel{ tr("no picture loaded"), this};
   canvas->setAlignment(Qt::AlignCenter);
   canvas->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   canvas->setBackgroundRole(QPalette::Dark);
   canvas->setAutoFillBackground(true);

   scroll_area = new QScrollArea{this};
   scroll_area->setBackgroundRole(QPalette::Dark);
   scroll_area->setWidget(canvas);

   // frame_slider = new QSlider(Qt::Horizontal, this);
   // frame_slider->setVisible(false);
   // frame_slider->setTickPosition(QSlider::TicksBelow);
   // frame_slider->setTickInterval(10);

   auto layout = new QVBoxLayout;
   layout->setContentsMargins(0,0,0,0);
   layout->addWidget(scroll_area);
   setLayout(layout);

   // actions
   auto close = new QAction{this};
   close->setShortcut(QKeySequence{Qt::Key::Key_Escape});

   auto fit_to_window = new QAction{this};
   fit_to_window->setObjectName("fit_to_window");
   fit_to_window->setCheckable(true);
   fit_to_window->setChecked(true);
   fit_to_window->setShortcut(QKeySequence{Qt::Key::Key_R});

   auto switch_fullscreen = new QAction{this};
   switch_fullscreen->setShortcut(QKeySequence{Qt::Key::Key_F10});

   //auto switch_speed = new QAction{this};
   //switch_speed->setShortcuts({ QKeySequence{Qt::Key::Key_Minus}, QKeySequence{Qt::Key::Key_Plus} });

   play_pause = new QAction{this};
   play_pause->setShortcut(QKeySequence{Qt::Key::Key_Space});

   previous_image = new QAction{this};
   previous_image->setShortcut(QKeySequence{Qt::Key::Key_Left});

   next_image = new QAction{this};
   next_image->setShortcut(QKeySequence{Qt::Key::Key_Right});

   canvas->addAction(fit_to_window);
   canvas->addAction(switch_fullscreen);
   canvas->addAction(play_pause);
   canvas->addAction(previous_image);
   canvas->addAction(next_image);
   canvas->addAction(close);

   //auto state_animation = new QState(state_main);
   //auto state_speed = new QState(state_main);

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

   QObject::connect(fit_to_window, &QAction::triggered, this, &main_widget::fit_image_to_widget);
   QObject::connect(switch_fullscreen, &QAction::triggered, [this]{ 
      spdlog::debug("switch fullscreen");
      setWindowState(windowState() ^ Qt::WindowFullScreen);
   });
   QObject::connect(previous_image, &QAction::triggered, [this]{ 
      std::rotate(std::begin(image_list), std::begin(image_list) + 1, std::end(image_list));
      show_image(*std::begin(image_list)); 
   });
   QObject::connect(next_image, &QAction::triggered, [this]{ 
      std::rotate(std::rbegin(image_list), std::rbegin(image_list) + 1, std::rend(image_list));
      show_image(*std::begin(image_list)); 
   });
   QObject::connect(close, &QAction::triggered, this, &QWidget::close);

   setContentsMargins(0,0,0,0);
   resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

void main_widget::fit_image_to_widget(bool value)
{
   spdlog::debug("fit image to widget: {}", value);
   scroll_area->setWidgetResizable(value);
   
   if (value == false)
      canvas->adjustSize();
}

void main_widget::showEvent(QShowEvent *event)
{
   show_image(*std::begin(image_list)); 
}

void main_widget::show_image(value_t const& image_filename)
{
   movie->stop();
   canvas->setMovie(movie);
   movie->setFileName(QString::fromStdString(image_filename));
   movie->start();

   auto fit_to_window = findChild<QAction*>("fit_to_window");
   emit fit_image_to_widget(fit_to_window->isChecked());

   setWindowTitle(QString::fromStdString(image_filename));
}