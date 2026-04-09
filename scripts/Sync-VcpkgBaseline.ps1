param(
    [Parameter(Mandatory = $false)]
    [string]$VcpkgRoot = "C:\vcpkg",

    [Parameter(Mandatory = $false)]
    [string]$ManifestPath = "./FireEngine/vcpkg.json",

    [Parameter(Mandatory = $false)]
    [string]$Triplet = "x64-windows",

    [Parameter(Mandatory = $false)]
    [string]$Features = "",

    [Parameter(Mandatory = $false)]
    [switch]$SkipInstall
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $ManifestPath)) {
    throw "Manifest file not found: $ManifestPath"
}

if (-not (Test-Path $VcpkgRoot)) {
    throw "vcpkg root not found: $VcpkgRoot"
}

Write-Host "[1/5] Fetching full vcpkg git history..."
git -C $VcpkgRoot fetch --all --tags --prune
if ($LASTEXITCODE -ne 0) {
    throw "git fetch failed for $VcpkgRoot"
}

Write-Host "[2/5] Reading current vcpkg HEAD..."
$baseline = (git -C $VcpkgRoot rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($baseline)) {
    throw "Could not determine vcpkg HEAD commit."
}

Write-Host "[3/5] Writing builtin-baseline to $ManifestPath"
$manifest = Get-Content $ManifestPath -Raw | ConvertFrom-Json
$manifest."builtin-baseline" = $baseline
$manifest | ConvertTo-Json -Depth 100 | Set-Content $ManifestPath -Encoding UTF8

if ($SkipInstall) {
    Write-Host "[4/5] Skipping install as requested."
    Write-Host "[5/5] Done. Baseline synchronized to: $baseline"
    exit 0
}

Write-Host "[4/5] Running vcpkg install for triplet $Triplet..."
$vcpkgExe = Join-Path $VcpkgRoot "vcpkg.exe"
if (-not (Test-Path $vcpkgExe)) {
    throw "vcpkg executable not found: $vcpkgExe"
}

$manifestRoot = (Resolve-Path (Split-Path -Parent $ManifestPath)).Path
$installArgs = @("install", "--triplet", $Triplet, "--x-manifest-root", $manifestRoot)

if (-not [string]::IsNullOrWhiteSpace($Features)) {
    $installArgs += "--x-feature=$Features"
}

& $vcpkgExe @installArgs
if ($LASTEXITCODE -ne 0) {
    throw "vcpkg install failed with exit code $LASTEXITCODE"
}

Write-Host "[5/5] Done. Baseline synchronized to: $baseline"
