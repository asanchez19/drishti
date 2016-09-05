/*!
  @file   finder/FrameHandler.cpp
  @author David Hirvonen
  @brief Common state and stored frame handlers.

  \copyright Copyright 2014-2016 Elucideye, Inc. All rights reserved.
  \license{This project is released under the 3 Clause BSD License.}

*/

#include "FrameHandler.h"

#include "drishti/sensor/Sensor.h"

#include "drishti/core/Logger.h"


// Sample:
//
//"sensor": {
//    "intrinsic": {
//        "size": {
//            "width": 640,
//            "height": 852
//        }
//        "principal": {
//            "x": 320.0,
//            "y": 426.0
//        }
//        "focal_length_x": 768.0
//     }
// }

FrameHandlerManager * FrameHandlerManager::m_instance = nullptr;

FrameHandlerManager::FrameHandlerManager(Settings *settings) : m_settings(settings)
{
    const auto &intrinsic = (*settings)["sensor"]["intrinsic"];
    
    cv::Size size;
    size.width = intrinsic["size"]["width"].get<int>();
    size.height = intrinsic["size"]["height"].get<int>();

    cv::Point2f p;
    p.x = intrinsic["principal"]["x"].get<float>();
    p.y = intrinsic["principal"]["y"].get<float>();
    
    const auto fx = intrinsic["focal_length_x"].get<float>();
    
    drishti::sensor::SensorModel::Intrinsic params(p, fx, size);
    
    m_logger = drishti::core::Logger::create("drishti");
    m_logger->info() << "FaceFinder #################################################################";
    
    m_threads = std::unique_ptr<ThreadPool<128>>(new ThreadPool<128>);
    
    m_sensor = std::make_shared<drishti::sensor::SensorModel>(params);
}

FrameHandlerManager::~FrameHandlerManager()
{
    if(m_instance)
    {
        delete m_instance;
    }
    m_instance = 0;
}

void FrameHandlerManager::setSize(const cv::Size &size)
{
    if(m_sensor)
    {
        m_sensor->intrinsic().setSize(size);
    }
}

cv::Size FrameHandlerManager::getSize() const
{
    return m_sensor ? m_sensor->intrinsic().getSize() : cv::Size(0,0);
}

FrameHandlerManager * FrameHandlerManager::get(nlohmann::json *settings)
{
    if(!m_instance)
    {
        m_instance = new FrameHandlerManager(settings);
    }
    return m_instance;
}
