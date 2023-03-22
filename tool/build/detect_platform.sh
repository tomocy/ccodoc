#!/bin/bash

PLATFORM="${*}"

if [[ "${PLATFORM}" == "" ]]; then

  NAME=$(uname -s)

  if [[ "${NAME}" == "Linux" ]]; then
    PLATFORM="linux"
  fi

  if [[ "${NAME}" == "Darwin" ]]; then
    PLATFORM="mac"
  fi

fi

if [[ "${PLATFORM}" != "linux" ]] && [[ "${PLATFORM}" != "mac" ]]; then
  echo "invalid platform: the value must be one of (linux, mac)" >&2
  exit 1
fi

echo "${PLATFORM}"
