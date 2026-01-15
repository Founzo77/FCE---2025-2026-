

```bash
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

"C:\Program Files (x86)\Intel\oneAPI\setvars.bat"

```

### Mode Debug
```bash
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake" ..
cmake --build . --parallel

dxc closesthit.hlsl -T lib_6_5 -E ClosestHitShader -Fo closesthit.dxil -Fc output.asm
```

### Mode Release

```bash
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="/O2 /Ox /Ot /Ob3 /Oi /GL /fp:fast /Gy /Gw /DNDEBUG" ..
cmake --build . --parallel

main scene/shinjuku_gojo.xml

cmake -G "Ninja" -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icx -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE="/O3 /QxHost /fp:fast" -DCMAKE_CXX_FLAGS_RELEASE="/O3 /QxHost /fp:fast /DNDEBUG" -DCMAKE_BUILD_TYPE=Release ..

```
