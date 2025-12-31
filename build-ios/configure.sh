(
set -ue

cd "$(dirname "$0")"
cmake .. -G Xcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=26.0
)
