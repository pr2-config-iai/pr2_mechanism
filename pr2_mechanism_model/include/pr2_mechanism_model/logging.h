/*
 * Logging and time helpers for the ROS 2 port of pr2_mechanism_model.
 *
 * - logger(): one shared logger for the package (ROS 1 used the global rosconsole).
 * - now():    the clock used by the simulated-actuator code paths. It uses RCL_SYSTEM_TIME, the
 *             same clock type as pr2_hardware_interface's current_time_ and ethercat_hardware,
 *             because rclcpp::Time objects with different clock types throw when combined.
 */
#ifndef PR2_MECHANISM_MODEL__LOGGING_H
#define PR2_MECHANISM_MODEL__LOGGING_H

#include <cassert>

#include <rclcpp/clock.hpp>
#include <rclcpp/logger.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/time.hpp>

namespace pr2_mechanism_model
{

inline rclcpp::Logger logger()
{
  static rclcpp::Logger logger = rclcpp::get_logger("pr2_mechanism_model");
  return logger;
}

inline rclcpp::Time now()
{
  static rclcpp::Clock clock(RCL_SYSTEM_TIME);
  return clock.now();
}

}  // namespace pr2_mechanism_model

#endif  // PR2_MECHANISM_MODEL__LOGGING_H
