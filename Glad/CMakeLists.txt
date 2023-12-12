cmake_minimum_required (VERSION 3.20)

add_library(GLAD STATIC glad.c)
target_include_directories(GLAD PUBLIC "../include")