cmake_minimum_required(VERSION 3.16)
project(pr2_mechanism_model)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pr2_hardware_interface REQUIRED)
find_package(urdf REQUIRED)
find_package(kdl_parser REQUIRED)
find_package(orocos_kdl_vendor REQUIRED)
find_package(orocos_kdl REQUIRED)
find_package(pluginlib REQUIRED)
find_package(angles REQUIRED)
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TINYXML REQUIRED IMPORTED_TARGET tinyxml)

add_library(${PROJECT_NAME} SHARED
  src/joint.cpp
  src/robot.cpp
  src/chain.cpp
  src/tree.cpp
  src/joint_calibration_simulator.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
# urdfdom >= 1.0 (std::shared_ptr based API); the sources still carry the old #if switch
target_compile_definitions(${PROJECT_NAME} PUBLIC URDFDOM_1_0_0_API=1)
target_link_libraries(${PROJECT_NAME} PUBLIC
  rclcpp::rclcpp
  pr2_hardware_interface::pr2_hardware_interface
  urdf::urdf
  kdl_parser::kdl_parser
  orocos-kdl
  pluginlib::pluginlib
  angles::angles
  Boost::boost
  Eigen3::Eigen
  PkgConfig::TINYXML)
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--no-undefined)

# The transmissions are pluginlib plugins and live in their own library that nothing links (ROS 2
# port; ROS 1 had them in libpr2_mechanism_model). class_loader attributes a plugin library's
# factories to whichever loader is active when the library gets loaded. If the transmissions sat in
# the core library, loading a plugin that links it (e.g. pr2_ros2_control's hardware component)
# would load them under that plugin's loader, and Robot's transmission loader would then find
# "no factory" for SimpleTransmission and friends.
add_library(${PROJECT_NAME}_transmissions SHARED
  src/simple_transmission.cpp
  src/pr2_gripper_transmission.cpp
  src/wrist_transmission.cpp
  src/pr2_belt_transmission.cpp
)
target_link_libraries(${PROJECT_NAME}_transmissions PUBLIC ${PROJECT_NAME})
target_link_options(${PROJECT_NAME}_transmissions PRIVATE -Wl,--no-undefined)

pluginlib_export_plugin_description_file(${PROJECT_NAME} transmission_plugins.xml)

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_transmissions EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_chain test/test_chain.cpp)
  target_compile_definitions(test_chain PRIVATE PR2_URDF_PATH="${CMAKE_CURRENT_SOURCE_DIR}/pr2.urdf")
  target_link_libraries(test_chain ${PROJECT_NAME}_transmissions)
  ament_add_gtest(test_wrist_transmission test/test_wrist_transmission.cpp)
  target_link_libraries(test_wrist_transmission ${PROJECT_NAME}_transmissions)
  # test_joint_calibration_simulator is a rostest node; not ported.
endif()

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp pr2_hardware_interface urdf kdl_parser orocos_kdl_vendor orocos_kdl pluginlib angles Boost Eigen3 PkgConfig)
ament_package(CONFIG_EXTRAS cmake/${PROJECT_NAME}-extras.cmake)
