#include <jni.h>
#include <android/bitmap.h>
#include <algorithm>
#include <cstring>
#include <limits>
#include "ImageEngine.h"
#include "Effects.h"

namespace {
struct LockedBitmap {
 JNIEnv* env; jobject bitmap; AndroidBitmapInfo info{}; void* pixels=nullptr;
 bool open(JNIEnv* e,jobject b){env=e;bitmap=b;if(!b||AndroidBitmap_getInfo(e,b,&info)!=ANDROID_BITMAP_RESULT_SUCCESS)return false;if(info.format!=ANDROID_BITMAP_FORMAT_RGBA_8888||info.width==0||info.height==0)return false;if(info.width>uint32_t(std::numeric_limits<int>::max())||info.height>uint32_t(std::numeric_limits<int>::max()))return false;if(uint64_t(info.width)*4>info.stride)return false;if(uint64_t(info.width)*info.height>std::numeric_limits<size_t>::max()/4)return false;if(AndroidBitmap_lockPixels(e,b,&pixels)!=ANDROID_BITMAP_RESULT_SUCCESS)return false;return true;}
 ~LockedBitmap(){if(pixels)AndroidBitmap_unlockPixels(env,bitmap);}
 bool read(picsay::Image& out){if(!pixels||info.width==0||info.height==0)return false;out.width=int(info.width);out.height=int(info.height);out.rgba.resize(size_t(out.width)*out.height*4);for(int y=0;y<out.height;y++)std::memcpy(&out.rgba[size_t(y)*out.width*4],static_cast<uint8_t*>(pixels)+size_t(y)*info.stride,size_t(out.width)*4);return true;}
 bool write(const picsay::Image& in){if(!pixels||in.width!=int(info.width)||in.height!=int(info.height)||!in.valid())return false;for(int y=0;y<in.height;y++)std::memcpy(static_cast<uint8_t*>(pixels)+size_t(y)*info.stride,&in.rgba[size_t(y)*in.width*4],size_t(in.width)*4);return true;}
};
template<bool(*Op)(picsay::Image&)> jboolean unary(JNIEnv* env,jclass,jobject bitmap){LockedBitmap b;if(!b.open(env,bitmap))return JNI_FALSE;picsay::Image im;if(!b.read(im)||!Op(im)||!b.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean invert(JNIEnv*e,jclass c,jobject b){return unary<picsay::invert>(e,c,b);}
jboolean gray(JNIEnv*e,jclass c,jobject b){return unary<picsay::grayscale>(e,c,b);}
jboolean sharpen(JNIEnv*e,jclass,jobject b,jfloat a){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::sharpen(im,a)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean blur(JNIEnv*e,jclass,jobject b,jint r){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::boxBlur(im,r)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean sepia(JNIEnv*e,jclass,jobject b,jfloat a){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::sepia(im,a)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean temperature(JNIEnv*e,jclass,jobject b,jfloat a){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::temperature(im,a)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean pixelize(JNIEnv*e,jclass,jobject b,jint n){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::pixelize(im,n)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean bulge(JNIEnv*e,jclass,jobject b,jfloat x,jfloat y,jfloat r,jfloat s){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::warpBulge(im,x,y,r,s)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean twirl(JNIEnv*e,jclass,jobject b,jfloat x,jfloat y,jfloat r,jfloat a){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::twirl(im,x,y,r,a)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}

jboolean adjust(JNIEnv*e,jclass,jobject b,jfloat brightness,jfloat contrast,jfloat saturation){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::adjust(im,brightness,contrast,saturation)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean vignette(JNIEnv*e,jclass,jobject b,jfloat strength,jfloat radius){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::vignette(im,strength,radius)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean blend(JNIEnv*e,jclass,jobject dst,jobject src,jint x,jint y,jfloat opacity){if(!dst||!src||e->IsSameObject(dst,src))return JNI_FALSE;LockedBitmap d,s;if(!d.open(e,dst)||!s.open(e,src))return JNI_FALSE;picsay::Image di,si;if(!d.read(di)||!s.read(si)||!picsay::blend(di,si,x,y,opacity)||!d.write(di))return JNI_FALSE;return JNI_TRUE;}

jboolean heatMap(JNIEnv*e,jclass,jobject b,jfloat amount){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::heatMap(im,amount)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean mirror(JNIEnv*e,jclass,jobject b,jboolean h,jboolean v){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::mirror(im,h==JNI_TRUE,v==JNI_TRUE)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean quantize(JNIEnv*e,jclass,jobject b,jint levels){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::quantizeLuminance(im,levels)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean boost(JNIEnv*e,jclass,jobject b,jfloat amount){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::boost(im,amount)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
jboolean multiplyColor(JNIEnv*e,jclass,jobject b,jint color,jfloat amount){LockedBitmap l;if(!l.open(e,b))return JNI_FALSE;picsay::Image im;if(!l.read(im)||!picsay::multiplyColor(im,uint32_t(color),amount)||!l.write(im))return JNI_FALSE;return JNI_TRUE;}
}
static JNINativeMethod methods[]={
 {const_cast<char*>("invert"),const_cast<char*>("(Landroid/graphics/Bitmap;)Z"),reinterpret_cast<void*>(invert)},
 {const_cast<char*>("grayscale"),const_cast<char*>("(Landroid/graphics/Bitmap;)Z"),reinterpret_cast<void*>(gray)},
 {const_cast<char*>("sharpen"),const_cast<char*>("(Landroid/graphics/Bitmap;F)Z"),reinterpret_cast<void*>(sharpen)},
 {const_cast<char*>("blur"),const_cast<char*>("(Landroid/graphics/Bitmap;I)Z"),reinterpret_cast<void*>(blur)},
 {const_cast<char*>("sepia"),const_cast<char*>("(Landroid/graphics/Bitmap;F)Z"),reinterpret_cast<void*>(sepia)},
 {const_cast<char*>("temperature"),const_cast<char*>("(Landroid/graphics/Bitmap;F)Z"),reinterpret_cast<void*>(temperature)},
 {const_cast<char*>("pixelize"),const_cast<char*>("(Landroid/graphics/Bitmap;I)Z"),reinterpret_cast<void*>(pixelize)},
 {const_cast<char*>("bulge"),const_cast<char*>("(Landroid/graphics/Bitmap;FFFF)Z"),reinterpret_cast<void*>(bulge)},
 {const_cast<char*>("twirl"),const_cast<char*>("(Landroid/graphics/Bitmap;FFFF)Z"),reinterpret_cast<void*>(twirl)},
 {const_cast<char*>("adjust"),const_cast<char*>("(Landroid/graphics/Bitmap;FFF)Z"),reinterpret_cast<void*>(adjust)},
 {const_cast<char*>("vignette"),const_cast<char*>("(Landroid/graphics/Bitmap;FF)Z"),reinterpret_cast<void*>(vignette)},
 {const_cast<char*>("blend"),const_cast<char*>("(Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;IIF)Z"),reinterpret_cast<void*>(blend)},
 {const_cast<char*>("heatMap"),const_cast<char*>("(Landroid/graphics/Bitmap;F)Z"),reinterpret_cast<void*>(heatMap)},
 {const_cast<char*>("mirror"),const_cast<char*>("(Landroid/graphics/Bitmap;ZZ)Z"),reinterpret_cast<void*>(mirror)},
 {const_cast<char*>("quantizeLuminance"),const_cast<char*>("(Landroid/graphics/Bitmap;I)Z"),reinterpret_cast<void*>(quantize)},
 {const_cast<char*>("boost"),const_cast<char*>("(Landroid/graphics/Bitmap;F)Z"),reinterpret_cast<void*>(boost)},
 {const_cast<char*>("multiplyColor"),const_cast<char*>("(Landroid/graphics/Bitmap;IF)Z"),reinterpret_cast<void*>(multiplyColor)}
};
JNIEXPORT jint JNI_OnLoad(JavaVM* vm,void*){JNIEnv* env=nullptr;if(vm->GetEnv(reinterpret_cast<void**>(&env),JNI_VERSION_1_6)!=JNI_OK)return JNI_ERR;jclass cls=env->FindClass("com/devilhandle/picsay/NativeImageEngine");if(!cls)return JNI_ERR;if(env->RegisterNatives(cls,methods,sizeof(methods)/sizeof(methods[0]))!=JNI_OK)return JNI_ERR;return JNI_VERSION_1_6;}
