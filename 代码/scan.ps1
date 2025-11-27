# 项目根目录
$ProjectRoot = "E:\mystudy\newmsg-sync_signal\newmsg-sync_signal\dd"

# include 目录列表（所有自定义头文件路径都要加）
$IncludeDirs = @(
    "$ProjectRoot/Debug",
    "$ProjectRoot/Core",
    "$ProjectRoot/User",
    "$ProjectRoot/Peripheral/inc",
    "$ProjectRoot/pwm",
    "$ProjectRoot/screen",
    "$ProjectRoot/encode",
    "$ProjectRoot/Gpio",
    "$ProjectRoot/Time",
    "$ProjectRoot/wave_wheel",
    "$ProjectRoot/adc",
    "$ProjectRoot/Lora"
)

# 宏定义
$Macros = @(
    "__ASM=asm",            # inline asm 宏
    "CH32V00x",             # MCU 宏
    "__clang_analyzer__"    # 分析时用
)

# Clang 路径
$ClangPath = "E:\LLVM\bin\clang.exe"

# 报告输出目录
$ReportDir = "$ProjectRoot\scan_report"
if (-not (Test-Path $ReportDir)) {
    New-Item -ItemType Directory -Path $ReportDir | Out-Null
}

# 找到所有 C 文件
$CFiles = Get-ChildItem -Path $ProjectRoot -Recurse -Filter *.c

$IncludeArgs = $IncludeDirs | ForEach-Object { "-I$_" }
$MacroArgs   = $Macros | ForEach-Object { "-D$_" }

foreach ($file in $CFiles) {
    $FileName = [System.IO.Path]::GetFileNameWithoutExtension($file.FullName)
    $OutDir = Join-Path $ReportDir $FileName
    if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Path $OutDir | Out-Null }
    $OutFile = Join-Path $OutDir "$FileName.html"

    Write-Host "Analyzing $($file.FullName)..."

    & $ClangPath --analyze -Xanalyzer -analyzer-output=html $MacroArgs $IncludeArgs -o $OutFile $file.FullName
}


Write-Host "Static analysis finished. Reports are in $ReportDir"
