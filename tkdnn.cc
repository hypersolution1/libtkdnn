
#include "tkdnn.h"

Napi::FunctionReference Tkdnn::constructor;

Napi::Object Tkdnn::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Tkdnn", {
      InstanceMethod("load", &Tkdnn::load),
      InstanceMethod("execute", &Tkdnn::execute),
  });

  constructor = Napi::Persistent(func);
  //constructor.SuppressDestruct();

  exports.Set("Tkdnn", func);
  return exports;
}

Tkdnn::Tkdnn(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Tkdnn>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
};

Napi::Object Tkdnn::NewInstance(Napi::Env env, Napi::Value arg) {
  Napi::EscapableHandleScope scope(env);
  Napi::Object obj = constructor.New({ arg });
  return scope.Escape(napi_value(obj)).ToObject();
}

Napi::Value Tkdnn::load(const Napi::CallbackInfo &info)
{
  int n_classes = 80;
  int n_batch = 1;
  float conf_thresh = 0.3;

  Napi::Env env = info.Env();
  std::string fname = std::string(info[0].As<Napi::String>());

  yolo.init(fname, n_classes, n_batch, conf_thresh);

  return env.Undefined();
}

Napi::Value Tkdnn::execute(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::Array arrinput = info[0].As<Napi::Array>();
  int arrinput_len = ((Napi::Value)arrinput["length"]).As<Napi::Number>().Int32Value();

  std::vector<cv::Mat> *batch_dnn_input = new std::vector<cv::Mat>; // <-- to cleanup

  for(int i = 0; i < arrinput_len; i++) {
    Napi::Object img = arrinput.Get(i).As<Napi::Object>();
    int height = img.Get("height").As<Napi::Number>().Int32Value();
    int width = img.Get("width").As<Napi::Number>().Int32Value();
    Napi::Buffer<unsigned char> jsdata = img.Get("data").As<Napi::Buffer<unsigned char>>();

    cv::Mat mat(height, width, CV_8UC3, jsdata.Data());
    batch_dnn_input->push_back(mat.clone());

  }

  PromiseWorker *wk = new PromiseWorker(env,
  [=]() {

    yolo.update(*batch_dnn_input, arrinput_len);

  },
  [=](Napi::Env env) -> Napi::Value {

    Napi::Array outputs = Napi::Array::New(env);

    tk::dnn::box b;

    int baseline = 0;
    float font_scale = 0.5;
    int thickness = 2;   

    for(int bi = 0; bi < arrinput_len; ++bi){
      
      Napi::Array arrDetected = Napi::Array::New(env);
      outputs.Set(bi, arrDetected);

      for(int i = 0; i < yolo.batchDetected[bi].size(); i++) {

        Napi::Object detected = Napi::Object::New(env);
        arrDetected.Set(i, detected);

        b = yolo.batchDetected[bi][i];
        detected.Set("x1", Napi::Number::New(env, b.x));
        detected.Set("y1", Napi::Number::New(env, b.y));
        detected.Set("x2", Napi::Number::New(env, b.x + b.w));
        detected.Set("y2", Napi::Number::New(env, b.y + b.h));
        detected.Set("prob", Napi::Number::New(env, b.prob));
        detected.Set("idx", Napi::Number::New(env, b.cl));
        detected.Set("name", yolo.classesNames[b.cl]);

      }
    }

    delete batch_dnn_input;

    return outputs;

  });
  wk->Queue();
  return wk->Deferred().Promise();

}

// Napi::Value Tkdnn::fnAsync(const Napi::CallbackInfo &info)
// {
//   Napi::Env env = info.Env();
//   Napi::Object inputobj = info[0].As<Napi::Object>();
  
//   //Memory allocation of inputs and outputs to persist through the async call
//   Napi::Buffer<unsigned char> jsdata = inputobj.Get("data").As<Napi::Buffer<unsigned char>>();
//   std::vector<unsigned char> *input1 = new std::vector<unsigned char>(jsdata.Data(), jsdata.Data()+jsdata.ByteLength());

//   std::string *output1 = new std::string();

//   PromiseWorker *wk = new PromiseWorker(env,
//   [=]() {

//     //This part is in a separate thread
//     //You can't interact with V8/Napi here
    
//     //(*output1) = blocking_fn(input1->data());

//   },
//   [=](Napi::Env env) -> Napi::Value {

//     Napi::Object retval = Napi::Object::New(env);
//     retval.Set("output1", (*output1));

//     //cleanup what you allocated
//     delete input1;
//     delete output1;

//     return retval;
//   });
//   wk->Queue();
//   return wk->Deferred().Promise();
// }

// Napi::Value Tkdnn::fnSync(const Napi::CallbackInfo &info)
// {
//   Napi::Env env = info.Env();
//   Napi::Object inputobj = info[0].As<Napi::Object>();
  
//   Napi::Buffer<unsigned char> jsdata = inputobj.Get("data").As<Napi::Buffer<unsigned char>>();

//   std::string output1;

//   //output1 = blocking_fn(jsdata.Data());

//   Napi::Object retval = Napi::Object::New(env);

//   retval.Set("output1", output1);

//   return retval;
// }