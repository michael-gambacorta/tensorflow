/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "tensorflow/core/profiler/utils/xplane_utils.h"

#include "absl/strings/match.h"
#include "tensorflow/core/profiler/utils/timespan.h"

namespace tensorflow {
namespace profiler {
namespace {

// Creates a Timespan from an XEvent.
// WARNING: This should only be used when comparing events from the same XLine.
Timespan XEventTimespan(const XEvent& event) {
  return Timespan(event.offset_ps(), event.duration_ps());
}

}  // namespace

const XPlane* FindPlaneWithName(const XSpace& space, absl::string_view name) {
  for (const XPlane& plane : space.planes()) {
    if (plane.name() == name) return &plane;
  }
  return nullptr;
}

std::vector<const XPlane*> FindPlanesWithPrefix(const XSpace& space,
                                                absl::string_view prefix) {
  std::vector<const XPlane*> result;
  for (const XPlane& plane : space.planes()) {
    if (absl::StartsWith(plane.name(), prefix)) result.push_back(&plane);
  }
  return result;
}

XPlane* GetOrCreatePlane(XSpace* space, absl::string_view name) {
  for (XPlane& plane : *space->mutable_planes()) {
    if (plane.name() == name) return &plane;
  }
  XPlane* plane = space->add_planes();
  plane->set_name(std::string(name));
  return plane;
}

bool IsNested(const XEvent& event, const XEvent& parent) {
  return XEventTimespan(parent).Includes(XEventTimespan(event));
}

void AddOrUpdateIntStat(int64 metadata_id, int64 value, XEvent* event) {
  for (auto& stat : *event->mutable_stats()) {
    if (stat.metadata_id() == metadata_id) {
      stat.set_int64_value(value);
      return;
    }
  }
  XStat* stat = event->add_stats();
  stat->set_metadata_id(metadata_id);
  stat->set_int64_value(value);
}

void AddOrUpdateStrStat(int64 metadata_id, absl::string_view value,
                        XEvent* event) {
  for (auto& stat : *event->mutable_stats()) {
    if (stat.metadata_id() == metadata_id) {
      stat.set_str_value(std::string(value));
      return;
    }
  }
  XStat* stat = event->add_stats();
  stat->set_metadata_id(metadata_id);
  stat->set_str_value(std::string(value));
}

}  // namespace profiler
}  // namespace tensorflow
