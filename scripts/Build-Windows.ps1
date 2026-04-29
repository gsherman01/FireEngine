param(
    [ValidateSet('Debug','Release')]
    [string]$Configuration = 'Debug',
    [ValidateSet('Win32','x64')]
    [string]$Platform = 'x64'
)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$slnPath = Join-Path $repoRoot 'FireEngine/FireEngine.sln'

if (-not (Test-Path $slnPath)) {
    throw "Solution not found: $slnPath"
}

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw 'vswhere.exe not found. Install Visual Studio Build Tools 2022 or Visual Studio 2022.'
}

$msbuildPath = & $vswhere -latest -requires Microsoft.Component.MSBuild -find 'MSBuild/**/Bin/MSBuild.exe' | Select-Object -First 1
if ([string]::IsNullOrWhiteSpace($msbuildPath)) {
    throw 'MSBuild.exe not found from vswhere.'
}

Write-Host "Using MSBuild: $msbuildPath"
Write-Host "Building: $slnPath ($Configuration|$Platform)"

& $msbuildPath $slnPath /m /t:Build /p:Configuration=$Configuration /p:Platform=$Platform /verbosity:minimal

if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

Write-Host 'Build completed successfully.'
