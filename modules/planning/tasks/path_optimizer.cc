/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#include "modules/planning/tasks/path_optimizer.h"

#include "modules/planning/common/planning_gflags.h"

namespace apollo {
namespace planning {

using apollo::common::Status;

PathOptimizer::PathOptimizer(const std::string& name) : Task(name) {}

apollo::common::Status PathOptimizer::Execute(
    Frame* frame, ReferenceLineInfo* const reference_line_info) {
    // 调用父类同名函数,目的是传参
  Task::Execute(frame, reference_line_info);
	// 路径规划过程,会在具体的路径优化器中重写
  auto ret = Process(
      reference_line_info->speed_data(), reference_line_info->reference_line(),
      frame->PlanningStartPoint(), reference_line_info->mutable_path_data());
  RecordDebugInfo(reference_line_info->path_data());
  if (ret != Status::OK()) {
    reference_line_info->SetDrivable(false);
    AERROR << "Reference Line " << reference_line_info->Lanes().Id()
           << " is not drivable after " << Name();
  }
  return ret;
}

void PathOptimizer::RecordDebugInfo(const PathData& path_data) {
  const auto& path_points = path_data.discretized_path().path_points();
  auto* ptr_optimized_path = reference_line_info_->mutable_debug()
                                 ->mutable_planning_data()
                                 ->add_path();
  ptr_optimized_path->set_name(Name());
  ptr_optimized_path->mutable_path_point()->CopyFrom(
      {path_points.begin(), path_points.end()});
}

}  // namespace planning
}  // namespace apollo
