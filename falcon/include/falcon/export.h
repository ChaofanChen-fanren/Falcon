#ifndef FALCON_INCLUDE_FALCON_EXPORT_H
#define FALCON_INCLUDE_FALCON_EXPORT_H

#if defined(_WIN32) && defined(FALCON_SHARED_LIBRARY)
#if defined(FALCON_BUILDING_LIBRARY)
#define FALCON_API __declspec(dllexport)
#else
#define FALCON_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && defined(FALCON_SHARED_LIBRARY)
#define FALCON_API __attribute__((visibility("default")))
#else
#define FALCON_API
#endif

#endif // FALCON_INCLUDE_FALCON_EXPORT_H