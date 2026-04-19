# ── copy libEGL.dll next to every SDL2 exe ────────────────────────────
# libEGL.dll is a runtime dependency of SDL2 on Windows — SDL2 loads it
# dynamically so vcpkg does not copy it automatically for SDL2 targets.

# only copy into folders that actually contain a .exe
$egl = "C:\vcpkg\installed\x64-windows\bin\libEGL.dll"

Get-ChildItem -Path "build_windows" -Recurse -Filter "*.exe" | ForEach-Object {
    Copy-Item $egl $_.DirectoryName -Force
    Write-Host "Copied libEGL.dll to $($_.DirectoryName)"
}


Write-Host ""
Write-Host "Open in Visual Studio:   start build_windows\gles2_tutorial.sln"
Write-Host "Or build from terminal:  cmake --build build_windows --config Release"
Write-Host "Run:                     .\build_windows\04_imgui\Release\04_imgui.exe"
Write-Host "Run:                     .\build_windows\01_hello_triangle_SDL2\Release\01_hello_triangle_SDL2.exe"
Write-Host "Run:                     .\build_windows\02_triangle_color_attribute\Release\02_triangle_color_attribute.exe"
Write-Host "Run:                     .\build_windows\03_transform\Release\03_transform.exe"
