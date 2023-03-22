#!/bin/bash

PLATFORM="${*}"

if [[ "${PLATFORM}" == "" ]]; then

  NAME=$(uname -s)

  if [[ "${NAME}" == "Linux" ]]; then
    PLATFORM="linux"
  fi

  if [[ "${NAME}" == "Darwin" ]]; then
    PLATFORM="macos"
  fi

fi

if [[ "${PLATFORM}" != "linux" ]] && [[ "${PLATFORM}" != "macos" ]]; then
  echo "invalid platform: the value must be one of (linux, macos)" >&2
  exit 1
fi

echo "${PLATFORM}"
