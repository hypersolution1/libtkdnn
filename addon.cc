#include <napi.h>
#include "tkdnn.h"

Napi::Object CreateObject(const Napi::CallbackInfo& info) {
  return Tkdnn::NewInstance(info.Env(), info[0]);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Napi::Object new_exports = Napi::Function::New(env, CreateObject, "CreateObject");
  return Tkdnn::Init(env, new_exports);
}

NODE_API_MODULE(addon, InitAll)
