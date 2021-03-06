// Licensed to the Software Freedom Conservancy (SFC) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The SFC licenses this file
// to you under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "SwitchToFrameCommandHandler.h"
#include "errorcodes.h"
#include "../Browser.h"
#include "../Element.h"
#include "../IECommandExecutor.h"

namespace webdriver {

SwitchToFrameCommandHandler::SwitchToFrameCommandHandler(void) {
}

SwitchToFrameCommandHandler::~SwitchToFrameCommandHandler(void) {
}

void SwitchToFrameCommandHandler::ExecuteInternal(
    const IECommandExecutor& executor,
    const ParametersMap& command_parameters,
    Response* response) {
  Json::Value frame_id = Json::Value::null;
  ParametersMap::const_iterator it = command_parameters.find("id");
  if (it != command_parameters.end()) {
    frame_id = it->second;
  } else {
    response->SetErrorResponse(400, "Missing parameter: id");
    return;
  }
  BrowserHandle browser_wrapper;
  int status_code = executor.GetCurrentBrowser(&browser_wrapper);
  if (status_code != WD_SUCCESS) {
    response->SetErrorResponse(status_code, "Unable to get browser");
    return;
  }

  if (frame_id.isNull()) {
    status_code = browser_wrapper->SetFocusedFrameByElement(NULL);
  } else if (frame_id.isObject()) {
    // TODO: Remove the check for "ELEMENT" once all target bindings 
    // have been updated to use spec-compliant protocol.
    Json::Value element_id = frame_id.get("element-6066-11e4-a52e-4f735466cecf", Json::Value::null);
    if (element_id.isNull()) {
      element_id = frame_id.get("ELEMENT", Json::Value::null);
    }

    if (element_id.isNull()) {
      status_code = ENOSUCHFRAME;
    } else {
      std::string frame_element_id = element_id.asString();

      ElementHandle frame_element_wrapper;
      status_code = this->GetElement(executor,
                                      frame_element_id,
                                      &frame_element_wrapper);
      if (status_code == WD_SUCCESS) {
        status_code = browser_wrapper->SetFocusedFrameByElement(frame_element_wrapper->element());
      }
    }
  } else if (frame_id.isString()) {
    std::string frame_name = frame_id.asString();
    status_code = browser_wrapper->SetFocusedFrameByName(frame_name);
  } else if(frame_id.isIntegral()) {
    int frame_index = frame_id.asInt();
    status_code = browser_wrapper->SetFocusedFrameByIndex(frame_index);
  }

  if (status_code != WD_SUCCESS) {
    response->SetErrorResponse(status_code, "No frame found");
  } else {
    response->SetSuccessResponse(Json::Value::null);
  }
}

} // namespace webdriver
