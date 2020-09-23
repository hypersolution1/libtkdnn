#ifndef TKDNN_H
#define TKDNN_H

#include <napi.h>

#include "promiseWorker.hpp"

#include "Yolo3Detection.h"

class Tkdnn : public Napi::ObjectWrap<Tkdnn> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, Napi::Value arg);
    Tkdnn(const Napi::CallbackInfo& info);

  private:
    static Napi::FunctionReference constructor;
    Napi::Value load(const Napi::CallbackInfo& info);
    Napi::Value execute(const Napi::CallbackInfo& info);

    int n_classes;
    int n_batch;
    float conf_thresh;

    tk::dnn::Yolo3Detection yolo;

};

#endif
