#!/bin/bash

set -e

if [ -f /etc/debian_version ]; then
	sudo apt install -y \
		libvulkan-dev \
		libxinerama-dev \
		libxcursor-dev \
		xorg-dev \
		libglu1-mesa-dev \
		pkg-config \
		build-essential \
		ninja-build \
		glslang-tools \
		vulkan-validationlayers \
		spirv-tools \
		cmake
elif [ -f /etc/arch-release ]; then
	sudo pacman -S --needed \
		vulkan-headers \
		vulkan-icd-loader \
		libxinerama \
		libxcursor \
		xorg-server \
		mesa \
		pkgconf \
		base-devel \
		ninja \
		glslang \
		vulkan-validation-layers \
		spirv-tools \
		cmake
elif [ -f /etc/fedora-release ]; then
	sudo dnf install -y \
		vulkan-headers \
		vulkan-loader-devel \
		libXinerama-devel \
		libXcursor-devel \
		xorg-x11-server-devel \
		mesa-libGLU-devel \
		pkgconf \
		gcc-c++ \
		ninja-build \
		glslang \
		vulkan-validation-layers \
		spirv-tools \
		cmake
else
	echo "Unsupported distro. Please install dependencies manually."
	exit 1
fi

echo -e "\n\n\nDone! You can now execute the following commands in order."
echo -e '\tgit submodule update --init'
echo -e '\tcmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j'
echo -e '\tcd build'
echo -e '/t./hel'