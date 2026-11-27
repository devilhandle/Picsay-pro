# Native source required

The APK contains stripped 32-bit machine-code libraries, not C/C++ source.

Required source trees:

- `ImageOp/`: image decoding, pixel buffers, filters, transforms, JPEG/PNG output, JNI registration.
- `GLImageOp/`: OpenGL image effects and `PEWarpSession` JNI registration.

Do not add empty implementations of JNI methods. They may let the app load a library but will not make photo editing work. Preserve the class names, method signatures, registration tables, ownership rules, pixel formats, and error behavior observed in the reference binaries.
