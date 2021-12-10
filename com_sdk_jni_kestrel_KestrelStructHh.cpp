#include <kestrel.hh>
#include <kestrel_core.hh>
#include <iostream>
#include "com_sdk_jni_kestrel_KestrelStructHh.h"
using namespace Kestrel;

#define PLUGIN_NAME "sensetivewords"

extern "C" {

std::string JavaStringToString(JNIEnv* env, jstring str) 
{
    if (env == nullptr || str == nullptr)
    {
        return "";
    }
    const jchar* chars = env->GetStringChars(str, NULL);
    if (chars == nullptr)
    {
        return "";
    }
    std::string u8_string = UTF16StringToUTF8String(reinterpret_cast<const char16_t*>(chars), env->GetStringLength(str));
    env->ReleaseStringChars(str, chars);
    return u8_string;
}

//1. env init
JNIEXPORT void JNICALL Java_com_sdk_jni_kestrel_KestrelStructHh_kestrel_1env_1init(JNIEnv *env, jobject, jstring sdkNameString)
{
    Env::Init(PLUGIN_NAME);
}

// 2. load license
JNIEXPORT void JNICALL Java_com_sdk_jni_kestrel_KestrelStructHh_kestrel_1license_1add_1from_1file(JNIEnv *env, jobject, jstring licenseNameString, jstring suffix)
{
    License::AddFromFile("KESTREL.lic", "");
}

// 3. load kestrel plugin
JNIEXPORT void JNICALL Java_com_sdk_jni_kestrel_KestrelStructHh_kestrel_1plugin_1load(JNIEnv *env, jobject, jstring pluginNameString, jstring suffix)
{
    Kestrel::Log::SetLevel(KESTREL_LL_TRACE);
    Kestrel::Log(KESTREL_INFO, "正在加载...\n");

    Kestrel::Plugin::Load(PLUGIN_NAME ".kep", "");
}

// 4. load kestrel Model
JNIEXPORT void JNICALL Java_com_sdk_jni_kestrel_KestrelStructHh_kestrel_1annotator_1open(JNIEnv *env, jobject, jstring pluginNameString, jstring modelJsonString)
{
    Kestrel::Annotator annotator(
            PLUGIN_NAME,
            R"({"model":"./model/sensetivewords/KM_sensetivewords_ppl_1.0.5.tar","max_batch_size":1})");
    Kestrel::Log(KESTREL_INFO, "revision : %s\n", annotator.Revision().c_str());
    Kestrel::Log(KESTREL_INFO, "version : %s\n", annotator.Version().c_str());
}

// 5. process kestrel Process
JNIEXPORT jstring JNICALL Java_com_sdk_jni_kestrel_KestrelStructHh_kestrel_1annotator_1process(JNIEnv *env, jobject, jint openPointer, jstring inputJsonString)
{
    Kestrel::Keson::Value input_data{ Kestrel::Keson::ARRAY };
    Kestrel::Log(KESTREL_INFO, "加载成功！\n");
    std::string str = JavaStringToString(env, inputJsonString);
    if(str.empty())
    {
        Kestrel::Keson::Value data = { { "content", str }, { "content_id", 0 } };
        input_data.AppendItem(data);
    }
    Kestrel::Keson::Value result;
    {
        Timehepler process("process");
        result = annotator.Process(NULL, { { "id", 0 }, { "targets", input_data } });
    }

    return result.ToString(true).c_str());
}