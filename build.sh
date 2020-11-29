#!/bin/bash

chcon -R -t container_file_t .

podman build -t jbofihe-build-image .

podman kill jbofihe-build
podman rm jbofihe-build

podman run -it \
    --userns=keep-id \
    -v $(pwd):/src:rw \
    --name jbofihe-build \
    jbofihe-build-image \
    /src/build-in-container.sh

podman kill jbofihe-build
podman rm jbofihe-build
