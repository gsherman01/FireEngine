param(
    [Parameter(Mandatory = $false)]
    [string]$VcpkgRoot = "C:\vcpkg",

    [Parameter(Mandatory = $false)]
    [string]$ManifestPath = "./vcpkg.json",

    [Parameter(Mandatory = $false)]
    [string]$Triplet = "x64-windows"
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

Write-Host "[2/5] Reading current vcpkg HEAD..."
$baseline = (git -C $VcpkgRoot rev-parse HEAD).Trim()
if ([string]::IsNullOrWhiteSpace($baseline)) {
    throw "Could not determine vcpkg HEAD commit."
}

Write-Host "[3/5] Writing builtin-baseline to $ManifestPath"
$manifest = Get-Content $ManifestPath -Raw | ConvertFrom-Json
$manifest."builtin-baseline" = $baseline
$manifest | ConvertTo-Json -Depth 100 | Set-Content $ManifestPath -Encoding UTF8

Write-Host "[4/5] Running vcpkg install for triplet $Triplet..."
$vcpkgExe = Join-Path $VcpkgRoot "vcpkg.exe"
if (-not (Test-Path $vcpkgExe)) {
    throw "vcpkg executable not found: $vcpkgExe"
}

$manifestRoot = (Resolve-Path (Split-Path -Parent $ManifestPath)).Path
& $vcpkgExe install --triplet $Triplet --x-manifest-root $manifestRoot

Write-Host "[5/5] Done. Baseline synchronized to: $baseline"
